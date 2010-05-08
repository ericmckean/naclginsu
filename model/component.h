// Copyright (c) 2010 The Ginsu Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GINSU_MODEL_COMPONENT_H_
#define GINSU_MODEL_COMPONENT_H_

#include "model/mesh.h"

namespace ginsu {
namespace model {

class Component {
 public:
  // Make a unit cube from (0, 0, 0) to (1, 1, 1).
  static Component* MakeCube();
  // TODO(gwink): other primitives; sphere, cone.
  
  // TODO(gwink):
  //  - Iterators to extract geometry for viewing.
  //  - Modify associated transform.

 protected:
  // Can't instantiate directly. Use the Make*** functions above.
  Component();

  // Get embedded Mesh instance.
  friend class Tessellator;
  const Mesh& mesh() const { return mesh_; }

 private:
  // Component transform; defaults to identity.
  Aff_transformation_3 transform_;
  // The geometry.
  Mesh mesh_;
};

}  // namespace model
}  // namespace ginsu
#endif  // GINSU_MODEL_COMPONENT_H_
