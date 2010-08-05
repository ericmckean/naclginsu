// Copyright (c) 2010 The Ginsu Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GINSU_GEOMETRY_CGAL_EXT_PARTIALDSENTITY_H_
#define GINSU_GEOMETRY_CGAL_EXT_PARTIALDSENTITY_H_

#include <CGAL/basic.h>

namespace ginsu {
namespace geometry {

// PartialDSEntity: template class for an entity in the partial-entity data
// structure. It serves as a base class for all the entities, vertex, edges,
// etc. in the data structure. The template paramater is:
//   TypeRefs: gives access to the types declared and used within the PartialDS
//             data structure. (See PartialDSTypes in partialds.h.)
template <class TypeRefs>
class PartialDSEntity {
 public:
  typedef TypeRefs                  PartialDS;
  typedef PartialDSEntity<TypeRefs> Base;

 private:
  // TODO(gwink): We may need to store an ID and attributes for each entity.
  // If not, get rid of this private section.
};

}  // namespace geometry
}  // namespace ginsu


#endif  // GINSU_GEOMETRY_CGAL_EXT_PARTIALDSENTITY_H_
