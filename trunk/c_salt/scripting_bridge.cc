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
#include "c_salt/npapi/browser_binding.h"
#include "c_salt/type.h"

using c_salt::npapi::BrowserBinding;

namespace c_salt {

ScriptingBridge* ScriptingBridge::CreateScriptingBridgeWithInstance(
    const Instance& instance) {
  BrowserBinding* browser_binding =
      BrowserBinding::CreateBrowserBinding(instance);
  return browser_binding ? browser_binding->scripting_bridge() : NULL;
}

ScriptingBridge::ScriptingBridge(BrowserBinding* browser_binding)
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

NPObject* ScriptingBridge::CopyBrowserBinding() {
  if (browser_binding_) browser_binding_->Retain();
  return browser_binding_;
}

void ScriptingBridge::ReleaseBrowserBinding() {
  if (browser_binding_) {
    bool will_dealloc = browser_binding_->referenceCount == 1;
    BrowserBinding* tmp_binding = browser_binding_;
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

bool ScriptingBridge::LogToConsole(const std::string& msg) const {
  bool success = false;
  NPObject* window = window_object();
  if (window) {
    static const char* kConsoleAccessor = "top.console";
    NPString console_script = { 0 };
    console_script.UTF8Length = strlen(kConsoleAccessor);
    console_script.UTF8Characters = kConsoleAccessor;
    NPVariant console;
    if (NPN_Evaluate(GetBrowserInstance(),
                     window,
                     &console_script,
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

const NPP& ScriptingBridge::GetBrowserInstance() const {
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
  property_dictionary_.clear();
  // Unhook the weak reference.
  browser_binding_ = NULL;
}

bool ScriptingBridge::HasScriptMethod(const std::string& name) {
  MethodDictionary::const_iterator i;
  i = method_dictionary_.find(name);
  return i != method_dictionary_.end();
}

bool ScriptingBridge::InvokeScriptMethod(const std::string& method_name,
                                         const NPVariant* args,
                                         uint32_t arg_count,
                                         NPVariant* return_value) {
  MethodDictionary::iterator i;
  i = method_dictionary_.find(method_name);
  if (i != method_dictionary_.end()) {
    return (*i->second).Execute(args, arg_count, return_value);
  }
  return false;
}

bool ScriptingBridge::AddProperty(const Property& property) {
  if (property.name().empty())
    return false;
  property_dictionary_.insert(PropertyDictionary::value_type(property.name(),
                                                             property));
  return true;
}

bool ScriptingBridge::GetValueForPropertyNamed(const std::string& name,
                                               Type** value) const {
  assert(value);
  if (!value)
    return false;
  SharedType shared_value(new Type(Type::kNullTypeId));
  bool success = GetScriptProperty(name, shared_value);
  // This is a little contorted because Type is a non-copyable class.
  // Copying ObjectTypes is not straight-forward due to the ref counting in
  // the browser.
  *value = Type::CreateFromTypeWithTypeId(*(shared_value.get()),
                                          shared_value->type_id());
  return success;
}

bool ScriptingBridge::SetValueForPropertyNamed(const std::string& name,
                                               const Type& value) {
  // Note that there is no copy constructor for a Type, so this method is how
  // to create a copy of a Type.
  SharedType shared_value(Type::CreateFromTypeWithTypeId(value,
                                                         value.type_id()));
  return SetScriptProperty(name, shared_value);
}

bool ScriptingBridge::HasScriptProperty(const std::string& name) {
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
  bool has_method = HasScriptMethod(name);
  // Writing this out long-hand so hopefully it's more understandable.
  return has_method ? false : true;
}

bool ScriptingBridge::GetScriptProperty(const std::string& name,
                                        SharedType return_value) const {
  assert(return_value.get());
  if (!return_value.get())
    return false;
  if (!name.empty()) {
    PropertyDictionary::const_iterator iter = property_dictionary_.find(name);
    if (iter != property_dictionary_.end()) {
      return_value = iter->second.GetValue();
      return true;
    }
  }
  return_value.reset(new Type(Type::kNullTypeId));
  return false;
}

bool ScriptingBridge::SetScriptProperty(const std::string& name,
                                        const SharedType& value) {
  if (name.empty())
    return false;
  PropertyDictionary::iterator iter = property_dictionary_.find(name);
  if (iter != property_dictionary_.end()) {
    if (iter->second.is_mutable()) {
      if (iter->second.is_static()) {
        // If it is static, force the type to stick.
        SharedType retype_value(Type::CreateFromTypeWithTypeId(*(value.get()),
            iter->second.GetValue()->type_id()));
        iter->second.SetValue(retype_value);
      } else {
        // It is dynamic, set the type to whatever is passed in.
        iter->second.SetValue(value);
      }
      // It is mutable, and we've successfully set it now.
      return true;
    }
    // It was not mutable, so disallow assignment.
    return false;
  }
  // No property was found.  Create one and add it.  Note that properties
  // created by the browser are always dynamic and mutable.
  PropertyAttributes prop_attrs(name, value);
  prop_attrs.set_dynamic().set_mutable();
  property_dictionary_.insert(
      PropertyDictionary::value_type(name, Property(prop_attrs)));
  return true;
}

bool ScriptingBridge::RemoveScriptProperty(const std::string& name) {
  // If the property exists and is dynamic, delete it.
  PropertyDictionary::iterator iter = property_dictionary_.find(name);
  if (iter != property_dictionary_.end()) {
    if (!iter->second.is_static()) {
      property_dictionary_.erase(iter);
      return true;
    }
  }
  return false;
}

}  // namespace c_salt
