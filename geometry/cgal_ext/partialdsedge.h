// Copyright (c) 2010 The Ginsu Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#ifndef GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_EDGE_H_
#define GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_EDGE_H_

#include <CGAL/basic.h>
#include "geometry/cgal_ext/partialdscirculators.h"
#include "geometry/cgal_ext/partialdsentity.h"

namespace ginsu {
namespace geometry {

template <class T> class PartialDS;
template <class T> class PartialDSUtils;

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
  typedef TypeRefs                               PartialDSTypes;

  typedef typename PartialDSTypes::Entity             Entity;
  typedef typename PartialDSTypes::EdgeHandle         EdgeHandle;
  typedef typename PartialDSTypes::EdgeConstHandle    EdgeConstHandle;
  typedef typename PartialDSTypes::PEdgeHandle        PEdgeHandle;
  typedef typename PartialDSTypes::PEdgeConstHandle   PEdgeConstHandle;
  typedef typename PartialDSTypes::VertexHandle       VertexHandle;
  typedef typename PartialDSTypes::VertexConstHandle  VertexConstHandle;
  typedef typename PartialDSTypes::PVertexHandle      PVertexHandle;
  typedef typename PartialDSTypes::PVertexConstHandle PVertexConstHandle;

  typedef circulator::PEdgeRadialCirculator<PEdgeConstHandle>
                                                  PEdgeRadialConstCirculator;
  typedef circulator::PEdgeRadialCirculator<PEdgeHandle>
                                                  PEdgeRadialCirculator;

  PartialDSEdge()
    : parent_pedge_(NULL), start_pvertex_(NULL), end_pvertex_(NULL) { }

  // Accessors
  PEdgeConstHandle parent_pedge() const { return parent_pedge_; }
  PEdgeHandle parent_pedge() { return parent_pedge_; }
  PVertexConstHandle start_pvertex() const { return start_pvertex_; }
  PVertexHandle start_pvertex() { return start_pvertex_; }
  PVertexConstHandle end_pvertex() const { return end_pvertex_; }
  PVertexHandle end_pvertex() { return end_pvertex_; }

  // Return true if this is a wire edge and false otherwise.
  bool IsWireEdge() const {
    // Walk up to the pface and verify that it is degenerate (i.e. has no mate)
    // and check that the edge's start and end vertices are not the same (other-
    // wise, we're dealing with a degenerate edge about an isolated vertex).
    return parent_pedge()->parent_loop()->parent_face()->IsDegenerate() &&
           start_pvertex() != end_pvertex();
  }

  // Iterate over p-edges that have this edge as child.
  PEdgeRadialConstCirculator pedge_begin() const {
    return PEdgeRadialConstCirculator(parent_pedge_);
  }
  PEdgeRadialCirculator pedge_begin() {
    return PEdgeRadialCirculator(parent_pedge_);
  }

  // Return true if pe is a radial edge about this edge, and false otherwise.
  bool FindRadialPEdge(PEdgeConstHandle pe) const {
    return ginsu::geometry::circulator::find_if(pedge_begin(),
        std::bind2nd(std::equal_to<PEdgeConstHandle>(), pe)) != NULL;
  }

  // Return the edge that follows |this| edge across vertex. Note that this
  // function is most useful for vertex with exactly two incident edges. If
  // vertex has more than two incident edges, any of the edges may be returned
  // indeterminately. 
  EdgeHandle GetEdgeAcrossVertex(VertexConstHandle vertex) {
    // We need to follow the parent p-edge around the loop. But the direction we
    // need to follow depend on whether we want to get across the start or end
    // vertex and the parent p-edge's orientation. 
    PEdgeHandle parent_pe = parent_pedge();
    if (vertex == start_pvertex()->vertex()) {
      if (parent_pe->orientation() == Entity::kPEdgeForward) {
        return parent_pe->loop_previous()->child_edge();
      } else {
        return parent_pe->loop_next()->child_edge();
      } 
    } else if (vertex == end_pvertex()->vertex()) {
      if (parent_pe->orientation() == Entity::kPEdgeForward) {
        return parent_pe->loop_next()->child_edge();
      } else {
        return parent_pe->loop_previous()->child_edge();
      }
    } else {
      assert(!"Vertex is not connected to this edge.\n");
      return NULL;
    }
  }

 protected:
  friend class PartialDS<typename PartialDSTypes::Traits>;
  friend class PartialDSUtils<PartialDSTypes>;

  // Mutators
  void set_parent_pedge(PEdgeHandle pedge) { parent_pedge_ = pedge; }
  void set_start_pvertex(PVertexHandle pv) { start_pvertex_ = pv; }
  void set_end_pvertex(PVertexHandle pv) { end_pvertex_ = pv; }

 private:
  PEdgeHandle parent_pedge_;
  PVertexHandle start_pvertex_;
  PVertexHandle end_pvertex_;
};

}  // namespace geometry
}  // namespace ginsu

#endif  // GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_EDGE_H_
