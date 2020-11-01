//
// NameServer wrapper
//
// (C) 2018 LAPLJ. All rights reserved.
//
// NameServer is a singleton.
// NameServer uses its own static ORB so that to be independent of some other orbs
// it could use.
//

#ifndef __NAMESERVER_H__
#define __NAMESERVER_H__

#include <memory>
#include <string>
#include "ORBManager.h"
#include <orbsvcs/CosNamingC.h>

namespace colibry {

    class NameServer {
    protected:

        static CORBA::ORB_var nsorb_;
        static bool ownorb_;           // do I own the orb?
        static std::unique_ptr<NameServer> instance_;

        CosNaming::NamingContext_var ns_;

    protected:

        NameServer();
        virtual void verifyNamingContexts(const CosNaming::Name& n);

    public:

    	 // create new ORB, if none provided
    	static NameServer* Instance(int argc, char* argv[]); // create own ORB
        static NameServer* Instance(CORBA::ORB_ptr orb = CORBA::ORB::_nil());
        static NameServer* Instance(ORBManager& om) { return Instance(om.orb()); }
        virtual ~NameServer();

        // Resolve
        virtual CORBA::Object_ptr resolve(const std::string& name);
        template<typename T> T* resolven(const std::string& name)
        { return T::_narrow(resolve(name)); }

        // Bind
        virtual void bind(const std::string& name, CORBA::Object_ptr ref);
        virtual void rebind(const std::string& name, CORBA::Object_ptr ref);

        // Unbind
        virtual void unbind(const std::string& name);
    };

};

#endif
