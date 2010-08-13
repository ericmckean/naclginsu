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
  enum VertexFlavor {
    kNormalVertex,  // A normal vertex associated with a face.
    kIsolatedVertex,  // An isolated vertex without face.
  };

  enum EdgeFlavor {
    kNormalEdge,  // A normal edge associated with a loop.
    kWireEdge,  // A wire edge without face.
  };

  enum PEdgeOrientation {
    kPEdgeReversed,  // p-edge direction is reversed from child edge.
    kPEdgeForward,  // p-edge follows same direction as child edge.
    kPEdgeUnoriented,  // special p-edge associated with an isolated vertex.
  };

  enum PFaceOrientation {
    kPFaceForward,  // p-face has same orientation as child face normal.
    kPFaceReversed,  // p-face has opposite orientation from child face normal.
    kPFaceWireEdge,  // p-face is associated with a wire edge.
    kPFaceIsolatedVertex,  // p-face is associated with an isolated.
  };
  
  enum RegionFlavor {
    kFilledRegion,
    kEmptyRegion,
  };

  typedef TypeRefs                  PartialDS;
  typedef PartialDSEntity<TypeRefs> Base;

 private:
  // TODO(gwink): We may need to store an ID and attributes for each entity.
  // If not, get rid of this private section.
};

}  // namespace geometry
}  // namespace ginsu


#endif  // GINSU_GEOMETRY_CGAL_EXT_PARTIALDSENTITY_H_
