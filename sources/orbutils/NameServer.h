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
#include <orbsvcs/CosNamingC.h>

namespace colibry {

    class NameServer {
    protected:

        static CORBA::ORB_var s_nsorb;
        static bool s_ownorb;           // do I own the orb?
        static std::unique_ptr<NameServer> s_instance;
        CosNaming::NamingContext_var m_ns;

    protected:

        NameServer();
        virtual void init();
        virtual void verifyNamingContexts(const CosNaming::Name& n);

    public:

        static NameServer* Instance(CORBA::ORB_ptr orb = CORBA::ORB::_nil()); // create new ORB, if none provided
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
