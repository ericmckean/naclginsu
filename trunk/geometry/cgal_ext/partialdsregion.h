// Copyright (c) 2010 The Ginsu Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#ifndef GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_REGION_H_
#define GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_REGION_H_

#include <CGAL/basic.h>
#include "geometry/cgal_ext/partialdsentity.h"
#include "geometry/cgal_ext/partialdsiterators.h"

namespace ginsu {
namespace geometry {

template <class T> class PartialDS;

// PartialDSRegion: template class for region entity in the partial-entity
// data structure. The template parameters are:
//   TypeRefs: gives access to the types declared and used within the PartialDS
//             data structure. (See PartialDSTypes in partialds.h.)
template <class TypeRefs>
class PartialDSRegion : public PartialDSEntity<TypeRefs> {
 public:
  typedef PartialDSRegion<TypeRefs>            Self;
  typedef PartialDSEntity<TypeRefs>            Base;
  typedef TypeRefs                             PartialDSTypes;

  typedef typename Base::RegionFlavor               RegionFlavor;
  typedef typename PartialDSTypes::ShellHandle      ShellHandle;
  typedef typename PartialDSTypes::ShellConstHandle ShellConstHandle;

  typedef VoidShellIterator<ShellConstHandle> VoidShellConstIterator;

  PartialDSRegion()
    : flavor_(Base::kEmptyRegion), outer_shell_(NULL) { }

  // Accessors
  RegionFlavor flavor() const { return flavor_; }
  ShellConstHandle outer_shell() const { return outer_shell_; }
  ShellHandle outer_shell() { return outer_shell_; }

  // Return true if the region is empty and false otherwise.
  bool IsEmpty() {
    // The region is empty either if it has no shell or only an outer shell.
    return (outer_shell_ == NULL || outer_shell()->next_void_shell() == NULL);
  }

  // Iterating over the void shells attached to this region.
  VoidShellConstIterator void_shell_begin() const { 
    return VoidShellConstIterator(outer_shell()->next_void_shell());
  }
  VoidShellConstIterator void_shell_end() const { return VoidShellConstIterator(); }

  // Find whether void_shell is found in the list of void shells attached to
  // this region's outer shell.
  bool FindVoidShell(ShellConstHandle void_shell) const {
    VoidShellConstIterator i;
    for (i = void_shell_begin(); i != void_shell_end(); ++i) {
      if (i == void_shell) return true;
    }
    return false;
  }

 protected:
  friend class PartialDS<typename PartialDSTypes::Traits>;

  // Mutators
  void set_flavor(RegionFlavor flavor) { flavor_ = flavor; }
  void set_outer_shell(ShellHandle shell) { outer_shell_ = shell; }

 private:
  RegionFlavor flavor_;
  ShellHandle outer_shell_;  // The shell that encloses this region.
};

}  // namespace geometry
}  // namespace ginsu

#endif  // GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_REGION_H_
