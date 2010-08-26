// Copyright (c) 2010 The Ginsu Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ginsu/ginsu.h"

#include <string.h>
#include <sys/time.h>

#include <cassert>

#include "c_salt/instance.h"
#include "c_salt/int32_type.h"
#include "c_salt/scripting_bridge.h"
#include "model/component.h"
#include "model/model.h"
#include "view/view.h"

NPDevice* NPN_AcquireDevice(NPP instance, NPDeviceID device);

namespace {
const int32_t kCommandBufferSize = 1024 * 1024;

// The minimum time in between frames.  Measured in milliseconds.
const double kFrameElapsedTime = 1.0/15.0 * 1000.0;
const uint32_t kQuaternionElementCount = 4;

// Keys for properties that are exposed ot the browser.
const char* kViewKey = "view";

}  // namespace

using c_salt::ScriptingBridge;

// The c_salt module instance allocator.
namespace c_salt {
Instance* Instance::CreateInstance() {
  return new ginsu::Ginsu();
}
}

// Helper function to return the current time in milliseconds.  If there is an
// error getting the current time, this returns -1.
static double TimeNow() {
  struct timeval time_val;
  if (gettimeofday(&time_val, NULL) == 0) {
    return time_val.tv_sec * 1000.0 + time_val.tv_usec / 1000.00;
  }
  return -1.0;
}

namespace ginsu {

Ginsu::Ginsu()
    : npp_instance_(NULL),
      device3d_(NULL) {
  memset(&context3d_, 0, sizeof(context3d_));
  model_.reset(new model::Model);
  model_->InitDemo();
  view_.reset(new view::View(model_.get()));
  last_update_ = TimeNow();
  // TODO(dspringer, dmichael): Update this when the scripting brdige for
  // arbitrary (in this case, View) objects is added.
  property_dictionary_[kViewKey].reset(new c_salt::Int32Type(42));
}

Ginsu::~Ginsu() {
  pglMakeCurrent(pgl_context_);
  view_->ReleaseGL();
  pglMakeCurrent(PGL_NO_CONTEXT);
  DestroyContext(npp_instance_);
}

bool Ginsu::InstanceDidLoad(const NPP instance, int width, int height) {
  npp_instance_ = instance;
  device3d_ = NPN_AcquireDevice(npp_instance_, NPPepper3DDevice);
  assert(device3d_);
  if (!pgl_context_) {
    CreateContext(npp_instance_);
    // Schedule first call to Tick.
    NPN_PluginThreadAsyncCall(npp_instance_, TickCallback, this);
  }
  return true;
}

void Ginsu::InitializeMethods(ScriptingBridge* bridge) {
  bridge->AddMethodNamed(
      "getValueForKey", this, &Ginsu::GetValueForKey);
  bridge->AddMethodNamed(
      "setValueForKey", this, &Ginsu::SetValueForKey);
}

void Ginsu::WindowDidChangeSize(const NPP instance, int width, int height) {
  view_->SetWindowSize(width, height);
}

void Ginsu::RepaintCallback(NPP npp, NPDeviceContext3D* /* context */) {
  Ginsu* ginsu = static_cast<Ginsu*>(npp->pdata);
  ginsu->Paint();
}

void Ginsu::TickCallback(void* data) {
  static_cast<ginsu::Ginsu*>(data)->Tick();
}

int32_t Ginsu::GetValueForKey(std::string key) {
  // In this first iteration, support a 'view_id' key, which is used to test
  // the scripting interface.
  // TODO(dspringer): Complete support for getting the View object's
  // scripting bridge.
  PropertyDictionary::const_iterator it = property_dictionary_.find(key);
  if (it != property_dictionary_.end()) {
    return it->second->int32_value();
  }
  return 0;
}

bool Ginsu::SetValueForKey(std::string key, int32_t value) {
  PropertyDictionary::const_iterator it = property_dictionary_.find(key);
  if (it != property_dictionary_.end()) {
    property_dictionary_[key].reset(new c_salt::Int32Type(value));
    return true;
  }
  return false;
}

void Ginsu::Tick() {
  if (UpdateAnimation())
    Paint();
  // Schedule another call to Tick.
  NPN_PluginThreadAsyncCall(npp_instance_, TickCallback, this);
}

void Ginsu::Paint() {
  if (!pglMakeCurrent(pgl_context_) && pglGetError() == PGL_CONTEXT_LOST) {
    DestroyContext(npp_instance_);
    CreateContext(npp_instance_);
    pglMakeCurrent(pgl_context_);
  }

  view_->Draw();
  pglSwapBuffers();
  pglMakeCurrent(PGL_NO_CONTEXT);
}

void Ginsu::CreateContext(const NPP instance) {
  assert(pgl_context_ == PGL_NO_CONTEXT);

  // Initialize a 3D context.
  NPDeviceContext3DConfig config;
  config.commandBufferSize = kCommandBufferSize;
  device3d_->initializeContext(instance, &config, &context3d_);
  context3d_.repaintCallback = RepaintCallback;

  // Create a PGL context.
  pgl_context_ = pglCreateContext(instance, device3d_, &context3d_);

  // Initialize GL resources.
  pglMakeCurrent(pgl_context_);
  view_->InitGL();
  pglMakeCurrent(PGL_NO_CONTEXT);
}

void Ginsu::DestroyContext(const NPP instance) {
  assert(pgl_context_ != PGL_NO_CONTEXT);

  pglDestroyContext(pgl_context_);
  pgl_context_ = PGL_NO_CONTEXT;

  device3d_->destroyContext(instance, &context3d_);
}

bool Ginsu::UpdateAnimation() {
  static int all_count = 0;
  static int update_count = 0;

  ++all_count;
  double now = TimeNow();
  if (now == -1.0)
    return false;
  double time_laps = now - last_update_;
  if (time_laps < kFrameElapsedTime)
    return false;

  ++update_count;
  model_->UpdateDemo(time_laps);
  last_update_ = now;
  return true;
}

}  // namespace ginsu

