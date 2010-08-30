// Copyright (c) 2010 The Ginsu Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Although file declares three template classes, you're only likely to use
// PartialDS directly. It builds upon the other two classes:
//
// PartialDSListItem: This is a utility template class, parameterized with
// class Item. It augments Item with next/previous pointers, effectively adding
// double-list capability. A crucial characteristic of PartialDSItemList is that
// an iterator in the d-list can also be used as a handle to Item. It is used
// to build lists of vertices, edges and faces that can easily be iterated over.
//
// PartialDSTypes: Although it is a class, it is effectively used as a wrapper
// around all the types used in the partial-entity data structure. For instance
// it declares types such as VertexHandle, EntityHandle, etc. It is used as the
// template parameter in all the classes where you see a template named
// TypeRefs. It is also used as the base class for class PartialDS.
//
// PartialDS: The partial-entity data structure, used the represent non-manifold
// geometry, per "Partial Entity Structure: A Compact Non-Manifold Boundary
// Representation Based on Partial Topological Entities," Sang Hun Lee and
// Kunwoo Lee in Solid Modeling 2001. We actually use a variation of Lees' work
// where the data structure is uniform and wire edges and isolated vertices are
// associated with degenerate elements. E.g. an isolated vertex links to a
// p-vertex that links to a degenerate (zero-length) edge, etc. In other words,
// we got rid of the dashed edges in Lees class diagram in Figure 6.

#ifndef GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_H_
#define GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_H_

#include <cassert>
#include "CGAL/basic.h"
#include "CGAL/In_place_list.h"
#include "CGAL/memory.h"
#include "geometry/cgal_ext/partialdsitems.h"

namespace ginsu {
namespace geometry {

// PartialDSListItem: This template class relies on cgal's In_place_list and
// multiple-inheritance to add d-link pointers to the Item class. As such, it
// allows an iterator into the d-list to also act as an Item handle (i.e. a
// pointer to Item). 
// Template parameter:
// - Item: The item type that needs to be inserted into a doubly-linked list,
//   most likely a vertex, edge, face, etc.
template <class Item>
class PartialDSListItem : 
    public Item,
    public CGAL::In_place_list_base<PartialDSListItem<Item> > {
 public:
  typedef PartialDSListItem<Item> Self;
    
  PartialDSListItem() {}
  explicit PartialDSListItem(const Item& v) : Item(v) {}

  // Assignment operator: copy item data without affecting the dlist pointers.
  Self& operator=(const Self& v) {
    *(static_cast<Item*>(this)) = ((const Item&)v);
    return *this;
  }
};

// PartialDSTypes encapsulates all the types used by the Partial DS. It doesn't
// have any code or data. It is used by other template classes every time you
// see a template named TypeRefs.
// Template parameters:
// - TraitsType: the cgal traits (math kernel, and geometric types) the
//   partial-entity DS is built upon.
// - ItemsType: declares wrappers for all the types, vertex, edge, etc, used to
//   construct the partial-entity DS.
template <class TraitsType, class ItemsType>
class PartialDSTypes {
 public:
  typedef PartialDSTypes<TraitsType, ItemsType> Self;
  typedef TraitsType                            Traits;
  typedef ItemsType                             Items;

  // We keep all vertices in a list to easily iterate over them. A vertex
  // handle is also an iterator into that list.
  typedef typename Items::template
      VertexWrapper<Self, Traits>               VertexWrapper;
  typedef typename VertexWrapper::Vertex        VertexBase;
  typedef PartialDSListItem<VertexBase>         Vertex;
  typedef CGAL_ALLOCATOR(Vertex)                VertexAllocator;
  typedef CGAL::In_place_list<Vertex, false,
                              VertexAllocator>  VertexList;
  typedef typename VertexList::iterator         VertexIterator;
  typedef typename VertexList::const_iterator   VertexConstIterator;
  typedef VertexIterator                        VertexHandle;
  typedef VertexConstIterator                   VertexConstHandle;

  // Ditto with p-vertices.
  typedef typename Items::template
      PVertexWrapper<Self, Traits>              PVertexWrapper;
  typedef typename PVertexWrapper::PVertex      PVertexBase;
  typedef PartialDSListItem<PVertexBase>        PVertex;
  typedef CGAL_ALLOCATOR(PVertex)               PVertexAllocator;
  typedef CGAL::In_place_list<PVertex, false,
                              PVertexAllocator> PVertexList;
  typedef typename PVertexList::iterator        PVertexIterator;
  typedef typename PVertexList::const_iterator  PVertexConstIterator;
  typedef PVertexIterator                       PVertexHandle;
  typedef PVertexConstIterator                  PVertexConstHandle;
  
  // Edges and p-edges
  typedef typename Items::template
      EdgeWrapper<Self, Traits>                 EdgeWrapper;
  typedef typename EdgeWrapper::Edge            EdgeBase;
  typedef PartialDSListItem<EdgeBase>           Edge;
  typedef CGAL_ALLOCATOR(Edge)                  EdgeAllocator;
  typedef CGAL::In_place_list<Edge, false,
                              EdgeAllocator>    EdgeList;
  typedef typename EdgeList::iterator           EdgeIterator;
  typedef typename EdgeList::const_iterator     EdgeConstIterator;
  typedef EdgeIterator                          EdgeHandle;
  typedef EdgeConstIterator                     EdgeConstHandle;
  
  typedef typename Items::template
      PEdgeWrapper<Self, Traits>                PEdgeWrapper;
  typedef typename PEdgeWrapper::PEdge          PEdgeBase;
  typedef PartialDSListItem<PEdgeBase>          PEdge;
  typedef CGAL_ALLOCATOR(PEdge)                 PEdgeAllocator;
  typedef CGAL::In_place_list<PEdge, false,
                              PEdgeAllocator>   PEdgeList;
  typedef typename PEdgeList::iterator          PEdgeIterator;
  typedef typename PEdgeList::const_iterator    PEdgeConstIterator;
  typedef PEdgeIterator                         PEdgeHandle;
  typedef PEdgeConstIterator                    PEdgeConstHandle;

  // Loop
  typedef typename Items::template
      LoopWrapper<Self, Traits>                 LoopWrapper;
  typedef typename LoopWrapper::Loop            LoopBase;
  typedef PartialDSListItem<LoopBase>           Loop;
  typedef CGAL_ALLOCATOR(Loop)                  LoopAllocator;
  typedef CGAL::In_place_list<Loop, false,
                              LoopAllocator>    LoopList;
  typedef typename LoopList::iterator           LoopIterator;
  typedef typename LoopList::const_iterator     LoopConstIterator;
  typedef LoopIterator                          LoopHandle;
  typedef LoopConstIterator                     LoopConstHandle;

  // Face and PFace
  typedef typename Items::template
      FaceWrapper<Self, Traits>                 FaceWrapper;
  typedef typename FaceWrapper::Face            FaceBase;
  typedef PartialDSListItem<FaceBase>           Face;
  typedef CGAL_ALLOCATOR(Face)                  FaceAllocator;
  typedef CGAL::In_place_list<Face, false,
                              FaceAllocator>    FaceList;
  typedef typename FaceList::iterator           FaceIterator;
  typedef typename FaceList::const_iterator     FaceConstIterator;
  typedef FaceIterator                          FaceHandle;
  typedef FaceConstIterator                     FaceConstHandle;
  
  typedef typename Items::template
      PFaceWrapper<Self, Traits>                PFaceWrapper;
  typedef typename PFaceWrapper::PFace          PFaceBase;
  typedef PartialDSListItem<PFaceBase>          PFace;
  typedef CGAL_ALLOCATOR(PFace)                 PFaceAllocator;
  typedef CGAL::In_place_list<PFace, false,
                              PFaceAllocator>   PFaceList;
  typedef typename PFaceList::iterator          PFaceIterator;
  typedef typename PFaceList::const_iterator    PFaceConstIterator;
  typedef PFaceIterator                         PFaceHandle;
  typedef PFaceConstIterator                    PFaceConstHandle;

  // Shell
  typedef typename Items::template
      ShellWrapper<Self, Traits>                ShellWrapper;
  typedef typename ShellWrapper::Shell          ShellBase;
  typedef PartialDSListItem<ShellBase>          Shell;
  typedef CGAL_ALLOCATOR(Shell)                 ShellAllocator;
  typedef CGAL::In_place_list<Shell, false,
                              ShellAllocator>   ShellList;
  typedef typename ShellList::iterator          ShellIterator;
  typedef typename ShellList::const_iterator    ShellConstIterator;
  typedef ShellIterator                         ShellHandle;
  typedef ShellConstIterator                    ShellConstHandle;

  // Region
  typedef typename Items::template
      RegionWrapper<Self, Traits>               RegionWrapper;
  typedef typename RegionWrapper::Region        RegionBase;
  typedef PartialDSListItem<RegionBase>         Region;
  typedef CGAL_ALLOCATOR(Region)                RegionAllocator;
  typedef CGAL::In_place_list<Region, false,
                              RegionAllocator>  RegionList;
  typedef typename RegionList::iterator         RegionIterator;
  typedef typename RegionList::const_iterator   RegionConstIterator;
  typedef RegionIterator                        RegionHandle;
  typedef RegionConstIterator                   RegionConstHandle;
};

// PartialDS: The the partial-entity data structure.
// Template parameter:
// - TraitTypes: cgal traits, which define the math kernel and geometry upon
//               which the DS is built.
template <class TraitsType>
class PartialDS : public PartialDSTypes<TraitsType, PartialDSItems> {
 public:
  typedef PartialDS<TraitsType>                      Self;
  typedef PartialDSTypes<TraitsType, PartialDSItems> Types;

  typedef typename Types::VertexList                 VertexList;
  typedef typename Types::PVertexList                PVertexList;
  typedef typename Types::EdgeList                   EdgeList;
  typedef typename Types::PEdgeList                  PEdgeList;
  typedef typename Types::LoopList                   LoopList;
  typedef typename Types::FaceList                   FaceList;
  typedef typename Types::PFaceList                  PFaceList;
  typedef typename Types::ShellList                  ShellList;
  typedef typename Types::RegionList                 RegionList;

  typedef typename Types::VertexHandle               VertexHandle;
  typedef typename Types::VertexConstHandle          VertexConstHandle;
  typedef typename Types::PVertexHandle              PVertexHandle;
  typedef typename Types::PVertexConstHandle         PVertexConstHandle;
  typedef typename Types::EdgeHandle                 EdgeHandle;
  typedef typename Types::EdgeConstHandle            EdgeConstHandle;
  typedef typename Types::PEdgeHandle                PEdgeHandle;
  typedef typename Types::FaceHandle                 FaceHandle;
  typedef typename Types::PFaceHandle                PFaceHandle;
  typedef typename Types::LoopHandle                 LoopHandle;
  typedef typename Types::ShellHandle                ShellHandle;
  typedef typename Types::RegionHandle               RegionHandle;

  // Use these function to allocate and destroy Partial DS items. The Make
  // function both allocate an item and insert it into the corresponding list.
  VertexHandle MakeVertex() {
    return MakeItem<VertexHandle, VertexList>(&vertices_);
  }
  void DestroyVertex(VertexHandle v) {
    DestroyItem<VertexHandle, VertexList>(v, &vertices_);
  }
  PVertexHandle MakePVertex() {
    return MakeItem<PVertexHandle, PVertexList>(&pvertices_);
  }
  void DestroyPVertex(PVertexHandle v) {
    DestroyItem<PVertexHandle, PVertexList>(v, &pvertices_);
  }
  EdgeHandle MakeEdge() {
    return MakeItem<EdgeHandle, EdgeList>(&edges_);
  }
  void DestroyEdge(EdgeHandle e) {
    DestroyItem<EdgeHandle, EdgeList>(e, &edges_);
  }
  PEdgeHandle MakePEdge() {
    return MakeItem<PEdgeHandle, PEdgeList>(&pedges_);
  }
  void DestroyPEdge(PEdgeHandle e) {
    DestroyItem<PEdgeHandle, PEdgeList>(e, &pedges_);
  }
  FaceHandle MakeFace() {
    return MakeItem<FaceHandle, FaceList>(&faces_);
  }
  void DestroyFace(FaceHandle f) {
    DestroyItem<FaceHandle, FaceList>(f, &faces_);
  }
  PFaceHandle MakePFace() {
    return MakeItem<PFaceHandle, PFaceList>(&pfaces_);
  }
  void DestroyPFace(PFaceHandle f) {
    DestroyItem<PFaceHandle, PFaceList>(f, &pfaces_);
  }
  LoopHandle MakeLoop() {
    return MakeItem<LoopHandle, LoopList>(&loops_);
  }
  void DestroyLoop(LoopHandle l) {
    DestroyItem<LoopHandle, LoopList>(l, &loops_);
  }
  ShellHandle MakeShell() {
    return MakeItem<ShellHandle, ShellList>(&shells_);
  }
  void DestroyShell(ShellHandle s) {
    DestroyItem<ShellHandle, ShellList>(s, &shells_);
  }
  RegionHandle MakeRegion() {
    return MakeItem<RegionHandle, RegionList>(&regions_);
  }
  void DestroyRegion(RegionHandle r) {
    DestroyItem<RegionHandle, RegionList>(r, &regions_);
  }
  
  // List accessors, to iterate over these vertices, edges, etc. E.g. to display
  // the geometry.
  const VertexList& vertices() const { return vertices_; }
  const EdgeList& edges() const { return edges_;}
  const FaceList& faces() const { return faces_; }
  const ShellList& shells() const { return shells_; }
  const RegionList& regions() const { return regions_; }

  // Validation functions; no-op unless either _DEBUG or _GEOM_TESTS id defined.
  static bool ValidateVertex(VertexConstHandle v);
  static bool ValidatePVertex(PVertexConstHandle pv);

 private:
  // Template function for making and destroying PartialDS items.
  template <class ItemHandle, class ItemList>
  ItemHandle MakeItem (ItemList* item_list) {
    typedef typename ItemList::pointer Pointer;
    typedef typename ItemList::value_type Vertex;
    
    Pointer pv = item_list->get_allocator().allocate(1);
    new (pv) Vertex();
    item_list->push_back(*pv);
    return item_list->begin();
  }
  
  template <class ItemHandle, class ItemList>
  void DestroyItem(ItemHandle item, ItemList* item_list) {
    item_list->erase(item);
    item_list->get_allocator().destroy(&*item);
  }

  VertexList vertices_;
  PVertexList pvertices_;
  EdgeList edges_;
  PEdgeList pedges_;
  LoopList loops_;
  FaceList faces_;
  PFaceList pfaces_;
  ShellList shells_;
  RegionList regions_;
  // Note: In Ginsu, a PartialDS instance represent a single non-manifold mesh.
  // We keep the list of models at a higher level, outside of the PE data
  // structure.
};

}  // namespace geometry
}  // namespace ginsu

#include "geometry/cgal_ext/partialds_validations_inl.h"


#endif  // GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_H_
