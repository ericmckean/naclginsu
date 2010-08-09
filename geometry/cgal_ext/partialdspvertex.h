// Copyright (c) 2010 The Ginsu Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#ifndef GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_PVERTEX_H_
#define GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_PVERTEX_H_

#include <CGAL/basic.h>
#include "geometry/cgal_ext/partialdsentity.h"

namespace ginsu {
namespace geometry {

// PartialDSPVertex: template class forp-vertex entity in the partial-entity
// data structure. The template parameter is:
//   TypeRefs: gives access to the types declared and used within the PartialDS
//             data structure. (See PartialDSTypes in partialds.h.)
template <class TypeRefs>
class PartialDSPVertex : public PartialDSEntity<TypeRefs> {
 public:
  typedef PartialDSPVertex<TypeRefs>             Self;
  typedef TypeRefs                               PartialDS;

  typedef typename PartialDS::VertexHandle       VertexHandle;
  typedef typename PartialDS::PVertexHandle      PVertexHandle;
  typedef typename PartialDS::PVertexConstHandle PVertexConstHandle;

  PartialDSPVertex() { }

  // TODO(gwink): Declare this accessors once the corresponding types have been
  // defined.
  // EntityHandle parent();
  // EntityConstHandle parent() const;
  // void set_parent(EntityHandle parent);

 private:
  // TODO(gwink): Implement the parent pointer once the corresponding types
  // have been defined.
  // union parent {
  //  PEdgeHandle pedge_;
  //  EdgeHandle edge_;
  // };
  VertexHandle vertex_;  // The actual topological vertex.
  PVertexHandle next_;  // Next p-vertex associated with vertex_.
};

}  // namespace geometry
}  // namespace ginsu

#endif  // GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_PVERTEX_H_
