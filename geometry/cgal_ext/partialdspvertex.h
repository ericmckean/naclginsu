// Copyright (c) 2010 The Ginsu Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#ifndef GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_PVERTEX_H_
#define GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_PVERTEX_H_

#include "CGAL/basic.h"
#include "geometry/cgal_ext/partialdsentity.h"

namespace ginsu {
namespace geometry {

template <class T> class PartialDS;

// PartialDSPVertex: template class for p-vertex entity in the partial-entity
// data structure. The template parameter is:
//   TypeRefs: gives access to the types declared and used within the PartialDS
//             data structure. (See PartialDSTypes in partialds.h.)
template <class TypeRefs>
class PartialDSPVertex : public PartialDSEntity<TypeRefs> {
 public:
  typedef PartialDSPVertex<TypeRefs>             Self;
  typedef PartialDSEntity<TypeRefs>              Base;
  typedef TypeRefs                               PartialDSTypes;

  typedef typename PartialDSTypes::VertexHandle       VertexHandle;
  typedef typename PartialDSTypes::VertexConstHandle  VertexConstHandle;
  typedef typename PartialDSTypes::PVertexHandle      PVertexHandle;
  typedef typename PartialDSTypes::PVertexConstHandle PVertexConstHandle;
  typedef typename PartialDSTypes::EdgeHandle         EdgeHandle;
  typedef typename PartialDSTypes::EdgeConstHandle    EdgeConstHandle;

  PartialDSPVertex()
    : parent_edge_(NULL), vertex_(NULL), next_pvertex_(NULL) { }

  // Accessors
  EdgeConstHandle parent_edge() const { return parent_edge_; }
  EdgeHandle parent_edge() { return parent_edge_; }
  VertexConstHandle vertex() const { return vertex_; }
  PVertexConstHandle next_pvertex() const { return next_pvertex_; }
  
 protected:
  friend class PartialDS<typename PartialDSTypes::Traits>;

  // Mutators
  void set_parent_edge(EdgeHandle edge) { parent_edge_ = edge; }
  void set_vertex(VertexHandle vertex) { vertex_ = vertex; }
  void set_next_pvertex(PVertexHandle pvertex) { next_pvertex_ = pvertex; }

 private:
  EdgeHandle parent_edge_;  // Parent edge.
  VertexHandle vertex_;  // The actual topological vertex.
  PVertexHandle next_pvertex_;  // Next p-vertex associated with vertex_.
};

}  // namespace geometry
}  // namespace ginsu

#endif  // GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_PVERTEX_H_
