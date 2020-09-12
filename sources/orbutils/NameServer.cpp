#include "NameServer.h"
#include <iostream>
#include <string>
#include <vector>

using namespace std;
using namespace colibry;
using namespace CosNaming;

// helper

Name StringToName(const string& name)
{
	vector<string> vs;

	// parse for '/' separators
	string::size_type inf = 0;
	string::size_type sup = name.find("/");
	while (sup != string::npos) {
		vs.push_back(name.substr(inf,sup-inf));
		inf = sup+1;
		sup = name.find("/",inf);
	}
	vs.push_back(name.substr(inf));

	Name nm(vs.size());
	nm.length(vs.size());
	int i{0};
	for (string& ctxt : vs)
		nm[i++].id = CORBA::string_dup(ctxt.c_str());

	return nm;
}

//
// static members
//

CORBA::ORB_var NameServer::s_nsorb = CORBA::ORB::_nil();
bool NameServer::s_ownorb = true;
unique_ptr<NameServer> NameServer::s_instance{nullptr};

//
// method implementations
//

NameServer* NameServer::Instance(CORBA::ORB_ptr orb)
{
    if (s_instance.get() == nullptr) {
		// instance does not exist
		if (CORBA::is_nil(s_nsorb)) {
            if (CORBA::is_nil(orb)) {
                // init s_nsorb
                int fake_argc = 1;
                char prog[] = "NameServer";
                char* fake_argv[] = { prog, nullptr };
                s_nsorb = CORBA::ORB_init (fake_argc,fake_argv,"NS-ORB");
            } else {
                s_nsorb = CORBA::ORB::_duplicate(orb);
                s_ownorb = false;
            }
        }
		s_instance = unique_ptr<NameServer>(new NameServer);
    }
    return s_instance.get();
}

NameServer::NameServer()
{
    ACE_LOG_MSG->set_flags(ACE_Log_Msg::SILENT); // turn off error messages
    m_ns = NamingContext::_nil();
    init();
}

void NameServer::init()
{
    CORBA::Object_ptr ref = s_nsorb->resolve_initial_references("NameService");
    m_ns = NamingContext::_narrow(ref);
}

NameServer::~NameServer()
{
    if (s_ownorb && !is_nil(s_nsorb.in())) {
		s_nsorb->destroy();
		s_nsorb = CORBA::ORB::_nil();
    }
}

CORBA::Object_ptr NameServer::resolve(const string& name)
{
    // does not create naming contexts from name...
    return m_ns->resolve(StringToName(name));
}

void NameServer::bind(const string& name, CORBA::Object_ptr ref)
{
    Name n = StringToName(name);
    // Create intermediary naming contexts if needed
    verifyNamingContexts(n);
    m_ns->bind(n,ref);
}

void NameServer::rebind(const string& name, CORBA::Object_ptr ref)
{
    Name n = StringToName(name);
    // Create intermediary naming contexts if needed
    verifyNamingContexts(n);
    m_ns->rebind(n,ref);
}

void NameServer::unbind(const string& name)
{
    m_ns->unbind(StringToName(name));
}

void NameServer::verifyNamingContexts(const Name& n)
{
    // create all naming contexts if necessary
    NamingContext_var nc = NamingContext::_duplicate(m_ns);
    for (unsigned int i=0; i<n.length()-1; i++) {   //  (last in n is not a nc)
		try {
		    nc = nc->bind_new_context(StringToName(n[i].id.in()));
		} catch (CosNaming::NamingContext::AlreadyBound& ) {
		    nc = NamingContext::_narrow(nc->resolve(StringToName(n[i].id.in())));
		}
    }
}
