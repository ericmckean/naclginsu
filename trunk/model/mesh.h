// Copyright (c) 2010 The Ginsu Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GINSU_MODEL_MESH_H_
#define GINSU_MODEL_MESH_H_

#include "model/kernel.h"
#include <CGAL/Nef_polyhedron_3.h>

namespace ginsu {
namespace model {

// A mesh is a nef-polyhedron, which support robust boolean operations.
class Mesh : public CGAL::Nef_polyhedron_3<Kernel> {
 public:
  Mesh(Content space) : CGAL::Nef_polyhedron_3<Kernel>(space) {}
  Mesh(const CGAL::Nef_polyhedron_3<Kernel>& poly)
      : CGAL::Nef_polyhedron_3<Kernel>(poly) {}

 private:
  // Per Google style guide, disallow assigment operator.
  Mesh& operator=(const Mesh& mesh);
};

}  // namespace model
}  // namespace ginsu
#endif  // GINSU_MODEL_MESH_H_
