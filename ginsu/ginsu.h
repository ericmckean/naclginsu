// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GINSU_H_
#define GINSU_H_

#include <time.h>
#include <nacl/nacl_npapi.h>
#include <nacl/npruntime.h>
#include <nacl/npapi_extensions.h>
#include <pgl/pgl.h>
#include "boost/scoped_ptr.hpp"
#include "c_salt/module.h"

namespace ginsu {

namespace model {
class Model;
}  // namespace model
namespace view {
class View;
}  // namespace view

// The main class for the Ginsu application.
class Ginsu : public c_salt::Module {
 public:
  Ginsu();
  virtual ~Ginsu();

  virtual bool InstanceDidLoad(const NPP instance, int width, int height);
  virtual void WindowDidChangeSize(const NPP instance, int width, int height);

 private:
  static void RepaintCallback(NPP npp, NPDeviceContext3D* context);
  static void TickCallback(void* data);
  void Paint();
  void Tick();

  // Returns true if anything was updated.
  bool UpdateAnimation();

  // TODO(dspringer): Move this into a c_salt View3D class.
  void CreateContext(const NPP instance);
  void DestroyContext(const NPP instance);
  NPP npp_instance_;  // Weak reference.
  NPDevice* device3d_;
  NPDeviceContext3D context3d_;
  PGLContext pgl_context_;

  boost::scoped_ptr<model::Model> model_;
  boost::scoped_ptr<view::View> view_;

  time_t last_update_;
};

}  // namespace ginsu

#endif  // GINSU_H_
