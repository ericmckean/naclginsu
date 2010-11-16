// Copyright (c) 2010 The Ginsu Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#ifndef GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_SHELL_H_
#define GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_SHELL_H_

#include <CGAL/basic.h>
#include "geometry/cgal_ext/partialdsentity.h"

namespace ginsu {
namespace geometry {

template <class T> class PartialDS;
template <class T> class PartialDSRegion;

// PartialDSShell: template class for shell entity in the partial-entity
// data structure. The template parameters are:
//   TypeRefs: gives access to the types declared and used within the PartialDS
//             data structure. (See PartialDSTypes in partialds.h.)
template <class TypeRefs>
class PartialDSShell : public PartialDSEntity<TypeRefs> {
 public:
  typedef PartialDSShell<TypeRefs>              Self;
  typedef TypeRefs                              PartialDSTypes;

  typedef typename PartialDSTypes::RegionHandle      RegionHandle;
  typedef typename PartialDSTypes::RegionConstHandle RegionConstHandle;
  typedef typename PartialDSTypes::PFaceHandle       PFaceHandle;
  typedef typename PartialDSTypes::PFaceConstHandle  PFaceConstHandle;
  typedef typename PartialDSTypes::ShellHandle       ShellHandle;
  typedef typename PartialDSTypes::ShellConstHandle  ShellConstHandle;

  typedef circulator::PFaceOfShellCirculator<PFaceHandle>
                                                     PFaceCirculator;
  typedef circulator::PFaceOfShellCirculator<PFaceConstHandle>
                                                     PFaceConstCirculator;

  PartialDSShell()
    : parent_region_(NULL), next_void_shell_(NULL), pface_(NULL) { }

  // Accessors.
  ShellConstHandle next_void_shell() const { return next_void_shell_; }
  ShellHandle next_void_shell() { return next_void_shell_; }
  PFaceConstHandle pface() const { return pface_; }
  PFaceHandle pface() { return pface_; }
  RegionConstHandle parent_region() const { return parent_region_; }
  RegionHandle parent_region() { return parent_region_; }

  // A shell is empty if it has no p-face.
  bool IsEmpty() const { return pface() == NULL; }

  // Return a circulator for visiting the p-faces that form the shell.
  PFaceConstCirculator pface_begin() const {
    return PFaceConstCirculator(pface());
  }
  PFaceCirculator pface_begin() {
    return PFaceCirculator(pface());
  }

  // Return true if p-face pf is found around this shell and false otherwise.
  bool FindPFace(PFaceConstHandle pf) const {
    return ginsu::geometry::circulator::find_if(
        pface_begin(),
        std::bind2nd(std::equal_to<PFaceConstHandle>(), pf)) != NULL;
  }

  // Return true iff the shell is a void shell.
  bool IsVoidShell() const {
    // A shell is void if it's not the parent region's outershell.
    return &(*parent_region()->outer_shell()) != this;
  }

 protected:
  friend class PartialDS<typename PartialDSTypes::Traits>;

  // Mutators
  void set_next_void_shell(ShellHandle shell) { next_void_shell_ = shell; }
  void set_pface(PFaceHandle pface) { pface_ = pface; }
  void set_parent_region(RegionHandle region) { parent_region_ = region; }

 private:
  RegionHandle parent_region_;  // Parent region.
  ShellHandle next_void_shell_;  // Next void shell.
  PFaceHandle pface_;  // One of the pfaces forming this shell.
};

}  // namespace geometry
}  // namespace ginsu

#endif  // GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_SHELL_H_
