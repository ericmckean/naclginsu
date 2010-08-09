// Copyright (c) 2010 The Ginsu Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#ifndef GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_PEDGE_H_
#define GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_PEDGE_H_

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
  typedef TypeRefs                             PartialDS;

  typedef typename PartialDS::PEdgeHandle      PEdgeHandle;
  typedef typename PartialDS::PEdgeConstHandle PEdgeConstHandle;

  typedef typename PartialDS::EdgeHandle      EdgeHandle;
  typedef typename PartialDS::PVertexHandle   PVertexHandle;

  enum Orientation {
    reversed,  // p-edge direction is reversed from child edge.
    forward,  // p-edge follows same direction as child edge.
  };

  PartialDSPEdge() { }

  // TODO(gwink): Implement when LoopHandle type is available.
  // EntityConstHandle parent() const;
  // void set_parent(EntityHandle parent);

  // TODO(gwink): Child accessors; I'm not yet sure whether the type of the
  // child can be deduced from the context or it needs to be encoded here.
  // Defering implementation of child accessors until this is clearer.

  Orientation orientation() const { return orientation_; }
  void set_orientation(Orientation orientation) { orientation_ = orientation; }

  PEdgeConstHandle loop_previous() const { return loop_previous_; }
  void set_loop_previous(PEdgeHandle* pedge) { loop_previous_ = pedge; }
  PEdgeConstHandle loop_next() const { return loop_next_; }
  void set_loop_next(PEdgeHandle* pedge) { loop_next_ = pedge; }

  PEdgeConstHandle radial_previous() const { return radial_previous_; }
  void set_radial_previous(PEdgeHandle* pedge) { radial_previous_ = pedge; }
  PEdgeConstHandle radial_next() const { return radial_next_; }
  void set_radial_next(PEdgeHandle* pedge) { radial_next_ = pedge; }

 private:
  // TODO(gwink): Implement the parent pointer once the corresponding types
  // have been defined.
  // LoopHandle parent_;
  union child {
    EdgeHandle edge_;
    PVertexHandle pvertex_;
  };
  Orientation orientation_;
  PEdgeHandle loop_previous_;  // Previous p-edge in loop cycle.
  PEdgeHandle loop_next_;  // Next p-edge in loop cycle.
  PEdgeHandle radial_previous_;  // Previous p-edge in radial cycle.
  PEdgeHandle radial_next_;  // Next p-edge in radial cycle.
};

}  // namespace geometry
}  // namespace ginsu

#endif  // GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_PEDGE_H_
