// Copyright (c) 2010 The Ginsu Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ginsu/ginsu.h"

#include <cassert>

#include <string.h>

#include "c_salt/module.h"
#include "model/component.h"
#include "model/model.h"
#include "view/view.h"

NPDevice* NPN_AcquireDevice(NPP instance, NPDeviceID device);
const int32_t kCommandBufferSize = 1024 * 1024;

// The c_salt module allocator.
namespace c_salt {

Module* Module::CreateModule() {
  return new ginsu::Ginsu();
}

}

namespace ginsu {

Ginsu::Ginsu() : npp_instance_(NULL), device3d_(NULL) {
  memset(&context3d_, 0, sizeof(context3d_));
  model_.reset(new model::Model);
  model_->InitDemo();
  //model_->AddComponent(model::Component::MakeCube());
  //model_->AddComponent(model::Component::MakeTruncatedCone(0.3, 1.0));
  view_.reset(new view::View(model_.get()));
  last_update_ = clock();
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
  clock_t now = clock();
  float time_laps = static_cast<float>(now - last_update_) / CLOCKS_PER_SEC;
  if (time_laps < 0.2)
    return false;

  ++update_count;
  model_->UpdateDemo(difftime(now, last_update_));
  last_update_ = now;
  return true;
}

}  // namespace ginsu

