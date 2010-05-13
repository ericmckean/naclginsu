// Copyright (c) 2010 The Ginsu Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GINSU_MODEL_COMPONENT_H_
#define GINSU_MODEL_COMPONENT_H_

#include "boost/scoped_ptr.hpp"

namespace ginsu {
namespace model {

class AffineTransform3D;
class Mesh;

class Component {
 public:
  // Make a unit cube from (0, 0, 0) to (1, 1, 1).
  static Component* MakeCube();
  // Make a truncated cone centered on the y-axis, between Z = 0 and Z =1 and
  // of given radii. At least one radius must be non-zero.
  static Component* MakeTruncatedCone(float top_radius, float bottom_radius);
  
  // TODO(gwink):
  //  - Iterators to extract geometry for viewing.
  //  - Modify associated transform.

 protected:
  // Can't instantiate directly. Use the Make*** functions above.
  Component();
  void Init(Mesh* mesh);

  // Get embedded Mesh instance.
  const Mesh* mesh() const { return mesh_.get(); }

 private:
  // Tessellator requires access to mesh().
  friend class Tessellator;
  // Component transform; defaults to identity.
  boost::scoped_ptr<AffineTransform3D> transform_;
  // The geometry.
  boost::scoped_ptr<Mesh> mesh_;
};

}  // namespace model
}  // namespace ginsu
#endif  // GINSU_MODEL_COMPONENT_H_
