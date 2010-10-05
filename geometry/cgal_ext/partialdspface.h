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

template <class T> class PartialDS;

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
  typedef TypeRefs                              PartialDSTypes;

  typedef typename Base::PFaceOrientation            PFaceOrientation;
  typedef typename PartialDSTypes::FaceHandle        FaceHandle;
  typedef typename PartialDSTypes::FaceConstHandle   FaceConstHandle;
  typedef typename PartialDSTypes::PFaceHandle       PFaceHandle;
  typedef typename PartialDSTypes::PFaceConstHandle  PFaceConstHandle;
  typedef typename PartialDSTypes::ShellHandle       ShellHandle;
  typedef typename PartialDSTypes::ShellConstHandle  ShellConstHandle;

  PartialDSPFace()
    : parent_shell_(NULL), child_face_(NULL), next_pface_(NULL),
      mate_pface_(NULL), orientation_() { }

  PFaceOrientation orientation() const { return orientation_; }
  void set_orientation(PFaceOrientation orientation) {
    orientation_ = orientation;
  }

  // Accessors
  ShellConstHandle parent_shell() const { return parent_shell_; }
  ShellHandle parent_shell() { return parent_shell_; }
  FaceConstHandle child_face() const { return child_face_; }
  PFaceConstHandle next_pface() const { return next_pface_; }
  PFaceConstHandle mate_pface() const { return mate_pface_; }

 protected:
  friend class PartialDS<typename PartialDSTypes::Traits>;

  void Init(PFaceOrientation orientation, ShellHandle parent_shell,
            FaceHandle child_face, PFaceHandle next_pface,
            PFaceHandle mate_pface) {
    orientation_ = orientation;
    parent_shell_ = parent_shell;
    child_face_ = child_face;
    next_pface_ = next_pface;
    mate_pface_ = mate_pface;
  }

  // Mutators
  void set_parent_shell(ShellHandle shell) { parent_shell_ = shell; }
  void set_child_face(FaceHandle face) { child_face_ = face; }
  void set_next_pface(PFaceHandle pface) { next_pface_ = pface; }
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
