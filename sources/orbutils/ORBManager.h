//
// ORBManager
// (C) LAPLJ.
//
// TYPICAL USAGE:
//
//	SERVER
//	----------
//	ORBManager om{argc,argv};
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
// ORBManager om;		non-initialized om: needes init() in order to be used
// om.init(argc, argv);
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
		static ORBManager* global;		// last created ORBManager
	public:
		ORBManager();
		void init(int argc, char* argv[]);
		bool orb_initiated() const;

		ORBManager(int argc, char* argv[]);		// will call init()
		virtual ~ORBManager();

		void activate_rootpoa();
		CORBA::ORB_ptr orb() { return m_ORB.in(); }
		CORBA::ORB_ptr operator*() { return orb(); }
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
		T* string_to_object(const std::string& ior);

		void save_ior(const std::string& fname, CORBA::Object_ptr obj);
		void shutdown();
	protected:
		void check_init();
	protected:
		CORBA::ORB_var m_ORB = CORBA::ORB::_nil();
		PortableServer::POA_var m_rootpoa = PortableServer::POA::_nil();
		PortableServer::POAManager_var m_poamgr;
	};

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
