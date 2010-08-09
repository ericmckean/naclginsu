// Copyright (c) 2010 The Ginsu Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#ifndef GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_EDGE_H_
#define GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_EDGE_H_

#include <CGAL/basic.h>
#include "geometry/cgal_ext/partialdsentity.h"

namespace ginsu {
namespace geometry {

// PartialDSEdge: template class for edge entity in the partial-entity
// data structure. The template parameters are:
//   TypeRefs: gives access to the types declared and used within the PartialDS
//             data structure. (See PartialDSTypes in partialds.h.)
//   TODO(gwink): add template param for curve geometry.
template <class TypeRefs>
class PartialDSEdge : public PartialDSEntity<TypeRefs> {
 public:
  typedef PartialDSEdge<TypeRefs>             Self;
  typedef TypeRefs                            PartialDS;

  typedef typename PartialDS::EdgeHandle      EdgeHandle;
  typedef typename PartialDS::EdgeConstHandle EdgeConstHandle;

  typedef typename PartialDS::PVertexHandle   PVertexHandle;

  PartialDSEdge() { }

  // TODO(gwink): Declare this accessors once the corresponding types have been
  // defined.
  // EntityHandle parent();
  // EntityConstHandle parent() const;
  // void set_parent(EntityHandle parent);

 private:
  // TODO(gwink): Implement the parent pointer once the corresponding types
  // have been defined.
  // union parent {
  //  PFaceHandle pface_;
  //  PEdgeHandle pedge_;
  // };
  PVertexHandle start_;
  PVertexHandle end_;
};

}  // namespace geometry
}  // namespace ginsu

#endif  // GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_EDGE_H_
