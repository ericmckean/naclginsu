// Copyright 2010 The Ginsu Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

// TODO(dspringer): This file will disappear when we migrate to Pepper v2 API.
// It gets replaced by pp::Instance.

#include "c_salt/instance.h"

namespace c_salt {

Instance::~Instance() {
  set_is_loaded(false);
}

void Instance::InitializeMethods(ScriptingBridge* bridge) {
}

void Instance::InitializeProperties(ScriptingBridge* bridge) {
}

bool Instance::InstanceDidLoad(const NPP instance, int width, int height) {
  return true;
}

void Instance::WindowDidChangeSize(const NPP instance, int width, int height) {
}

bool Instance::ReceiveEvent(const NPPepperEvent& event) {
  return false;
}

NPObject* Instance::CreateScriptingBridge() {
  if (!scripting_bridge_.get()) {
    // This is a synchronous call.
    scripting_bridge_.reset(
        ScriptingBridge::CreateScriptingBridgeWithInstance(this));
    InitializeMethods(scripting_bridge_.get());
    InitializeProperties(scripting_bridge_.get());
  }
  return scripting_bridge_->CopyBrowserBinding();
}

}  // namespace c_salt
