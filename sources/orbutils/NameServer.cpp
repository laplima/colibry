#include "NameServer.h"
#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <stdexcept>

using namespace std;
using namespace colibry;

// helper

vector<string> cosplit(const string& input)
{
	regex re{"/"};
	sregex_token_iterator first{input.begin(), input.end(), re, -1};
	sregex_token_iterator last;
	return {first, last};
}

CosNaming::Name StringToName(const string& name)
{
	auto vs = cosplit(name);

	CosNaming::Name nm(vs.size());
	nm.length(vs.size());
	for (int i=0; i<vs.size(); ++i)
		nm[i].id = CORBA::string_dup(vs[i].c_str());
	return nm;
}

//
// static members
//

// CORBA::ORB_var NameServer::nsorb_ = CORBA::ORB::_nil();
// bool NameServer::ownorb_ = true;
// unique_ptr<NameServer> NameServer::instance_{nullptr};

NameServer* NameServer::global_ = nullptr;

NameServer* NameServer::global()
{
	if (global_ == nullptr)
		throw std::runtime_error{"NameServer: no global BS"};
	return global_;
}

//
// method implementations
//
NameServer::NameServer()
{
	orb_ = CORBA::ORB::_duplicate(ORBManager::global()->orb());
	init_ns_ref();
}

NameServer::NameServer(int argc, char* argv[])
{
	orb_ = CORBA::ORB_init(argc, argv, "NS-ORB");
	init_ns_ref();
}

NameServer::NameServer(CORBA::ORB_ptr orb)
{
	if (CORBA::is_nil(orb))
		throw runtime_error{"NameServer: No ORB provided for NS"};

	orb_ = CORBA::ORB::_duplicate(orb);
	init_ns_ref();
}

NameServer::NameServer(ORBManager& om)
{
	if (!om.orb_initiated())
		throw runtime_error{"NameServer: ORBManager not initiated"};

	orb_ = CORBA::ORB::_duplicate(om.orb());
	init_ns_ref();
}

void NameServer::init_ns_ref()
{
	if (global_ == nullptr) {
		ACE_LOG_MSG->set_flags(ACE_Log_Msg::SILENT); // turn off error messages
		global_ = this;
	}

	CORBA::Object_ptr ref = orb_->resolve_initial_references("NameService");
	ns_ = CosNaming::NamingContext::_narrow(ref);
}

NameServer::~NameServer()
{
	// I have a copy: always destroy
	orb_->destroy();
}

CORBA::Object_ptr NameServer::resolve(const string& name)
{
	// does not create naming contexts from name...
	return ns_->resolve(StringToName(name));
}

void NameServer::bind(const string& name, CORBA::Object_ptr ref)
{
	CosNaming::Name n = StringToName(name);
	verifyNamingContexts(n);	// Create intermediary naming contexts if needed
	ns_->bind(n,ref);			// throw if already bound
}

void NameServer::rebind(const string& name, CORBA::Object_ptr ref)
{
	CosNaming::Name n = StringToName(name);
	verifyNamingContexts(n);	// Create intermediary naming contexts if needed
	ns_->rebind(n,ref);
}

void NameServer::unbind(const string& name)
{
	ns_->unbind(StringToName(name));
}

void NameServer::verifyNamingContexts(const CosNaming::Name& n)
{
	// create all naming contexts if necessary
	CosNaming::NamingContext_var nc = CosNaming::NamingContext::_duplicate(ns_);
	for (unsigned int i=0; i<n.length()-1; i++) {   //  (last in n is not a nc)
		try {
			nc = nc->bind_new_context(StringToName(n[i].id.in()));
		} catch (CosNaming::NamingContext::AlreadyBound& ) {
			nc = CosNaming::NamingContext::_narrow(nc->resolve(StringToName(n[i].id.in())));
		}
	}
}
