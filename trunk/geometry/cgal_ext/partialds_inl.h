// Copyright (c) 2010 The Ginsu Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_VALIDATIONS_INL_H_
#define GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_VALIDATIONS_INL_H_

#include "geometry/cgal_ext/partialdspedge.h"
#include "geometry/cgal_ext/partialdsutils.h"
#include <set>
#include <vector>

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
    
    pv2->Init(edge, v2, pv2);
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
    loop->Init(face, pe_f, NULL /* no mate */);
    face->Init(pface, loop);
    pface->Init(Entity::kPFaceUnoriented, void_shell, face, pface, NULL);

    // Finally, we need a new void shell within the p-edges.
    void_shell->Init(shell->parent_region(), NULL, pface);
    shell->AddVoidShell(void_shell);

    return edge;
  }
  return NULL;
}

template <class TraitsType> typename PartialDS<TraitsType>::EdgeHandle
    PartialDS<TraitsType>::CreateWireEdgeInLoop(LoopHandle loop,
                                                PVertexHandle pvertex) {
  // Before anything else, let's make sure that the loop is not associated with
  // with a wire edge or isolated vertex. We can do that by checking if the face
  // is degenerate.
  assert(!loop->parent_face()->IsDegenerate());
  if (loop->parent_face()->IsDegenerate()) return NULL;

  // Find the loop's p-edge that starts at p-vertex. It'll become the new edge's
  // next p-edge.
  PEdgeHandle next_pedge = loop->FindStartPVertex(pvertex);
  assert(next_pedge != NULL);
  if (next_pedge == NULL) {
    // Bad karma; it seems the loop and pvertex have nothing in common.
    return NULL;
  }

  // Prepare or create other entities for the new edge.
  PEdgeHandle prev_pedge = next_pedge->loop_previous();
  VertexHandle new_vertex = MakeVertex();
  PVertexHandle new_pvertex = MakePVertex();
  EdgeHandle new_edge = MakeEdge();
  PEdgeHandle new_pe_f = MakePEdge();
  PEdgeHandle new_pe_r = MakePEdge();

  // Connect the entities, rerouting the loop through the new wire edge.
  new_vertex->set_parent_pvertex(new_pvertex);
  new_pvertex->Init(new_edge, new_vertex, new_pvertex);
  new_edge->Init(new_pe_f, pvertex, new_pvertex);
  new_pe_f->Init(Entity::kPEdgeForward, loop, new_edge, pvertex,
                 prev_pedge, new_pe_r, new_pe_r, new_pe_r);
  new_pe_r->Init(Entity::kPEdgeReversed, loop, new_edge, new_pvertex,
                 new_pe_f, next_pedge, new_pe_f, new_pe_f);
  return new_edge;
}

template <class TraitsType>
void PartialDS<TraitsType>::DeleteWireEdgeAndVertex(EdgeHandle edge) {
  // We can only delete wire edges with at least one singular vertex.
  assert(edge->IsWireEdge());
  if (!edge->IsWireEdge()) return;

  // Figure out what end has a singular vertex. That is the vertex we'll delete.
  // If both vertices are singular, then delete the end vertex.
  VertexHandle del_vertex, keep_vertex;
  PVertexHandle del_pvertex, keep_pvertex;
  del_pvertex = edge->end_pvertex();
  del_vertex = del_pvertex->vertex();
  keep_pvertex = edge->start_pvertex();
  keep_vertex = keep_pvertex->vertex();
  if (del_vertex->GetPVertexCount() != 1) {
    std::swap(del_vertex, keep_vertex);
    std::swap(del_pvertex, keep_pvertex);
  }
  assert(del_vertex->GetPVertexCount() == 1);
  if (del_vertex->GetPVertexCount() != 1) return;

  // Start gathering and deleting entities around del_vertex. Note that
  // keep_vertex may be singular too, in which case we'll be left with an
  // isolated vertex. In that case, we get to keep most entities and re-wired
  // them around keep_vertex.
  PEdgeHandle del_pe = edge->parent_pedge();
  PEdgeHandle keep_pe = del_pe->loop_next();
  LoopHandle loop = keep_pe->parent_loop();
  assert(keep_pe->loop_next() == del_pe);
  DestroyPEdge(del_pe);
  DestroyPVertex(del_pvertex);
  DestroyVertex(del_vertex);
  if (keep_vertex->GetPVertexCount() == 1) {
    // The keep vertex is singular too. Re-wire the edge and one p-edge to form
    // an isolated vertex.
    edge->Init(keep_pe, keep_pvertex, keep_pvertex);
    keep_pe->Init(Entity::kPEdgeUnoriented, keep_pe->parent_loop(), edge,
                  keep_pvertex, keep_pe, keep_pe, keep_pe, keep_pe);
    loop->set_boundary_pedge(keep_pe);
    // We're done. But for good form, lets validate the pface now associated
    // with the isolated keep_vertex.
    PFaceConstHandle pface = loop->parent_face()->parent_pface();
    assert(pface != NULL && pface->mate_pface() == NULL);
  } else {
    // keep_vertex isn't singular. We can delete everything but keep_vertex.

    // Take keep_pvertex out of the cloud of p-vertices about keep_vertex.
    PVertexOfVertexCirculator i, start = keep_vertex->pvertex_begin();
    for (i = start; ; ++i) {
      if (i->next_pvertex() == keep_pvertex) {
        i->set_next_pvertex(keep_pvertex->next_pvertex());
        break;
      }
    }
    if (keep_vertex->parent_pvertex() == keep_pvertex) {
      keep_vertex->set_parent_pvertex(keep_pvertex->next_pvertex());
    }

    // Remove the void shell from the region before deleting it.
    FaceHandle face = loop->parent_face();
    PFaceHandle pface = face->parent_pface();
    ShellHandle shell = pface->parent_shell();
    shell->parent_region()->outer_shell()->RemoveVoidShell(shell);

    assert(pface->mate_pface() == NULL);
    DestroyShell(shell);
    DestroyPFace(pface);
    DestroyFace(face);
    DestroyLoop(loop);
    DestroyPEdge(keep_pe);
    DestroyEdge(edge);
    DestroyPVertex(keep_pvertex);
  }
}

template <class TraitsType>
typename PartialDS<TraitsType>::VertexHandle
    PartialDS<TraitsType>::SplitEdgeCreateVertex(EdgeHandle edge) {
  typedef PartialDSUtils<Types> Utils;

  // We'll certainly need a new edge and vertex.
  EdgeHandle new_e = MakeEdge();
  VertexHandle new_v = MakeVertex();

  // We'll need to link all p-vertices around the new vertex together and all
  // the p-edges around the new edge together. To this end, we accumulate them
  // in lists for post-processing.
  std::vector<PVertexHandle> new_pv_list;
  std::vector<PEdgeHandle> new_pe_list;

  // Every radial p-edge about edge needs to be split. Every bundle of p-edges
  // that share the same start/end p-vertices will share the same new p-vertex.
  PEdgeRadialCirculator start_pe = Utils::FindRadialPEdgeBundle(edge);
  PEdgeRadialCirculator pe = start_pe;
  PVertexHandle pv1, pv2, split_pv;
  do {
    // Each time we start a bundle of p-edges we create a new split p-vertex.
    if (pe->start_pvertex() != pv1 && pe->start_pvertex() != pv2) {
      pv1 = pe->start_pvertex();
      pv2 = pe->end_pvertex();
      split_pv = MakePVertex();
      split_pv->set_parent_edge(new_e);
      new_pv_list.push_back(split_pv);
    }
    // Create a new p-edge and insert it into pe's loop.
    PEdgeHandle new_pe = MakePEdge();
    new_pe_list.push_back(new_pe);
    if (pe->orientation() == Entity::kPEdgeForward) {
      new_pe->set_orientation(Entity::kPEdgeForward);
      new_pe->set_start_pvertex(split_pv);
      new_pe->set_loop_previous(pe);
      new_pe->set_loop_next(pe->loop_next());
    } else {
      new_pe->set_orientation(Entity::kPEdgeReversed);
      new_pe->set_start_pvertex(pe->start_pvertex());
      pe->set_start_pvertex(split_pv);
      new_pe->set_loop_previous(pe->loop_previous());
      new_pe->set_loop_next(pe);
    }
    new_pe->loop_previous()->set_loop_next(new_pe);
    new_pe->loop_next()->set_loop_previous(new_pe);
    new_pe->set_parent_loop(pe->parent_loop());
    ++pe;
  } while(pe != start_pe);

  // Insert the new edge in its rightful place.
  new_e->set_start_pvertex(split_pv);
  new_e->set_end_pvertex(edge->end_pvertex());
  edge->set_end_pvertex(split_pv);
  // Link the new p-vertices and p-edges together and to their respective child.
  Utils::LinkPVertices(new_v, new_pv_list);
  Utils::LinkRadialPEdges(new_e, new_pe_list);

  return new_v;
}

template <class TraitsType>
void PartialDS<TraitsType>::DeleteVertexJoinEdge(VertexHandle vertex,
                                                 EdgeHandle edge) {
  typedef PartialDSUtils<Types> Utils;

  // Verify that vertex has exactly two incident edges.
  std::set<EdgeHandle, typename Utils::LT_EdgeConstHandle> edge_set;
  Utils::VisitVertexEdges(vertex, &edge_set);
  assert(edge_set.size() == 2);
  if (edge_set.size() != 2) return;

  // Skip over vertex to the next edge, del_e; that's the one we'll delete.
  EdgeHandle del_e = edge->GetEdgeAcrossVertex(vertex);
  assert(del_e != NULL);
  if (del_e == NULL) return;

  // Visit the radial p-edges about del_e and disconnect them from their loops.
  PEdgeRadialCirculator start_pe = del_e->pedge_begin();
  PEdgeRadialCirculator current_pe = start_pe;
  do {
    // Disconnect current_pe from its loop.
    PEdgeHandle prev_pe = current_pe->loop_previous();
    PEdgeHandle next_pe = current_pe->loop_next();
    prev_pe->set_loop_next(next_pe);
    next_pe->set_loop_previous(prev_pe);
    if (current_pe->start_pvertex()->vertex() == vertex) {
      // Ensure the remaining p-vertex doesn't point to del_e.
      if (next_pe->start_pvertex()->parent_edge() == del_e) {
        next_pe->start_pvertex()->set_parent_edge(prev_pe->child_edge());
      }
    } else {
      assert(current_pe->end_pvertex()->vertex() == vertex);
      // P-edge next_pe has a new start p-vertex.
      next_pe->set_start_pvertex(current_pe->start_pvertex());
      // Ensure the remaining p-vertex doesn't point to del_e.
      if (next_pe->start_pvertex()->parent_edge() == del_e) {
        next_pe->start_pvertex()->set_parent_edge(next_pe->child_edge());
      }
    }
    
    // Ensure the loop doesn't point to the p-edge that'll be deleted.
    LoopHandle loop = next_pe->parent_loop();
    if (loop->boundary_pedge() == current_pe) {
      loop->set_boundary_pedge(next_pe);
    }

    ++current_pe;
  } while(current_pe != start_pe);

  // We're ready to destroy old stuff, namely vertex and all its p-vertices
  // and del_e and all its radial p-edges.
  DestroyVertexCloud(vertex);
  DestroyEdgeCloud(del_e);
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

template <class TraitsType>
void PartialDS<TraitsType>::DestroyVertexCloud(VertexHandle v) {
  PVertexOfVertexCirculator start_pv = v->pvertex_begin();
  PVertexOfVertexCirculator del_pv = start_pv;
  // Skip start_pv for now; it's our end-of-loop marker.
  ++del_pv;
  while(del_pv != start_pv) {
    PVertexHandle temp = del_pv;
    ++del_pv;
    DestroyPVertex(temp);
  }
  DestroyPVertex(start_pv);
  DestroyVertex(v);
}

template <class TraitsType>
void PartialDS<TraitsType>::DestroyEdgeCloud(EdgeHandle e) {
  PEdgeRadialCirculator start_pe = e->pedge_begin();
  PEdgeRadialCirculator del_pe = start_pe;
  // Skip start_pe for now; it's our end-of-loop marker.
  ++del_pe;
  while(del_pe != start_pe) {
    PEdgeHandle temp = del_pe;
    ++del_pe;
    DestroyPEdge(temp);
  }
  DestroyPEdge(start_pe);
  DestroyEdge(e);
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
  if (v == NULL) {
    assert(!"*** ValidateEdge: end vertex is null. ***");
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

template <class TraitsType>
bool PartialDS<TraitsType>::ValidateFace(FaceConstHandle f) {
#if defined(_DEBUG) || defined(_GEOM_TESTS)
  // Check the outer loop.
  if (f->outer_loop() == NULL) {
    assert(!"*** ValidateFace: face doesn't have a valid loop. ***");
    return false;
  }
  if (f->outer_loop()->parent_face() != f) {
    assert(!"*** ValidateFace: face's loop points to a different parent. ***");
    return false;
  }
  // Check the parent face.
  if (f->parent_pface() == NULL) {
    assert(!"*** ValidateFace: face doesn't have a valid parent pface. ***");
    return false;
  }
  if (f->parent_pface()->child_face() != f) {
    assert(!"*** ValidateFace: face's parent doesn't point to face. ***");
    return false;
  }
#endif
  return true;
}

template <class TraitsType>
bool PartialDS<TraitsType>::ValidatePFace(PFaceConstHandle pf) {
#if defined(_DEBUG) || defined(_GEOM_TESTS)
  // Check the parent shell.
  if (pf->parent_shell() == NULL) {
    assert(!"*** ValidatePFace: p-face doesn't have a valid parent shell. ***");
    return false;
  }
  if (!pf->parent_shell()->FindPFace(pf)) {
    assert(!"*** ValidatePFace: p-face parent shell doesn't include pface ***");
    return false;
  }
  // Check the child face.
  if (pf->child_face() == NULL) {
    assert(!"*** ValidatePFace: p-face doesn't have a valid child face. ***");
    return false;
  }
  if (pf->child_face()->parent_pface() != pf &&
      pf->child_face()->parent_pface()->mate_pface() != pf) {
    assert(!"*** ValidatePFace: p-face child face doesn't link to p-face. ***");
    return false;
  }
  // Check the mate p-face & orientation.
  if (pf->mate_pface() == NULL) {
    if (pf->orientation() != Entity::kPFaceUnoriented) {
      assert(!"*** ValidatePFace: p-face is associted with an isolated vertex"
              " or wire edge but is not marked 'unoriented'. ***");
      return false;
    }
  } else {
    if (pf->mate_pface()->child_face() != pf->child_face()) {
      assert(!"*** ValidatePFace: p-face and its mate do not point to the same"
              " child face. ***");
      return false;
    }
    if (pf->orientation() == Entity::kPFaceUnoriented) {
    assert(!"*** ValidatePFace: a p-face with a mate should have an "
            "orientation. ***");
    return false;
    }
  }
  // Check the next-p-face link.
  if (pf->next_pface() == NULL) {
    assert(!"*** ValidatePFace: p-face doesn't have a valid next p-face. ***");
    return false;
  }
#endif
  return true;
}

}  // namespace geometry
}  // namespace ginsu

#endif  // GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_VALIDATIONS_INL_H_