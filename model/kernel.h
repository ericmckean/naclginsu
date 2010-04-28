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
typedef CGAL::Aff_transformation_3<Kernel> Aff_transformation_3;

}  // namespace model
}  // namespace ginsu
#endif  // GINSU_MODEL_KERNEL_H_
