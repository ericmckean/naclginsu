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

  // Entity is the base class for all items in the partial DS.
  typedef typename Items::template EntityWrapper<Self,Traits> EntityWrapper;
  typedef typename EntityWrapper::Entity                      Entity;
  typedef Entity*                                             EntityHandle;
  typedef const Entity*                                       EntityConstHandle;

  // We keep all vertices in a list to easily iterate over them. A vertex
  // handle is also an iterator into that list.
  typedef typename Items::template VertexWrapper<Self,
                                                 Traits> VertexWrapper;
  typedef typename VertexWrapper::Vertex                 VertexBase;
  typedef PartialDSListItem<VertexBase>                  Vertex;
  typedef CGAL_ALLOCATOR(Vertex)                         VertexAllocator;
  typedef CGAL::In_place_list<Vertex, false,
                              VertexAllocator>           VertexList;
  typedef typename VertexList::iterator                  VertexIterator;
  typedef typename VertexList::const_iterator            VertexConstIterator;
  typedef VertexIterator                                 VertexHandle;
  typedef VertexConstIterator                            VertexConstHandle;
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

 protected:
  // All the vertices in the partial DS.
  VertexList vertices_;
};

}  // namespace geometry
}  // namespace ginsu


#endif  // GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_H_
