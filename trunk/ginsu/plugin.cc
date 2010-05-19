// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ginsu/plugin.h"

#include <cassert>

#include "model/component.h"
#include "model/model.h"
#include "view/view.h"

namespace {
using ginsu::Plugin;
const int32 kCommandBufferSize = 1024 * 1024;
NPExtensions* g_extensions = NULL;

// Plugin class functions.
NPObject* PluginAllocate(NPP npp, NPClass* the_class) {
  Plugin* plugin = new Plugin(npp);
  return plugin;
}

void PluginDeallocate(NPObject* header) {
  Plugin* plugin = static_cast<Plugin*>(header);
  delete plugin;
}

void PluginInvalidate(NPObject* obj) {
}

bool PluginHasMethod(NPObject* obj, NPIdentifier name) {
  return false;
}

bool PluginInvoke(NPObject* header,
                  NPIdentifier name,
                  const NPVariant* args, uint32 arg_count,
                  NPVariant* result) {
  return false;
}

bool PluginInvokeDefault(NPObject* obj,
                         const NPVariant* args, uint32 arg_count,
                         NPVariant* result) {
  VOID_TO_NPVARIANT(*result);
  return true;
}

bool PluginHasProperty(NPObject* obj, NPIdentifier name) {
  return false;
}

bool PluginGetProperty(NPObject* obj,
                       NPIdentifier name,
                       NPVariant* result) {
  return false;
}

bool PluginSetProperty(NPObject* obj,
                       NPIdentifier name,
                       const NPVariant* variant) {
  return false;
}

NPClass plugin_class = {
  NP_CLASS_STRUCT_VERSION,
  PluginAllocate,
  PluginDeallocate,
  PluginInvalidate,
  PluginHasMethod,
  PluginInvoke,
  PluginInvokeDefault,
  PluginHasProperty,
  PluginGetProperty,
  PluginSetProperty,
};

}

namespace ginsu {
NPNetscapeFuncs* g_browser;

Plugin::Plugin(NPP npp)
  : npp_(npp),
    device3d_(NULL),
    pgl_context_(PGL_NO_CONTEXT) {
  memset(&context3d_, 0, sizeof(context3d_));
}

Plugin::~Plugin() {
  pglMakeCurrent(pgl_context_);
  view_->ReleaseGL();
  pglMakeCurrent(PGL_NO_CONTEXT);

  DestroyContext();
}

NPClass* Plugin::GetPluginClass() {
  return &plugin_class;
}

void Plugin::New(NPMIMEType pluginType,
                 int16 argc, char* argn[], char* argv[]) {
  if (!g_extensions) {
    g_browser->getvalue(npp_, NPNVPepperExtensions, &g_extensions);
    assert(g_extensions);
  }

  device3d_ = g_extensions->acquireDevice(npp_, NPPepper3DDevice);
  assert(device3d_);

  model_.reset(new model::Model);
  model_->InitDemo();
  //model_->AddComponent(model::Component::MakeCube());
  //model_->AddComponent(model::Component::MakeTruncatedCone(0.3, 1.0));
  view_.reset(new view::View(model_.get()));
  time(&last_update_);
  start_time_ = last_update_;
}

void Plugin::SetWindow(const NPWindow& window) {
  if (!pgl_context_) {
    CreateContext();

    // Schedule first call to Tick.
    g_browser->pluginthreadasynccall(npp_, TickCallback, this);
  }
  view_->SetWindowSize(window.width, window.height);
}

int32 Plugin::HandleEvent(const NPPepperEvent& event) {
  return 0;
}

void Plugin::RepaintCallback(NPP npp, NPDeviceContext3D* /* context */) {
  Plugin* plugin = static_cast<Plugin*>(npp->pdata);
  plugin->Paint();
}

void Plugin::TickCallback(void* data) {
  static_cast<ginsu::Plugin*>(data)->Tick();
}


void Plugin::Tick() {
  if (UpdateAnimation())
    Paint();

  // Schedule another call to Tick.
  g_browser->pluginthreadasynccall(npp_, TickCallback, this);
}

void Plugin::Paint() {
  if (!pglMakeCurrent(pgl_context_) && pglGetError() == PGL_CONTEXT_LOST) {
    DestroyContext();
    CreateContext();
    pglMakeCurrent(pgl_context_);
  }

  view_->Draw();
  pglSwapBuffers();
  pglMakeCurrent(PGL_NO_CONTEXT);
}

void Plugin::CreateContext() {
  assert(pgl_context_ == PGL_NO_CONTEXT);

  // Initialize a 3D context.
  NPDeviceContext3DConfig config;
  config.commandBufferSize = kCommandBufferSize;
  device3d_->initializeContext(npp_, &config, &context3d_);
  context3d_.repaintCallback = RepaintCallback;

  // Create a PGL context.
  pgl_context_ = pglCreateContext(npp_, device3d_, &context3d_);

  // Initialize GL resources.
  pglMakeCurrent(pgl_context_);
  view_->InitGL();
  pglMakeCurrent(PGL_NO_CONTEXT);
}

void Plugin::DestroyContext() {
  assert(pgl_context_ != PGL_NO_CONTEXT);

  pglDestroyContext(pgl_context_);
  pgl_context_ = PGL_NO_CONTEXT;

  device3d_->destroyContext(npp_, &context3d_);
}

bool Plugin::UpdateAnimation() {
  time_t now;
  time(&now);
  double time_laps = difftime(now, last_update_);
  if (time_laps < 0.1)
    return false;

  model_->UpdateDemo(difftime(now, start_time_));
  last_update_ = now;
  return true;
}

}  // namespace ginsu
