// Copyright (c) 2010 The Ginsu Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#ifndef GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_FACE_H_
#define GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_FACE_H_

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
class PartialDSFace : public PartialDSEntity<TypeRefs> {
 public:
  typedef PartialDSFace<TypeRefs>              Self;
  typedef TypeRefs                             PartialDS;

  typedef typename PartialDS::PFaceHandle      PFaceHandle;
  typedef typename PartialDS::PFaceConstHandle PFaceConstHandle;
  typedef typename PartialDS::LoopHandle       LoopHandle;
  typedef typename PartialDS::LoopConstHandle  LoopConstHandle;

  PartialDSFace() { }

  PFaceConstHandle incident_pface() const { return incident_pface_; }
  void set_incident_pface(PFaceHandle pface) { incident_pface_ = pface; }

  LoopConstHandle boundary() const { return boundary_; }
  void set_boundary(LoopHandle loop) { boundary_ = loop; }

 private:
  PFaceHandle incident_pface_;  // One of the incident pfaces.
  LoopHandle boundary_;  // The loop that forms the boundary of this face.
};

}  // namespace geometry
}  // namespace ginsu

#endif  // GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_FACE_H_
