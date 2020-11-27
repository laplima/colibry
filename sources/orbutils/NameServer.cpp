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
// method implementations
//

NameServer::NameServer()
{
	// delayed orb initialization
}

NameServer::NameServer(CORBA::ORB_ptr orb)
{
	use_orb(orb);
	init_ns_ref();
}

NameServer::NameServer(ORBManager& om)
{
	use_orb(om);
	init_ns_ref();
}

void NameServer::use_orb(ORBManager& om)
{
	orb_ = CORBA::ORB::_duplicate(om.orb());
}

void NameServer::use_orb(CORBA::ORB_ptr orb)
{
	orb_ = CORBA::ORB::_duplicate(orb);
}

void NameServer::init_ns_ref()
{
	if (CORBA::is_nil(ns_.in())) {
		verify_orb();
		ACE_LOG_MSG->set_flags(ACE_Log_Msg::SILENT); // turn off error messages
		CORBA::Object_ptr ref = orb_->resolve_initial_references("NameService");
		ns_ = CosNaming::NamingContext::_narrow(ref);
	}
}

NameServer::~NameServer() noexcept
{
	try {
		// if tounbind_ is not empty, then ns_ is (or was...) valid
		for (const auto& name : tounbind_)
			unbind(name);
	} catch (...) {
	}
}

void NameServer::verify_orb()
{
	if (CORBA::is_nil(orb_))
		throw std::runtime_error{"NameServer: ORB not initiated"};
}

CORBA::Object_ptr NameServer::raw_resolve(const string& name)
{
	init_ns_ref();
	// does not create naming contexts from name...
	return ns_->resolve(StringToName(name));
}

void NameServer::bind(const string& name, CORBA::Object_ptr ref,
	bool auto_unbind)
{
	init_ns_ref();
	CosNaming::Name n = StringToName(name);
	verify_nctxs(n);	// Create intermediary naming contexts if needed
	ns_->bind(n,ref);			// throw if already bound
	if (auto_unbind)
		tounbind_.push_back(name);
}

void NameServer::rebind(const string& name, CORBA::Object_ptr ref,
	bool auto_unbind)
{
	init_ns_ref();
	CosNaming::Name n = StringToName(name);
	verify_nctxs(n);	// Create intermediary naming contexts if needed
	ns_->rebind(n,ref);
	if (auto_unbind)
		tounbind_.push_back(name);
}

void NameServer::unbind(const string& name)
{
	init_ns_ref();
	ns_->unbind(StringToName(name));
}

void NameServer::verify_nctxs(const CosNaming::Name& n)
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
