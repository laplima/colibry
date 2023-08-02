#include "ORBManager.h"
#include <fstream>
#include <stdexcept>
#include <iostream>

using namespace std;
using namespace colibry;

// ----------------------------------------------------------------------------
// POAM
// ----------------------------------------------------------------------------

MPOA::MPOA(PortableServer::POA_ptr poa)
	: poa_{PortableServer::POA::_duplicate(poa)}
{}

MPOA::MPOA(const MPOA& mpoa)
{
	if (mpoa.is_nil())
		poa_ = PortableServer::POA::_nil();
	else
		poa_ = PortableServer::POA::_duplicate(mpoa.poa_);
}

MPOA::MPOA(MPOA&& mpoa) noexcept
{
	poa_ = std::exchange(mpoa.poa_,PortableServer::POA::_nil());
}

MPOA::~MPOA()
{
	if (!is_nil())
		poa_->destroy(true, true);
}

void MPOA::swap(MPOA& v) noexcept
{
	if (is_nil() && v.is_nil())
		return;
	if (is_nil()) {
		poa_ = PortableServer::POA::_duplicate(v.poa_.in());
		v.poa_ = PortableServer::POA::_nil();
	} else if (v.is_nil()) {
		poa_ = PortableServer::POA::_nil();
		v.poa_ = PortableServer::POA::_duplicate(poa_.in());
	} else {
		// neihter are nil
		PortableServer::POA_var aux = PortableServer::POA::_duplicate(poa_.in());
		poa_ = PortableServer::POA::_duplicate(v.poa_.in());
		v.poa_ = PortableServer::POA::_duplicate(aux.in());
	}
}

MPOA& MPOA::operator=(MPOA poam)
{
	swap(poam);
	return *this;
}

void MPOA::activate()
{
	poa_->the_POAManager()->activate();
}

MPOA MPOA::create_child_poa(
	const std::string& name,
	const std::vector<POAPolicy>& policies)
{
	CORBA::PolicyList cpolicies(policies.size());
	cpolicies.length(policies.size());

	unsigned int count = 0;
	for (POAPolicy p : policies) {
		switch (p) {
			using enum POAPolicy;
		case SYSTEM_ID:
			cpolicies[count] = poa_->create_id_assignment_policy(PortableServer::SYSTEM_ID);
			break;
		case USER_ID:
			cpolicies[count] = poa_->create_id_assignment_policy(PortableServer::USER_ID);
			break;
		case IMPLICIT_ACTIVATION: // default
			cpolicies[count] = poa_->create_implicit_activation_policy(PortableServer::IMPLICIT_ACTIVATION);
			break;
		case NO_IMPLICIT_ACTIVATION:
			cpolicies[count] = poa_->create_implicit_activation_policy(PortableServer::NO_IMPLICIT_ACTIVATION);
			break;
		case UNIQUE_ID: // default
			cpolicies[count] = poa_->create_id_uniqueness_policy(PortableServer::UNIQUE_ID);
			break;
		case MULTIPLE_ID:
			cpolicies[count] = poa_->create_id_uniqueness_policy(PortableServer::MULTIPLE_ID);
			break;
		case TRANSIENT: // default
			cpolicies[count] = poa_->create_lifespan_policy(PortableServer::TRANSIENT);
			break;
		case PERSISTENT:
			cpolicies[count] = poa_->create_lifespan_policy(PortableServer::PERSISTENT);
			break;
		case USE_ACTIVE_OBJECT_MAP_ONLY:	// default
			cpolicies[count] = poa_->create_request_processing_policy(PortableServer::USE_ACTIVE_OBJECT_MAP_ONLY);
			break;
    	case USE_DEFAULT_SERVANT:
			cpolicies[count] = poa_->create_request_processing_policy(PortableServer::USE_DEFAULT_SERVANT);
    		break;
    	case USE_SERVANT_MANAGER:
			cpolicies[count] = poa_->create_request_processing_policy(PortableServer::USE_SERVANT_MANAGER);
    		break;
    	case RETAIN:	// default
			cpolicies[count] = poa_->create_servant_retention_policy(PortableServer::RETAIN);
    		break;
    	case NON_RETAIN:
			cpolicies[count] = poa_->create_servant_retention_policy(PortableServer::NON_RETAIN);
    		break;
    	case ORB_CTRL_MODEL:	// default
			cpolicies[count] = poa_->create_thread_policy(PortableServer::ORB_CTRL_MODEL);
    		break;
    	case SINGLE_THREAD_MODEL:
			cpolicies[count] = poa_->create_thread_policy(PortableServer::SINGLE_THREAD_MODEL);
			break;
		}
		++count;
	}

	PortableServer::POA_ptr childpoa = poa_->create_POA(name.c_str(),poa_->the_POAManager(),cpolicies);

	for (count=0; count<cpolicies.length(); ++count)
		cpolicies[count]->destroy();

	return MPOA{childpoa};
}

// ----------------------------------------------------------------------------
// ORBManager
// ----------------------------------------------------------------------------

ORBManager::ORBManager(const std::string& orbname)
{
	init(orbname);
}

ORBManager::ORBManager(int argc, char** argv, const string& orbname)
{
	init(argc,argv,orbname);
}

ORBManager::ORBManager(CORBA::ORB_ptr orb)
	: orbname_{orb->id()},
	orb_{CORBA::ORB::_duplicate(orb)}
{
}

ORBManager::ORBManager(const ORBManager& om)
{
	*this = om;
}

ORBManager::~ORBManager()
{
	// std::cout << "ORBM " << orbname_ << " to be destroyed\n";
	if (initiated())
		orb_->destroy();
	// std::cout << "ORBM " << orbname_ << " destroyed\n";
}

ORBManager& ORBManager::operator=(const ORBManager& om)
{
	if (&om != this) {
		orbname_ = om.orbname_;
		rootpoa_ = om.rootpoa_;
		orb_ = CORBA::ORB::_nil();
		if (!CORBA::is_nil(om.orb_.in()))
			orb_ = CORBA::ORB::_duplicate(om.orb_.in());
	}
	return *this;
}

ORBManager& ORBManager::operator=(CORBA::ORB_ptr orb)
{
	orbname_ = orb->id();

	orb_ = (!CORBA::is_nil(orb)) ? CORBA::ORB::_duplicate(orb) : CORBA::ORB::_nil();
	rootpoa_ = MPOA{};
	return *this;
}

void ORBManager::init(int argc, char** argv, const string& orbname)
{
	// always initiate (may return existing ORB if orbname has been
	// previously initiated and not destroyed)
	orb_ = CORBA::ORB_init(argc,argv,orbname.c_str());
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
	return (!CORBA::is_nil(orb_.in()));
}

void ORBManager::check_init() const
{
	if (!initiated())
		throw runtime_error{"ORB was not initialized"};
}

void ORBManager::activate_rootpoa()
{
	check_init();
	rootpoa_ = MPOA{bootstrap<PortableServer::POA>("RootPOA")};
	rootpoa_.activate();
}

string ORBManager::object_to_string(CORBA::Object_ptr obj)
{
	check_init();
	CORBA::String_var s = orb_->object_to_string(obj);
	return string{s.in()};
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
		orb_->shutdown();
	}
}
