// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VIEW_VIEW_H_
#define VIEW_VIEW_H_

#include <nacl/nacl_npapi.h>
#include <nacl/npruntime.h>

#include "boost/scoped_ptr.hpp"
#include "c_salt/scriptable_native_object.h"
#include "c_salt/scripting_bridge_ptrs.h"

namespace ginsu {

namespace model {
class Model;
}  // namespace model

namespace view {

class Scene;
class SceneView;

class View : public c_salt::ScriptableNativeObject {
 public:
  explicit View(model::Model* model);
  ~View();

  void InitGL();
  void ReleaseGL();
  void SetWindowSize(int width, int height);
  void Draw();

  // Methods to implement ScriptableNativeObject
  virtual void InitializeMethods(c_salt::ScriptingBridge* bridge);
  virtual void InitializeProperties(c_salt::ScriptingBridge* bridge);

  // These are exposed to the browser via the ScriptingBridge (see
  // InitializeMethods).
  // GetCameraOrientation returns a std::vector<double> of 4 doubles which
  // represents the camera orientation as a quaternion.  SetCameraOrientation
  // expects a std::vector<double> of 4 doubles, which is the camera
  // orientation as a quaternion.
  // TODO(dspringer): When c_salt supports std::vector<>s as types, make these
  // methods match the documentation.
  double GetCameraOrientation();
  bool SetCameraOrientation(double orientation);

 private:
  model::Model* model_;
  boost::scoped_ptr<Scene> scene_;
  boost::scoped_ptr<SceneView> scene_view_;
};

}  // namespace view
}  // namespace ginsu
#endif  // VIEW_VIEW_H_
