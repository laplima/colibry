#include "ORBManager.h"
#include <fstream>
#include <stdexcept>
#include <iostream>

using namespace std;
using namespace colibry;

ORBManager::ORBManager(const std::string& orbname)
{
	init(orbname);
}

ORBManager::ORBManager(int argc, char* argv[], const string& orbname)
{
	init(argc,argv,orbname);
}

ORBManager::ORBManager(CORBA::ORB_ptr orb) : orbname_{orb->id()}
{
	m_ORB = CORBA::ORB::_duplicate(orb);
}

ORBManager::ORBManager(const ORBManager& om)
{
	*this = om;
}

ORBManager::~ORBManager()
{
	// std::cout << "ORBM " << orbname_ << " to be destroyed\n";
	if (initiated()) {
		if (!CORBA::is_nil(m_rootpoa.in()))
			m_rootpoa->destroy(true,true);
		m_ORB->destroy();
	}
	// std::cout << "ORBM " << orbname_ << " destroyed\n";
}

ORBManager& ORBManager::operator=(const ORBManager& om)
{
	if (&om != this) {
		orbname_ = om.orbname_;
		m_ORB = CORBA::ORB::_nil();
		m_rootpoa = PortableServer::POA::_nil();
		m_poamgr = PortableServer::POAManager::_nil();
		if (!CORBA::is_nil(om.m_ORB.in()))
			m_ORB = CORBA::ORB::_duplicate(om.m_ORB.in());
		if (!CORBA::is_nil(om.m_rootpoa.in()))
			m_rootpoa = PortableServer::POA::_duplicate(om.m_rootpoa.in());
		if (!CORBA::is_nil(om.m_poamgr.in()))
			m_poamgr = PortableServer::POAManager::_duplicate(om.m_poamgr.in());
	}
	return *this;
}

ORBManager& ORBManager::operator=(CORBA::ORB_ptr orb)
{
	orbname_ = orb->id();

	m_ORB = (!CORBA::is_nil(orb)) ? CORBA::ORB::_duplicate(orb) : CORBA::ORB::_nil();
	m_rootpoa = PortableServer::POA::_nil();
	m_poamgr = PortableServer::POAManager::_nil();
	return *this;
}

void ORBManager::init(int argc, char* argv[], const string& orbname)
{
	// always initiate (may return existing ORB if orbname has been
	// previously initiated and not destroyed)
	m_ORB = CORBA::ORB_init(argc,argv,orbname.c_str());
	orbname_ = orbname;
}

void ORBManager::init(const std::string& orbname)
{
	int argc = 1;
	char *argv[1] = { nullptr };
	init(argc,argv,orbname);
}

bool ORBManager::initiated() const
{
	return (!CORBA::is_nil(m_ORB.in()));
}

void ORBManager::check_init()
{
	if (!initiated())
		throw runtime_error{"ORB was not initialized"};
}

void ORBManager::activate_rootpoa()
{
	check_init();
	m_rootpoa = bootstrap<PortableServer::POA>("RootPOA");
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
	if (initiated() && !shutdown_) {
		shutdown_ = true;
		m_ORB->shutdown();
	}
}
