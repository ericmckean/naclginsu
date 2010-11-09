// Copyright (c) 2010 The Ginsu Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#ifndef GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_FACE_H_
#define GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_FACE_H_

#include <CGAL/basic.h>
#include "geometry/cgal_ext/partialdsentity.h"

namespace ginsu {
namespace geometry {

template <class T> class PartialDS;

// PartialDSFace: template class for face entity in the partial-entity
// data structure. The template parameters are:
//   TypeRefs: gives access to the types declared and used within the PartialDS
//             data structure. (See PartialDSTypes in partialds.h.)
//   TODO(gwink): add template param for surface geometry.
template <class TypeRefs>
class PartialDSFace : public PartialDSEntity<TypeRefs> {
 public:
  typedef PartialDSFace<TypeRefs>              Self;
  typedef TypeRefs                             PartialDSTypes;

  typedef typename PartialDSTypes::PFaceHandle      PFaceHandle;
  typedef typename PartialDSTypes::PFaceConstHandle PFaceConstHandle;
  typedef typename PartialDSTypes::LoopHandle       LoopHandle;
  typedef typename PartialDSTypes::LoopConstHandle  LoopConstHandle;

  PartialDSFace()
    : parent_pface_(NULL), outer_loop_(NULL) { }

  // Accessors
  PFaceConstHandle parent_pface() const { return parent_pface_; }
  PFaceHandle parent_pface() { return parent_pface_; }
  LoopConstHandle outer_loop() const { return outer_loop_; }

  // Return true if this face is degenerate, i.e. associated with a wire edge or
  // isolated vertex.
  bool IsDegenerate() const {
    // Degenerate faces have a p-face with no mate.
    return parent_pface()->mate_pface() == NULL;
  }

 protected:
  friend class PartialDS<typename PartialDSTypes::Traits>;

  // Mutators
  void set_parent_pface(PFaceHandle pface) { parent_pface_ = pface; }
  void set_outer_loop(LoopHandle loop) { outer_loop_ = loop; }

 private:
  PFaceHandle parent_pface_;  // One of the incident pfaces.
  LoopHandle outer_loop_;  // The loop that forms the boundary of this face.
};

}  // namespace geometry
}  // namespace ginsu

#endif  // GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_FACE_H_
