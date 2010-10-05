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

template <class T> class PartialDS;

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
  typedef TypeRefs                             PartialDSTypes;

  typedef typename Base::PEdgeOrientation             PEdgeOrientation;
  typedef typename PartialDSTypes::PVertexHandle      PVertexHandle;
  typedef typename PartialDSTypes::PVertexConstHandle PVertexConstHandle;
  typedef typename PartialDSTypes::EdgeHandle         EdgeHandle;
  typedef typename PartialDSTypes::EdgeConstHandle    EdgeConstHandle;
  typedef typename PartialDSTypes::PEdgeHandle        PEdgeHandle;
  typedef typename PartialDSTypes::PEdgeConstHandle   PEdgeConstHandle;
  typedef typename PartialDSTypes::LoopHandle         LoopHandle;
  typedef typename PartialDSTypes::LoopConstHandle    LoopConstHandle;

  PartialDSPEdge()
    : orientation_(Base::kPEdgeUnoriented), child_edge_(NULL),
      start_pvertex_(NULL), loop_previous_(NULL), loop_next_(NULL),
      radial_previous_(NULL), radial_next_(NULL) { }

  PEdgeOrientation orientation() const { return orientation_; }
  void set_orientation(PEdgeOrientation orientation) {
    orientation_ = orientation;
  }

  // Accessors
  LoopConstHandle parent_loop() const { return parent_loop_; }
  LoopHandle parent_loop() { return parent_loop_; }
  EdgeConstHandle child_edge() const { return child_edge_; }
  PVertexConstHandle start_pvertex() const { return start_pvertex_; }
  PEdgeConstHandle loop_previous() const { return loop_previous_; }
  PEdgeConstHandle loop_next() const { return loop_next_; }
  PEdgeConstHandle radial_previous() const { return radial_previous_; }
  PEdgeConstHandle radial_next() const { return radial_next_; }

 protected:
  friend class PartialDS<typename PartialDSTypes::Traits>;

  void Init(PEdgeOrientation orientation, LoopHandle parent_loop,
            EdgeHandle child_edge, PVertexHandle start_pvertex,
            PEdgeHandle loop_previous, PEdgeHandle loop_next,
            PEdgeHandle radial_previous, PEdgeHandle radial_next) {
    orientation_ = orientation;
    parent_loop_ = parent_loop;
    child_edge_ = child_edge;
    start_pvertex_ = start_pvertex;
    loop_previous_ = loop_previous;
    loop_next_ = loop_next;
    radial_previous_ = radial_previous;
    radial_next_ = radial_next;
  }

  // Mutators
  void set_parent_loop(LoopHandle loop) { parent_loop_ = loop; }
  void set_child_edge(EdgeHandle edge) { child_edge_ = edge; }
  void set_start_pvertex(PVertexHandle pvertex) { start_pvertex_ = pvertex; }
  void set_loop_previous(PEdgeHandle pedge) { loop_previous_ = pedge; }
  void set_loop_next(PEdgeHandle pedge) { loop_next_ = pedge; }
  void set_radial_previous(PEdgeHandle pedge) { radial_previous_ = pedge; }
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
