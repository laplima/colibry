//
// ORBManager
// (C) LAPLJ.
//
// See README.md
//

#ifndef ORB_MANAGER_H
#define ORB_MANAGER_H

#include <string>
#include <vector>
#include <tao/ORB.h>
#include <tao/PortableServer/PortableServer.h>

namespace colibry {

	enum class POAPolicy : char {
		SYSTEM_ID,					// default
		USER_ID,
		IMPLICIT_ACTIVATION,		// default
		NO_IMPLICIT_ACTIVATION,
		UNIQUE_ID,					// default
		MULTIPLE_ID,
		TRANSIENT,					// default
		PERSISTENT,
		USE_ACTIVE_OBJECT_MAP_ONLY,	// default
		USE_DEFAULT_SERVANT,
		USE_SERVANT_MANAGER,
		RETAIN,						// default
		NON_RETAIN,
		ORB_CTRL_MODEL,				// default
		SINGLE_THREAD_MODEL
	};

	// Managed POA = MPOA
	// TODO: use MPOA instead of plain POA in the ORBManager

	class MPOA {
	public:
		MPOA() : poa_{PortableServer::POA::_nil()} {}
		explicit MPOA(PortableServer::POA_ptr poa);
		MPOA(const MPOA& mpoa);		// copy constructor
		MPOA(MPOA&& mpoa) noexcept; // move constructor
		virtual ~MPOA();
		MPOA& operator=(MPOA poam);
		void swap(MPOA& v) noexcept;

		// get underlying POA
		[[nodiscard]] PortableServer::POA_ptr poa() { return poa_.in(); }
		[[nodiscard]] bool is_nil() const { return CORBA::is_nil(poa_.in()); }

		// get reference from an object ID (string)
		template <typename T=CORBA::Object_ptr>
		TAO_Objref_Var_T<T> get_reference(const std::string& id);

		// register servant and get reference
		template <typename T=CORBA::Object_ptr>
		TAO_Objref_Var_T<T> activate_object(PortableServer::ServantBase& servant);

		// register servant with an ID
		template <typename T=CORBA::Object_ptr>
		TAO_Objref_Var_T<T> activate_object_with_id(
			const std::string& id,
			PortableServer::ServantBase& servant);

		void activate();
		MPOA create_child_poa(
			const std::string& name,
			const std::vector<POAPolicy>& policies);
	private:
		PortableServer::POA_var poa_;
	};

	// ORBManager ---------------------------------------------------

	class ORBManager {
	public:
		ORBManager() = default;						// non-initiated
		ORBManager(const std::string& orbname);		// initiated w/ default args
		ORBManager(int argc, char** argv,
			const std::string& orbname="ORB");		// will call init()
		ORBManager(CORBA::ORB_ptr orb);
		ORBManager(const ORBManager& om);
		ORBManager(ORBManager&&) = default;
		virtual ~ORBManager();

		ORBManager& operator=(const ORBManager& om);
		ORBManager& operator=(CORBA::ORB_ptr orb);
		ORBManager& operator=(ORBManager&&) = default;

		void init(int argc, char** argv,
			const std::string& orbname="ORB");
		void init(const std::string& orbname="ORB");    // default args

		[[nodiscard]] std::string name() const { return orbname_; }
		[[nodiscard]] bool initiated() const;
		CORBA::ORB_ptr orb() { return orb_.in(); }

		void run() { orb_->run(); }

		void activate_rootpoa();
		MPOA& rootpoa() { return rootpoa_; }

		// activate object in the root poa
		// A short-hand for:
		// auto rpoa = orbm.root_poa();
		// rpoa.activate_object(...);
		template<typename T=CORBA::Object_ptr>
		TAO_Objref_Var_T<T> activate_object(PortableServer::ServantBase& servant);

		std::string object_to_string(CORBA::Object_ptr obj);

		template<typename T=CORBA::Object_ptr>
		TAO_Objref_Var_T<T> string_to_object(const std::string& ior);

		template<typename T=CORBA::Object_ptr>
		TAO_Objref_Var_T<T> bootstrap(const std::string& service);

		void save_ior(const std::string& fname, CORBA::Object_ptr obj);
		void shutdown();
		[[nodiscard]] bool has_shutdown() const { return shutdown_; }
		[[nodiscard]] bool work_pending() const {
			return orb_->work_pending(); }
		void perform_work() { orb_->perform_work(); }

	protected:

		bool shutdown_ = false;
		void check_init() const;
		std::string orbname_;
		CORBA::ORB_var orb_ = CORBA::ORB::_nil();
		MPOA rootpoa_;
	};

	//
	// --- template implementations
	//

	template<typename T>
	TAO_Objref_Var_T<T> ORBManager::bootstrap(const std::string& service)
	{
		CORBA::Object_ptr ref = orb_->resolve_initial_references(service.c_str());
		return T::_narrow(ref);
	}

	template<typename T>
	TAO_Objref_Var_T<T> /* T* */ ORBManager::activate_object(PortableServer::ServantBase& servant)
	{
		return rootpoa_.activate_object<T>(servant);
	}

	template<typename T>
	TAO_Objref_Var_T<T> /* T* */ ORBManager::string_to_object(
		const std::string& ior)
	{
		check_init();
		auto *ref = orb_->string_to_object(ior.c_str());
		return T::_narrow(ref);
	}

	// POA

	template <typename T>
	TAO_Objref_Var_T<T> MPOA::get_reference(const std::string& id)
	{
		try {
			PortableServer::ObjectId_var oid = PortableServer::string_to_ObjectId(id.c_str());
			CORBA::Object_ptr ref = poa_->id_to_reference(oid.in());
			return T::_narrow(ref);
		} catch (const PortableServer::POA::ObjectNotActive&) {
			return T::_nil();
		}
	}

	template <typename T>
	TAO_Objref_Var_T<T> MPOA::activate_object(
		PortableServer::ServantBase& servant)
	{
		PortableServer::ObjectId_var oid = poa_->activate_object(&servant);
		CORBA::Object_ptr ref = poa_->id_to_reference(oid.in());
		return T::_narrow(ref);
	}

	template <typename T>
	TAO_Objref_Var_T<T> MPOA::activate_object_with_id(
		const std::string& id,
		PortableServer::ServantBase& servant)
	{
		PortableServer::ObjectId_var oid = PortableServer::string_to_ObjectId(id.c_str());
		poa_->activate_object_with_id(oid.in(),&servant);
		CORBA::Object_ptr ref = poa_->id_to_reference(oid.in());
		return T::_narrow(ref);
	}

};	// namespace

#endif
