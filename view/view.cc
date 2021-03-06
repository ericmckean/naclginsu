// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "view/view.h"

#include <stdio.h>

#include "c_salt/notification_center.h"
#include "c_salt/opengl_context.h"
#include "c_salt/scripting_bridge.h"
#include "osg/Matrix"
#include "osg/Vec3"
#include "view/scene.h"
#include "view/scene_view.h"

namespace ginsu {
namespace view {

View::View(const c_salt::Instance& instance, model::Model* model)
    : OpenGLView(instance), model_(model) {
}

View::~View() {
}

void View::InitializeOpenGL(const c_salt::OpenGLContext& context) {
printf("View::InitializeOpenGL\n");
  assert(model_ != NULL);

  scene_.reset(new Scene(model_));
  scene_->Init();

  scene_view_.reset(new SceneView);
  scene_view_->Init(scene_->root());

  // Setup an ISO view so that everything in the scene is shown.
  const osg::BoundingSphere& bound = scene_->GetBound();
  const osg::Vec3f& target = bound.center();
  // Eye is along (1.0, -1.0, 0.5) axis.
  float eye_dist = 2.5f * bound.radius();
  osg::Vec3 eye = target + osg::Vec3(eye_dist, -eye_dist, 0.5f * eye_dist);
  // Up is along the z-axis.
  osg::Vec3 up(0.0f, 0.0f, 1.0f);
  scene_view_->SetLookAt(eye, target, up);
}

void View::ReleaseOpenGL(const c_salt::OpenGLContext& context) {
printf("View::ReleaseOpenGL\n");
  scene_view_.reset();
  scene_.reset();
}

void View::ResizeViewport() {
printf("View::ResizeViewport\n");
  opengl_context()->MakeContextCurrent();
  int32_t view_width = width();
  int32_t view_height = height();
  scene_view_->SetViewport(0, 0, view_width, view_height);
  float fovy = 30.0f;
  float aspect_ratio =
      static_cast<float>(view_width) / static_cast<float>(view_height);
  float z_near = 1.0f;
  float z_far = 1000.0f;
  scene_view_->SetProjectionMatrix(osg::Matrix::perspective(
      fovy, aspect_ratio, z_near, z_far));
}

void View::RenderOpenGL(const c_salt::OpenGLContext& context) {
printf("View::RenderOpenGL\n");
  scene_->Update();
  scene_view_->Draw();
}

double View::GetCameraOrientation() {
  return 42.0;
}

bool View::SetCameraOrientation(double orientation) {
  return true;
}

void View::InitializeMethods(c_salt::ScriptingBridge* bridge) {
  bridge->AddMethodNamed("getCameraOrientation",
                         this,
                         &View::GetCameraOrientation);
  bridge->AddMethodNamed("setCameraOrientation",
                         this,
                         &View::SetCameraOrientation);
}

void View::InitializeProperties(c_salt::ScriptingBridge* bridge) {
  // No properties in View.
}

}  // namespace view
}  // namespace ginsu
