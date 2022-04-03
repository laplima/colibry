//
// ORBManager
// (C) LAPLJ.
//
// TYPICAL USAGE:
//
//	SERVER
//	----------
//	ORBManager om{argc,argv}; // default name is "ORB"
//	om.activate_rootpoa();
//	T_i obji;
//	auto obj = om.activate_object<T>(obji);	// auto = T_var
//	cout << om.object_to_string(obj) << endl;
//	om.run();
//
//	// (with <ENTER> to quit, instead of `om.run()`)
//
//  thread orbth{[&om]() { om.run(); }};
//  cout << "Press ENTER to quit." << endl;
//  cin.get();
//  om.shutdown();
//  orbth.join();
//
//	CLIENT
//	----------
//	ORBManager om{argc,argv};
//	auto obj = om.string_to_object<T>("file://...."); // auto = T_var
//
// Notes:
// ORBManager om;	// non-initialized om: requires late init() before use
// om.init(argc, argv);
//
// ORB name is optional, but if the same name is provided, the
// same ORB is used (unless it has been previously destroyed).
//

#ifndef ORB_MANAGER_H
#define ORB_MANAGER_H

#include <string>
#include <vector>
#include <tao/ORB.h>
#include <tao/PortableServer/PortableServer.h>

namespace colibry {

	enum class POAPolicy : char {
		SYSTEM_ID, // default
		USER_ID,
		IMPLICIT_ACTIVATION, // default
		NO_IMPLICIT_ACTIVATION,
		UNIQUE_ID, // default
		MULTIPLE_ID,
		TRANSIENT, // default
		PERSISTENT,
		USE_ACTIVE_OBJECT_MAP_ONLY,	// default
		USE_DEFAULT_SERVANT,
		USE_SERVANT_MANAGER,
		RETAIN,	// default
		NON_RETAIN,
		ORB_CTRL_MODEL,	// default
		SINGLE_THREAD_MODEL
	};

	class ORBManager {
	public:
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

		[[nodiscard]] std::string name() const { return orbname_; }
		void init(int argc, char** argv, const std::string& orbname="ORB");
		void init(const std::string& orbname="ORB");    // default args
		[[nodiscard]] bool initiated() const;
		CORBA::ORB_ptr orb() { return orb_.in(); }
		void run() { orb_->run(); }

		void activate_rootpoa();
		PortableServer::POA_ptr rootpoa() { return rootpoa_.in(); }
		PortableServer::POA_var create_child_poa(const std::string& name,
			const std::vector<POAPolicy>& policies);

		// T_var = TAO_Objref_Var_T<T>
		template<typename T=CORBA::Object_ptr>
		TAO_Objref_Var_T<T> activate_object(PortableServer::ServantBase& servant);
		std::string object_to_string(CORBA::Object_ptr obj);
		template<typename T=CORBA::Object_ptr>
		TAO_Objref_Var_T<T> string_to_object(const std::string& ior);
		// CORBA::Object_ptr string_to_object(const std::string& ior);

		template<typename T=CORBA::Object_ptr>
		TAO_Objref_Var_T<T> bootstrap(const std::string& service);

		void save_ior(const std::string& fname, CORBA::Object_ptr obj);
		void shutdown();
		[[nodiscard]] bool has_shutdown() const { return shutdown_; }
		[[nodiscard]] bool work_pending() const { return orb_->work_pending(); }
		void perform_work() { orb_->perform_work(); }
	protected:
		bool shutdown_ = false;
		void check_init();
	protected:
		std::string orbname_;
		CORBA::ORB_var orb_ = CORBA::ORB::_nil();
		PortableServer::POA_var rootpoa_ = PortableServer::POA::_nil();
		PortableServer::POAManager_var poamgr_ = PortableServer::POAManager::_nil();
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
		PortableServer::ObjectId_var oid = rootpoa_->activate_object(&servant);
		CORBA::Object_ptr ref = rootpoa_->id_to_reference(oid.in());
		return T::_narrow(ref);
	}

	template<typename T>
	TAO_Objref_Var_T<T> /* T* */ ORBManager::string_to_object(const std::string& ior)
	{
		check_init();
		auto ref = orb_->string_to_object(ior.c_str());
		return T::_narrow(ref);
	}

};	// namespace

#endif
