// Copyright 2010 The Native Client Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#ifndef C_SALT_NPAPI_BROWSER_BINDING_H_
#define C_SALT_NPAPI_BROWSER_BINDING_H_

#include <nacl/nacl_npapi.h>
#include <nacl/npruntime.h>

#include "boost/scoped_ptr.hpp"
#include "c_salt/scripting_bridge.h"

namespace c_salt {
namespace npapi {

class Instance;

// A thin wrapper that owns the ScriptingBridge class.  This is necessary
// because the NPObject layout has to be preserved, and it cannot have things
// like a vtable inserted into it.
class BrowserBinding : public NPObject {
 public:
  explicit BrowserBinding(const NPP& npp)
      : npp_(npp), scripting_bridge_(new ScriptingBridge(this)) {}
  // The dtor *cannot* be virtual because this object must preserve NPObject's
  // POD memory layout.
  ~BrowserBinding() {}

  // Factory method to create a browser binding.  This asks the browser to
  // create the proxy object via NPN_CreateObject() that represents this
  // instance.  Calling this causes the browser to call the NPAPI Allocate()
  // function, which then calls the ctor for this class.
  // This is a synchronous call to the browser.  Memory has been allocated
  // and ctors called by the time it returns.
  static BrowserBinding* CreateBrowserBinding(const c_salt::Instance& instance);

  // Bump the retain count of the proxy object in the browser.
  void Retain();

  const NPP& npp() const {
    return npp_;
  }

  ScriptingBridge* scripting_bridge() {
    return scripting_bridge_.get();
  }

 private:
  // A small helper class that converts an NPIdentifier into a std::string.
  // It follows the RAII pattern: when the class goes out of scope, the memory
  // used to get the string is freed with NPN_MemFree().
  class ScopedNPIdToStringConverter {
   public:
    explicit ScopedNPIdToStringConverter(const NPIdentifier& np_id)
        : np_string_(NPN_UTF8FromIdentifier(np_id)) {
      string_value_ = static_cast<const char*>(np_string_);
    }
    ~ScopedNPIdToStringConverter() {
      NPN_MemFree(np_string_);
    }
    const std::string& string_value() const {
      return string_value_;
    }
   private:
    NPUTF8* np_string_;
    std::string string_value_;
  };

  // NPAPI support methods; the browser calls these on scriptable objects.
  bool HasMethod(NPIdentifier name) const;
  void Invalidate();
  bool Invoke(NPIdentifier name,
              const NPVariant* args,
              uint32_t arg_count,
              NPVariant* return_value);
  bool HasProperty(NPIdentifier name) const;
  bool GetProperty(NPIdentifier name, NPVariant* return_value) const;
  bool SetProperty(NPIdentifier name, const NPVariant& np_value);
  bool RemoveProperty(NPIdentifier name);

  // These are the free functions that the browser actually calls.  They are
  // all simple wrappers to the above NPAPI support methods.
  friend void Invalidate(NPObject* object);
  friend bool Invoke(NPObject* object, NPIdentifier name,
                     const NPVariant* args,
                     uint32_t arg_count,
                     NPVariant* return_value);
  friend bool HasMethod(NPObject* object, NPIdentifier name);
  friend bool HasProperty(NPObject* object, NPIdentifier name);
  friend bool GetProperty(NPObject* object,
                          NPIdentifier name,
                          NPVariant* result);
  friend bool SetProperty(NPObject* object,
                          NPIdentifier name,
                          const NPVariant* value);
  friend bool RemoveProperty(NPObject* object, NPIdentifier name);

  NPP npp_;
  boost::scoped_ptr<ScriptingBridge> scripting_bridge_;
};

}  // namespace npapi
}  // namespace c_salt

#endif  // C_SALT_NPAPI_BROWSER_BINDING_H_
