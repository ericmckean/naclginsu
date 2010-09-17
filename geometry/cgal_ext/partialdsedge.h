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
  typedef PartialDSEdge<TypeRefs>                Self;
  typedef PartialDSEntity<TypeRefs>              Base;
  typedef TypeRefs                               PartialDS;

  typedef typename PartialDS::EdgeHandle         EdgeHandle;
  typedef typename PartialDS::EdgeConstHandle    EdgeConstHandle;
  typedef typename PartialDS::PEdgeHandle        PEdgeHandle;
  typedef typename PartialDS::PEdgeConstHandle   PEdgeConstHandle;
  typedef typename PartialDS::PVertexHandle      PVertexHandle;
  typedef typename PartialDS::PVertexConstHandle PVertexConstHandle;

  PartialDSEdge()
    : parent_pedge_(NULL), start_pvertex_(NULL), end_pvertex_(NULL) { }

  PEdgeConstHandle parent_pedge() const { return parent_pedge_; }
  void set_parent_pedge(PEdgeHandle pedge) { parent_pedge_ = pedge; }

  PVertexConstHandle start_pvertex() const { return start_pvertex_; }
  void set_start_pvertex(PVertexHandle pv) { start_pvertex_ = pv; }
  PVertexConstHandle end_pvertex() const { return end_pvertex_; }
  void set_end_pvertex(PVertexHandle pv) { end_pvertex_ = pv; }

 private:
  PEdgeHandle parent_pedge_;
  PVertexHandle start_pvertex_;
  PVertexHandle end_pvertex_;
};

}  // namespace geometry
}  // namespace ginsu

#endif  // GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_EDGE_H_
