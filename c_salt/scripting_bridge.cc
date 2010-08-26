// Copyright 2010 The Ginsu Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#include "c_salt/scripting_bridge.h"

#include <assert.h>
#include <boost/scoped_ptr.hpp>
#include <string.h>
#include <string>

#include "c_salt/callback.h"
#include "c_salt/instance.h"
#include "c_salt/type.h"

namespace c_salt {

// A thin wrapper that owns the ScriptingBridge class.  This is necessary
// because the NPObject layout has to be preserved, and it cannot have things
// like a vtable inserted into it.
class BrowserBinding : public NPObject {
 public:
  explicit BrowserBinding(NPP npp)
      : scripting_bridge_(new ScriptingBridge(npp, this)) {}
  ~BrowserBinding() {}

  bool HasMethod(NPIdentifier name) {
    return scripting_bridge_->HasMethod(name);
  }
  bool Invoke(NPIdentifier name,
              const NPVariant* args,
              uint32_t arg_count,
              NPVariant* return_value) {
    return scripting_bridge_->Invoke(name, args, arg_count, return_value);
  }
  bool HasProperty(NPIdentifier name) {
    return scripting_bridge_->HasProperty(name);
  }
  bool GetProperty(NPIdentifier name, NPVariant* return_value) {
    return scripting_bridge_->GetProperty(name, return_value);
  }
  bool SetProperty(NPIdentifier name, const NPVariant* return_value) {
    return scripting_bridge_->SetProperty(name, return_value);
  }

  ScriptingBridge* scripting_bridge() {
    return scripting_bridge_.get();
  }

 private:
  boost::scoped_ptr<ScriptingBridge> scripting_bridge_;
};

// Helper function for dereferencing the bridging object.
static inline BrowserBinding* cast_browser_binding(NPObject* np_object) {
  assert(np_object);
  return static_cast<BrowserBinding*>(np_object);
}
// The browser-facing entry points that represent the bridge's class methods.
// These are the function wrappers that the browser calls.

NPObject* Allocate(NPP npp, NPClass* npclass) {
  return new BrowserBinding(npp);
}

void Deallocate(NPObject* object) {
  delete cast_browser_binding(object);
}

void Invalidate(NPObject* object) {
}

bool HasMethod(NPObject* object, NPIdentifier name) {
  return cast_browser_binding(object)->HasMethod(name);
}

bool Invoke(NPObject* object, NPIdentifier name,
            const NPVariant* args,
            uint32_t arg_count,
            NPVariant* return_value) {
  return cast_browser_binding(object)->Invoke(
      name, args, arg_count, return_value);
}

bool InvokeDefault(NPObject* object,
                   const NPVariant* args,
                   uint32_t arg_count,
                   NPVariant* return_value) {
  return false;
}

bool HasProperty(NPObject* object, NPIdentifier name) {
  return cast_browser_binding(object)->HasProperty(name);
}

bool GetProperty(NPObject* object, NPIdentifier name, NPVariant* result) {
  return cast_browser_binding(object)->GetProperty(name, result);
}

bool SetProperty(NPObject* object, NPIdentifier name, const NPVariant* value) {
  return cast_browser_binding(object)->SetProperty(name, value);
}

bool RemoveProperty(NPObject* object, NPIdentifier name) {
  return false;
}

static NPClass bridge_class = {
  NP_CLASS_STRUCT_VERSION,
  c_salt::Allocate,
  c_salt::Deallocate,
  c_salt::Invalidate,
  c_salt::HasMethod,
  c_salt::Invoke,
  c_salt::InvokeDefault,
  c_salt::HasProperty,
  c_salt::GetProperty,
  c_salt::SetProperty,
  c_salt::RemoveProperty
};

ScriptingBridge* ScriptingBridge::CreateScriptingBridge(NPP npp) {
  // This is a synchronous call to the browser.  Memory has been allocated
  // and ctors called by the time it returns.
  BrowserBinding* browser_binding =
       static_cast<BrowserBinding*>(NPN_CreateObject(npp, &bridge_class));
  if (browser_binding)
    return browser_binding->scripting_bridge();
  return NULL;
}

ScriptingBridge::ScriptingBridge(NPP npp, NPObject* browser_binding)
    : npp_(npp),
      browser_binding_(browser_binding),
      window_object_(NULL) {
}

ScriptingBridge::~ScriptingBridge() {
  ReleaseBrowserBinding();
  if (window_object_) {
    NPN_ReleaseObject(window_object_);
    window_object_ = NULL;
  }
}

void ScriptingBridge::ReleaseBrowserBinding() {
  if (browser_binding_) {
    bool will_dealloc = browser_binding_->referenceCount == 1;
    NPObject* tmp_binding = browser_binding_;
    browser_binding_ = NULL;
    NPN_ReleaseObject(tmp_binding);
    // |this| might be deallocated at this point.  Further calls might have
    // unpredictable results.
    if (!will_dealloc) {
      browser_binding_ = tmp_binding;
    }
  }
  // |this| might be deallocated at this point.  Further calls might have
  // unpredictable results.
}

bool ScriptingBridge::AddPropertyNamed(
    const char* property_name,
    SharedPropertyAccessorCallbackExecutor property_accessor,
    SharedPropertyMutatorCallbackExecutor property_mutator) {
  if (property_name == NULL || property_accessor == NULL)
    return false;
  NPIdentifier property_id = NPN_GetStringIdentifier(property_name);
  property_accessor_dictionary_.insert(
      std::pair<NPIdentifier,
      SharedPropertyAccessorCallbackExecutor>(property_id, property_accessor));
  if (property_mutator) {
    property_mutator_dictionary_.insert(
        std::pair<NPIdentifier,
        SharedPropertyMutatorCallbackExecutor>(property_id, property_mutator));
  }
  return true;
}

bool ScriptingBridge::LogToConsole(const std::string& msg) const {
  bool success = false;
  NPObject* window = window_object();
  if (window) {
    static const char* kConsoleAccessor = "top.console";
    NPString console_stript = { 0 };
    console_stript.UTF8Length = strlen(kConsoleAccessor);
    console_stript.UTF8Characters = kConsoleAccessor;
    NPVariant console;
    if (NPN_Evaluate(npp(), window, &console_stript, &console)) {
      if (NPVARIANT_IS_OBJECT(console)) {
        // Convert the message to NPString;
        NPVariant text;
        STRINGN_TO_NPVARIANT(msg.c_str(), msg.size(), text);
        NPVariant result;
        if (NPN_Invoke(npp(), NPVARIANT_TO_OBJECT(console),
                       NPN_GetStringIdentifier("log"), &text, 1, &result)) {
          NPN_ReleaseVariantValue(&result);
          success = true;
        }
      }
      NPN_ReleaseVariantValue(&console);
    }
  }
  return success;
}

NPObject* ScriptingBridge::window_object() const {
  if (!window_object_) {
    NPN_GetValue(npp(), NPNVWindowNPObject,  &window_object_);
  }
  return window_object_;
}

bool ScriptingBridge::HasMethod(NPIdentifier name) {
  MethodDictionary::const_iterator i;
  i = method_dictionary_.find(name);
  return i != method_dictionary_.end();
}

bool ScriptingBridge::HasProperty(NPIdentifier name) {
  // Only look for the "get" property - there is never a "set" without a "get",
  // but there can be "get" without "set" (read-only).
  PropertyAccessorDictionary::const_iterator i;
  i = property_accessor_dictionary_.find(name);
  return i != property_accessor_dictionary_.end();
}

bool ScriptingBridge::GetProperty(NPIdentifier name,
                                  NPVariant *return_value) {
  VOID_TO_NPVARIANT(*return_value);

  PropertyAccessorDictionary::iterator i;
  i = property_accessor_dictionary_.find(name);
  if (i != property_accessor_dictionary_.end()) {
    return (*i->second).Execute(this, return_value);
  }
  return true;
}

bool ScriptingBridge::SetProperty(NPIdentifier name, const NPVariant* value) {
  PropertyMutatorDictionary::iterator i;
  i = property_mutator_dictionary_.find(name);
  if (i != property_mutator_dictionary_.end()) {
    return (*i->second).Execute(this, value);
  }
  return true;
}

bool ScriptingBridge::Invoke(NPIdentifier name,
                             const NPVariant* args,
                             uint32_t arg_count,
                             NPVariant* return_value) {
  MethodDictionary::iterator i;
  i = method_dictionary_.find(name);
  if (i != method_dictionary_.end()) {
    return (*i->second).Execute(this, args, arg_count, return_value);
  }
  return false;
}

}  // namespace c_salt
