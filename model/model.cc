// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "model/model.h"
#include "model/component.h"
#include "model/mesh.h"
#include "osg/Matrix"
#include "osg/Vec3"

namespace ginsu {
namespace model {

Model::Model() {
}

Model::~Model() {
  Clear();
}

void Model::AddComponent(Component* component) {
  if (component != NULL) {
    components_.push_back(ComponentItem(component));
  }
}

void Model::Clear() {
  components_.clear();
}

void Model::DemoComputeIntersection() {
  if (components_[0] != NULL && components_[1] != NULL) {
    if (components_[2] == NULL) {
      components_[2].reset(Component::MakeEmpty());
    }
    
    components_[2]->Intersect(components_[0].get(), components_[1].get());
  }
}

void Model::DemoAnimationUpdate(double time_laps) {
  if(components_[0] != NULL) {
    // TODO(gwink): Use cgal AffineTransform3D instead, to be consistent. Then
    // remove dependency on osg from scons file. 
    osg::Matrix rotate;
    const static osg::Vec3 axis(0.2f, 0.0f, 0.5f);
    rotate.makeRotate(0.1f * static_cast<float>(time_laps), axis);
    components_[0]->Transform(rotate.ptr());
  }
}

}  // namespace model
}  // namespace ginsu
