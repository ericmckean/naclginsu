// Copyright (c) 2010 The Ginsu Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#ifndef GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_LOOP_H_
#define GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_LOOP_H_

#include <CGAL/basic.h>
#include "geometry/cgal_ext/partialdscirculators.h"
#include "geometry/cgal_ext/partialdsentity.h"

namespace ginsu {
namespace geometry {

template <class T> class PartialDS;

// PartialDSLoop: template class for loop entity in the partial-entity
// data structure. The template parameter is:
//   TypeRefs: gives access to the types declared and used within the PartialDS
//             data structure. (See PartialDSTypes in partialds.h.)
template <class TypeRefs>
class PartialDSLoop : public PartialDSEntity<TypeRefs> {
 public:
  typedef PartialDSLoop<TypeRefs>              Self;
  typedef TypeRefs                             PartialDSTypes;

  typedef typename PartialDSTypes::VertexConstHandle  VertexConstHandle;
  typedef typename PartialDSTypes::PVertexConstHandle PVertexConstHandle;
  typedef typename PartialDSTypes::PEdgeHandle        PEdgeHandle;
  typedef typename PartialDSTypes::PEdgeConstHandle   PEdgeConstHandle;
  typedef typename PartialDSTypes::FaceHandle         FaceHandle;
  typedef typename PartialDSTypes::FaceConstHandle    FaceConstHandle;
  typedef typename PartialDSTypes::LoopHandle         LoopHandle;
  typedef typename PartialDSTypes::LoopConstHandle    LoopConstHandle;

  typedef circulator::PEdgeLoopCirculator<PEdgeHandle>
                                                      PEdgeCirculator;
  typedef circulator::PEdgeLoopCirculator<PEdgeConstHandle>
                                                      PEdgeConstCirculator;

  PartialDSLoop()
    : parent_face_(NULL), boundary_pedge_(NULL), next_hole_(NULL) { }

  // Accessors
  FaceConstHandle parent_face() const { return parent_face_; }
  FaceHandle parent_face() { return parent_face_; }
  PEdgeConstHandle boundary_pedge() const { return boundary_pedge_; }
  LoopConstHandle next_hole() const { return next_hole_; }

  // Iterate over p-edges that form the loop.
  PEdgeConstCirculator pedge_begin() const {
    return PEdgeConstCirculator(boundary_pedge_);
  }
  PEdgeCirculator pedge_begin() {
    return PEdgeCirculator(boundary_pedge_);
  }

  // Return true if p-edge pe is found along this loop and false otherwise.
  bool FindPEdge(PEdgeConstHandle pe) const {
    return ginsu::geometry::circulator::find_if(pedge_begin(),
        std::bind2nd(std::equal_to<PEdgeConstHandle>(), pe)) != NULL;
  }

  // Return the p-edge that starts at vertex or NULL.
  struct equal_to_start_vertex {
    VertexConstHandle start_vertex_;
    explicit equal_to_start_vertex(VertexConstHandle start_vertex)
        : start_vertex_(start_vertex) {}
    bool operator() (PEdgeConstHandle pe) {
      return pe->start_pvertex()->vertex() == start_vertex_;
    }
  };
  PEdgeHandle FindStartVertex(VertexConstHandle vertex) {
    return ginsu::geometry::circulator::find_if(
        pedge_begin(), equal_to_start_vertex(vertex));
  }

 protected:
  friend class PartialDS<typename PartialDSTypes::Traits>;

  void Init(FaceHandle parent_face, PEdgeHandle boundary_pedge,
            LoopHandle next_hole) {
    parent_face_ = parent_face;
    boundary_pedge_ = boundary_pedge;
    next_hole_ = next_hole;
  }

  // Mutators
  void set_parent_face(FaceHandle face) { parent_face_ = face; }
  void set_boundary_pedge(PEdgeHandle pedge) { boundary_pedge_ = pedge; }
  void set_next_hole(LoopHandle loop) { next_hole_ = loop; }

 private:
  FaceHandle parent_face_;  // Parent face.
  PEdgeHandle boundary_pedge_;  // A pedge along the boundary.
  LoopHandle next_hole_;  // Next hole loop.
};

}  // namespace geometry
}  // namespace ginsu

#endif  // GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_LOOP_H_
