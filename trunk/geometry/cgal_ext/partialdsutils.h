// Copyright (c) 2010 The Ginsu Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_UTILS_INL_H_
#define GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_UTILS_INL_H_

#include <boost/tr1/unordered_set.hpp>

namespace ginsu {
namespace geometry {

// A collection of utility functions, wrapped into a templated class. The
// template class Types should be something like PartialDSTypes. It is used
// to effectively convey entity types, such as VertexHandle, to the functions.
template <class Types>
class PartialDSUtils {
 public:
  // Global PartialDS types.
  typedef typename Types::VertexHandle     VertexHandle;
  typedef typename Types::PVertexHandle    PVertexHandle;
  typedef typename Types::EdgeConstHandle  EdgeConstHandle;
  typedef typename Types::EdgeHandle       EdgeHandle;
  typedef typename Types::PFaceConstHandle PFaceConstHandle;
  typedef typename Types::PFaceHandle      PFaceHandle;
  typedef typename Types::PEdgeHandle      PEdgeHandle;
  typedef typename Types::ShellHandle      ShellHandle;
  typedef typename Types::RegionHandle     RegionHandle;
  
  typedef typename Types::EdgeBase::PEdgeRadialCirculator
                                           PEdgeRadialCirculator;
  typedef typename Types::VertexBase::PVertexCirculator
                                           PVertexOfVertexCirculator;

  // A hash-set for EdgeHandle; used for traversals.
  struct HashEdgeHandle {
      size_t operator()(EdgeConstHandle e) const {
          return reinterpret_cast<size_t>(&(*e));
      }
  };
  typedef std::tr1::unordered_set<EdgeHandle, HashEdgeHandle> EdgeHandleSet;

  // LinkPVertices:
  // Link vertex to one of the p-vertices and all p-vertices together into
  // a cloud around vertex.
  template <class PVertexList>
  static void LinkPVertices(VertexHandle vertex, PVertexList pvertices) {
    if (pvertices.empty()) return;

    // Link vertex to one of the p-vertices, link all the p-vertices together
    // as a loop around vertex and link all vertices to vertex.
    vertex->set_parent_pvertex(*pvertices.begin());
    typename PVertexList::iterator current = pvertices.begin();
    typename PVertexList::iterator next = current + 1;
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

  // LinkRadialPEdges:
  // Link all p-edges together as a doubly-link list about edge, each p-edge
  // to edge as a child, and edge to one of the p-edges as parent.
  template <class PEdgeList>
  static void LinkRadialPEdges(EdgeHandle edge, PEdgeList pedges) {
    if (pedges.empty()) return;

    edge->set_parent_pedge(pedges.front());
    typename PEdgeList::iterator current = pedges.begin();
    typename PEdgeList::iterator next = current + 1;
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

  // FindRadialPEdgeBundle:
  // A bundle of p-edges is such that all p-edges shared to same start/end
  // p-vertices, either as (start, end) or (end, start). All p-edges in a bundle
  // appear consecutively in the radial arrangment around an edge. This function
  // returns the first p-edge in a bundle around edge.
  static PEdgeRadialCirculator FindRadialPEdgeBundle(EdgeHandle edge) {
    PEdgeRadialCirculator start_pe = edge->pedge_begin();
    PEdgeRadialCirculator pe = start_pe;
    PVertexHandle start_pv = pe->start_pvertex();
    do {
      if (pe->start_pvertex() != start_pv && pe->end_pvertex() != start_pv) {
        break;
      }
      ++pe;
    } while (pe != start_pe);
    return pe;
  }

  // GetWireEdgeVoidShell:
  // Get the void shell of a wire edge.
  static ShellHandle GetWireEdgeVoidShell(EdgeHandle wire_edge) {
    // Check that the p-face has no mate; otherwise there's no unique shell.
    PFaceHandle pface = wire_edge->parent_pedge()->parent_loop()->
                        parent_face()->parent_pface();
    assert(pface->mate_pface() == NULL);
    return pface->parent_shell();
  }

  // GetWireEdgeRegion:
  // Get the outer region of a wire edge. (Other edges may be adjacent to two
  // regions.
  static RegionHandle GetWireEdgeRegion(EdgeHandle wire_edge) {
    return GetWireEdgeVoidShell(wire_edge)->parent_region();
  }

  // GetIncidentEdgeCount:
  // Get the number of edges incident upon a vertex.
  static int GetIncidentEdgeCount(VertexHandle vertex) {
    if (vertex->IsIsolated()) {
      return 0;
    } else {
      std::vector<EdgeHandle> incident_edges;
      VisitVertexEdges(vertex, &incident_edges);
      return static_cast<int>(incident_edges.size());
    }
  }

  // VisitVertexEdges:
  // Visit all edges incident upon vertex, accumulating them into list |edges|.
  // Template class EdgeList is a container that supports push_back, such as
  // list or vector. To count incident edges, it could also be a custom class
  // that increment a counter each time push_back is called.
  template <class EdgeList>
  static void VisitVertexEdges(VertexHandle vertex, EdgeList* edges) {
    // We keep track of visited edges in a set rather than tagging them.
    EdgeHandleSet visited_edges;
    // Visit all p-vertices of vertex. For each such p-vertex, visit the
    // incident edges.
    PVertexOfVertexCirculator start_pv = vertex->pvertex_begin();
    PVertexOfVertexCirculator current_pv = start_pv;
    do {
      VisitVertexEdgesHelper(current_pv, current_pv->parent_edge(),
                             &visited_edges);
      ++current_pv;
    } while(current_pv != start_pv);

    // Copy all the visited edges to the edge list.
    typename EdgeHandleSet::iterator i;
    for (i = visited_edges.begin(); i != visited_edges.end(); ++i) {
      edges->push_back(*i);
    }
  }

 protected:
  // Helper for VisitVertexEdges.
  template <class EdgeHandleSet>
  static void VisitVertexEdgesHelper(PVertexHandle pvertex,
                                     EdgeHandle edge,
                                     EdgeHandleSet* visited_edges) {
    // Add edge to the set of visited edges.
    visited_edges->insert(edge);
    
    if (!edge->IsWireEdge()) {
      // Visit each of the p-edges about edge.
      PEdgeRadialCirculator start_pe = edge->pedge_begin();
      PEdgeRadialCirculator current_pe = start_pe;
      do {
        // Follow the loop of edges forward or backward to next_e, the next edge
        // incident upon the same p-vertex as edge.
        EdgeHandle next_e;
        if (current_pe->start_pvertex() == pvertex) {
          next_e = current_pe->loop_previous()->child_edge();
        } else {
          assert(current_pe->end_pvertex() == pvertex);
          next_e = current_pe->loop_next()->child_edge();
        }
        // If next_e has not already been visited, recursively visit it next.
        if (visited_edges->find(next_e) == visited_edges->end()) {
          VisitVertexEdgesHelper(pvertex, next_e, visited_edges);
        }

        ++current_pe;
      } while(current_pe != start_pe);
    }
  }

};
}  // namespace geometry
}  // namespace ginsu

#endif  // GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_UTILS_INL_H_