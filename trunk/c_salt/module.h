// Copyright 2010 The Ginsu Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#ifndef C_SALT_MODULE_H_
#define C_SALT_MODULE_H_

#include <nacl/nacl_npapi.h>
#include <nacl/npruntime.h>
#include <nacl/npapi_extensions.h>

#include "c_salt/basic_macros.h"

namespace c_salt {

class ScriptingBridge;

// The base class for the Native Client module.  Subclasses must implement the
// CreateModule() factory method.  A Module instance can publish a
// ScriptingBridge instance to the browser.  A Module will create an instance
// of a ScriptingBridge when asked via the NPP_GetScriptableInstance()
// NPAPI.  When the Module instance is deallocated, this ScriptingBridge
// instance is also deallocated.

// TODO(dspringer): This becomes a subclass of pp::Instance when Pepper v2
// becomes available.

class Module {
 public:
  // Factory method must be implemented in the subclass.
  static Module* CreateModule();

  Module() : scripting_bridge_(NULL), is_loaded_(false) {}
  virtual ~Module() = 0;

  // Called during initialization to publish the module's method names that
  // can be called from JavaScript.
  virtual void InitializeMethods(ScriptingBridge* bridge);
  
  // Called during initialization to publish the module's properties that can
  // be called from JavaScript.
  virtual void InitializeProperties(ScriptingBridge* bridge);

  // Called the first time this module instance is loaded into the browser
  // document.  When this method is called, all the Pepper devices are
  // available to the module instance.
  virtual bool InstanceDidLoad(const NPP instance, int width, int height);

  // Called when there is a valid browser window for rendering, or whenever the
  // in-browser view changes size.
  virtual void WindowDidChangeSize(const NPP instance, int width, int height);

  // Receive an event from the browser.  Return |false| if the module does not
  // handle the event.
  virtual bool ReceiveEvent(const NPPepperEvent& event);

  // Called when the browser wants an object that conforms to the scripting
  // protocol.  Creates a new ScriptingBridge instance if needed, otherwise,
  // increments the ref count of the existing instance.  When a new
  // ScriptingBridge instance is created, both InitializeMethods() and
  // InitializeProperties() are called with the new instance.
  virtual NPObject* CreateScriptingBridge(NPP instance);

  // Accessor for the internal scripting bridge object.
  ScriptingBridge* scripting_bridge() const {
    return scripting_bridge_;
  }

  // Accessor/mutator for |is_loaded_|.  This is used to determine when to call
  // the ModuleDidLoad() method.
  bool is_loaded() const {
    return is_loaded_;
  }
  void set_is_loaded(bool flag) {
    is_loaded_ = flag;
  }

 private:
  // TODO(dspringer): this should be a smart (scoped?) pointer.
  ScriptingBridge* scripting_bridge_;
  bool is_loaded_;
  DISALLOW_COPY_AND_ASSIGN(Module);
};

}  // namespace c_salt

#endif  // C_SALT_MODULE_H_
