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

// PartialDSLoop: template class for loop entity in the partial-entity
// data structure. The template parameter is:
//   TypeRefs: gives access to the types declared and used within the PartialDS
//             data structure. (See PartialDSTypes in partialds.h.)
template <class TypeRefs>
class PartialDSLoop : public PartialDSEntity<TypeRefs> {
 public:
  typedef PartialDSLoop<TypeRefs>              Self;
  typedef TypeRefs                             PartialDS;

  typedef typename PartialDS::PEdgeHandle      PEdgeHandle;
  typedef typename PartialDS::PEdgeConstHandle PEdgeConstHandle;
  typedef typename PartialDS::FaceHandle       FaceHandle;
  typedef typename PartialDS::FaceConstHandle  FaceConstHandle;
  typedef typename PartialDS::LoopHandle       LoopHandle;
  typedef typename PartialDS::LoopConstHandle  LoopConstHandle;

  typedef PEdgeLoopCirculator<PEdgeConstHandle>
                                         PEdgeAroundLoopConstCirculator;

  PartialDSLoop()
    : parent_face_(NULL), boundary_pedge_(NULL), next_hole_(NULL) { }

  FaceConstHandle parent_face() const { return parent_face_; }
  void set_parent_face(FaceHandle face) { parent_face_ = face; }

  PEdgeConstHandle boundary_pedge() const { return boundary_pedge_; }
  void set_boundary_pedge(PEdgeHandle pedge) { boundary_pedge_ = pedge; }

  LoopConstHandle next_hole() const { return next_hole_; }
  void set_next_hole(LoopHandle loop) { next_hole_ = loop; }

  // Iterate over p-edges that form the loop.
  PEdgeAroundLoopConstCirculator begin() const {
    return PEdgeAroundLoopConstCirculator(boundary_pedge_);
  }

  // Return true if p-edge pe is found along this loop and false otherwise.
  bool FindPEdge(PEdgeConstHandle pe) const {
    if (pe == NULL) return false;

    PEdgeAroundLoopConstCirculator i = begin();
    PEdgeAroundLoopConstCirculator start = i;
    do {
      if (i++ == PEdgeAroundLoopConstCirculator(pe)) return true;
    } while(i != start);
    return false;
  }

 private:
  FaceHandle parent_face_;  // Parent face.
  PEdgeHandle boundary_pedge_;  // A pedge along the boundary.
  LoopHandle next_hole_;  // Next hole loop.
};

}  // namespace geometry
}  // namespace ginsu

#endif  // GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_LOOP_H_
