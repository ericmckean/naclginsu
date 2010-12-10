// Copyright (c) 2010 The Ginsu Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ginsu/ginsu.h"

#include <sys/time.h>

#include <cassert>

#include "c_salt/instance.h"
#include "c_salt/scripting_bridge.h"
#include "model/component.h"
#include "model/model.h"
#include "view/view.h"

namespace {
// The minimum time in between frames.  Measured in milliseconds.
const double kFrameElapsedTime = 1.0/15.0 * 1000.0;
const uint32_t kQuaternionElementCount = 4;
}  // namespace

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

Ginsu::Ginsu(const NPP& npp_instance)
    : c_salt::Instance(npp_instance) {
  model_.reset(new model::Model);
  model_->InitDemo();
  view_.reset(new view::View(*this, model_.get()));
  this->CreateScriptingBridgeForObject(view_);
  last_update_ = TimeNow();
}

Ginsu::~Ginsu() {
}

bool Ginsu::InstanceDidLoad(int width, int height) {
  // Schedule first call to Tick.
  NPN_PluginThreadAsyncCall(npp_instance(), TickCallback, this);
  return true;
}

void Ginsu::InitializeMethods(c_salt::ScriptingBridge* bridge) {
  bridge->AddMethodNamed("getView", this, &Ginsu::GetView);
}

void Ginsu::WindowDidChangeSize(int width, int height) {
  view_->SetSize(width, height);
}

void Ginsu::TickCallback(void* data) {
  static_cast<ginsu::Ginsu*>(data)->Tick();
}

boost::shared_ptr<ginsu::view::View> Ginsu::GetView() {
  return view_;
}

void Ginsu::Tick() {
  if (UpdateAnimation())
    Paint();
  // Schedule another call to Tick.
  NPN_PluginThreadAsyncCall(npp_instance(), TickCallback, this);
}

void Ginsu::Paint() {
  view_->SetNeedsRedraw(true);
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
