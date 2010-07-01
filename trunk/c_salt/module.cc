// Copyright 2010 The Ginsu Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

// TODO(dspringer): This file will disappear when we migrate to Pepper v2 API.
// It gets replaced by pp::Instance.

#include "c_salt/module.h"

#include <assert.h>

#include "c_salt/scripting_bridge.h"

namespace c_salt {

// Must be implemented in the sub-class!
// Module* Module::CreateModule() {}

Module::~Module() {
  if (scripting_bridge_) {
    scripting_bridge_->ReleaseBrowserBinding();
    scripting_bridge_ = NULL;
  }
  set_is_loaded(false);
}

void Module::InitializeMethods(ScriptingBridge* bridge) {
}

void Module::InitializeProperties(ScriptingBridge* bridge) {
}

bool Module::InstanceDidLoad(const NPP instance, int width, int height) {
  return true;
}

void Module::WindowDidChangeSize(const NPP instance, int width, int height) {
}

bool Module::ReceiveEvent(const NPPepperEvent& event) {
  return false;
}

NPObject* Module::CreateScriptingBridge(NPP instance) {
  if (!scripting_bridge_) {
    // This is a synchronous call.
    scripting_bridge_ = ScriptingBridge::CreateScriptingBridge(instance);
    InitializeMethods(scripting_bridge_);
    InitializeProperties(scripting_bridge_);
  }
  return scripting_bridge_->CopyBrowserBinding();
}

}  // namespace c_salt
