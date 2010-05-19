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
  // Make an empty-set component.
  static Component* MakeEmpty();
  // Make a unit cube from (0, 0, 0) to (1, 1, 1).
  static Component* MakeCube();
  // Make a truncated cone centered on the y-axis, between Z = 0 and Z =1 and
  // of given radii. At least one radius must be non-zero.
  static Component* MakeTruncatedCone(float top_radius, float bottom_radius);
  // Make a copy.
  static Component* MakeCopy(const Component& component);

  // Store the intersection of c1 * c2 into this.
  //void Intersect(const Component* component1, const Component* component2);

  // Subdivide the mesh using num_steps steps of Catmull-Clark.
  void Subdivide(int num_steps);
  
  void ReadOffStream(std::istream& input_stream);

  // Load the component affine transform in transform[16], in row-major
  // order. (The translation parts are stored in t[3][0-2].)
  void GetTransformMatrix44(float transform[16]) const;

  // Set the component transform.
  void SetIdentityTranform();
  void SetTransform(const float transform[16]);

  // Is it an empty set?
  bool IsEmpty() const;

 protected:
  // Can't instantiate directly. Use the Make*** functions above.
  Component();
  void Init(Mesh* mesh);

  // Get embedded Mesh instance.
  const Mesh* mesh() const;

 private:
  // Tessellator requires access to mesh().
  friend class Tessellator;
  // Component transform; defaults to identity.
  boost::scoped_ptr<AffineTransform3D> transform_;
  // The geometry.
  boost::scoped_ptr<Mesh> original_mesh_;
  // A copy of the geometry transform with transform_.
  mutable boost::scoped_ptr<Mesh> mesh_;
};

}  // namespace model
}  // namespace ginsu
#endif  // GINSU_MODEL_COMPONENT_H_
