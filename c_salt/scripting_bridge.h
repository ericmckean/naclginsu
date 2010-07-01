// Copyright 2010 The Ginsu Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#ifndef C_SALT_SCRIPTING_BRIDGE_H_
#define C_SALT_SCRIPTING_BRIDGE_H_

#include <map>

#include <nacl/nacl_npapi.h>
#include <nacl/npruntime.h>

#include "c_salt/basic_macros.h"

namespace c_salt {

class BrowserBinding;
class MethodCallbackExecutor;
class Module;
class PropertyAccessorCallbackExecutor;
class PropertyMutatorCallbackExecutor;

// This class handles all the calls across the bridge to the browser via its
// browser binding object, |browser_binding_|.  Note that NPObjects cannot have
// a vtable, hence the PIMPL pattern used here.  Use AddMethodNamed() and
// AddPropertyNamed() to publish methods and properties that can be accessed
// from the browser code.

// TODO(dspringer): |browser_binding_| gets replaced by pp::ScriptableObject
// when Pepper v2 becomes available.

class ScriptingBridge {
 public:
  // Creates an instance of the scripting bridge object in the browser, with
  // a corresponding ScriptingBridge object instance.
  static ScriptingBridge* CreateScriptingBridge(NPP npp);

  // Causes |method_name| to be published as a method that can be called by
  // JavaScript.  Associated this method with |method|.
  bool AddMethodNamed(const char* method_name, MethodCallbackExecutor* method);

  // Associate property accessor and mutator with |property_name|.  This
  // publishes |property_name| to the JavaScript.  |property_accessor| must not
  // be NULL; if |property_mutator| is NULL the property is considered read-only.
  bool AddPropertyNamed(const char* property_name,
                        PropertyAccessorCallbackExecutor* property_accessor,
                        PropertyMutatorCallbackExecutor* property_mutator);

  // Make a copy of the browser binding object by asking the browser to retain
  // it.  Use this for the return value of functions that expect the retain
  // count to increment, such as NPP_GetScriptableInstance().
  NPObject* CopyBrowserBinding() {
    if (browser_binding_)
      return NPN_RetainObject(browser_binding_);
    return NULL;
  }

  // Release the browser binding object.  Note that this *might* cause |this|
  // to get deleted, if the ref count of the browser binding object falls to 0.
  void ReleaseBrowserBinding();

  // Accessors.
  const NPP npp() const {
    return npp_;
  }
  const NPObject* browser_binding() const {
    return browser_binding_;
  }

  // A hidden class that wraps the NPObject, preserving its memory layout
  // for the browser.
  friend class BrowserBinding;

 private:
  typedef std::map<NPIdentifier, MethodCallbackExecutor*> MethodDictionary;
  typedef std::map<NPIdentifier, PropertyAccessorCallbackExecutor*>
      PropertyAccessorDictionary;
  typedef std::map<NPIdentifier, PropertyMutatorCallbackExecutor*>
      PropertyMutatorDictionary;

  ScriptingBridge(NPP npp, NPObject* browser_binding);
  virtual ~ScriptingBridge();

  // NPAPI support methods.
  bool HasMethod(NPIdentifier name);
  bool Invoke(NPIdentifier name,
              const NPVariant* args,
              uint32_t arg_count,
              NPVariant* return_value);
  bool HasProperty(NPIdentifier name);
  bool GetProperty(NPIdentifier name, NPVariant* return_value);
  bool SetProperty(NPIdentifier name, const NPVariant* return_value);

  NPP npp_;
  NPObject* browser_binding_;

  MethodDictionary method_dictionary_;
  PropertyAccessorDictionary property_accessor_dictionary_;
  PropertyMutatorDictionary property_mutator_dictionary_;

  DISALLOW_IMPLICIT_CONSTRUCTORS(ScriptingBridge);
};

}  // namespace c_salt

#endif  // C_SALT_SCRIPTING_BRIDGE_H_
