// Copyright 2010 The Ginsu Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#include "c_salt/scripting_bridge.h"

#include <cassert>
#include <cstring>
#include <string>

#include "boost/scoped_ptr.hpp"
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
      : npp_(npp), scripting_bridge_(new ScriptingBridge(this)) {}
  ~BrowserBinding() {}

  bool HasMethod(NPIdentifier name) {
    return scripting_bridge_->HasMethod(name);
  }
  void Invalidate() {
    return scripting_bridge_->Invalidate();
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
  bool SetProperty(NPIdentifier name, const NPVariant& value) {
    return scripting_bridge_->SetProperty(name, value);
  }
  bool RemoveProperty(NPIdentifier name) {
    return scripting_bridge_->RemoveProperty(name);
  }

  const NPP npp() const {
    return npp_;
  }

  ScriptingBridge* scripting_bridge() {
    return scripting_bridge_.get();
  }

 private:
  NPP npp_;
  boost::scoped_ptr<ScriptingBridge> scripting_bridge_;
};

// Helper functions for dereferencing the bridging object.
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
  cast_browser_binding(object)->Invalidate();
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
  return cast_browser_binding(object)->SetProperty(name, *value);
}

bool RemoveProperty(NPObject* object, NPIdentifier name) {
  return cast_browser_binding(object)->RemoveProperty(name);
}

static NPClass bridge_class = {
  NP_CLASS_STRUCT_VERSION,
  c_salt::Allocate,
  c_salt::Deallocate,
  c_salt::Invalidate,
  c_salt::HasMethod,
  c_salt::Invoke,
  NULL,  // InvokeDefault is not implemented
  c_salt::HasProperty,
  c_salt::GetProperty,
  c_salt::SetProperty,
  c_salt::RemoveProperty
};

ScriptingBridge* ScriptingBridge::CreateScriptingBridgeWithInstance(
    const Instance* instance) {
  // This is a synchronous call to the browser.  Memory has been allocated
  // and ctors called by the time it returns.
  BrowserBinding* browser_binding = static_cast<BrowserBinding*>(
      NPN_CreateObject(instance->npp_instance(), &bridge_class));
  if (browser_binding)
    return browser_binding->scripting_bridge();
  return NULL;
}

ScriptingBridge::ScriptingBridge(NPObject* browser_binding)
    : browser_binding_(browser_binding),
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
    if (NPN_Evaluate(GetBrowserInstance(),
                     window,
                     &console_stript,
                     &console)) {
      if (NPVARIANT_IS_OBJECT(console)) {
        // Convert the message to NPString;
        NPVariant text;
        STRINGN_TO_NPVARIANT(msg.c_str(), msg.size(), text);
        NPVariant result;
        if (NPN_Invoke(GetBrowserInstance(),
                       NPVARIANT_TO_OBJECT(console),
                       NPN_GetStringIdentifier("log"),
                       &text,
                       1,
                       &result)) {
          NPN_ReleaseVariantValue(&result);
          success = true;
        }
      }
      NPN_ReleaseVariantValue(&console);
    }
  }
  return success;
}

const NPP ScriptingBridge::GetBrowserInstance() const {
  return static_cast<const BrowserBinding*>(browser_binding_)->npp();
}

NPObject* ScriptingBridge::window_object() const {
  if (!window_object_) {
    NPN_GetValue(GetBrowserInstance(),
                 NPNVWindowNPObject,
                 &window_object_);
  }
  return window_object_;
}

void ScriptingBridge::Invalidate() {
  // This is called from the browser after NPP_Delete, on all objects with
  // dangling references.
  method_dictionary_.clear();
  property_accessor_dictionary_.clear();
  property_mutator_dictionary_.clear();
  dynamic_property_dictionary_.clear();
}

bool ScriptingBridge::HasMethod(NPIdentifier name) {
  MethodDictionary::const_iterator i;
  i = method_dictionary_.find(name);
  return i != method_dictionary_.end();
}

bool ScriptingBridge::HasProperty(NPIdentifier name) {
  // If |name| is not a property, then it *could* be a method.  Consider this
  // JavaScript:
  //   module.method();
  // In NPAPI, the browser first calls HasProperty("method"), and if it gets a
  // |false| return it then calls HasMethod("method").  That means this
  // function should return |false| _only_ if |name| is a valid method name.
  // In all other cases, it returns |true|.  The |true| return will
  // cause the browser to attempt either a GetProperty() or a SetProperty().
  // If a Get is attempted on a non-existent property, then GetProperty()
  // method will return |false| and the JavaScript will (correctly) get an
  // undefined object.  Alternatively, if the browser attempts a Set then
  // SetProperty will update an existing property or insert it as a new dynamic
  // if it doesn't exist.
  bool has_method = HasMethod(name);
  // Writing this out long-hand so hopefully it's more understandable.
  return has_method ? false : true;
}

bool ScriptingBridge::GetProperty(NPIdentifier name,
                                  NPVariant *return_value) {
  VOID_TO_NPVARIANT(*return_value);
  PropertyAccessorDictionary::const_iterator get_iter =
      property_accessor_dictionary_.find(name);
  if (get_iter != property_accessor_dictionary_.end()) {
    return (*get_iter->second).Execute(this, return_value);
  }
  // See if the property is in the list of dynamic properties.
  DynamicPropertyDictionary::const_iterator dyn_iter =
      dynamic_property_dictionary_.find(name);
  if (dyn_iter != dynamic_property_dictionary_.end()) {
    // TODO(dspringer,dmichael): implement the observer pattern here.
    // delegate_->WillGetPropertyNamed(name);
    dyn_iter->second->ConvertToNPVariant(return_value);
    // delegate_->DidGetPropertyNamed(name);
    return true;
  }
  // |name| does not exist as a property.
  return false;
}

bool ScriptingBridge::SetProperty(NPIdentifier name, const NPVariant& value) {
  PropertyMutatorDictionary::iterator set_iter =
      property_mutator_dictionary_.find(name);
  if (set_iter != property_mutator_dictionary_.end()) {
    // SetProperty() can only return |false| if this callback returns |false|.
    return (*set_iter->second).Execute(this, value);
  }
  // |name| is not a static property: insert if absent, update if present.
  // TODO(dspringer,dmichael): implement the observer pattern here.
  // delegate_->WillSetPropertyNamed(name);
  dynamic_property_dictionary_[name] = Type::CreateFromNPVariant(value);
  // delegate_->DidSetPropertyNamed(name);
  // Setting a dynamic property always succeeds.
  return true;
}

bool ScriptingBridge::RemoveProperty(NPIdentifier name) {
  // Remove a property only if it has been dynamically added.  Pre-declared
  // properties cannot be removed.
  PropertyAccessorDictionary::iterator dyn_iter;
  dyn_iter = property_accessor_dictionary_.find(name);
  if (dyn_iter == property_accessor_dictionary_.end()) {
    return false;  // This is a static property, or |name| doesn't exist.
  }
  // TODO(dspringer,dmichael): implement the observer pattern here.
  // delegate_->WillRemovePropertyNamed(name);
  property_accessor_dictionary_.erase(dyn_iter);
  // delegate_->DidRemovePropertyNamed(name);
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
