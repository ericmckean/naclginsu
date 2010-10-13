// Copyright (c) 2010 The Ginsu Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_UTILS_INL_H_
#define GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_UTILS_INL_H_

namespace ginsu {
namespace geometry {

// A collection of utility functions, wrapped into a templated class. The
// template class Types should be something like PartialDSTypes. It is used
// to effectively convey entity types, such as VertexHandle, to the functions.
template <class Types>
class PartialDSUtils {
 public:
  typedef typename Types::VertexHandle  VertexHandle;
  typedef typename Types::PVertexHandle PVertexHandle;
  typedef typename Types::EdgeHandle    EdgeHandle;
  typedef typename Types::PEdgeHandle   PEdgeHandle;
  
  typedef typename Types::EdgeBase::PEdgeOfEdgeCirculator
                                        PEdgeOfEdgeCirculator;

  // Link vertex to one of the p-vertices and all p-vertices together into
  // a cloud around vertex.
  template <class PVertexList>
  static void LinkPVertices(VertexHandle vertex, PVertexList pvertices) {
    if (pvertices.empty()) return;

    // Link vertex to one of the p-vertices, link all the p-vertices together
    // as a loop around vertex and link all vertices to vertex.
    vertex->set_parent_pvertex(*pvertices.begin());
    typename PVertexList::iterator current = pvertices.begin();
    typename PVertexList::iterator next = current;
    ++next;
    while (current != pvertices.end()) {
      if (next != pvertices.end()) {
        (*current)->set_next_pvertex(*next);
      }
      (*current)->set_vertex(vertex);
      ++current;
      ++next;
    }
    // Link the loop of p-vertices around from last to first.
    (pvertices.back())->set_next_pvertex(pvertices.front());
  }

  // Link all p-edges together as a doubly-link list about edge, each p-edge
  // to edge as a child, and edge to one of the p-edges as parent.
  template <class PEdgeList>
  static void LinkRadialPEdges(EdgeHandle edge, PEdgeList pedges) {
    if (pedges.empty()) return;

    edge->set_parent_pedge(pedges.front());
    typename PEdgeList::iterator current = pedges.begin();
    typename PEdgeList::iterator next = current;
    ++next;
    while (current != pedges.end()) {
      if (next != pedges.end()) {
        (*current)->set_radial_next(*next);
        (*next)->set_radial_previous(*current);
      }
      (*current)->set_child_edge(edge);
      ++current;
      ++next;
    }
    // Link back to front to make a full loop.
    (pedges.back())->set_radial_next(pedges.front());
    (pedges.front())->set_radial_previous(pedges.back());
  }

  // A bundle of p-edges is such that all p-edges shared to same start/end
  // p-vertices, either as (start, end) or (end, start). All p-edges in a bundle
  // appear consecutively in the radial arrangment around an edge. This function
  // returns the first p-edge in a bundle around edge.
  static PEdgeOfEdgeCirculator FindRadialPEdgeBundle(EdgeHandle edge) {
    PEdgeOfEdgeCirculator start_pe = edge->pedge_begin();
    PEdgeOfEdgeCirculator pe = start_pe;
    PVertexHandle start_pv = pe->start_pvertex();
    do {
      if (pe->start_pvertex() != start_pv && pe->GetEndPVertex() != start_pv) {
        break;
      }
      ++pe;
    } while (pe != start_pe);
    return pe;
  }
};
}  // namespace geometry
}  // namespace ginsu

#endif  // GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_UTILS_INL_H_