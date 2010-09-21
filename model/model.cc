// Copyright (c) 2010 The Ginsu Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "model/model.h"

// TODO(alokp): Remove it after IO Demo.
#include <sstream>

#include "model/component.h"
#include "model/mesh.h"
#include "osg/Matrix"
#include "osg/Vec3"

namespace {
// Corner.off found in CGAL distribution.
const char kDemoMesh[] =
"OFF\n"
"14 24 0\n"
"\n"
"-0.125 -0.125 -0.125\n"
"-0.125 0.125 -0.125\n"
"0.125 0.125 -0.125\n"
"0.125 -0.125 -0.125\n"
"-0.125 -0.125 0.125\n"
"-0.125 0.125 0.125\n"
"0.125 0.125 0.125\n"
"0.125 -0.125 0.125\n"
"0.5 0 0\n"
"0 0.5 0\n"
"-0.5 0 0\n"
"0 -0.5 0\n"
"0 0 0.5\n"
"0 0 -0.5\n"
"3  0 1 13\n"
"3  1 2 13\n"
"3  2 3 13\n"
"3  3 0 13\n"
"3  3 2 8\n"
"3  7 3 8\n"
"3  6 7 8\n"
"3  2 6 8\n"
"3  7 6 12\n"
"3  4 7 12\n"
"3  5 4 12\n"
"3  6 5 12\n"
"3  4 5 10\n"
"3  5 1 10\n"
"3  1 0 10\n"
"3  0 4 10\n"
"3  5 6 9\n"
"3  6 2 9\n"
"3  2 1 9\n"
"3  1 5 9\n"
"3  7 4 11\n"
"3  3 7 11\n"
"3  0 3 11\n"
"3  4 0 11\n";
}

namespace ginsu {
namespace model {

Model::Model()
: subdiv_steps_(2),
  steps_increment_(1) {
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


void Model::InitDemo() {
  std::stringstream off_stream;
  off_stream.str(kDemoMesh);

  Component* comp = Component::MakeEmpty();
  comp->ReadOffStream(off_stream);
  AddComponent(comp);
}

void Model::UpdateDemo(double time_laps) {
  static const float kTwoPi = 6.283f;
  if (components_.size() > 0 && components_[0] != NULL) {
    // TODO(gwink): Use cgal AffineTransform3D instead, to be consistent. Then
    // remove dependency on osg from scons file.
    osg::Matrix rotate;
    static const osg::Vec3 axis(0.2f, 0.0f, 0.5f);
    float rotation = 0.1f * static_cast<float>(time_laps);
    if (rotation > kTwoPi) rotation -= kTwoPi;
    rotate.makeRotate(rotation, axis);
    components_[0]->SetTransform(rotate.ptr());

    // Make a copy of component 0 and subdivide it.
    if (components_.size() < 2) {
      AddComponent(Component::MakeCopy(*(components_[0].get())));
    } else {
      components_[1].reset(Component::MakeCopy(*(components_[0].get())));
    }
    components_[1]->Subdivide(subdiv_steps_);
  }
}

// void Model::DemoComputeIntersection() {
//   if (components_[0] != NULL && components_[1] != NULL) {
//     if (components_[2] == NULL) {
//       components_[2].reset(Component::MakeEmpty());
//     }
//
//     components_[2]->Intersect(components_[0].get(), components_[1].get());
//   }
// }

// void Model::DemoAnimationUpdate(double time_laps) {
//   static const float kTwoPi = 6.283f;
//   if(components_.size() > 0 && components_[0] != NULL) {
//     // TODO(gwink): Use cgal AffineTransform3D instead, to be consistent.
//     // Then remove dependency on osg from scons file.
//     osg::Matrix rotate;
//     const static osg::Vec3 axis(0.2f, 0.0f, 0.5f);
//     float rotation = 0.1f * static_cast<float>(time_laps);
//     if (rotation > kTwoPi) rotation -= kTwoPi;
//     rotate.makeRotate(rotation, axis);
//     components_[0]->SetTransform(rotate.ptr());
//   }
//   if(components_.size() > 1 && components_[1] != NULL) {
//     // TODO(gwink): Use cgal AffineTransform3D instead, to be consistent.
//     // Then remove dependency on osg from scons file.
//     osg::Matrix rotate;
//     const static osg::Vec3 axis(0.7f, 0.2f, 0.0f);
//     float rotation = 0.2f * static_cast<float>(time_laps);
//     if (rotation > kTwoPi) rotation -= kTwoPi;
//     rotate.makeRotate(rotation, axis);
//     components_[1]->SetTransform(rotate.ptr());
//   }
// }

}  // namespace model
}  // namespace ginsu
