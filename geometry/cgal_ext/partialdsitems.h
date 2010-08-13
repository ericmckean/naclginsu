// Copyright (c) 2010 The Ginsu Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GINSU_GEOMETRY_CGAL_EXT_PARTIALDSITEMS_H_
#define GINSU_GEOMETRY_CGAL_EXT_PARTIALDSITEMS_H_

#include <CGAL/basic.h>
#include "geometry/cgal_ext/partialdsentity.h"
#include "geometry/cgal_ext/partialdsedge.h"
#include "geometry/cgal_ext/partialdsface.h"
#include "geometry/cgal_ext/partialdsloop.h"
#include "geometry/cgal_ext/partialdspedge.h"
#include "geometry/cgal_ext/partialdspface.h"
#include "geometry/cgal_ext/partialdspvertex.h"
#include "geometry/cgal_ext/partialdsregion.h"
#include "geometry/cgal_ext/partialdsshell.h"
#include "geometry/cgal_ext/partialdsvertex.h"

namespace ginsu {
namespace geometry {

// Class PartialDSItems encapsulates the items, vertex, partial-vertex, etc.
// that make up the partial-entity data structure. 
class PartialDSItems {
 public:
  template <class TypeRefs>
  struct EntityWrapper {
      typedef PartialDSEntity<TypeRefs> Entity;
  };

  template <class TypeRefs, class Traits>
  struct VertexWrapper {
    typedef typename Traits::Point_3 Point;
    typedef PartialDSVertex<TypeRefs, Point> Vertex;
  };

  template <class TypeRefs, class Traits>
  struct PVertexWrapper {
    typedef PartialDSPVertex<TypeRefs> PVertex;
  };

  template <class TypeRefs, class Traits>
  struct EdgeWrapper {
    // TODO(gwink) add reference to curve geometry.
    typedef PartialDSEdge<TypeRefs> Edge;
  };

  template <class TypeRefs, class Traits>
  struct PEdgeWrapper {
    typedef PartialDSPEdge<TypeRefs> PEdge;
  };

  template <class TypeRefs, class Traits>
  struct LoopWrapper {
    typedef PartialDSLoop<TypeRefs> Loop;
  };

  template <class TypeRefs, class Traits>
  struct FaceWrapper {
    // TODO(gwink) add reference to surface geometry.
    typedef PartialDSFace<TypeRefs> Face;
  };

  template <class TypeRefs, class Traits>
  struct PFaceWrapper {
    typedef PartialDSPFace<TypeRefs> PFace;
  };

  template <class TypeRefs, class Traits>
  struct ShellWrapper {
    typedef PartialDSShell<TypeRefs> Shell;
  };

  template <class TypeRefs, class Traits>
  struct RegionWrapper {
    typedef PartialDSRegion<TypeRefs> Region;
  };
};

}  // namespace geometry
}  // namespace ginsu


#endif  // GINSU_GEOMETRY_CGAL_EXT_PARTIALDSITEMS_H_
