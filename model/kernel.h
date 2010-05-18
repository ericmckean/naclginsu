// Copyright (c) 2010 The Ginsu Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GINSU_MODEL_KERNEL_H_
#define GINSU_MODEL_KERNEL_H_

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>

namespace ginsu {
namespace model {

// An exact kernel that supports cartesian coordinates. An exact kernel is
// required for boolean computations. The cartesian coordinates make it easier
// to interface with openSceneGraph.
typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel;

// Related types.
typedef Kernel::Point_3 Point_3;
typedef Kernel::Vector_3 Vector_3;


class AffineTransform3D : public CGAL::Aff_transformation_3<Kernel> {
 public:
  AffineTransform3D() {}
  AffineTransform3D(const CGAL::Aff_transformation_3<Kernel>& t)
      : CGAL::Aff_transformation_3<Kernel>(t) {}
  AffineTransform3D(const CGAL::Identity_transformation& tag)
      : CGAL::Aff_transformation_3<Kernel>(tag) {}
  AffineTransform3D(const CGAL::Translation tag, const Kernel::Vector_3& v)
      : CGAL::Aff_transformation_3<Kernel>(tag, v) {}
  AffineTransform3D(const CGAL::Scaling tag,
                    const Kernel::RT& s,
                    const Kernel::RT& w = Kernel::RT(1.0f))
      : CGAL::Aff_transformation_3<Kernel>(tag, s, w) {}

  void Set(float m11, float m12, float m13, float m14,
           float m21, float m22, float m23, float m24,
           float m31, float m32, float m33, float m34) {
    *this = CGAL::Aff_transformation_3<Kernel>(m11, m12, m13, m14,
                                               m21, m22, m23, m24,
                                               m31, m32, m33, m34, 1.0f);
  }
};

}  // namespace model
}  // namespace ginsu
#endif  // GINSU_MODEL_KERNEL_H_
