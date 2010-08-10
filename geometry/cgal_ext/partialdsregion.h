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
  typedef TypeRefs                             PartialDS;

  typedef typename PartialDS::ShellHandle      ShellHandle;
  typedef typename PartialDS::ShellConstHandle ShellConstHandle;

  PartialDSRegion() { }

  ShellConstHandle shell() const { return shell_; }
  void set_shell(ShellHandle shell) { shell_ = shell; }

 private:
  ShellHandle shell_;  // The peripheral shell for this region.
};

}  // namespace geometry
}  // namespace ginsu

#endif  // GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_REGION_H_
