// Copyright (c) 2010 The Ginsu Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#ifndef GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_PEDGE_H_
#define GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_PEDGE_H_

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
class PartialDSPEdge : public PartialDSEntity<TypeRefs> {
 public:
  typedef PartialDSPEdge<TypeRefs>             Self;
  typedef PartialDSEntity<TypeRefs>            Base;
  typedef TypeRefs                             PartialDS;

  typedef typename Base::PEdgeOrientation        PEdgeOrientation;
  typedef typename PartialDS::PVertexHandle      PVertexHandle;
  typedef typename PartialDS::PVertexConstHandle PVertexConstHandle;
  typedef typename PartialDS::EdgeHandle         EdgeHandle;
  typedef typename PartialDS::EdgeConstHandle    EdgeConstHandle;
  typedef typename PartialDS::PEdgeHandle        PEdgeHandle;
  typedef typename PartialDS::PEdgeConstHandle   PEdgeConstHandle;
  typedef typename PartialDS::LoopHandle         LoopHandle;
  typedef typename PartialDS::LoopConstHandle    LoopConstHandle;

  PartialDSPEdge()
    : orientation_(Base::kPEdgeUnoriented), child_edge_(NULL),
      start_pvertex_(NULL), loop_previous_(NULL), loop_next_(NULL),
      radial_previous_(NULL), radial_next_(NULL) { }

  PEdgeOrientation orientation() const { return orientation_; }
  void set_orientation(PEdgeOrientation orientation) {
    orientation_ = orientation;
  }

  LoopConstHandle parent_loop() const { return parent_loop_; }
  void set_parent_loop(LoopHandle loop) { parent_loop_ = loop; }

  EdgeConstHandle child_edge() const { return child_edge_; }
  void set_child_edge(EdgeHandle edge) { child_edge_ = edge; }

  PVertexConstHandle start_pvertex() const { return start_pvertex_; }
  void set_start_pvertex(PVertexHandle pvertex) { start_pvertex_ = pvertex; }

  PEdgeConstHandle loop_previous() const { return loop_previous_; }
  void set_loop_previous(PEdgeHandle pedge) { loop_previous_ = pedge; }
  PEdgeConstHandle loop_next() const { return loop_next_; }
  void set_loop_next(PEdgeHandle pedge) { loop_next_ = pedge; }

  PEdgeConstHandle radial_previous() const { return radial_previous_; }
  void set_radial_previous(PEdgeHandle pedge) { radial_previous_ = pedge; }
  PEdgeConstHandle radial_next() const { return radial_next_; }
  void set_radial_next(PEdgeHandle pedge) { radial_next_ = pedge; }

 private:
  PEdgeOrientation orientation_;
  LoopHandle parent_loop_;
  EdgeHandle child_edge_;
  PVertexHandle start_pvertex_;  // Start pvertex for this pedge
  PEdgeHandle loop_previous_;  // Previous p-edge in loop cycle.
  PEdgeHandle loop_next_;  // Next p-edge in loop cycle.
  PEdgeHandle radial_previous_;  // Previous p-edge in radial cycle.
  PEdgeHandle radial_next_;  // Next p-edge in radial cycle.
};

}  // namespace geometry
}  // namespace ginsu

#endif  // GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_PEDGE_H_
