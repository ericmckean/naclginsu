// Copyright (c) 2010 The Ginsu Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_VALIDATIONS_INL_H_
#define GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_VALIDATIONS_INL_H_

#include "geometry/cgal_ext/partialdspedge.h"

namespace ginsu {
namespace geometry {

// Euler operators
template <class TraitsType> typename PartialDS<TraitsType>::RegionHandle
    PartialDS<TraitsType>::CreateEmptyRegion() {
  return MakeRegion();
}

template <class TraitsType>
void PartialDS<TraitsType>::DeleteEmptyRegion(RegionHandle region) {
  if (region == NULL) return;

  assert(region->IsEmpty() && "Must empty the region first.");
  if (region->IsEmpty()) {
    if (region->outer_shell() != NULL) {
      DestroyShell(region->outer_shell());
      region->set_outer_shell(NULL);
    }
    DestroyRegion(region);
  }
}

template <class TraitsType> typename PartialDS<TraitsType>::VertexHandle
    PartialDS<TraitsType>::CreateIsolatedVertex(RegionHandle region) {
  // Must have a region.
  assert(region != NULL);
  if (region == NULL) return NULL;

  // Build the entire chain of entities, up to a shell to host the isolated
  // vertex, starting with a vertex:
  VertexHandle vertex = MakeVertex();
  PVertexHandle pvertex = MakePVertex();
  vertex->set_parent_pvertex(pvertex);
  pvertex->set_vertex(vertex);
  pvertex->set_next_pvertex(pvertex);

  // The edge simply points to the same start and end vertex.
  EdgeHandle edge = MakeEdge();
  pvertex->set_parent_edge(edge);
  edge->set_start_pvertex(pvertex);
  edge->set_end_pvertex(pvertex);

  // The p-edge links to itself, both radially and along the loop.
  PEdgeHandle pedge = MakePEdge();
  edge->set_parent_pedge(pedge);
  pedge->set_orientation(Entity::kPEdgeUnoriented);
  pedge->set_child_edge(edge);
  pedge->set_start_pvertex(pvertex);
  pedge->set_loop_previous(pedge);
  pedge->set_loop_next(pedge);
  pedge->set_radial_previous(pedge);
  pedge->set_radial_next(pedge);

  // The loop and face are degenerate.
  LoopHandle loop = MakeLoop();
  FaceHandle face = MakeFace();
  pedge->set_parent_loop(loop);
  loop->set_boundary_pedge(pedge);
  loop->set_next_hole(NULL);
  loop->set_parent_face(face);
  face->set_outer_loop(loop);

  // The p-face is unoriented, links to itself and has no mate.
  PFaceHandle pface = MakePFace();
  face->set_parent_pface(pface);
  pface->set_orientation(Entity::kPFaceUnoriented);
  pface->set_child_face(face);
  pface->set_next_pface(pface);
  pface->set_mate_pface(NULL);

  // A shell to host it all.
  ShellHandle shell = MakeShell();
  pface->set_parent_shell(shell);
  shell->set_pface(pface);
  shell->set_next_void_shell(NULL);

  // Lastly, insert the shell in the region. If the region is empty, then create
  // an outer shell - the half-open shell that surrounds the outer region - and
  // then add the vertex shell to existing shell's list of void shells.
  shell->set_parent_region(region);
  ShellHandle outer_shell = region->outer_shell();
  if (outer_shell == NULL) {
    outer_shell = MakeShell();
    region->set_outer_shell(outer_shell);
    outer_shell->set_parent_region(region);
  }
  outer_shell->AddVoidShell(shell);

  return vertex;
}

template <class TraitsType>
void PartialDS<TraitsType>::DeleteIsolatedVertex(VertexHandle vertex) {
  assert(vertex->IsIsolated());
  if (vertex->IsIsolated()) {
    PVertexHandle pv = vertex->parent_pvertex();
    DestroyVertex(vertex);
    assert(pv->next_pvertex() == pv);
    EdgeHandle e = pv->parent_edge();
    DestroyPVertex(pv);

    PEdgeHandle pe = e->parent_pedge();
    DestroyEdge(e);
    assert(pe->loop_next() == pe && pe->radial_next() == pe);
    LoopHandle loop = pe->parent_loop();
    assert(loop->boundary_pedge() == pe);
    DestroyPEdge(pe);

    assert(loop->next_hole() == NULL);
    FaceHandle f = loop->parent_face();
    assert(f->outer_loop() == loop);
    DestroyLoop(loop);
    PFaceHandle pf = f->parent_pface();
    assert(pf->child_face() == f);
    DestroyFace(f);

    assert(pf->next_pface() == pf && pf->mate_pface() == NULL);
    ShellHandle shell = pf->parent_shell();
    assert(shell->pface() == pf);
    DestroyPFace(pf);

    RegionHandle region = shell->parent_region();
    assert(region->outer_shell() != NULL && region->outer_shell() != shell);
    region->outer_shell()->RemoveVoidShell(shell);
    DestroyShell(shell);
  }
}

template <class TraitsType> typename PartialDS<TraitsType>::EdgeHandle
    PartialDS<TraitsType>::CreateWireEdgeInShell(ShellHandle shell,
                                                 VertexHandle v1) {
  assert(shell != NULL && v1 != NULL);
  if (v1->IsIsolated()) {
    assert(shell->parent_region()->FindVoidShell(
        v1->parent_pvertex()->parent_edge()->parent_pedge()->parent_loop()->
        parent_face()->parent_pface()->parent_shell()));

    // The isolated vertex already has a degenerate edge, p-edge, loop, etc.
    // We unwrap those to connect them with the new vertex. We also add a new
    // p-edge to form a void loop around the existing edge.
    VertexHandle v2 = MakeVertex();
    PVertexHandle pv2 = MakePVertex();
    PVertexHandle pv1 = v1->parent_pvertex();
    EdgeHandle edge = pv1->parent_edge();
    PEdgeHandle pe1 = edge->parent_pedge();
    PEdgeHandle pe2 = MakePEdge();
    LoopHandle loop = pe1->parent_loop();

    v2->set_parent_pvertex(pv2);
    
    pv2->Init(edge, v2, pv1);
    pv1->set_parent_edge(edge);
    edge->set_end_pvertex(pv2);

    pe1->Init(Entity::kPEdgeForward, loop, edge, pv1, pe2, pe2, pe2, pe2);
    pe2->Init(Entity::kPEdgeReversed, loop, edge, pv2, pe1, pe1, pe1, pe1);

    // We're done. The degenerate face associated with v1 become the degenerate
    // face for the wire edge. Likewise for the p-face and void shell.
    return edge;
  } else {
    // We're adding a wire edge to a vertex that is already attached to other
    // edges. In this case we're creating a new manifold component with a new
    // p-vertex, etc. attached to v1.
    VertexHandle v2 = MakeVertex();
    PVertexHandle pv2 = MakePVertex();
    EdgeHandle edge = MakeEdge();
    PVertexHandle pv1_a = v1->parent_pvertex();
    PVertexHandle pv1_b = MakePVertex();
    PEdgeHandle pe_f = MakePEdge();
    PEdgeHandle pe_r = MakePEdge();
    LoopHandle loop = MakeLoop();
    FaceHandle face = MakeFace();
    PFaceHandle pface = MakePFace();
    ShellHandle void_shell = MakeShell();

    // Setup the new vertex and p-vertex. Existing vertex v1 also gets a new
    // p-vertex pv1_b, in addition to a least one existing p-vertex pv1_a.
    v2->set_parent_pvertex(pv2);
    pv2->Init(edge, v2, pv2);
    pv1_b->Init(edge, v1, pv1_a->next_pvertex());
    pv1_a->set_next_pvertex(pv1_b);

    // Setup a new edge and two new p-edges, one forward and one reversed.
    edge->Init(pe_f, pv1_b, pv2);
    pe_f->Init(Entity::kPEdgeForward, loop, edge, pv1_b,
               pe_r, pe_r, pe_r, pe_r);
    pe_r->Init(Entity::kPEdgeReversed, loop, edge, pv2, pe_f, pe_f, pe_f, pe_f);

    // Setup a void loop and degenerate face and p-face.
    loop->Init(face, pe_f, NULL);
    face->Init(pface, loop);
    pface->Init(Entity::kPFaceUnoriented, void_shell, face, pface, NULL);

    // Finally, we need a new void shell within the p-edges.
    void_shell->Init(shell->parent_region(), NULL, pface);
    shell->AddVoidShell(void_shell);

    return edge;
  }
  return NULL;
}

template <class TraitsType>
void PartialDS<TraitsType>::DeleteWireEdge(EdgeHandle edge) {
  // TODO(gwink)
}

// Basic (non-topological) make<Item> and Destroy<Item> functions.
template <class TraitsType> typename PartialDS<TraitsType>::VertexHandle
    PartialDS<TraitsType>::MakeVertex() {
  return MakeItem<VertexHandle, VertexList>(&vertices_);
}

template <class TraitsType>
void PartialDS<TraitsType>::DestroyVertex(VertexHandle v) {
  DestroyItem<VertexHandle, VertexList>(v, &vertices_);
}

template <class TraitsType> typename PartialDS<TraitsType>::PVertexHandle
    PartialDS<TraitsType>::MakePVertex() {
  return MakeItem<PVertexHandle, PVertexList>(&pvertices_);
}

template <class TraitsType>
void PartialDS<TraitsType>::DestroyPVertex(PVertexHandle v) {
  DestroyItem<PVertexHandle, PVertexList>(v, &pvertices_);
}

template <class TraitsType>
typename PartialDS<TraitsType>::EdgeHandle PartialDS<TraitsType>::MakeEdge() {
  return MakeItem<EdgeHandle, EdgeList>(&edges_);
}

template <class TraitsType>
void PartialDS<TraitsType>::DestroyEdge(EdgeHandle e) {
  DestroyItem<EdgeHandle, EdgeList>(e, &edges_);
}

template <class TraitsType>
typename PartialDS<TraitsType>::PEdgeHandle PartialDS<TraitsType>::MakePEdge() {
  return MakeItem<PEdgeHandle, PEdgeList>(&pedges_);
}

template <class TraitsType>
void PartialDS<TraitsType>::DestroyPEdge(PEdgeHandle e) {
  DestroyItem<PEdgeHandle, PEdgeList>(e, &pedges_);
}

template <class TraitsType>
typename PartialDS<TraitsType>::FaceHandle PartialDS<TraitsType>::MakeFace() {
  return MakeItem<FaceHandle, FaceList>(&faces_);
}

template <class TraitsType>
void PartialDS<TraitsType>::DestroyFace(FaceHandle f) {
  DestroyItem<FaceHandle, FaceList>(f, &faces_);
}

template <class TraitsType>
typename PartialDS<TraitsType>::PFaceHandle PartialDS<TraitsType>::MakePFace() {
  return MakeItem<PFaceHandle, PFaceList>(&pfaces_);
}

template <class TraitsType>
void PartialDS<TraitsType>::DestroyPFace(PFaceHandle f) {
  DestroyItem<PFaceHandle, PFaceList>(f, &pfaces_);
}

template <class TraitsType>
typename PartialDS<TraitsType>::LoopHandle PartialDS<TraitsType>::MakeLoop() {
  return MakeItem<LoopHandle, LoopList>(&loops_);
}

template <class TraitsType>
void PartialDS<TraitsType>::DestroyLoop(LoopHandle l) {
  DestroyItem<LoopHandle, LoopList>(l, &loops_);
}

template <class TraitsType>
typename PartialDS<TraitsType>::ShellHandle PartialDS<TraitsType>::MakeShell() {
  return MakeItem<ShellHandle, ShellList>(&shells_);
}

template <class TraitsType>
void PartialDS<TraitsType>::DestroyShell(ShellHandle s) {
  DestroyItem<ShellHandle, ShellList>(s, &shells_);
}

template <class TraitsType> typename PartialDS<TraitsType>::RegionHandle
    PartialDS<TraitsType>::MakeRegion() {
  return MakeItem<RegionHandle, RegionList>(&regions_);
}

template <class TraitsType>
void PartialDS<TraitsType>::DestroyRegion(RegionHandle r) {
  DestroyItem<RegionHandle, RegionList>(r, &regions_);
}

// Validation functions.
template <class TraitsType>
bool PartialDS<TraitsType>::ValidateVertex(VertexConstHandle v) {
#if defined(_DEBUG) || defined(_GEOM_TESTS)
  // Check that there is a parent pvertex.
  if (v->parent_pvertex() == NULL) {
    assert(!"*** ValidateVertex: vertex has NULL parent pointer. ***");
    return false;
  }
  // Check that the chain of p-vertices around v all point to v.
  PVertexConstHandle pv0 = v->parent_pvertex();
  PVertexConstHandle pv = pv0;
  while(1) {
    if (pv == NULL) {
      assert(!"*** ValidateVertex: null p-vertex pointer in the chain "
              "of vertices around vertex. ***");
      return false;
    }
    if (pv->vertex() != v) {
      assert(!"*** ValidateVertex: one of the p-vertices around vertex does "
              "not point to vertex. ***");
      return false;
    }
    if ((pv = pv->next_pvertex()) == pv0) break;
  }
#endif
  return true;
}

template <class TraitsType>
bool PartialDS<TraitsType>::ValidatePVertex(PVertexConstHandle pv) {
#if defined(_DEBUG) || defined(_GEOM_TESTS)
  // Check that parent edge links down to this pvertex.
  EdgeConstHandle parent_edge = pv->parent_edge();
  if (pv->parent_edge() == NULL) {
    assert(!"*** ValidatePVertex: pvertex has NULL parent-edge pointer. ***");
    return false;
  }
  if (pv->parent_edge()->start_pvertex() != pv &&
      pv->parent_edge()->end_pvertex() != pv) {
    assert(!"*** ValidatePVertex: parent edge does not point to pvertex. ***");
    return false;
  }
  // Check that there is a child vertex.
  if (pv->vertex() == NULL) {
    assert(!"*** ValidatePVertex: pvertex has NULL child-vertex pointer. ***");
    return false;
  }
#endif
  return true;
}

template <class TraitsType>
bool PartialDS<TraitsType>::ValidateEdge(EdgeConstHandle e) {
#if defined(_DEBUG) || defined(_GEOM_TESTS)
  // Check that parent pedge points down to this edge.
  PEdgeConstHandle parent_pedge = e->parent_pedge();
  if (parent_pedge == NULL) {
    assert(!"*** ValidateEdge: edge has NULL parent-pedge pointer. ***");
    return false;
  }
  if (parent_pedge->child_edge() != e) {
    assert(!"*** ValidateEdge: parent pedge does not point to edge. ***");
    return false;
  }
  // Check that start and end pvertex point to this edge.
  PVertexConstHandle v = e->start_pvertex();
  if (v == NULL || v->parent_edge() != e) {
    assert(!"*** ValidateEdge: start vertex is null or points"
            " to wrong edge. ***");
    return false;
  }
  v = e->end_pvertex();
  if (v == NULL || v->parent_edge() != e) {
    assert(!"*** ValidateEdge: end vertex is null or points"
            " to wrong edge. ***");
    return false;
  }
#endif
  return true;
}

template <class TraitsType>
bool PartialDS<TraitsType>::ValidatePEdge(PEdgeConstHandle pe) {
#if defined(_DEBUG) || defined(_GEOM_TESTS)
  // Check that parent loop links to this pedge.
  LoopConstHandle loop = pe->parent_loop();
  if (loop == NULL || loop->boundary_pedge() == NULL || !loop->FindPEdge(pe)) {
    assert(!"*** ValidatePEdge: parent loop is null or does not link to"
            " p-edge ***");
    return false;
  }
  // Check that child edge links to this edge.
  EdgeConstHandle edge = pe->child_edge();
  if (edge == NULL || edge->parent_pedge() == NULL ||
      !edge->FindRadialPEdge(pe)) {
    assert(!"*** ValidatePEdge: child edge is null or does not link to"
            " p-edge ***");
    return false;
  }
  // Check the radial next and previous.
  PEdgeConstHandle scan_pedge = pe;
  PEdgeConstHandle start_pedge = scan_pedge;
  while(1) {
    if (scan_pedge->radial_next() == NULL) {
      assert(!"*** ValidatePEdge: radial_next p-edge is NULL. ***");
      return false;
    }
    if (scan_pedge->radial_next()->radial_previous() != scan_pedge) {
      assert(!"*** ValidatePEdge: radial_next p-edge does not point back to "
              "to same p-edge with radial_previous pointer. ***");
      return false;
    }
    scan_pedge = scan_pedge->radial_next();
    if (scan_pedge == start_pedge) break;
  }
#endif
  return true;
}

template <class TraitsType>
bool PartialDS<TraitsType>::ValidateLoop(LoopConstHandle loop) {
#if defined(_DEBUG) || defined(_GEOM_TESTS)
  // Ensure the loop points to a valid p-edge.
  PEdgeConstHandle scan_pedge = loop->boundary_pedge();
  if (scan_pedge == NULL) {
    assert(!"*** ValidateLoop: loop does not point to a valid p-edge. ***");
    return false;
  }
  // Check the chain of p-edges that form the loop.
  PEdgeConstHandle start_pedge = scan_pedge;
  while(1) {
    if (scan_pedge->loop_next() == NULL) {
      assert(!"*** ValidateLoop: loop_next p-edge is NULL. ***");
      return false;
    }
    if (scan_pedge->loop_next()->loop_previous() != scan_pedge) {
      assert(!"*** ValidateLoop: loop_next p-edge does not point back to "
              "to same p-edge with loop_previous pointer. ***");
      return false;
    }
    if (scan_pedge->parent_loop() != loop) {
      assert(!"*** ValidateLoop: an edge around the loop does not point "
              "to the correct parent loop. ***");
      return false;
    }

    scan_pedge = scan_pedge->loop_next();
    if (scan_pedge == start_pedge) break;
  }
#endif
  return true;
}

}  // namespace geometry
}  // namespace ginsu

#endif  // GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_VALIDATIONS_INL_H_