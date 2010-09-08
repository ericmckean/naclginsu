// Copyright (c) 2010 The Ginsu Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_VALIDATIONS_INL_H_
#define GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_VALIDATIONS_INL_H_

#include "geometry/cgal_ext/partialdspedge.h"

namespace ginsu {
namespace geometry {

// Euler operators

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
  if (pv->parent_edge->start_vertex() != pv &&
      pv->parent_edge->end_vertex() != pv) {
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
  PVertexConstHandle v = parent_pedge->start_vertex();
  if (v == NULL || v->parent_edge() != e) {
    assert(!"*** ValidateEdge: start vertex is null or points"
            " to wrong edge. ***");
    return false;
  }
  v = parent_pedge->end_vertex();
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
  // TODO(gwink): implement this.
#endif
  return true;
}

}  // namespace geometry
}  // namespace ginsu

#endif  // GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_VALIDATIONS_INL_H_