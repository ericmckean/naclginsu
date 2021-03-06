// Copyright (c) 2006  GeometryFactory (France). All rights reserved.
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
// $URL: svn+ssh://scm.gforge.inria.fr/svn/cgal/branches/CGAL-3.6-branch/Surface_mesh_simplification/include/CGAL/Surface_mesh_simplification/Detail/Common.h $
// $Id: Common.h 51456 2009-08-24 17:10:04Z spion $
//
// Author(s)     : Fernando Cacciola <fernando.cacciola@geometryfactory.com>
//
#ifndef CGAL_SURFACE_MESH_SIMPLIFICATION_DETAIL_COMMON_H
#define CGAL_SURFACE_MESH_SIMPLIFICATION_DETAIL_COMMON_H 1

#include <functional>
#include <utility>
#include <vector>
#include <vector>
#include <set>

#include <boost/config.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/iterator_adaptors.hpp>
#include <boost/optional/optional.hpp>
#include <boost/none.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/format.hpp>
#include <boost/scoped_array.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/adjacency_list.hpp>

#include <CGAL/Cartesian/MatrixC33.h>
#include <CGAL/Modifiable_priority_queue.h>
#include <CGAL/boost/graph/halfedge_graph_traits.h>

CGAL_BEGIN_NAMESPACE

namespace Surface_mesh_simplification 
{

using boost::num_edges ;
using boost::num_vertices ;
using boost::edges ;
using boost::out_edges ;
using boost::in_edges ;
using boost::source ;
using boost::target ;

using boost::shared_ptr ;
using boost::optional ;
using boost::none ;
using boost::put_get_helper ;
using boost::get ;
using boost::put ;
using boost::addressof ;

using namespace boost::tuples ;

template<class Handle>
inline bool handle_assigned( Handle h ) { Handle null ; return h != null ; }

template<class Iterator, class Handle>
bool handle_exists ( Iterator begin, Iterator end, Handle h )
{
 if ( handle_assigned(h) )
 {
   while ( begin != end )
     if ( begin++ == h )
       return true ;
 }
 return false ;
}

} // namespace Surface_mesh_simplification

template<class N>
inline std::string n_to_string( N const& n )
{
  return boost::str( boost::format("%|5.19g|") % n ) ;   
}

template<class XYZ>
inline std::string xyz_to_string( XYZ const& xyz )
{
  return boost::str( boost::format("(%|5.19g|,%|5.19g|,%|5.19g|)") % xyz.x() % xyz.y() % xyz.z() ) ;   
}

template<class Matrix>
inline std::string matrix_to_string( Matrix const& m )
{
  return boost::str( boost::format("[%1%|%2%|%3%]") % xyz_to_string(m.r0()) % xyz_to_string(m.r1()) % xyz_to_string(m.r2()) ) ;
}

template<class T>
inline std::string optional_to_string( boost::optional<T> const& o )
{
  if ( o )
       return boost::str( boost::format("%1%") % *o ) ;   
  else return std::string("NONE");  
}


CGAL_END_NAMESPACE

#if   defined(CGAL_SURFACE_SIMPLIFICATION_ENABLE_TRACE)    \
   || defined(CGAL_SURFACE_SIMPLIFICATION_ENABLE_LT_TRACE) 
#define CGAL_ECMS_ENABLE_TRACE
#endif

#ifdef CGAL_ECMS_ENABLE_TRACE

#  include<string>
#  include<iostream>
#  include<sstream>
namespace internal { namespace  { bool cgal_enable_ecms_trace = false ; } }
#  define CGAL_ECMS_TRACE_IMPL(m) \
     if ( ::internal::cgal_enable_ecms_trace ) { \
       std::ostringstream ss ; ss << m ; std::string s = ss.str(); \
       Surface_simplification_external_trace(s); \
     }
     
#  define CGAL_ECMS_DEBUG_CODE(code) code     

#else

#  define CGAL_ECMS_DEBUG_CODE(code)

#endif

#ifdef CGAL_SURFACE_SIMPLIFICATION_ENABLE_LT_TRACE
#  define CGAL_ECMS_LT_TRACE(l,m) if ( (l) <= CGAL_SURFACE_SIMPLIFICATION_ENABLE_LT_TRACE ) CGAL_ECMS_TRACE_IMPL(m)
#else
#  define CGAL_ECMS_LT_TRACE(l,m)
#endif

#ifdef CGAL_SURFACE_SIMPLIFICATION_ENABLE_TRACE
#  define CGAL_ECMS_TRACE_IF(c,l,m) if ( (c) && ( (l) <= CGAL_SURFACE_SIMPLIFICATION_ENABLE_TRACE) ) CGAL_ECMS_TRACE_IMPL(m)
#  define CGAL_ECMS_TRACE(l,m)      if ( (l) <= CGAL_SURFACE_SIMPLIFICATION_ENABLE_TRACE ) CGAL_ECMS_TRACE_IMPL(m)
#else
#  define CGAL_ECMS_TRACE_IF(c,l,m)
#  define CGAL_ECMS_TRACE(l,m)
#endif

#undef CGAL_ECMS_ENABLE_TRACE

#ifdef CGAL_TESTING_SURFACE_MESH_SIMPLIFICATION
#  define CGAL_SURF_SIMPL_TEST_assertion(EX)         CGAL_assertion(EX)
#  define CGAL_SURF_SIMPL_TEST_assertion_msg(EX,MSG) CGAL_assertion_msg(EX,MSG) 
#  define CGAL_SURF_SIMPL_TEST_assertion_code(CODE)  CGAL_assertion_code(CODE)
#else
#  define CGAL_SURF_SIMPL_TEST_assertion(EX) 
#  define CGAL_SURF_SIMPL_TEST_assertion_msg(EX,MSG) 
#  define CGAL_SURF_SIMPL_TEST_assertion_code(CODE) 
#endif


#endif // CGAL_SURFACE_MESH_SIMPLIFICATION_DETAIL_COMMON_H //
// EOF //
 
