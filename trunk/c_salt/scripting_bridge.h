// Copyright 2010 The Ginsu Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#ifndef C_SALT_SCRIPTING_BRIDGE_H_
#define C_SALT_SCRIPTING_BRIDGE_H_

#include <map>
#include <string>

#include "boost/noncopyable.hpp"
#include "boost/shared_ptr.hpp"
#include "c_salt/callback.h"
#include "c_salt/npapi/npapi_method_callback.h"
#include "c_salt/property.h"
#include "c_salt/variant.h"

namespace c_salt {

class Instance;
class MethodCallbackExecutor;
class Module;

namespace npapi {
class BrowserBinding;
}  // namespace npapi

// This class handles all the calls across the bridge to the browser via its
// browser binding object, |browser_binding_|.  Note that NPObjects cannot have
// a vtable, hence the PIMPL pattern used here.  Use AddMethodNamed() and
// AddProperty() to publish methods and static properties that can be accessed
// from the browser code.
//
// TODO(dspringer): |browser_binding_| gets replaced by pp::ScriptableObject
// when Pepper v2 becomes available.

class ScriptingBridge : public boost::noncopyable {
 public:
  // Shared pointer types used in the method and property maps.
  typedef boost::shared_ptr<npapi::NPAPIMethodCallbackExecutor>
      SharedNPAPIMethodCallbackExecutor;

  // Creates an instance of the scripting bridge object in the browser, with
  // a corresponding ScriptingBridge object instance.
  static ScriptingBridge* CreateScriptingBridgeWithInstance(
      const Instance& instance);
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
    SharedNPAPIMethodCallbackExecutor method_ptr(
      new npapi::NPAPIMethodCallbackExecutorImpl<Signature>(handler, method));
    method_dictionary_.insert(MethodDictionary::value_type(method_name,
                                                           method_ptr));
    return true;
  }

  // Adds |property| to the property dictionary.  These properties should all
  // be declared as static, mutable properties (the default).  See
  // c_salt/property.h for details.  Example usage:
  //   SharedVariant value(new c_salt::Variant(42));
  //   PropertyAttributes prop_attrib("myProp", value);
  //   bridge->AddProperty(Property(prop_attribs));
  bool AddProperty(const Property& property);
  // Return a Property by ref counting so this can be made thread-safe.  Note
  // that this method bumps the ref count of the underlying instance to a Value,
  // the caller is responsible for freeing it.  Sets |value| to point to a NULL
  // Value and returns |false| if no such property exists.
  bool GetValueForPropertyNamed(const std::string& name,
                                SharedVariant value) const;
  // Sets the value of the property associated with |name|.  Returns |false|
  // if no such property exists.
  bool SetValueForPropertyNamed(const std::string& name, const Variant& value);


  // Make a copy of the browser binding object by asking the browser to retain
  // it.  Use this for the return value of functions that expect the retain
  // count to increment, such as NPP_GetScriptableInstance().
  NPObject* CopyBrowserBinding();

  // Release the browser binding object.  Note that this *might* cause |this|
  // to get deleted, if the ref count of the browser binding object falls to 0.
  void ReleaseBrowserBinding();

  // Log a message to the browser's console window.  You can usually see this
  // message when using a JavaScript debugger, such as Chrome Developer Tools.
  bool LogToConsole(const std::string& msg) const;

  // Return the browser instance associated with this ScriptingBridge.
  const NPP& GetBrowserInstance() const;

  // Accessors.
  const npapi::BrowserBinding* browser_binding() const {
    return browser_binding_;
  }

  // This does not return a const NPObject* because none of the NPAPI that uses
  // this value accepts a const NPObject*.  This will go away with Pepper V2
  // so I don't think it's worth thinking about too hard.
  NPObject* window_object() const;

  // A hidden class that wraps the NPObject, preserving its memory layout
  // for the browser.
  friend class npapi::BrowserBinding;

 private:
  typedef std::map<std::string,
                   SharedNPAPIMethodCallbackExecutor> MethodDictionary;
  typedef std::map<std::string, Property> PropertyDictionary;

  ScriptingBridge();  // Not implemented, do not use.
  explicit ScriptingBridge(npapi::BrowserBinding* browser_binding);

  // Support for browser-exposed methods.  The browser proxy (a private,
  // platform-specific implementation) invokes a method by first calling
  // HasScriptMethod(), and if that returns |true|, calls InvokeScriptMethod().
  // The browser proxy is responsible for all the variant marshaling from
  // platform-specific types (for example NPVariant or pp::Var) into c_salt
  // Types.
  bool HasScriptMethod(const std::string& name);
  // TODO(dspringer,dmichael): Migrate this code so it doesn't have NPAPI
  // stuff in it.  This is kind of a really huge refactoring job, which touches
  // the callback machinery and method invoking stuff and all kinds of things.
  bool InvokeScriptMethod(const std::string& method_name,
                          const NPVariant* args,
                          uint32_t arg_count,
                          NPVariant* return_value);

  // Support for browser-exposed properties.  The browser proxy (which is
  // platform-specific) first calls HasProperty() before getting or setting;
  // the Get or Set is performed only if HasProperty() returns |true|.  The
  // brwoser proxy is responsible for all the variant marshaling.
  bool HasScriptProperty(const std::string& name);
  // Set |return_value| to the value associated with property |name|.  If
  // property |name| doesn't exist, then set |return_value| to the null type
  // and return |false|.
  bool GetScriptProperty(const std::string& name,
                         SharedVariant return_value) const;
  // If |name| is associated with a static property, return that value.  Else,
  // if there is no property associated with |name|, add it as a dynamic
  // property.  See property.h for definitions and more details.
  bool SetScriptProperty(const std::string& name, const SharedVariant& value);
  // This succeeds only if |name| is associated with a dynamic property.
  bool RemoveScriptProperty(const std::string& name);

  // This is called by some browser proxies when all references to a proxy
  // object have been deallocated, but the proxy's ref count has not gone to 0.
  // It's kind of an anti-leak clean-up mechanism.
  void Invalidate();

  // This is a weak reference.  Some kind of smart_ptr would be useful here,
  // but the |browser_binding_| instance is actually a proxy object that is
  // managed by the browser.  In addition, this pointer is actually a circular
  // reference to the BrowserBinding object that owns this ScriptingBridge
  // instance.
  npapi::BrowserBinding* browser_binding_;
  // |window_object_| is mutable so that the const accessor can create it
  // lazily.
  // TODO(dspringer): move this into BrowserBinding.
  mutable NPObject* window_object_;

  MethodDictionary method_dictionary_;
  PropertyDictionary property_dictionary_;
};

}  // namespace c_salt

#endif  // C_SALT_SCRIPTING_BRIDGE_H_
