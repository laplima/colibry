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
//	OO_i obji;
//	OO_var obj = om.activate_object<OO>(obji);
//	cout << om.object_to_string(obj.in()) << endl;
//	om.run();
//
//	CLIENT
//	----------
//	ORBManager om{argc,argv};
//	OO_var obj = om.string_to_object<OO>("file://,,,,");
//
// Notes:
// ORBManager om;	// non-initialized om: needes init() in order
// om.init(argc, argv);
//
// ORB name is optional, but if the same name is provided, the
// same ORB is used (unless it has been previously destroyed).
//

#ifndef __ORB_MANAGER_H__
#define __ORB_MANAGER_H__

#include <string>
#include <vector>
#include <tao/ORB.h>
#include <tao/PortableServer/PortableServer.h>

namespace colibry {

	class ORBManager {
	public:
		ORBManager();								// non-initiated
		ORBManager(const std::string& orbname);		// initiated w/ default args
		ORBManager(int argc, char* argv[],
			const std::string& orbname="ORB");		// will call init()
		ORBManager(CORBA::ORB_ptr orb);
		ORBManager(const ORBManager& om);
		virtual ~ORBManager();

		ORBManager& operator=(const ORBManager& om);
		ORBManager& operator=(CORBA::ORB_ptr orb);

		std::string name() const { return orbname_; }

		void init(int argc, char* argv[], const std::string& orbname="ORB");
		void init(const std::string& orbname="ORB");    // default args
		bool initiated() const;

		void activate_rootpoa();

		CORBA::ORB_ptr orb() { return m_ORB.in(); }
		PortableServer::POA_ptr rootpoa() { return m_rootpoa.in(); }

		enum class Policy : char {
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
		PortableServer::POA_var create_child_poa(const std::string& name, const std::vector<Policy>& policies);

		template<typename T>
		T* activate_object(PortableServer::ServantBase& servant);

		void run() { m_ORB->run(); }

		std::string object_to_string(CORBA::Object_ptr obj);
		CORBA::Object_ptr string_to_object(const std::string& ior);

		template<typename T>
		T* bootstrap(const std::string& service);

		template<typename T>
		T* string_to_object(const std::string& ior);

		void save_ior(const std::string& fname, CORBA::Object_ptr obj);
		void shutdown();
	protected:
		void check_init();
	protected:
		std::string orbname_;
		CORBA::ORB_var m_ORB = CORBA::ORB::_nil();
		PortableServer::POA_var m_rootpoa = PortableServer::POA::_nil();
		PortableServer::POAManager_var m_poamgr = PortableServer::POAManager::_nil();
	};

	template<typename T>
	T* ORBManager::bootstrap(const std::string& service) {
		CORBA::Object_ptr ref = m_ORB->resolve_initial_references(service.c_str());
		return T::_narrow(ref);
	}

	template<typename T>
	T* ORBManager::activate_object(PortableServer::ServantBase& servant)
	{
		PortableServer::ObjectId_var oid = m_rootpoa->activate_object(&servant);
		CORBA::Object_ptr ref = m_rootpoa->id_to_reference(oid.in());
		return T::_narrow(ref);
	}

	template<typename T>
	T* ORBManager::string_to_object(const std::string& ior)
	{
		CORBA::Object_ptr ref = m_ORB->string_to_object(ior.c_str());
		return T::_narrow(ref);
	}

};	// namespace

#endif
