// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GINSU_VIEW_SCENE_H_
#define GINSU_VIEW_SCENE_H_

#include "osg/BoundingSphere"
#include "osg/ref_ptr"

namespace osg {
class Node;
class Program;
}

namespace ginsu {

namespace model {
class Model;
}  // namespace model

namespace view {

class Scene {
 public:
  Scene(model::Model* model);
  ~Scene();

  void Init();
  void Update();

  osg::Node* root() const { return root_.get(); }
  const osg::BoundingSphere& GetBound() const;

 private:
  model::Model* model_;
  osg::ref_ptr<osg::Node> root_;
  osg::ref_ptr<osg::Program> face_shader_;
};

}  // namespace view
}  // namespace ginsu
#endif  // GINSU_VIEW_SCENE_H_
