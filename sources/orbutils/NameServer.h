//
// NameServer wrapper
//
// (C) 2018-20 LAPLJ. All rights reserved.
//
// NameServer needs an existing (EXTERNAL) ORB to run upon.
// Global NS are possible, but they require late ORB initialization.
//
// The name can be in the form of "/c1/c2/name" (slash-separated).
//

#ifndef NAMESERVER_H
#define NAMESERVER_H

#include <memory>
#include <string>
#include <vector>
#include "ORBManager.h"
#include <orbsvcs/CosNamingC.h>

namespace colibry {

	class NameServer {
	public:

		NameServer() = default;				// delayed orb initialization
		NameServer(ORBManager& om);			// om must have been initiated
		NameServer(CORBA::ORB_ptr orb);		// orb must have been initiated
		NameServer(const NameServer&) = delete;
		NameServer(NameServer&&) = default;
		NameServer& operator=(const NameServer&) = delete;
		NameServer& operator=(NameServer&&) = delete;
		virtual ~NameServer() noexcept;

		void use_orb(ORBManager& om);		// om must have been initiated
		void use_orb(CORBA::ORB_ptr orb);	// orb must be initiated

		template<typename T>
		T* resolve(const std::string& name) {
			return T::_narrow(raw_resolve(name));
		}

		// Bind
		virtual void bind(const std::string& name, CORBA::Object_ptr ref,
			bool auto_unbind=false);
		virtual void rebind(const std::string& name, CORBA::Object_ptr ref,
			bool auto_unbind=false);

		// Unbind
		virtual void unbind(const std::string& name);

	protected:

		CORBA::ORB_var orb_ = CORBA::ORB::_nil();
		CosNaming::NamingContext_var ns_ = CosNaming::NamingContext::_nil();
		std::vector<std::string> tounbind_;	// names to automatically unbind

		void verify_orb();
		void init_ns_ref();
		void verify_nctxs(const CosNaming::Name& n);
		CORBA::Object_ptr raw_resolve(const std::string& name);
	};

};

#endif
