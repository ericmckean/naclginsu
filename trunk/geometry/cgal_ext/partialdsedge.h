// Copyright (c) 2010 The Ginsu Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#ifndef GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_EDGE_H_
#define GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_EDGE_H_

#include <assert.h>
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

  typedef typename Base::EdgeFlavor              EdgeFlavor;
  typedef typename PartialDS::VariantHandle      VariantHandle;
  typedef typename PartialDS::EdgeHandle         EdgeHandle;
  typedef typename PartialDS::EdgeConstHandle    EdgeConstHandle;
  typedef typename PartialDS::PEdgeHandle        PEdgeHandle;
  typedef typename PartialDS::PEdgeConstHandle   PEdgeConstHandle;
  typedef typename PartialDS::PVertexHandle      PVertexHandle;
  typedef typename PartialDS::PVertexConstHandle PVertexConstHandle;
  typedef typename PartialDS::PFaceHandle        PFaceHandle;
  typedef typename PartialDS::PFaceConstHandle   PFaceConstHandle;

  PartialDSEdge() : parent_(NULL) { }

  EdgeFlavor flavor() const { return flavor_; }
  void set_flavor(EdgeFlavor flavor) { flavor_ = flavor; }

  PEdgeConstHandle GetPEdgeParent() const {
    assert(flavor() == Base::kNormalEdge);
    return PartialDS::AsPEdge(parent_);
  }
  PFaceConstHandle GetPFaceParent() const {
    assert(flavor() == Base::kWireEdge);
    return PartialDS::AsPFace(parent_);
  }
  void set_parent(VariantHandle parent) { parent_ = parent; }

  PVertexConstHandle start_vertex() const { return start_vertex_; }
  void set_start_vertex(PVertexHandle v) { start_vertex_ = v; }
  PVertexConstHandle end_vertex() const { return end_vertex_; }
  void set_end_vertex(PVertexHandle v) { end_vertex_ = v; }

 private:
  EdgeFlavor flavor_;
  VariantHandle parent_;  // Either a pface or a pedge.
  PVertexHandle start_vertex_;
  PVertexHandle end_vertex_;
};

}  // namespace geometry
}  // namespace ginsu

#endif  // GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_EDGE_H_
