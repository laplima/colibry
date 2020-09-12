#include "ORBManager.h"
#include <fstream>
#include <stdexcept>

using namespace std;
using namespace colibry;

ORBManager* ORBManager::global = nullptr;

ORBManager::ORBManager()
{
	global = this;
}

ORBManager::ORBManager(int argc, char* argv[])
{
	init(argc,argv);
	global = this;
}

ORBManager::~ORBManager()
{
	if (orb_initiated()) {
		if (!CORBA::is_nil(m_rootpoa.in()))
			m_rootpoa->destroy(true,true);
		m_ORB->destroy();
	}
}

void ORBManager::init(int argc, char* argv[])
{
	if (!orb_initiated())
		m_ORB = CORBA::ORB_init(argc,argv,"ORB");
}

bool ORBManager::orb_initiated() const
{
	return (!CORBA::is_nil(m_ORB.in()));
}

void ORBManager::check_init()
{
	if (!orb_initiated())
		throw runtime_error{"ORB was not initialized"};
}

void ORBManager::activate_rootpoa()
{
	check_init();
	CORBA::Object_ptr ref = m_ORB->resolve_initial_references("RootPOA");
	m_rootpoa = PortableServer::POA::_narrow(ref);
	m_poamgr = m_rootpoa->the_POAManager();
	m_poamgr->activate();
}

PortableServer::POA_var ORBManager::create_child_poa(const std::string& name, const std::vector<Policy>& policies)
{
	check_init();
	CORBA::PolicyList cpolicies(policies.size());
	cpolicies.length(policies.size());

	unsigned int count = 0;
	for (Policy p : policies) {
		switch (p) {
		case Policy::SYSTEM_ID:
			cpolicies[count] = m_rootpoa->create_id_assignment_policy(PortableServer::SYSTEM_ID);
			break;
		case Policy::USER_ID:
			cpolicies[count] = m_rootpoa->create_id_assignment_policy(PortableServer::USER_ID);
			break;
		case Policy::IMPLICIT_ACTIVATION: // default
			cpolicies[count] = m_rootpoa->create_implicit_activation_policy(PortableServer::IMPLICIT_ACTIVATION);
			break;
		case Policy::NO_IMPLICIT_ACTIVATION:
			cpolicies[count] = m_rootpoa->create_implicit_activation_policy(PortableServer::NO_IMPLICIT_ACTIVATION);
			break;
		case Policy::UNIQUE_ID: // default
			cpolicies[count] = m_rootpoa->create_id_uniqueness_policy(PortableServer::UNIQUE_ID);
			break;
		case Policy::MULTIPLE_ID:
			cpolicies[count] = m_rootpoa->create_id_uniqueness_policy(PortableServer::MULTIPLE_ID);
			break;
		case Policy::TRANSIENT: // default
			cpolicies[count] = m_rootpoa->create_lifespan_policy(PortableServer::TRANSIENT);
			break;
		case Policy::PERSISTENT:
			cpolicies[count] = m_rootpoa->create_lifespan_policy(PortableServer::PERSISTENT);
			break;
		case Policy::USE_ACTIVE_OBJECT_MAP_ONLY:	// default
			cpolicies[count] = m_rootpoa->create_request_processing_policy(PortableServer::USE_ACTIVE_OBJECT_MAP_ONLY);
			break;
    	case Policy::USE_DEFAULT_SERVANT:
			cpolicies[count] = m_rootpoa->create_request_processing_policy(PortableServer::USE_DEFAULT_SERVANT);
    		break;
    	case Policy::USE_SERVANT_MANAGER:
			cpolicies[count] = m_rootpoa->create_request_processing_policy(PortableServer::USE_SERVANT_MANAGER);
    		break;
    	case Policy::RETAIN:	// default
			cpolicies[count] = m_rootpoa->create_servant_retention_policy(PortableServer::RETAIN);
    		break;
    	case Policy::NON_RETAIN:
			cpolicies[count] = m_rootpoa->create_servant_retention_policy(PortableServer::NON_RETAIN);
    		break;
    	case Policy::ORB_CTRL_MODEL:	// default
			cpolicies[count] = m_rootpoa->create_thread_policy(PortableServer::ORB_CTRL_MODEL);
    		break;
    	case Policy::SINGLE_THREAD_MODEL:
			cpolicies[count] = m_rootpoa->create_thread_policy(PortableServer::SINGLE_THREAD_MODEL);
			break;
		}
		++count;
	}

	PortableServer::POA_ptr childpoa = m_rootpoa->create_POA(name.c_str(),m_poamgr,cpolicies);

	for (count=0; count<cpolicies.length(); ++count)
		cpolicies[count]->destroy();

	return childpoa;
}

string ORBManager::object_to_string(CORBA::Object_ptr obj)
{
	check_init();
	CORBA::String_var s = m_ORB->object_to_string(obj);
	return string{s.in()};
}

CORBA::Object_ptr ORBManager::string_to_object(const string& ior)
{
	check_init();
	return m_ORB->string_to_object(ior.c_str());
}

void ORBManager::save_ior(const std::string& fname, CORBA::Object_ptr obj)
{
	check_init();
	ofstream f{fname};
	f << object_to_string(obj);
}

void ORBManager::shutdown()
{
	if (orb_initiated())
		m_ORB->shutdown();
}
