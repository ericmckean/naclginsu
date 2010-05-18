// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GINSU_PLUGIN_H_
#define GINSU_PLUGIN_H_

#include <time.h>
#include "boost/scoped_ptr.hpp"
#include "nphostapi.h"
#include "pgl/pgl.h"

namespace ginsu {

namespace model {
class Model;
}  // namespace model
namespace view {
class View;
}  // namespace view

class Plugin : public NPObject {
 public:
  explicit Plugin(NPP npp);
  ~Plugin();

  static NPClass* GetPluginClass();

  NPP npp() const { return npp_; }
  void New(NPMIMEType pluginType, int16 argc, char* argn[], char* argv[]);
  void SetWindow(const NPWindow& window);
  int32 HandleEvent(const NPPepperEvent& event);

 private:
  static void RepaintCallback(NPP npp, NPDeviceContext3D* context);
  static void TickCallback(void* data);
  void Paint();
  void Tick();
  void CreateContext();
  void DestroyContext();

  void UpdateAnimation();

  // This class object needs to be safely casted to NPObject* and cross
  // c-c++ module boundaries. To accomplish that this class should not have
  // any virtual member function.
  NPP npp_;

  NPDevice* device3d_;
  NPDeviceContext3D context3d_;
  PGLContext pgl_context_;

  boost::scoped_ptr<model::Model> model_;
  boost::scoped_ptr<view::View> view_;

  time_t last_update_;
};

extern NPNetscapeFuncs* g_browser;

}  // namespace ginsu
#endif  // GINSU_PLUGIN_H_
