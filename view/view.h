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
#include "view/opengl_view.h"

namespace ginsu {

namespace model {
class Model;
}  // namespace model

namespace view {

class Scene;
class SceneView;

class View : public ginsu::view::OpenGLView,
             public c_salt::ScriptableNativeObject {
 public:
  View(const c_salt::Instance& instance, model::Model* model);
  ~View();

  // OpenGLView methods.
  virtual void InitializeOpenGL(const c_salt::OpenGLContext& context);
  virtual void ReleaseOpenGL(const c_salt::OpenGLContext& context);
  virtual void RenderOpenGL(const c_salt::OpenGLContext& context);
  virtual void ResizeViewport();

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
