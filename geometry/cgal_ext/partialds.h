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
// geometry.

#ifndef GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_H_
#define GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_H_

#include <CGAL/basic.h>
#include <CGAL/In_place_list.h>
#include <CGAL/memory.h>
#include "geometry/cgal_ext/partialdsitems.h"

using namespace CGAL;

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
class PartialDSListItem : public Item,
                          public In_place_list_base<PartialDSListItem<Item> > {
 public:
  typedef PartialDSListItem<Item> Self;
    
  PartialDSListItem() {}
  explicit PartialDSListItem(const Item& v) : Item(v) {}

  // Assignment operator: copy item data without affecting the dlist pointers.
  Self& operator=(const Self& v) {
    *((Item*)this) = ((const Item&)v);
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

  // Entity is the base class for all items in the partial DS. There should
  // never be a need to allocate Entity instances directly. Neither is there any
  // use for a list of entities. We're declaring type EntityList for the sole
  // purpose of declaring EntityIterator and EntityHandle, which are used
  // throughout the partial DS classes.
  typedef typename Items::template EntityWrapper<Self,Traits> EntityWrapper;
  typedef typename EntityWrapper::Entity                      EntityBase;
  typedef PartialDSListItem<EntityBase>                       Entity;
  typedef CGAL_ALLOCATOR(Entity)                              EntityAllocator;
  typedef CGAL::In_place_list<Entity, false, EntityAllocator> EntityList;
  typedef typename EntityList::iterator                       EntityIterator;
  typedef typename EntityList::const_iterator               EntityConstIterator;
  typedef EntityIterator                                      EntityHandle;
  typedef EntityConstIterator                                 EntityConstHandle;

  // We keep all vertices in a list to easily iterate over them. A vertex
  // handle is also an iterator into that list.
  typedef typename Items::template
      VertexWrapper<Self, Traits>              VertexWrapper;
  typedef typename VertexWrapper::Vertex       VertexBase;
  typedef PartialDSListItem<VertexBase>        Vertex;
  typedef CGAL_ALLOCATOR(Vertex)               VertexAllocator;
  typedef CGAL::In_place_list<Vertex, false,
                              VertexAllocator> VertexList;
  typedef typename VertexList::iterator        VertexIterator;
  typedef typename VertexList::const_iterator  VertexConstIterator;
  typedef VertexIterator                       VertexHandle;
  typedef VertexConstIterator                  VertexConstHandle;

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

 protected:
  VertexList vertices_;
  PVertexList pvertices_;
  EdgeList edges_;
  PEdgeList pedges_;
  LoopList loops_;
  FaceList faces_;
  PFaceList pfaces_;
};

}  // namespace geometry
}  // namespace ginsu


#endif  // GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_H_
