// Copyright (c) 2010 The Ginsu Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_VALIDATIONS_INL_H_
#define GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_VALIDATIONS_INL_H_

#include "geometry/cgal_ext/partialdspedge.h"
#include "geometry/cgal_ext/partialdsutils.h"
#include <vector>

namespace ginsu {
namespace geometry {

template <class TraitsType>
bool PartialDS<TraitsType>::s_exhaustive_mode_enabled_ = true;

// Euler operators
template <class TraitsType> typename PartialDS<TraitsType>::RegionHandle
    PartialDS<TraitsType>::CreateEmptyRegion() {
  return AllocateRegion();
}

template <class TraitsType>
void PartialDS<TraitsType>::DeleteEmptyRegion(RegionHandle region) {
  if (region == NULL) return;

  assert(region->IsEmpty() && "Must empty the region first.");
  if (region->IsEmpty()) {
    if (region->outer_shell() != NULL) {
      FreeShell(region->outer_shell());
      region->set_outer_shell(NULL);
    }
    FreeRegion(region);
  }
}

template <class TraitsType>
    typename PartialDS<TraitsType>::VertexHandle
        PartialDS<TraitsType>::CreateIsolatedVertex(RegionHandle region) {
  // Must have a region.
  assert(region != NULL);
  if (region == NULL) return NULL;

  // We need an outer shell to host the vertex' void shell. If the region
  // doesn't have one, create it.
  ShellHandle outer_shell = region->outer_shell();
  if (outer_shell == NULL) {
    outer_shell = AllocateShell();
    region->set_outer_shell(outer_shell);
    outer_shell->set_parent_region(region);
  }

  VertexHandle vertex = AllocateVertex();
  EdgeHandle edge = MakeWireEdge(vertex, vertex, outer_shell);
  return vertex;
}

template <class TraitsType>
void PartialDS<TraitsType>::DeleteIsolatedVertex(VertexHandle vertex) {
  assert(vertex->IsIsolated());
  if (vertex->IsIsolated()) {
    DestroyWireEdge(vertex->parent_pvertex()->parent_edge());
    FreeVertex(vertex);
  }
}

template <class TraitsType> 
    typename PartialDS<TraitsType>::EdgeHandle
        PartialDS<TraitsType>::CreateWireEdgeAndVertex(ShellHandle shell,
                                                       VertexHandle v1) {
  // Ensure the vertex lives in the given shell.
  assert(shell->parent_region()->FindVoidShell(
         v1->parent_pvertex()->parent_edge()->parent_pedge()->parent_loop()->
         parent_face()->parent_pface()->parent_shell()));
  if (v1->IsIsolated()) {
    // The isolated vertex already has an unoriented edge, p-edge, loop, etc.
    // We need to get rid of them.
    DestroyWireEdge(v1->parent_pvertex()->parent_edge());
    assert(v1->GetPVertexCount() == 0);
  }

  // Create a new wire edge from v1 to v2.
  VertexHandle v2 = AllocateVertex();
  EdgeHandle edge = MakeWireEdge(v1, v2, shell);
  return edge;
}

template <class TraitsType>
void PartialDS<TraitsType>::DeleteWireEdgeAndVertex(EdgeHandle edge,
                                                    VertexHandle vertex) {
  typedef PartialDSUtils<Types> Utils;

  // We can only delete wire edges.
  assert(edge->IsWireEdge());
  if (!edge->IsWireEdge()) return;
  // The vertex better be singular.
  assert(Utils::GetIncidentEdgeCount(vertex) == 1);
  if (Utils::GetIncidentEdgeCount(vertex) != 1) return;
  // And the edge and vertex better be connected.
  assert(edge->start_pvertex()->vertex() == vertex ||
         edge->end_pvertex()->vertex() == vertex);
  if (edge->start_pvertex()->vertex() != vertex &&
      edge->end_pvertex()->vertex() != vertex) {
    return;
  }

  // Get the 'other' vertex.
  VertexHandle keep_v = edge->start_pvertex()->vertex();
  if (keep_v == vertex) keep_v = edge->end_pvertex()->vertex();
  // We may need the region; grab it before deleting the edge.
  RegionHandle region = Utils::GetWireEdgeRegion(edge);
  // Ready to delete the edge and vertex.
  DestroyWireEdge(edge);
  FreeVertex(vertex);
  // If vertex keep_v is left lonely, we must re-wire it as an isolated vertex.
  if (keep_v->GetPVertexCount() == 0) {
    MakeWireEdge(keep_v, keep_v, region->outer_shell());
  }
}

template <class TraitsType>
    typename PartialDS<TraitsType>::EdgeHandle
        PartialDS<TraitsType>::CreateEdgeInLoop(LoopHandle loop,
                                                VertexHandle vertex) {
  // Before anything else, let's make sure that the loop is not associated with
  // a wire edge or isolated vertex. We can do that by checking if the face
  // is degenerate.
  assert(!loop->parent_face()->IsDegenerate());
  if (loop->parent_face()->IsDegenerate()) return NULL;

  // Find the loop's p-edge that starts at vertex. It'll become the new edge's
  // next p-edge.
  PEdgeHandle next_pedge = loop->FindStartVertex(vertex);
  assert(next_pedge != NULL);
  if (next_pedge == NULL) {
    // Bad karma; it seems the loop and pvertex have nothing in common.
    return NULL;
  }

  // Prepare or create other entities for the new edge.
  PVertexHandle pvertex = next_pedge->start_pvertex();
  PEdgeHandle prev_pedge = next_pedge->loop_previous();
  VertexHandle new_vertex = AllocateVertex();
  PVertexHandle new_pvertex = AddNewPVertex(new_vertex);
  EdgeHandle new_edge = AllocateEdge();
  PEdgeHandle new_pe_f = AllocatePEdge();
  PEdgeHandle new_pe_r = AllocatePEdge();

  // Connect the entities, re-routing the loop through the new edge.
  new_pvertex->set_parent_edge(new_edge);
  new_edge->set_parent_pedge(new_pe_f);
  new_edge->set_start_pvertex(pvertex);
  new_edge->set_end_pvertex(new_pvertex);
  new_pe_f->set_orientation(Entity::kPEdgeForward);
  new_pe_f->set_parent_loop(loop);
  new_pe_f->set_child_edge(new_edge);
  new_pe_f->set_start_pvertex(pvertex);
  new_pe_f->set_loop_previous(prev_pedge);
  new_pe_f->set_loop_next(new_pe_r);
  new_pe_f->set_radial_previous(new_pe_r);
  new_pe_f->set_radial_next(new_pe_r);
  new_pe_r->set_orientation(Entity::kPEdgeReversed);
  new_pe_r->set_parent_loop(loop);
  new_pe_r->set_child_edge(new_edge);
  new_pe_r->set_start_pvertex(new_pvertex);
  new_pe_r->set_loop_previous(new_pe_f);
  new_pe_r->set_loop_next(next_pedge);
  new_pe_r->set_radial_previous(new_pe_r);
  new_pe_r->set_radial_next(new_pe_f);
  prev_pedge->set_loop_next(new_pe_f);
  next_pedge->set_loop_previous(new_pe_r);

  return new_edge;
}

template <class TraitsType>
void PartialDS<TraitsType>::DeleteEdgeFromLoop(EdgeHandle edge) {
  typedef PartialDSUtils<Types> Utils;

  // Not suitable for wire edges.
  assert(!edge->IsWireEdge());
  if (edge->IsWireEdge()) return;
  // Get the edge end vertices and ensure we're not dealing with an isolated
  // or loop vertex.
  VertexHandle keep_v = edge->start_vertex()->vertex();
  VertexHandle del_v = edge->end_pvertex()->vertex();
  assert(keep_v != del_v);
  if (keep_v == del_v) return;
  // We must delete the singular vertex and keep the other.
  if (Utils::GetIncidentEdgeCount(keep_v) != 1) {
    std::swap(del_v, keep_v);
  }
  assert(Utils::GetIncidentEdgeCount(del_v) = 1);
  if (Utils::GetIncidentEdgeCount(del_v) != 1) return;
  // Gather the connecting p-edges and p-vertex. Ensure pe1 is the p-edge
  // leading into vertex del_v.
  PVertexHandle del_pv = del_v->parent_pvertex();
  PEdgeHandle pe1 = edge->parent_pedge();
  PEdgeHandle pe2 = pe1->radial_next();
  assert(pe1 != pe2 && pe2->radial_next() == pe1);
  if (pe1->end_pvertex() != del_pv) {
    std::swap(pe1, pe2);
  }
  assert(pe1->end_pvertex() == del_pv && pe2->start_pvertex() == del_pv);
  if (pe1->end_pvertex() != del_pv || pe2->start_pvertex() != del_pv) return;
  // Gather the adjacent p-edges, which we'll have to re-route.
  PEdgeHandle pe_prev = pe1->loop_previous();
  PEdgeHandle pe_next = pe2->loop_next();
  assert(pe_prev->end_pvertex()->vertex() == keep_v && 
         pe_next->start_pvertex()->vertex() == keep_v);
  if (pe_prev->end_pvertex()->vertex() != keep_v || 
      pe_next->start_pvertex()->vertex() != keep_v) {
    return;
  }
  // Re-route the p-edges around the entities we're about to delete.
  pe_prev->set_next_pedge(pe_next);
  pe_next->set_previous_pedge(pe_prev);
  pe_prev->end_pvertex()->set_parent_edge(pe_prev->parent_edge());
  pe_prev->parent_loop()->set_boundary_edge(pe_prev);
  // Ready to delete the edge and vertex.
  DestroyEdgeCloud(edge);
  DestroyVertexCloud(del_v);
}

template <class TraitsType>
typename PartialDS<TraitsType>::EdgeHandle
    PartialDS<TraitsType>::MakeEdgeCycle(
        ShellHandle shell, VertexHandle from_vertex, VertexHandle to_vertex) {
  // Make sure we do not create a degenerate cycle.
  assert(from_vertex != to_vertex);
  if (from_vertex == to_vertex) return NULL;
  // We simply create a wire edge between the two given vertices.
  EdgeHandle edge = MakeWireEdge(from_vertex, to_vertex, shell);
  return edge;
}

template <class TraitsType>
void PartialDS<TraitsType>::DeleteEdgeCycle(EdgeHandle edge) {
  // Can only delete a wire edge.
  assert(edge->IsWireEdge());
  // Get rid of the edge, but keep both end vertices.
  DestroyWireEdge(edge);
}

template <class TraitsType>
typename PartialDS<TraitsType>::VertexHandle
    PartialDS<TraitsType>::SplitEdgeCreateVertex(EdgeHandle edge) {
  typedef PartialDSUtils<Types> Utils;
  
  // Can't split wire edges. You could just use CreateWireEdgeInShell to attach
  // a wire edge to an existing wire edge's vertex instead.
  assert(!edge->IsWireEdge());
  if (edge->IsWireEdge()) return NULL;

  // We'll certainly need a new edge and vertex.
  EdgeHandle new_e = AllocateEdge();
  VertexHandle new_v = AllocateVertex();

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
      split_pv = AllocatePVertex();
      split_pv->set_parent_edge(new_e);
      new_pv_list.push_back(split_pv);
    }
    // Create a new p-edge and insert it into pe's loop.
    PEdgeHandle new_pe = AllocatePEdge();
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
  } while (pe != start_pe);

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

  // Verify that vertex has exactly two incident edges and a single p-vertex.
  if (s_exhaustive_mode_enabled_) {
    int incident_edge_count = Utils::GetIncidentEdgeCount(vertex);
    assert(incident_edge_count == 2);
    if (incident_edge_count != 2) return;
  }
  assert(vertex->GetPVertexCount() == 1);
  if (vertex->GetPVertexCount() != 1) return;

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
  } while (current_pe != start_pe);

  // We're ready to destroy old stuff, namely vertex and all its p-vertices
  // and del_e and all its radial p-edges.
  DestroyVertexCloud(vertex);
  DestroyEdgeCloud(del_e);
}

// Basic (non-topological) make<Item> and Destroy<Item> functions.
template <class TraitsType> typename PartialDS<TraitsType>::VertexHandle
    PartialDS<TraitsType>::AllocateVertex() {
  return AllocateItem<VertexHandle, VertexList>(&vertices_);
}

template <class TraitsType>
void PartialDS<TraitsType>::FreeVertex(VertexHandle v) {
  FreeItem<VertexHandle, VertexList>(v, &vertices_);
}

template <class TraitsType> typename PartialDS<TraitsType>::PVertexHandle
    PartialDS<TraitsType>::AllocatePVertex() {
  return AllocateItem<PVertexHandle, PVertexList>(&pvertices_);
}

template <class TraitsType>
void PartialDS<TraitsType>::FreePVertex(PVertexHandle v) {
  FreeItem<PVertexHandle, PVertexList>(v, &pvertices_);
}

template <class TraitsType>
typename PartialDS<TraitsType>::EdgeHandle
    PartialDS<TraitsType>::AllocateEdge() {
  return AllocateItem<EdgeHandle, EdgeList>(&edges_);
}

template <class TraitsType>
void PartialDS<TraitsType>::FreeEdge(EdgeHandle e) {
  FreeItem<EdgeHandle, EdgeList>(e, &edges_);
}

template <class TraitsType>
typename PartialDS<TraitsType>::PEdgeHandle
    PartialDS<TraitsType>::AllocatePEdge() {
  return AllocateItem<PEdgeHandle, PEdgeList>(&pedges_);
}

template <class TraitsType>
void PartialDS<TraitsType>::FreePEdge(PEdgeHandle e) {
  FreeItem<PEdgeHandle, PEdgeList>(e, &pedges_);
}

template <class TraitsType>
typename PartialDS<TraitsType>::FaceHandle
    PartialDS<TraitsType>::AllocateFace() {
  return AllocateItem<FaceHandle, FaceList>(&faces_);
}

template <class TraitsType>
void PartialDS<TraitsType>::FreeFace(FaceHandle f) {
  FreeItem<FaceHandle, FaceList>(f, &faces_);
}

template <class TraitsType>
typename PartialDS<TraitsType>::PFaceHandle
    PartialDS<TraitsType>::AllocatePFace() {
  return AllocateItem<PFaceHandle, PFaceList>(&pfaces_);
}

template <class TraitsType>
void PartialDS<TraitsType>::FreePFace(PFaceHandle f) {
  FreeItem<PFaceHandle, PFaceList>(f, &pfaces_);
}

template <class TraitsType>
typename PartialDS<TraitsType>::LoopHandle
    PartialDS<TraitsType>::AllocateLoop() {
  return AllocateItem<LoopHandle, LoopList>(&loops_);
}

template <class TraitsType>
void PartialDS<TraitsType>::FreeLoop(LoopHandle l) {
  FreeItem<LoopHandle, LoopList>(l, &loops_);
}

template <class TraitsType>
typename PartialDS<TraitsType>::ShellHandle
    PartialDS<TraitsType>::AllocateShell() {
  return AllocateItem<ShellHandle, ShellList>(&shells_);
}

template <class TraitsType>
void PartialDS<TraitsType>::FreeShell(ShellHandle s) {
  FreeItem<ShellHandle, ShellList>(s, &shells_);
}

template <class TraitsType> typename PartialDS<TraitsType>::RegionHandle
    PartialDS<TraitsType>::AllocateRegion() {
  return AllocateItem<RegionHandle, RegionList>(&regions_);
}

template <class TraitsType>
void PartialDS<TraitsType>::FreeRegion(RegionHandle r) {
  FreeItem<RegionHandle, RegionList>(r, &regions_);
}

template <class TraitsType>
    typename PartialDS<TraitsType>::EdgeHandle
        PartialDS<TraitsType>::MakeWireEdge(
            VertexHandle start_v, VertexHandle end_v, ShellHandle shell) {
  // Create the various entities a wire edge is made of. 
  EdgeHandle new_e = AllocateEdge();
  PEdgeHandle new_pe = AllocatePEdge();
  LoopHandle loop = AllocateLoop();
  FaceHandle face = AllocateFace();
  PFaceHandle pface = AllocatePFace();
  ShellHandle void_shell = AllocateShell();
  PVertexHandle start_pv = AddNewPVertex(start_v);
  PVertexHandle end_pv = (start_v == end_v) ? start_pv : AddNewPVertex(end_v);
  // Connect the edge to the p-vertices and all the entities in the hierarchy.
  // Note that if start_pv == end_pv we're creating an unoriented edge.
  start_pv->set_parent_edge(new_e);
  end_pv->set_parent_edge(new_e);
  new_e->set_parent_pedge(new_pe);
  new_e->set_start_pvertex(start_pv);
  new_e->set_end_pvertex(end_pv);
  new_pe->set_orientation((start_pv == end_pv)? Entity::kPEdgeUnoriented:
                                                Entity::kPEdgeForward);
  new_pe->set_parent_loop(loop);
  new_pe->set_child_edge(new_e);
  new_pe->set_start_pvertex(start_pv);
  new_pe->set_radial_previous(new_pe);
  new_pe->set_radial_next(new_pe);
  loop->set_parent_face(face);
  loop->set_boundary_pedge(new_pe);
  face->set_parent_pface(pface);
  face->set_outer_loop(loop);
  pface->set_orientation(Entity::kPFaceUnoriented);
  pface->set_parent_shell(void_shell);
  pface->set_child_face(face);
  pface->set_next_pface(pface);
  void_shell->set_parent_region(shell->parent_region());
  void_shell->set_pface(pface);
  shell->AddVoidShell(void_shell);

  return new_e;
}

template <class TraitsType>
void PartialDS<TraitsType>::DestroyWireEdge(EdgeHandle e) {
  // It must be either a 'real' wire edge or an edge associated with an
  // isolated vertex.
  assert(e->IsWireEdge() || e->start_pvertex()->vertex()->IsIsolated());
  // Gather the entities up to the void shell.
  PVertexHandle start_pv = e->start_pvertex();
  PVertexHandle end_pv = e->end_pvertex();
  PEdgeHandle pe = e->parent_pedge();
  LoopHandle loop = pe->parent_loop();
  FaceHandle face = loop->parent_face();
  PFaceHandle pface = face->parent_pface();
  ShellHandle shell = pface->parent_shell();
  // Remove the edge's void shell from the region.
  shell->parent_region()->outer_shell()->RemoveVoidShell(shell);
  // Destroy the entities.
  if (start_pv != end_pv) DestroyPVertex(end_pv);
  DestroyPVertex(start_pv);
  FreeShell(shell);
  FreePFace(pface);
  FreeFace(face);
  FreeLoop(loop);
  FreePEdge(pe);
  FreeEdge(e);
}

template <class TraitsType> typename PartialDS<TraitsType>::PVertexHandle
    PartialDS<TraitsType>::AddNewPVertex(VertexHandle v) {
  PVertexHandle pv = v->parent_pvertex();
  PVertexHandle new_pv = AllocatePVertex();
  new_pv->set_vertex(v);
  if (pv == NULL) {
    // This is the vertex' first p-vertex.
    v->set_parent_pvertex(new_pv);
    new_pv->set_next_pvertex(new_pv);
  } else {
    // Add the new p-vertex to the existing p-vertex cloud.
    new_pv->set_next_pvertex(pv->next_pvertex());
    pv->set_next_pvertex(new_pv);
  }
  return new_pv;
}

template <class TraitsType>
void PartialDS<TraitsType>::DestroyPVertex(PVertexHandle pv) {
  VertexHandle v = pv->vertex();
  if (v != NULL) {
    if (pv->next_pvertex() == pv) {
      // Only one p-vertex.
      assert(v->parent_pvertex() == pv);
      v->set_parent_pvertex(NULL);
      FreePVertex(pv);
    } else {
      // Remove pv from the cloud of p-vertices around v.
      PVertexOfVertexCirculator start_pv = v->pvertex_begin();
      PVertexOfVertexCirculator current_pv = start_pv;
      do {
        if (current_pv->next_pvertex() == pv) {
          // Found the p-vertex that links to pv. Bypass pv and delete it.
          current_pv->set_next_pvertex(pv->next_pvertex());
          if (v->parent_pvertex() == pv) {
            v->set_parent_pvertex(current_pv);
          }
          FreePVertex(pv);
          break;
        }

        ++current_pv;
        assert(current_pv != start_pv);  // Did not find pv around v?
      } while (current_pv != start_pv);
    }
  } else {
    // No child vertex; caller could have called FreePVertex instead. Oh well.
    FreePVertex(pv);
  }
}

template <class TraitsType>
void PartialDS<TraitsType>::DestroyVertexCloud(VertexHandle v) {
  PVertexOfVertexCirculator start_pv = v->pvertex_begin();
  PVertexOfVertexCirculator del_pv = start_pv;
  // Skip start_pv for now; it's our end-of-loop marker.
  ++del_pv;
  while (del_pv != start_pv) {
    PVertexHandle temp = del_pv;
    ++del_pv;
    FreePVertex(temp);
  }
  FreePVertex(start_pv);
  FreeVertex(v);
}

template <class TraitsType>
void PartialDS<TraitsType>::DestroyEdgeCloud(EdgeHandle e) {
  PEdgeRadialCirculator start_pe = e->pedge_begin();
  PEdgeRadialCirculator del_pe = start_pe;
  // Skip start_pe for now; it's our end-of-loop marker.
  ++del_pe;
  while (del_pe != start_pe) {
    PEdgeHandle temp = del_pe;
    ++del_pe;
    FreePEdge(temp);
  }
  FreePEdge(start_pe);
  FreeEdge(e);
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
  while (1) {
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
  // Check that child edge links to this edge.
  LoopConstHandle loop = pe->parent_loop();
  EdgeConstHandle edge = pe->child_edge();
  if (edge == NULL || edge->parent_pedge() == NULL ||
      !edge->FindRadialPEdge(pe)) {
    assert(!"*** ValidatePEdge: child edge is null or does not link to"
            " p-edge ***");
    return false;
  }
  // Check that parent loop links to this pedge.
  if (loop == NULL || loop->boundary_pedge() == NULL || !loop->FindPEdge(pe)) {
    assert(!"*** ValidatePEdge: parent loop is null or does not link to"
            " p-edge ***");
    return false;
  }
  // Check the radial next and previous.
  PEdgeConstHandle scan_pedge = pe;
  PEdgeConstHandle start_pedge = scan_pedge;
  while (1) {
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
  // Check the start vertex w.r.t. the underlying edge.
  if (pe->orientation() == Entity::kPEdgeForward) {
    if (pe->start_pvertex() != edge->start_pvertex()) {
      assert(!"*** ValidatePEdge: the p-edge start vertex doesn't match "
              "the child edge's start p-vertex. ***");
      return false;
    }
  } else if (pe->orientation() == Entity::kPEdgeReversed) {
    if (pe->start_pvertex() != edge->end_pvertex()) {
      assert(!"*** ValidatePEdge: the p-edge start vertex doesn't match "
              "the child edge's end p-vertex. ***");
      return false;
    }
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
  // For non-degenerate, loops check the chain of p-edges that form the loop.
  if (loop->parent_face()->parent_pface()->mate_pface() != NULL) {
    PEdgeConstHandle start_pedge = scan_pedge;
    while (1) {
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

template <class TraitsType>
template <class EdgeListType>
bool PartialDS<TraitsType>::ValidateEdgeCycle(const EdgeListType& cycle) {
#if defined(_DEBUG) || defined(_GEOM_TESTS)
  // Verify that all the edges in cycle form a complete loop. v0 is the first
  // vertex along the cycle, vi is the 'exploring' vertex as we move along
  // the cycle.
  typename EdgeListType::const_iterator it = cycle.begin();
  VertexConstHandle v0 = (*it)->start_pvertex()->vertex();
  VertexConstHandle vi = (*it)->end_pvertex()->vertex();
  // Edges don't have to be oriented consistently along the cycle. So either
  // v0 or vi could be the vertex in common with the next edge.
  ++it;
  if (vi != (*it)->start_pvertex()->vertex() &&
      vi != (*it)->end_pvertex()->vertex()) {
    std::swap(v0, vi);
  }
  for (; it != cycle.end(); ++it) {
    // Again, since edges need not be oriented consistently, either of the end
    // vertices could be vi. However one of them MUST be vi.
    VertexConstHandle w0 = (*it)->start_pvertex()->vertex();
    VertexConstHandle w1 = (*it)->end_pvertex()->vertex();
    if (w0 == vi) {
      vi = w1;
    } else if (w1 == vi) {
      vi = w0;
    } else {
      return false;
    }
  }
  // Lastly, verify that the cycle loops back to v0.
  return vi == v0;
#endif
  return true;
}

}  // namespace geometry
}  // namespace ginsu

#endif  // GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_VALIDATIONS_INL_H_