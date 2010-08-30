// Copyright 2010 The Ginsu Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#ifndef C_SALT_SCRIPTING_BRIDGE_H_
#define C_SALT_SCRIPTING_BRIDGE_H_

#include <nacl/nacl_npapi.h>
#include <nacl/npruntime.h>

#include <map>
#include <string>

#include "boost/noncopyable.hpp"
#include "boost/shared_ptr.hpp"
#include "c_salt/callback.h"

namespace c_salt {

class BrowserBinding;
class MethodCallbackExecutor;
class Module;
class PropertyAccessorCallbackExecutor;
class PropertyMutatorCallbackExecutor;
class Type;

// This class handles all the calls across the bridge to the browser via its
// browser binding object, |browser_binding_|.  Note that NPObjects cannot have
// a vtable, hence the PIMPL pattern used here.  Use AddMethodNamed() and
// AddPropertyNamed() to publish methods and properties that can be accessed
// from the browser code.
//
// This class also maintains a dictionary of dynamically-added properties.
// These properties are added at run-time by JavaScript; they can also be
// dynamically removed when the browser calls RemoveProperty().  Properties
// added via AddPropertyNamed() cannot be dynamically removed.

// TODO(dspringer): |browser_binding_| gets replaced by pp::ScriptableObject
// when Pepper v2 becomes available.

class ScriptingBridge : public boost::noncopyable {
 public:
  // Shared pointer types used in the method and property maps.
  typedef boost::shared_ptr<MethodCallbackExecutor>
      SharedMethodCallbackExecutor;
  typedef boost::shared_ptr<PropertyAccessorCallbackExecutor>
      SharedPropertyAccessorCallbackExecutor;
  typedef boost::shared_ptr<PropertyMutatorCallbackExecutor>
      SharedPropertyMutatorCallbackExecutor;

  // Creates an instance of the scripting bridge object in the browser, with
  // a corresponding ScriptingBridge object instance.
  static ScriptingBridge* CreateScriptingBridge(NPP npp);
  virtual ~ScriptingBridge();

  // Causes |method_name| to be published as a method that can be called by
  // JavaScript.  Associated this method with |method|.
  //
  // Usage:
  //   AddMethodNamed("myFunc",  /* This is the JavaScript name for the method
  //                                you want to expose. */
  //                  &handler,  /* This is a pointer to an instance of the
  //                                handler object. */
  //                  &Handler::MyFunc  /* This is the class method that will
  //                                       get called when "myFunc" is invoked
  //                                       from JavaScript. */
  //                 );
  //
  // Example:
  //   class MyClass {
  //     void InitializeMethods(ScriptingBridge*);
  //     bool MyFunc(int32_t, double, std::string);
  //   };
  //
  //   MyClass::InitializeMethods(ScriptingBridge* bridge) {
  //      bridge->AddMethodNamed("myFunc", this, MyClass::MyFunc);
  //   }
  //
  // Note that you may also accept a ScriptingBridge* as an argument, and
  // c_salt will dutifully pass you the ScriptingBridge with which the method
  // was registered.  Any ScriptingBridge* argument is not part of the
  // interface which is exposed to JavaScript.
  //
  // For example:
  //   class MyClass {
  //     void InitializeMethods(ScriptingBridge*);
  //     bool MyFunc(int32_t, double, std::string, ScriptingBridge*);
  //   };
  //
  //   MyClass::InitializeMethods(ScriptingBridge* bridge) {
  //      bridge->AddMethodNamed("myFunc", this, MyClass::MyFunc);
  //   }
  // In this example, the interface looks identical to the previous example
  // from the perspective of any JavaScript which tries to invoke it.  But in
  // this 2nd example, "bridge" will be passed to MyFunc as the last argument.
  // (We could put the ScriptingBridge* argument anywhere in our argument
  // list, if desired).
  //
  // Caveats:
  //   - Currently, only up to 6 arguments are supported.
  //   - void return types are currently not supported.
  //   - Only double, int32_t, bool, and std::string are supported currently.
  //   - Type safety is the goal in this prototype.  If JavaScript passes a
  //     float and the native code accepts a string, the method will not be
  //     invoked, and a false return will be given to NPAPI.  Future versions
  //     may allow for reasonable automatic type conversions, if that is
  //     desired.
  template <class T, class Signature>
  bool AddMethodNamed(const std::string& method_name,
                      T* handler,
                      Signature method) {
    if (method_name.empty() || method == NULL)
      return false;
    NPIdentifier method_id = NPN_GetStringIdentifier(method_name.c_str());
    SharedMethodCallbackExecutor method_ptr(MakeMethodCallbackExecutor(handler,
                                                                       method));
    method_dictionary_.insert(MethodDictionary::value_type(method_id,
                                                           method_ptr));
    return true;
  }

  // Associate property accessor and mutator with |property_name|.  This
  // publishes |property_name| to the JavaScript.  |property_accessor| must not
  // be NULL; if |property_mutator| is NULL the property is considered
  // read-only.
  bool AddPropertyNamed(const char* property_name,
      SharedPropertyAccessorCallbackExecutor property_accessor,
      SharedPropertyMutatorCallbackExecutor property_mutator);

  // Make a copy of the browser binding object by asking the browser to retain
  // it.  Use this for the return value of functions that expect the retain
  // count to increment, such as NPP_GetScriptableInstance().
  NPObject* CopyBrowserBinding() {
    if (browser_binding_)
      NPN_RetainObject(browser_binding_);
    return browser_binding_;
  }

  // Release the browser binding object.  Note that this *might* cause |this|
  // to get deleted, if the ref count of the browser binding object falls to 0.
  void ReleaseBrowserBinding();

  // Log a message to the browser's console window.  You can usually see this
  // message when using a JavaScript debugger, such as Chrome Developer Tools.
  bool LogToConsole(const std::string& msg) const;

  // Accessors.
  const NPP npp() const {
    return npp_;
  }
  const NPObject* browser_binding() const {
    return browser_binding_;
  }

  // This does not return a const NPObject* because none of the NPAPI that uses
  // this value accepts a const NPObject*.  This will go away with Pepper V2
  // so I don't think it's worth thinking about too hard.
  NPObject* window_object() const;

  // A hidden class that wraps the NPObject, preserving its memory layout
  // for the browser.
  friend class BrowserBinding;

 private:
  typedef std::map<NPIdentifier, SharedMethodCallbackExecutor> MethodDictionary;
  typedef std::map<NPIdentifier, SharedPropertyAccessorCallbackExecutor>
      PropertyAccessorDictionary;
  typedef std::map<NPIdentifier, SharedPropertyMutatorCallbackExecutor>
      PropertyMutatorDictionary;
  typedef std::map<NPIdentifier, Type*> DynamicPropertyDictionary;

  ScriptingBridge() : boost::noncopyable() {}
  ScriptingBridge(NPP npp, NPObject* browser_binding);

  // NPAPI support methods.
  bool HasMethod(NPIdentifier name);
  void Invalidate();
  bool Invoke(NPIdentifier name,
              const NPVariant* args,
              uint32_t arg_count,
              NPVariant* return_value);
  bool HasProperty(NPIdentifier name);
  bool GetProperty(NPIdentifier name, NPVariant* return_value);
  bool SetProperty(NPIdentifier name, const NPVariant& return_value);
  bool RemoveProperty(NPIdentifier name);

  NPP npp_;
  NPObject* browser_binding_;
  // |window_object_| is mutable so that the const accessor can create it
  // lazily.
  mutable NPObject* window_object_;

  MethodDictionary method_dictionary_;
  PropertyAccessorDictionary property_accessor_dictionary_;
  PropertyMutatorDictionary property_mutator_dictionary_;
  DynamicPropertyDictionary dynamic_property_dictionary_;
};

}  // namespace c_salt

#endif  // C_SALT_SCRIPTING_BRIDGE_H_
