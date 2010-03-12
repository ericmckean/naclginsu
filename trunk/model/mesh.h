// Copyright (c) 2010 The Ginsu Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GINSU_MODEL_MESH_H_
#define GINSU_MODEL_MESH_H_

#include "model/kernel.h"
#include <CGAL/Nef_polyhedron_3.h>

namespace ginsu {
namespace model {

typedef CGAL::Nef_polyhedron_3<Kernel> Mesh;

}  // namespace model
}  // namespace ginsu
#endif  // GINSU_MODEL_MESH_H_
