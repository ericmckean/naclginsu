// Copyright (c) 2006-2007  INRIA Sophia-Antipolis (France).
// Copyright (c) 2008 GeometryFactory, Sophia Antipolis (France)
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org); you may redistribute it under
// the terms of the Q Public License version 1.0.
// See the file LICENSE.QPL distributed with CGAL.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $URL: svn+ssh://scm.gforge.inria.fr/svn/cgal/branches/CGAL-3.6-branch/Mesh_3/include/CGAL/Mesh_cell_base_3.h $
// $Id: Mesh_cell_base_3.h 52813 2009-10-30 08:06:37Z stayeb $
//
//
// Author(s)     : Laurent Rineau, Stephane Tayeb


#ifndef CGAL_MESH_CELL_BASE_3_H
#define CGAL_MESH_CELL_BASE_3_H


#include <CGAL/Regular_triangulation_cell_base_3.h>
#include <CGAL/Triangulation_cell_base_with_circumcenter_3.h>
#include <CGAL/Mesh_3/Mesh_surface_cell_base_3.h>

namespace CGAL {
  
// Class Mesh_cell_base_3
// Cell base class used in 3D meshing process.
// Adds information to Cb about the cell of the input complex containing it
template< class GT,
          class MD,
          class Cb = CGAL::Regular_triangulation_cell_base_3<
              GT, CGAL::Triangulation_cell_base_with_circumcenter_3<GT> > >
class Mesh_cell_base_3
: public Mesh_3::Mesh_surface_cell_base_3<GT, MD, Cb>
{
  // Base
  typedef Mesh_3::Mesh_surface_cell_base_3<GT, MD, Cb> Base;
  typedef typename GT::FT FT;
  
public:
  // Index Type
  typedef typename MD::Subdomain_index Subdomain_index;
  typedef typename MD::Surface_index Surface_index;
  
  //
  typedef typename Base::Tds Tds;
  typedef typename Tds::Vertex_handle Vertex_handle;
  typedef typename Tds::Cell_handle Cell_handle;
  
  // To get correct cell type in TDS
  template < class TDS3 >
  struct Rebind_TDS
  {
    typedef typename Cb::template Rebind_TDS<TDS3>::Other Cb3;
    typedef Mesh_cell_base_3 <GT, MD, Cb3> Other;
  };
  
  // Constructors
  Mesh_cell_base_3()
    : Base()
    , subdomain_index_()
    , sliver_value_(FT(0.))
    , sliver_cache_validity_(false) { };
  
  Mesh_cell_base_3 (Vertex_handle v0,
                    Vertex_handle v1,
                    Vertex_handle v2,
                    Vertex_handle v3)
    : Base (v0, v1, v2, v3)
    , subdomain_index_() 
    , sliver_value_(FT(0.))
    , sliver_cache_validity_(false) { };
  
  Mesh_cell_base_3 (Vertex_handle v0,
                    Vertex_handle v1,
                    Vertex_handle v2,
                    Vertex_handle v3,
                    Cell_handle n0,
                    Cell_handle n1,
                    Cell_handle n2,
                    Cell_handle n3)
    : Base (v0, v1, v2, v3, n0, n1, n2, n3)
    , subdomain_index_()
    , sliver_value_(FT(0.))
    , sliver_cache_validity_(false) { };
  
  // Destructor
  virtual ~Mesh_cell_base_3() { };
  
  // Default copy constructor and assignment operator are ok
  
  // Returns the index of the cell of the input complex that contains the cell
  Subdomain_index subdomain_index() const { return subdomain_index_; };
  
  // Sets the index of the cell of the input complex that contains the cell
  void set_subdomain_index(const Subdomain_index& index)
  { subdomain_index_ = index; };
  
  
  void set_sliver_value(const FT& value)
  { 
    sliver_cache_validity_ = true;
    sliver_value_ = value;
  }
  const FT& sliver_value() const { return sliver_value_; }
  bool is_cache_valid() const { return sliver_cache_validity_; }
  void reset_cache_validity() const { sliver_cache_validity_ = false;  }
  
private:
  // The index of the cell of the input complex that contains me
  Subdomain_index subdomain_index_;
  
  FT sliver_value_;
  mutable bool sliver_cache_validity_;
};  // end class Mesh_cell_base_3



template < class GT, class MT, class Cb >
std::istream&
operator>>(std::istream &is,
           Mesh_cell_base_3<GT, MT, Cb> &c)
{
  return is >> static_cast<Cb&>(c);
}

template < class GT, class MT, class Cb >
std::ostream&
operator<<(std::ostream &os,
           const Mesh_cell_base_3<GT, MT, Cb> &c)
{
  return os << c.subdomain_index() << static_cast<const Cb&>(c);
}

}  // end namespace CGAL


#endif // CGAL_MESH_CELL_BASE_3_H
