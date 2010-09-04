// Copyright 2010 The Ginsu Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#ifndef C_SALT_INSTANCE_H_
#define C_SALT_INSTANCE_H_

#include <nacl/nacl_npapi.h>
#include <nacl/npruntime.h>
#include <nacl/npapi_extensions.h>

#include "boost/noncopyable.hpp"
#include "boost/scoped_ptr.hpp"
#include "c_salt/scripting_bridge.h"

namespace c_salt {

// The base class for the Native Client module instance.  An Instance can
// publish a ScriptingBridge to the browser.  An Instance will create an
// instance of a ScriptingBridge when asked via the NPP_GetScriptableInstance()
// NPAPI.  When the Instance is deallocated, this ScriptingBridge instance is
// also deallocated.

// TODO(dspringer): This becomes a subclass of pp::Instance when Pepper v2
// becomes available.

class Instance : public boost::noncopyable {
 public:
  explicit Instance(const NPP& npp_instance)
      : npp_instance_(npp_instance), is_loaded_(false) {}
  virtual ~Instance();

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
  virtual NPObject* CreateScriptingBridge();

  // Accessor for the in-browser NPAPI instance associated with this Instance.
  const NPP npp_instance() const {
    return npp_instance_;
  }

  // Accessor/mutator for |is_loaded_|.  This is used to determine when to call
  // the InstanceDidLoad() method.
  bool is_loaded() const {
    return is_loaded_;
  }
  void set_is_loaded(bool flag) {
    is_loaded_ = flag;
  }

  // Access to window object if the scripting bridge is necessary for now in
  // order to support NPAPI coding for subclasses.
  NPObject* WindowObject() const {
    return scripting_bridge_->window_object();
  }

 private:
  Instance();  // Not implemented, do not use.

  boost::scoped_ptr<ScriptingBridge> scripting_bridge_;
  NPP npp_instance_;
  bool is_loaded_;
};

}  // namespace c_salt

#endif  // C_SALT_INSTANCE_H_
