// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GINSU_VIEW_VIEW_H_
#define GINSU_VIEW_VIEW_H_

#include <nacl/nacl_npapi.h>
#include <nacl/npruntime.h>

#include "boost/scoped_ptr.hpp"
#include "c_salt/scripting_bridge.h"

namespace ginsu {

namespace model {
class Model;
}  // namespace model

namespace view {

class Scene;
class SceneView;

class View {
 public:
  View(model::Model* model);
  ~View();

  void InitGL();
  void ReleaseGL();
  void SetWindowSize(int width, int height);
  void Draw();

  // Create a ScriptingBridge that represents this View instance.  If such a
  // ScriptingBridge already exists, then ask the browser to ref count it and
  // return the ref counted instance.
  c_salt::ScriptingBridge* CreateScriptingBridgeWithInstance(
      const c_salt::Instance* instance);

  // These are exposed to the browser via |scripting_bridge_|.
  // GetCameraOrientation returns a std::vector<double> of 4 doubles which
  // represents the camera orientation as a quaternion.  SetCameraOrientation
  // expects a std::vector<double> of 4 doubles, which is the camera
  // orientation as a quaternion.
  // TODO(dspringer): When c_salt supports sttd::vector<>s as types, make these
  // methods match the documentation.
  double GetCameraOrientation();
  bool SetCameraOrientation(double orientation);

 private:
  model::Model* model_;
  boost::scoped_ptr<Scene> scene_;
  boost::scoped_ptr<SceneView> scene_view_;
  boost::scoped_ptr<c_salt::ScriptingBridge> scripting_bridge_;
};

}  // namespace view
}  // namespace ginsu
#endif  // GINSU_VIEW_VIEW_H_
