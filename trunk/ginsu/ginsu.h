// Copyright (c) 2010 The Ginsu Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GINSU_GINSU_H_
#define GINSU_GINSU_H_

#include <time.h>
#include <nacl/nacl_npapi.h>
#include <nacl/npruntime.h>
#include <nacl/npapi_extensions.h>
#include <pgl/pgl.h>

#include <map>
#include <string>

#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include "c_salt/instance.h"
#include "c_salt/module.h"

namespace ginsu {

namespace model {
class Model;
}  // namespace model
namespace view {
class View;
}  // namespace view

// The main class for the Ginsu application.
class Ginsu : public c_salt::Instance {
 public:
  explicit Ginsu(const NPP& npp_instance);
  virtual ~Ginsu();

  // Specialization of c_salt::Instance informal protocol.
  virtual bool InstanceDidLoad(int width, int height);
  virtual void WindowDidChangeSize(int width, int height);
  virtual void InitializeMethods(c_salt::ScriptingBridge* bridge);

  // Return the view to JavaScript.
  boost::shared_ptr<view::View> GetView();

 private:
  Ginsu();  // Not implemented, do not use.
  static void RepaintCallback(NPP npp, NPDeviceContext3D* context);
  static void TickCallback(void* data);
  void Paint();
  void Tick();

  // Returns true if anything was updated.
  bool UpdateAnimation();

  // TODO(dspringer): Move this into a c_salt View3D class.
  void CreateContext();
  void DestroyContext();
  NPDevice* device3d_;
  NPDeviceContext3D context3d_;
  PGLContext pgl_context_;

  boost::scoped_ptr<model::Model> model_;
  boost::shared_ptr<view::View> view_;

  double last_update_;  // Time measured in milliseconds.
};
}  // namespace ginsu

#endif  // GINSU_GINSU_H_
