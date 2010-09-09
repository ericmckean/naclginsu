// Copyright (c) 2010 The Ginsu Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#ifndef GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_PFACE_H_
#define GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_PFACE_H_

#include <assert.h>
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
  typedef PartialDSEntity<TypeRefs>             Base;
  typedef TypeRefs                              PartialDS;

  typedef typename Base::PFaceOrientation       PFaceOrientation;
  typedef typename PartialDS::FaceHandle        FaceHandle;
  typedef typename PartialDS::FaceConstHandle   FaceConstHandle;
  typedef typename PartialDS::PFaceHandle       PFaceHandle;
  typedef typename PartialDS::PFaceConstHandle  PFaceConstHandle;
  typedef typename PartialDS::ShellHandle       ShellHandle;
  typedef typename PartialDS::ShellConstHandle  ShellConstHandle;

  PartialDSPFace()
    : parent_shell_(NULL), child_face_(NULL), next_pface_(NULL),
      mate_pface_(NULL), orientation_() { }

  PFaceOrientation orientation() const { return orientation_; }
  void set_orientation(PFaceOrientation orientation) {
    orientation_ = orientation;
  }

  ShellConstHandle parent_shell() const { return parent_shell_; }
  void set_parent_shell(ShellHandle shell) { parent_shell_ = shell; }

  FaceConstHandle child_face() const { return child_face_; }
  void set_child_face(FaceHandle face) { child_face_ = face; }

  PFaceConstHandle next_pface() const { return next_pface_; }
  void set_next_pface(PFaceHandle pface) { next_pface_ = pface; }

  PFaceConstHandle mate_pface() const { return mate_pface_; }
  void set_mate_pface(PFaceHandle pface) { mate_pface_ = pface; }

 private:
  ShellHandle parent_shell_;
  FaceHandle child_face_;
  PFaceHandle next_pface_;  // Next pface around the same shell.
  PFaceHandle mate_pface_;  // Mate pface on opposite side of child face.
  PFaceOrientation orientation_;  // Orientation relative to child face.
};

}  // namespace geometry
}  // namespace ginsu

#endif  // GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_PFACE_H_