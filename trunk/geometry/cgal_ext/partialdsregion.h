// Copyright (c) 2010 The Ginsu Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#ifndef GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_REGION_H_
#define GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_REGION_H_

#include <CGAL/basic.h>
#include "geometry/cgal_ext/partialdsentity.h"

namespace ginsu {
namespace geometry {

// PartialDSRegion: template class for region entity in the partial-entity
// data structure. The template parameters are:
//   TypeRefs: gives access to the types declared and used within the PartialDS
//             data structure. (See PartialDSTypes in partialds.h.)
template <class TypeRefs>
class PartialDSRegion : public PartialDSEntity<TypeRefs> {
 public:
  typedef PartialDSRegion<TypeRefs>            Self;
  typedef PartialDSEntity<TypeRefs>            Base;
  typedef TypeRefs                             PartialDS;

  typedef typename Base::RegionFlavor          RegionFlavor;
  typedef typename PartialDS::ShellHandle      ShellHandle;
  typedef typename PartialDS::ShellConstHandle ShellConstHandle;

  PartialDSRegion()
    : flavor_(Base::kEmptyRegion), outer_shell_(NULL) { }

  RegionFlavor flavor() const { return flavor_; }
  void set_falvor(RegionFlavor flavor) { flavor_ = flavor; }

  ShellConstHandle outer_shell() const { return outer_shell_; }
  void set_outer_shell(ShellHandle shell) { outer_shell_ = shell; }

 private:
  RegionFlavor flavor_;
  ShellHandle outer_shell_;  // The shell that encloses this region.
};

}  // namespace geometry
}  // namespace ginsu

#endif  // GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_REGION_H_
