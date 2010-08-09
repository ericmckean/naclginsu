// Copyright (c) 2010 The Ginsu Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#ifndef GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_PFACE_H_
#define GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_PFACE_H_

#include <CGAL/basic.h>
#include "geometry/cgal_ext/partialdsentity.h"

namespace ginsu {
namespace geometry {

// PartialDSFace: template class for face entity in the partial-entity
// data structure. The template parameters are:
//   TypeRefs: gives access to the types declared and used within the PartialDS
//             data structure. (See PartialDSTypes in partialds.h.)
//   TODO(gwink): add template param for surface geometry.
template <class TypeRefs>
class PartialDSPFace : public PartialDSEntity<TypeRefs> {
 public:
  typedef PartialDSPFace<TypeRefs>              Self;
  typedef TypeRefs                              PartialDS;

  typedef typename PartialDS::EntityHandle      EntityHandle;
  typedef typename PartialDS::EdgeHandle        EdgeHandle;
  typedef typename PartialDS::EdgeConstHandle   EdgeConstHandle;
  typedef typename PartialDS::FaceHandle        FaceHandle;
  typedef typename PartialDS::FaceConstHandle   FaceConstHandle;
  typedef typename PartialDS::LoopHandle        LoopHandle;
  typedef typename PartialDS::LoopConstHandle   LoopConstHandle;
  typedef typename PartialDS::PFaceHandle       PFaceHandle;
  typedef typename PartialDS::PFaceConstHandle  PFaceConstHandle;
  // TODO(gwink): define this once Sheel type has been declared.
  // typedef typename PartialDS::Shell             ShellHandle
  typedef typename PartialDS::VertexHandle      VertexHandle;
  typedef typename PartialDS::VertexConstHandle VertexConstHandle;

  enum Orientation {
    forward,  // p-face has same orientation as child face normal.
    reverse,  // p-face has opposite orientation from child face normal.
    undefined  // p-face's child is not a face.
  };

  PartialDSPFace() { }

  // TODO(gwink): Declare this accessors once the corresponding types have been
  // defined.
  // ShellConstHandle parent_shell() const;
  // void set_parent-shell(ShellHandle shell);

  // TODO(gwink): need to map from EntityHandle to derived-class handles, such
  // as VertexHandle.
  //FaceConstHandle child_face() const { return child_.face; }
  //void set_child_face(FaceHandle face) { child_.face = face; }
  //EdgeConstHandle child_edge() const { return child_.edge; }
  //void set_child_edge(EdgeHandle edge) { child_.edge = edge; }
  //VertexConstHandle child_vertex() const { return child_.vertex; }
  //void set_child_vertex(VertexHandle vertex) { child_.vertex = vertex; }

  PFaceConstHandle next_pface() const { return next_pface_; }
  void set_next_pface(PFaceHandle pface) { next_pface_ = pface; }

  PFaceConstHandle mate_pface() const { return mate_pface_; }
  void set_mate_pface(PFaceHandle pface) { mate_pface_ = pface; }

  Orientation orientation() const { return orientation_; }
  void set_orientation(Orientation orientation) {orientation_ = orientation; }

 private:
  // TODO(gwink): Implement the shell pointer once the corresponding types
  // have been defined.
  // ShellHandle parent_shell_;
  EntityHandle  child_;  // Child pointer, either a Face, Edge, or VertexHandle.
  PFaceHandle next_pface_;  // Next pface around the same shell.
  PFaceHandle mate_pface_;  // Mate pface on opposite side of child face.
  Orientation orientation_;  // Orientation relative to child face.
};

}  // namespace geometry
}  // namespace ginsu

#endif  // GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_PFACE_H_
