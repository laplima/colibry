#include "NameServer.h"
#include <iostream>
#include <string>
#include <vector>
#include <regex>

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

CORBA::ORB_var NameServer::nsorb_ = CORBA::ORB::_nil();
bool NameServer::ownorb_ = true;
unique_ptr<NameServer> NameServer::instance_{nullptr};

//
// method implementations
//
NameServer* NameServer::Instance(int argc, char* argv[])
{
	if (CORBA::is_nil(nsorb_)) {
		nsorb_ = CORBA::ORB_init(argc, argv, "NS-ORB");
		ownorb_ = true;
	}
	if (!instance_)
		instance_ = unique_ptr<NameServer>{new NameServer};
	return instance_.get();
}

NameServer* NameServer::Instance(CORBA::ORB_ptr orb)
{
	if (CORBA::is_nil(nsorb_)) {
		if (CORBA::is_nil(orb))
			throw runtime_error{"No ORB provided for NS"};
		nsorb_ = CORBA::ORB::_duplicate(orb);
		ownorb_ = false;
	}
	if (!instance_)
		instance_ = unique_ptr<NameServer>{new NameServer};
	return instance_.get();
}

NameServer::NameServer()
{
	ACE_LOG_MSG->set_flags(ACE_Log_Msg::SILENT); // turn off error messages
	CORBA::Object_ptr ref = nsorb_->resolve_initial_references("NameService");
	ns_ = CosNaming::NamingContext::_narrow(ref);
}

NameServer::~NameServer()
{
	if (ownorb_ && !is_nil(nsorb_.in()))
		nsorb_->destroy();
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
