//
// NameServer wrapper
//
// (C) 2018-20 LAPLJ. All rights reserved.
//
// NameServer needs an ORB to run upon. A new ORB will only be created if
// argc + argv are provided. Otherwise, an existing provided ORB is used.
//
// The name can be in the form of "/nctxt1/nctxt2/name" (dash-separated).
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

		CORBA::ORB_var orb_;				// my orb
		CosNaming::NamingContext_var ns_;	// reference to NS

		static NameServer* global_;			// global NS

	protected:

		virtual void verifyNamingContexts(const CosNaming::Name& n);
		void init_ns_ref();

	public:

		static NameServer* global();			// first created NS

		NameServer();							// tries to use global ORBManager
		NameServer(int argc, char* argv[]);		// creates own ORBManager
		NameServer(ORBManager& om);				// uses this om
		NameServer(CORBA::ORB_ptr orb);			// uses this orb
		virtual ~NameServer();

		// Resolve
		CORBA::Object_ptr resolve(const std::string& name);

		template<typename T>
		T* resolve(const std::string& name) { return T::_narrow(resolve(name)); }

		// Bind
		virtual void bind(const std::string& name, CORBA::Object_ptr ref);
		virtual void rebind(const std::string& name, CORBA::Object_ptr ref);

		// Unbind
		virtual void unbind(const std::string& name);
	};

};

#endif
