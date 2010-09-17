// Copyright (c) 2010 The Ginsu Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#ifndef GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_SHELL_H_
#define GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_SHELL_H_

#include <CGAL/basic.h>
#include "geometry/cgal_ext/partialdsentity.h"

namespace ginsu {
namespace geometry {

// PartialDSShell: template class for shell entity in the partial-entity
// data structure. The template parameters are:
//   TypeRefs: gives access to the types declared and used within the PartialDS
//             data structure. (See PartialDSTypes in partialds.h.)
template <class TypeRefs>
class PartialDSShell : public PartialDSEntity<TypeRefs> {
 public:
  typedef PartialDSShell<TypeRefs>              Self;
  typedef TypeRefs                              PartialDS;

  typedef typename PartialDS::RegionHandle      RegionHandle;
  typedef typename PartialDS::RegionConstHandle RegionConstHandle;
  typedef typename PartialDS::PFaceHandle       PFaceHandle;
  typedef typename PartialDS::PFaceConstHandle  PFaceConstHandle;
  typedef typename PartialDS::ShellHandle       ShellHandle;
  typedef typename PartialDS::ShellConstHandle  ShellConstHandle;

  PartialDSShell()
    : parent_region_(NULL), next_void_shell_(NULL), pface_(NULL) { }

  ShellConstHandle next_void_shell() const { return next_void_shell_; }
  void set_next_void_shell(ShellHandle shell) { next_void_shell_ = shell; }
  
  PFaceConstHandle pface() const { return pface_; }
  void set_pface(PFaceHandle pface) { pface_ = pface; }

  RegionConstHandle parent_region() const { return parent_region_; }
  void set_parent_region(RegionHandle region) { parent_region_ = region; }

  // Add a shell to the list of void shells.
  void AddVoidShell(ShellHandle void_shell) {
    void_shell->set_next_void_shell(next_void_shell_);
    next_void_shell_ = void_shell;
  }

 private:
  RegionHandle parent_region_;  // Parent region.
  ShellHandle next_void_shell_;  // Next void shell.
  PFaceHandle pface_;  // One of the pfaces forming this shell.
};

}  // namespace geometry
}  // namespace ginsu

#endif  // GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_SHELL_H_
