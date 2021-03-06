// Copyright (c) 2005-2009 Tel-Aviv University (Israel).
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
// $URL: svn+ssh://scm.gforge.inria.fr/svn/cgal/branches/CGAL-3.6-branch/Arrangement_on_surface_2/include/CGAL/Arrangement_on_surface_2.h $
// $Id: Arrangement_on_surface_2.h 53164 2009-11-24 15:55:41Z efif $
// 
//
// Author(s): Ron Wein          <wein@post.tau.ac.il>
//            Efi Fogel         <efif@post.tau.ac.il>
//            Eric Berberich    <ericb@post.tau.ac.il>
//            (based on old version by: Iddo Hanniel,
//                                      Eyal Flato,
//                                      Oren Nechushtan,
//                                      Ester Ezra,
//                                      Shai Hirsch,
//                                      and Eugene Lipovetsky)
#ifndef CGAL_ARRANGEMENT_ON_SURFACE_2_H
#define CGAL_ARRANGEMENT_ON_SURFACE_2_H

/*! \file
 * The header file for the Arrangement_on_surface_2<Traits,Dcel> class.
 */

#include <boost/mpl/assert.hpp>
#include <CGAL/Arr_tags.h>
#include <CGAL/Arr_enums.h>
#include <CGAL/HalfedgeDS_iterator.h>
#include <CGAL/Arrangement_2/Arrangement_2_iterators.h>
#include <CGAL/In_place_list.h>
#include <CGAL/Arr_default_dcel.h>
#include <CGAL/Arr_observer.h>
#include <CGAL/Arr_accessor.h>
#include <CGAL/Arrangement_2/Arr_traits_adaptor_2.h>
#include <CGAL/function_objects.h>
#include <CGAL/Iterator_project.h>
#include <CGAL/Iterator_transform.h>
#include <map>
#include <vector>
#include <algorithm>

CGAL_BEGIN_NAMESPACE

/*! \class Arrangement_on_surface_2
 * The arrangement class, representing 2-dimensional subdivisions induced on
 * an arbitrary surface by a set of arbitrary planar. 
 * The GeomTraits parameter corresponds to a geometry-traits class that
 * is defines the Point_2 and X_monotone_curve_2 types and implements the
 * geometric predicates and constructions for the family of curves it defines.
 * The TopTraits parameter corresponds to a topology-traits class that defines
 * the topological structure of the surface. Note that the geometry traits
 * class should also be aware of the kind of surface on which its curves and
 * points are defined.
 */
template <class GeomTraits_, class TopTraits_> 
class Arrangement_on_surface_2
{
public:

  typedef GeomTraits_                                     Geometry_traits_2;
  typedef TopTraits_                                      Topology_traits;

  // first define adaptor ...
  typedef Arr_traits_basic_adaptor_2<Geometry_traits_2>   Traits_adaptor_2;
  
  // .. as it completes (potentially) missing side tags
  typedef typename Traits_adaptor_2::Arr_left_side_category    Arr_left_side_category;
  typedef typename Traits_adaptor_2::Arr_bottom_side_category  Arr_bottom_side_category;
  typedef typename Traits_adaptor_2::Arr_top_side_category     Arr_top_side_category;
  typedef typename Traits_adaptor_2::Arr_right_side_category   Arr_right_side_category;
  
  BOOST_MPL_ASSERT(
      (typename 
       Arr_sane_identified_tagging< Arr_left_side_category, Arr_bottom_side_category, 
       Arr_top_side_category, Arr_right_side_category >::result)
  );

public:
  
  typedef Arrangement_on_surface_2<Geometry_traits_2,
                                   Topology_traits>       Self;

  typedef typename Geometry_traits_2::Point_2             Point_2;
  typedef typename Geometry_traits_2::X_monotone_curve_2  X_monotone_curve_2;

  // maybe remove this in a future version (that supports complete handling
  // of all sides)
  typedef typename Arr_are_all_sides_oblivious_tag< 
    Arr_left_side_category, Arr_bottom_side_category, 
    Arr_top_side_category, Arr_right_side_category >::result
  Are_all_sides_oblivious_tag;
  
public:

  typedef typename Topology_traits::Dcel                  Dcel;
  typedef typename Dcel::Size                             Size;

protected:

  friend class Arr_observer<Self>;
  friend class Arr_accessor<Self>;
  
  // Internal DCEL types:
  typedef typename Dcel::Vertex                     DVertex;
  typedef typename Dcel::Halfedge                   DHalfedge;
  typedef typename Dcel::Face                       DFace;
  typedef typename Dcel::Outer_ccb                  DOuter_ccb;
  typedef typename Dcel::Inner_ccb                  DInner_ccb;
  typedef typename Dcel::Isolated_vertex            DIso_vertex;

  typedef typename Dcel::difference_type            DDifference;  
  typedef typename Dcel::iterator_category          DIterator_category;

  typedef typename Dcel::Vertex_iterator            DVertex_iter;
  typedef typename Dcel::Vertex_const_iterator      DVertex_const_iter;

  typedef typename Dcel::Halfedge_iterator          DHalfedge_iter;
  typedef typename Dcel::Halfedge_const_iterator    DHalfedge_const_iter;

  typedef typename Dcel::Edge_iterator              DEdge_iter;
  typedef typename Dcel::Edge_const_iterator        DEdge_const_iter;

  typedef typename Dcel::Face_iterator              DFace_iter;
  typedef typename Dcel::Face_const_iterator        DFace_const_iter;

  typedef typename DFace::Outer_ccb_iterator        DOuter_ccb_iter;
  typedef typename DFace::Outer_ccb_const_iterator  DOuter_ccb_const_iter;

  typedef typename DFace::Inner_ccb_iterator        DInner_ccb_iter;
  typedef typename DFace::Inner_ccb_const_iterator  DInner_ccb_const_iter;

  typedef typename DFace::Isolated_vertex_iterator
                                                    DIso_vertex_iter;
  typedef typename DFace::Isolated_vertex_const_iterator
                                                    DIso_vertex_const_iter;

protected:

  /*! \class
   * A functor for filtering DCEL vertices at infinity.
   */
  class _Is_concrete_vertex
  {
  private:

    const Topology_traits  *m_topol_traits;

  public:

    _Is_concrete_vertex () :
      m_topol_traits (NULL)
    {}

    _Is_concrete_vertex (const Topology_traits * topol_traits) :
      m_topol_traits (topol_traits)
    {}

    bool operator() (const DVertex& v) const
    {
      if (m_topol_traits == NULL)
        return (true);

      return (m_topol_traits->is_concrete_vertex (&v));
    }
  };

  /*! \class
   * A functor for filtering fictitious DCEL vertices.
   */
  class _Is_valid_vertex
  {
  private:

    const Topology_traits * m_topol_traits;

  public:

    _Is_valid_vertex () :
      m_topol_traits (NULL)
    {}

    _Is_valid_vertex (const Topology_traits * topol_traits) :
      m_topol_traits (topol_traits)
    {}

    bool operator() (const DVertex& v) const
    {
      if (m_topol_traits == NULL)
        return (true);

      return (m_topol_traits->is_valid_vertex (&v));
    }
  };

  /*! \struct
   * A functor for filtering fictitious DCEL halfedges.
   */
  class _Is_valid_halfedge
  {
  private:

    const Topology_traits  *m_topol_traits;

  public:

    _Is_valid_halfedge () :
      m_topol_traits (NULL)
    {}

    _Is_valid_halfedge (const Topology_traits * topol_traits) :
      m_topol_traits (topol_traits)
    {}

    bool operator() (const DHalfedge& he) const
    {
      if (m_topol_traits == NULL)
        return (true);

      return (m_topol_traits->is_valid_halfedge (&he));
    }
  };

  /*! \struct
   * A functor for filtering the fictitious faces.
   */
  class _Is_valid_face
  {
  private:

    const Topology_traits  *m_topol_traits;

  public:

    _Is_valid_face () :
      m_topol_traits (NULL)
    {}

    _Is_valid_face (const Topology_traits * topol_traits) :
      m_topol_traits (topol_traits)
    {}

    bool operator() (const DFace& f) const
    {
      if (m_topol_traits == NULL)
        return (true);

      return (m_topol_traits->is_valid_face (&f));
    }
  };

  /*! \struct
   * A functor for filtering bounded faces.
   */
  class _Is_unbounded_face
  {
  private:

    const Topology_traits * m_topol_traits;

  public:

    _Is_unbounded_face () :
      m_topol_traits (NULL)
    {}

    _Is_unbounded_face (const Topology_traits * topol_traits) :
      m_topol_traits (topol_traits)
    {}

    const Topology_traits * topology_traits() const { return m_topol_traits; }

    bool operator() (const DFace& f) const
    {
      return (m_topol_traits->is_valid_face (&f) &&
              m_topol_traits->is_unbounded (&f));
    }
  };

public:

  // Forward declerations:
  class Vertex;
  class Halfedge;
  class Face;

  // Definition of the halfedge data-structure itereators and circulators:
  typedef I_Filtered_iterator
    <DVertex_iter, _Is_concrete_vertex,
     Vertex, DDifference,
     DIterator_category>                          Vertex_iterator;
  
  typedef I_Filtered_const_iterator
    <DVertex_const_iter, _Is_concrete_vertex,
     DVertex_iter, Vertex,
     DDifference, DIterator_category>             Vertex_const_iterator;
  
  typedef I_Filtered_iterator
    <DHalfedge_iter, _Is_valid_halfedge, 
     Halfedge, DDifference,
     DIterator_category>                          Halfedge_iterator;
  
  typedef I_Filtered_const_iterator
    <DHalfedge_const_iter, _Is_valid_halfedge,
     DHalfedge_iter, Halfedge, DDifference,
     DIterator_category>                          Halfedge_const_iterator;

  /*! \class
   * Edges iterator - defined as a derived class to make it assignable
   * to the halfedge iterator type.
   */
  class Edge_iterator :
    public I_Filtered_iterator<DEdge_iter, _Is_valid_halfedge,
                               Halfedge, DDifference,
                               DIterator_category>
  {
    typedef I_Filtered_iterator<DEdge_iter, 
                                _Is_valid_halfedge,
                                Halfedge, DDifference,
                                DIterator_category>          Base;

  public:
 
    Edge_iterator ()
    {}

    Edge_iterator (DEdge_iter iter, DEdge_iter iend,
                   const _Is_valid_halfedge& pred) :
      Base (iter, iend, pred)
    {}

    // Casting to a halfedge iterator.
    operator Halfedge_iterator () const
    {
      return (Halfedge_iterator (DHalfedge_iter (this->current_iterator())));
    }

    operator Halfedge_const_iterator () const
    {
      return (Halfedge_const_iterator 
              (DHalfedge_const_iter (this->current_iterator())));
    }    
  };

  class Edge_const_iterator :
    public I_Filtered_const_iterator<DEdge_const_iter,
                                     _Is_valid_halfedge,
                                     DEdge_iter,
                                     Halfedge, DDifference,
                                     DIterator_category>
  {
    typedef I_Filtered_const_iterator<DEdge_const_iter,
                                      _Is_valid_halfedge,
                                      DEdge_iter,
                                      Halfedge, DDifference,
                                      DIterator_category>    Base;

  public:
 
    Edge_const_iterator ()
    {}

    Edge_const_iterator (Edge_iterator iter) :
      Base (iter.current_iterator(),
            iter.past_the_end(),
            iter.filter())
    {}

    Edge_const_iterator (DEdge_const_iter iter, DEdge_const_iter iend,
                         const _Is_valid_halfedge& pred) :
      Base (iter, iend, pred)
    {}

    // Casting to a halfedge iterator.
    operator Halfedge_const_iterator () const
    {
      return (Halfedge_const_iterator 
              (DHalfedge_const_iter (this->current_iterator())));
    }
  };
  
  typedef I_Filtered_iterator
    <DFace_iter, _Is_valid_face,
     Face, DDifference,
     DIterator_category>                          Face_iterator;
  
  typedef I_Filtered_const_iterator
    <DFace_const_iter, _Is_valid_face, 
     DFace_iter, Face,
     DDifference, DIterator_category>             Face_const_iterator;

  typedef _HalfedgeDS_vertex_circ
    <Halfedge, Halfedge_iterator,
     Bidirectional_circulator_tag>    Halfedge_around_vertex_circulator;

  typedef _HalfedgeDS_vertex_const_circ
    <Halfedge,
     Halfedge_const_iterator,
     Bidirectional_circulator_tag>    Halfedge_around_vertex_const_circulator;

  typedef _HalfedgeDS_facet_circ
    <Halfedge,
     Halfedge_iterator,
     Bidirectional_circulator_tag>    Ccb_halfedge_circulator;

  typedef _HalfedgeDS_facet_const_circ
    <Halfedge,
     Halfedge_const_iterator,
     Bidirectional_circulator_tag>    Ccb_halfedge_const_circulator;

  /*! \class
   * Unbounded faces iterator - defined as a derived class to make it
   * assignable to the face iterator type.
   */  
  class Unbounded_face_iterator :
    public I_Filtered_iterator<DFace_iter, _Is_unbounded_face,
                               Face, DDifference,
                               DIterator_category>
  {
    typedef I_Filtered_iterator<DFace_iter,
                                _Is_unbounded_face,
                                Face, DDifference,
                                DIterator_category>         Base;

  public:
 
    Unbounded_face_iterator ()
    {}

    Unbounded_face_iterator (DFace_iter iter, DFace_iter iend,
                             const _Is_unbounded_face & is_unbounded) :
      Base (iter, iend, is_unbounded)
    {}

    // Casting to a face iterator.
    operator Face_iterator () const
    {
      return (Face_iterator (DFace_iter (this->current_iterator()),
                             DFace_iter (this->past_the_end()),
                             _Is_valid_face(this->filter().topology_traits())));
    }

    operator Face_const_iterator () const
    {
      return (Face_const_iterator
              (DFace_const_iter (this->current_iterator()),
               DFace_const_iter (this->past_the_end()),
               _Is_valid_face(this->filter().topology_traits())));
    }    
  };

  class Unbounded_face_const_iterator :
    public I_Filtered_const_iterator<DFace_const_iter,
                                     _Is_unbounded_face, 
                                     DFace_iter, Face,
                                     DDifference,
                                     DIterator_category>
  {
    typedef I_Filtered_const_iterator<DFace_const_iter,
                                      _Is_unbounded_face, 
                                      DFace_iter, Face,
                                      DDifference,
                                      DIterator_category>   Base;

  public:
 
    Unbounded_face_const_iterator ()
    {}

    Unbounded_face_const_iterator (Unbounded_face_iterator iter) :
      Base (iter)
    {}

    Unbounded_face_const_iterator (DFace_const_iter iter,
                                   DFace_const_iter iend,
                                   const _Is_unbounded_face & is_unbounded) :
      Base (iter, iend, is_unbounded)
    {}

    // Casting to a face iterator.
    operator Face_const_iterator () const
    {
      return (Face_const_iterator (DFace_const_iter(this->current_iterator()),
                                   DFace_const_iter(this->past_the_end())));
    }
  };

protected:

  struct _Halfedge_to_ccb_circulator
  {
    typedef DHalfedge*               argument_type;
    typedef Ccb_halfedge_circulator  result_type;
 
    result_type operator() (argument_type s) const
    {
      return Ccb_halfedge_circulator (Halfedge_iterator (s));
    }
  };

  struct _Const_halfedge_to_ccb_circulator
  {
    typedef const DHalfedge*               argument_type;
    typedef Ccb_halfedge_const_circulator  result_type;
 
    result_type operator() (argument_type s) const
    {
      return Ccb_halfedge_const_circulator (Halfedge_const_iterator (s));
    }
  };

  typedef Cast_function_object<DVertex, Vertex>   _Vertex_to_vertex;

public:
  
  typedef Iterator_transform<DOuter_ccb_iter,
                             _Halfedge_to_ccb_circulator>
                                      Outer_ccb_iterator;

  typedef Iterator_transform<DOuter_ccb_const_iter,
                             _Const_halfedge_to_ccb_circulator> 
                                      Outer_ccb_const_iterator;

  typedef Iterator_transform<DInner_ccb_iter,
                             _Halfedge_to_ccb_circulator>
                                      Inner_ccb_iterator;

  typedef Iterator_transform<DInner_ccb_const_iter,
                             _Const_halfedge_to_ccb_circulator> 
                                      Inner_ccb_const_iterator;

  /*! \class
   * Isolated vertices iterator - defined as a class to make it assignable
   * to the vertex iterator type.
   */
  class Isolated_vertex_iterator :
    public Iterator_project<DIso_vertex_iter,
                            _Vertex_to_vertex>
  {
    typedef Iterator_project<DIso_vertex_iter,
                             _Vertex_to_vertex>         Base;

  public:

    Isolated_vertex_iterator ()
    {}

    Isolated_vertex_iterator (DIso_vertex_iter iter) :
      Base (iter)
    {}

    // Casting to a vertex iterator.
    operator Vertex_iterator () const
    {
      return (Vertex_iterator (DVertex_iter (this->ptr())));
    }

    operator Vertex_const_iterator () const
    {
      return (Vertex_const_iterator (DVertex_const_iter (this->ptr())));
    }
  };
  
  class Isolated_vertex_const_iterator :
    public Iterator_project<DIso_vertex_const_iter,
                            _Vertex_to_vertex>
  {
    typedef Iterator_project<DIso_vertex_const_iter,
                             _Vertex_to_vertex>          Base;

  public:

    Isolated_vertex_const_iterator ()
    {}

    Isolated_vertex_const_iterator (Isolated_vertex_iterator iter) :
      Base (iter)
    {}

    Isolated_vertex_const_iterator (DIso_vertex_const_iter iter) :
      Base (iter)
    {}

    // Casting to a vertex iterator.
    operator Vertex_const_iterator () const
    {
      return (Vertex_const_iterator (DVertex_const_iter (this->ptr())));
    }
  };

protected:

  class _Valid_vertex_iterator :
    public I_Filtered_iterator<DVertex_iter,
                               _Is_valid_vertex, Vertex,
                               DDifference,
                               DIterator_category>
  {
    typedef I_Filtered_iterator<DVertex_iter,
                                _Is_valid_vertex, Vertex,
                                DDifference,
                                DIterator_category>               Base;

   public:
  
    _Valid_vertex_iterator ()
    {}

    _Valid_vertex_iterator (DVertex_iter iter, DVertex_iter iend,
                            const _Is_valid_vertex& pred) :
      Base (iter, iend, pred)
    {}

    // Casting to a vertex iterator.
    operator Vertex_iterator () const
    {
      return (Vertex_iterator (DVertex_iter (this->current_iterator())));
    }

    operator Vertex_const_iterator () const
    {
      return (Vertex_const_iterator (DVertex_const_iter
                                     (this->current_iterator())));
    }
  };

public:

  // Definition of handles (equivalent to iterators):
  typedef Vertex_iterator              Vertex_handle;
  typedef Halfedge_iterator            Halfedge_handle;
  typedef Face_iterator                Face_handle;

  typedef Vertex_const_iterator        Vertex_const_handle;
  typedef Halfedge_const_iterator      Halfedge_const_handle;
  typedef Face_const_iterator          Face_const_handle;

  /*! \class
   * The arrangement vertex class.
   */
  class Vertex : public DVertex
  {
    typedef DVertex               Base;

  public:

    /*! Default constrcutor. */
    Vertex()
    {}

    /*! \deprecated Use is_at_open_boundary instead. */
    CGAL_DEPRECATED bool is_at_infinity() const
    { return (Base::has_null_point()); }

    /*! Check whether the vertex lies on an open boundary. */
    bool is_at_open_boundary () const
    {
      return (Base::has_null_point());
    }

    /*! Get the vertex degree (number of incident edges). */
    Size degree () const
    {
      if (this->is_isolated())
        return (0);

      // Go around the vertex and count the incident halfedges.
      const DHalfedge  *he_first = Base::halfedge();
      const DHalfedge  *he_curr = he_first;
      Size              n = 0;

      if (he_curr != NULL)
      {
        do
        {
          n++;
          he_curr = he_curr->next()->opposite();
        } while (he_curr != he_first);
      }
      return (n);
    }

    /*!
     * Get the incident halfedges (non-const version).
     * \pre The vertex is not isolated.
     */
    Halfedge_around_vertex_circulator incident_halfedges() 
    {
      CGAL_precondition (! this->is_isolated());

      return Halfedge_around_vertex_circulator
        (DHalfedge_iter (Base::halfedge()));
    }

    /*!
     * Get the incident halfedges (const version).
     * \pre The vertex is not isolated.
     */
    Halfedge_around_vertex_const_circulator incident_halfedges() const 
    {
      CGAL_precondition (! this->is_isolated());

      return Halfedge_around_vertex_const_circulator
        (DHalfedge_const_iter (Base::halfedge())); 
    }

    /*! 
     * Get the face that contains the vertex (non-const version).
     * \pre The vertex is isolated.
     */
    Face_handle face()
    {
      CGAL_precondition (this->is_isolated());

      return (DFace_iter (Base::isolated_vertex()->face()));
    }

    /*! 
     * Get the face that contains the vertex (const version).
     * \pre The vertex is isolated.
     */
    Face_const_handle face() const
    {
      CGAL_precondition (this->is_isolated());

      return (DFace_const_iter (Base::isolated_vertex()->face()));
    }

  private:

    // Blocking access to inherited functions from the Dcel::Vertex.
    bool has_null_point () const;
    void set_point (Point_2* );
    void set_boundary (Arr_parameter_space , Arr_parameter_space );
    const DHalfedge* halfedge () const;
    DHalfedge* halfedge ();
    void set_halfedge (DHalfedge* );
    const DIso_vertex* isolated_vertex () const;
    DIso_vertex* isolated_vertex ();
    void set_isolated_vertex (DIso_vertex* );
  };

  /*!
   * \class The arrangement halfedge class.
   */
  class Halfedge : public DHalfedge
  {
    typedef DHalfedge             Base;

  public:

    /*! Default constrcutor. */
    Halfedge ()
    {}

    /*! Check whether the halfedge is fictitious. */
    bool is_fictitious () const
    {
      return (Base::has_null_curve());
    }

    /*! Get the source vertex (non-const version). */
    Vertex_handle source ()
    {
      return (DVertex_iter (Base::opposite()->vertex()));
    }

    /*! Get the source vertex (const version). */
    Vertex_const_handle source () const
    {
      return (DVertex_const_iter (Base::opposite()->vertex()));
    }
    
    /*! Get the target vertex (non-const version). */
    Vertex_handle target ()
    {
      return (DVertex_iter (Base::vertex()));
    }

    /*! Get the target vertex (const version). */
    Vertex_const_handle target () const
    {
      return (DVertex_const_iter (Base::vertex()));
    }
    
    /*! Get the incident face (non-const version). */
    Face_handle face()
    {
      if (! Base::is_on_inner_ccb())
        return (DFace_iter (Base::outer_ccb()->face()));
      else
        return (DFace_iter (Base::inner_ccb()->face()));
    }

    /*! Get the incident face (const version). */
    Face_const_handle face() const
    {
      if (! Base::is_on_inner_ccb())
        return (DFace_const_iter (Base::outer_ccb()->face()));
      else
        return (DFace_const_iter (Base::inner_ccb()->face()));
    }

    /*! Get the twin halfedge (non-const version). */
    Halfedge_handle twin()
    {
      return (DHalfedge_iter (Base::opposite()));
    }

    /*! Get the twin halfedge (const version). */
    Halfedge_const_handle twin() const 
    {
      return (DHalfedge_const_iter (Base::opposite()));
    }

   /*! Get the previous halfegde in the chain (non-const version). */
    Halfedge_handle prev () 
    { 
      return (DHalfedge_iter (Base::prev()));
    }

    /*! Get the previous halfegde in the chain (const version). */
    Halfedge_const_handle prev () const 
    { 
      return (DHalfedge_const_iter (Base::prev()));
    }

    /*! Get the next halfegde in the chain (non-const version). */
    Halfedge_handle next () 
    { 
      return (DHalfedge_iter (Base::next()));
    }

    /*! Get the next halfegde in the chain (const version). */
    Halfedge_const_handle next () const 
    { 
      return (DHalfedge_const_iter (Base::next()));
    }

    /*! Get the connected component of the halfedge (non-const version). */
    Ccb_halfedge_circulator ccb ()
    { 
      return Ccb_halfedge_circulator (DHalfedge_iter (this));
    }

    /*! Get the connected component of the halfedge (const version). */
    Ccb_halfedge_const_circulator ccb () const
    { 
      return Ccb_halfedge_const_circulator (DHalfedge_const_iter (this));
    }

  private:

    // Blocking access to inherited functions from the Dcel::Halfedge.
    bool has_null_curve () const;
    void set_curve (X_monotone_curve_2* );
    const DHalfedge* opposite () const;
    DHalfedge* opposite ();
    void set_opposite (DHalfedge* );
    void set_direction (Arr_halfedge_direction );
    void set_prev (DHalfedge* );
    void set_next (DHalfedge* );
    const DVertex* vertex () const ;
    DVertex* vertex ();
    void set_vertex (DVertex* );
    const DOuter_ccb* outer_ccb () const;
    DOuter_ccb* outer_ccb ();
    void set_outer_ccb (DOuter_ccb* );
    const DInner_ccb* inner_ccb () const;
    DInner_ccb* inner_ccb ();
    void set_inner_ccb (DInner_ccb* );
  };

  /*!
   * \class The arrangement face class.
   */
  class Face : public DFace
  {
    typedef DFace                 Base;

  public:

    /*! Default constrcutor. */
    Face()
    {}

    /*! Get an iterator for the outer CCBs of the face (non-const version). */
    Outer_ccb_iterator outer_ccbs_begin() 
    {
      return (DOuter_ccb_iter (Base::outer_ccbs_begin()));
    }

    /*! Get an iterator for the outer CCBs the face (const version). */
    Outer_ccb_const_iterator outer_ccbs_begin() const
    {
      return (DOuter_ccb_const_iter (Base::outer_ccbs_begin()));
    }
    
    /*! Get a past-the-end iterator for the outer CCBs (non-const version). */
    Outer_ccb_iterator outer_ccbs_end() 
    {
      return (DOuter_ccb_iter (Base::outer_ccbs_end()));
    }

    /*! Get a past-the-end iterator for the outer CCBs (const version). */
    Outer_ccb_const_iterator outer_ccbs_end() const 
    {
      return (DOuter_ccb_const_iter (Base::outer_ccbs_end()));
    }


    /*! Get an iterator for the inner CCBs of the face (non-const version). */
    Inner_ccb_iterator inner_ccbs_begin() 
    {
      return (DInner_ccb_iter (Base::inner_ccbs_begin()));
    }

    /*! Get an iterator for the inner CCBs the face (const version). */
    Inner_ccb_const_iterator inner_ccbs_begin() const
    {
      return (DInner_ccb_const_iter (Base::inner_ccbs_begin()));
    }
    
    /*! Get a past-the-end iterator for the inner CCBs (non-const version). */
    Inner_ccb_iterator inner_ccbs_end() 
    {
      return (DInner_ccb_iter (Base::inner_ccbs_end()));
    }

    /*! Get a past-the-end iterator for the inner CCBs (const version). */
    Inner_ccb_const_iterator inner_ccbs_end() const 
    {
      return (DInner_ccb_const_iter (Base::inner_ccbs_end()));
    }


    /*! Get an iterator for the isolated_vertices inside the face
     * (non-const version). */
    Isolated_vertex_iterator isolated_vertices_begin ()
    {
      return (DIso_vertex_iter (Base::isolated_vertices_begin()));
    }

    /*! Get an iterator for the isolated_vertices inside the face
     * (const version). */
    Isolated_vertex_const_iterator isolated_vertices_begin () const
    {
      return (DIso_vertex_const_iter (Base::isolated_vertices_begin()));
    }
    
    /*! Get a past-the-end iterator for the isolated_vertices 
     * (non-const version). */
    Isolated_vertex_iterator isolated_vertices_end () 
    {
      return (DIso_vertex_iter (Base::isolated_vertices_end()));
    }

    /*! Get a past-the-end iterator for the isolated_vertices
     * (const version). */
    Isolated_vertex_const_iterator isolated_vertices_end () const 
    {
      return (DIso_vertex_const_iter (Base::isolated_vertices_end()));
    }

    /// \name These functions are kept for backward compatibility:
    //@{

    /*!
     * Get a circulator for the outer boundary (non-const version). 
     * \pre The face has a single outer CCB.
     */
    Ccb_halfedge_circulator outer_ccb () 
    {
      CGAL_precondition (Base::number_of_outer_ccbs() == 1);
      
      DOuter_ccb_iter     iter = Base::outer_ccbs_begin();
      DHalfedge          *he = *iter;

      return Ccb_halfedge_circulator (DHalfedge_iter (he));
    }

    /*!
     * Get a circulator for the outer boundary (const version).
     * \pre The face has a single outer CCB.
     */
    Ccb_halfedge_const_circulator outer_ccb () const
    {
      CGAL_precondition (Base::number_of_outer_ccbs() == 1);

      DOuter_ccb_const_iter  iter = Base::outer_ccbs_begin();
      const DHalfedge       *he = *iter;
 
      return Ccb_halfedge_const_circulator (DHalfedge_const_iter (he));
    }

    /*! Get the number of holes (inner CCBs) inside the face. */
    Size number_of_holes () const
    {
      return (Base::number_of_inner_ccbs());
    }

    /*! Get an iterator for the holes inside the face (non-const version). */
    Inner_ccb_iterator holes_begin() 
    {
      return (this->inner_ccbs_begin());
    }

    /*! Get an iterator for the holes inside the face (const version). */
    Inner_ccb_const_iterator holes_begin() const
    {
      return (this->inner_ccbs_begin());
    }
    
    /*! Get a past-the-end iterator for the holes (non-const version). */
    Inner_ccb_iterator holes_end() 
    {
      return (this->inner_ccbs_end());
    }

    /*! Get a past-the-end iterator for the holes (const version). */
    Inner_ccb_const_iterator holes_end() const 
    {
      return (this->inner_ccbs_end());
    }
    //@}

  private:

    // Blocking access to inherited functions from the Dcel::Face.
    void set_unbounded (bool );
    void set_fictitious (bool );
    void add_outer_ccb (DOuter_ccb*, Halfedge* );
    void erase_outer_ccb (DOuter_ccb* );
    void add_inner_ccb (DInner_ccb*, Halfedge* );
    void erase_inner_ccb (DInner_ccb* );
    void add_isolated_vertex (DIso_vertex*, DVertex* );
    void erase_isolated_vertex (DIso_vertex* );
  };

protected:

  typedef CGAL_ALLOCATOR(Point_2)                 Points_alloc;
  typedef CGAL_ALLOCATOR(X_monotone_curve_2)      Curves_alloc;

  typedef Arr_observer<Self>                      Observer;
  typedef std::list<Observer*>                    Observers_container;
  typedef typename Observers_container::iterator  Observers_iterator;

  typedef typename Observers_container::reverse_iterator  
                                                  Observers_rev_iterator;

  // Data members:
  Topology_traits          m_topol_traits;  // the topology traits.
  Points_alloc             m_points_alloc;  // allocator for the points.
  Curves_alloc             m_curves_alloc;  // allocator for the curves.
  Observers_container      m_observers;     // pointers to existing observers.
  const Traits_adaptor_2 * m_geom_traits;   // the geometry-traits adaptor.
  bool                     m_own_traits;    // inidicates whether the geometry
                                            // traits should be freed up.
  Arr_boundary_type        m_boundary_types[4];   

public:
  
  /// \name Constructors.
  //@{

  /*! Default constructor. */
  Arrangement_on_surface_2 ();

  /*! Copy constructor. */
  Arrangement_on_surface_2 (const Self & arr);

  /*! Constructor given a traits object. */
  Arrangement_on_surface_2 (const Geometry_traits_2 * geom_traits);
  //@}

  /// \name Assignment functions.
  //@{

  /*! Assignment operator. */
  Self& operator= (const Self & arr);

  /*! Assign an arrangement. */
  void assign (const Self & arr);
  //@}

  /// \name Destruction functions.
  //@{

  /*! Destructor. */
  virtual ~Arrangement_on_surface_2 ();

  /*! Clear the arrangement. */
  virtual void clear();
  //@}

  /// \name Access the traits-class objects.
  //@{

  /*! Access the geometry-traits object (const version). */
  inline const Traits_adaptor_2 * traits_adaptor () const
  {
    return (m_geom_traits);
  }

  /*! Access the geometry-traits object (const version). */
  inline const Geometry_traits_2 * geometry_traits () const
  {
    return (m_geom_traits);
  }

  /*! Access the topology-traits object (non-const version). */
  inline Topology_traits * topology_traits ()
  {
    return (&m_topol_traits);
  }
  
  /*! Access the topology-traits object (const version). */
  inline const Topology_traits * topology_traits () const
  {
    return (&m_topol_traits);
  }
  //@}

  /// \name Access the arrangement dimensions.
  //@{

  /*! Check whether the arrangement is empty. */
  bool is_empty () const
  {
    return (m_topol_traits.is_empty_dcel());
  }

  /*!
   * Check whether the arrangement is valid. In particular, check the
   * validity of each vertex, halfedge and face, their incidence relations
   * and the geometric properties of the arrangement.
   */
  bool is_valid() const;
  
  /*! Get the number of arrangement vertices. */
  Size number_of_vertices () const
  {
    return (m_topol_traits.number_of_concrete_vertices());
  }

  /*! Get the number of isolated arrangement vertices. */
  Size number_of_isolated_vertices () const
  {
    return (_dcel().size_of_isolated_vertices());
  }

  /*! Get the number of arrangement halfedges (the result is always even). */
  Size number_of_halfedges () const
  {
    return (m_topol_traits.number_of_valid_halfedges());
  }

  /*! Get the number of arrangement edges. */
  Size number_of_edges () const
  {
    return (m_topol_traits.number_of_valid_halfedges() / 2);
  }

  /*! Get the number of arrangement faces. */
  Size number_of_faces () const
  {
    return (m_topol_traits.number_of_valid_faces());
  }

  /*! Get the number of unbounded faces in the arrangement. */
  Size number_of_unbounded_faces () const
  {
    Unbounded_face_const_iterator    iter = unbounded_faces_begin();
    Unbounded_face_const_iterator    end = unbounded_faces_end();
    Size                             n_unb = 0;

    while (iter != end)
    {
      n_unb++;
      ++iter;
    }

    return (n_unb);
  }
  //@}

  /// \name Traversal functions for the arrangement vertices.
  //@{

  /*! Get an iterator for the first vertex in the arrangement. */
  Vertex_iterator vertices_begin() 
  { 
    return (Vertex_iterator (_dcel().vertices_begin(),
                             _dcel().vertices_end(),
                             _Is_concrete_vertex (&m_topol_traits))); 
  }

  /*! Get a past-the-end iterator for the arrangement vertices. */
  Vertex_iterator vertices_end()
  {
    return (Vertex_iterator (_dcel().vertices_end(),
                             _dcel().vertices_end(),
                             _Is_concrete_vertex (&m_topol_traits))); 
  }

  /*! Get a const iterator for the first vertex in the arrangement. */
  Vertex_const_iterator vertices_begin() const
  { 
    return (Vertex_const_iterator (_dcel().vertices_begin(),
                                   _dcel().vertices_end(),
                                   _Is_concrete_vertex (&m_topol_traits))); 
  }
  
  /*! Get a past-the-end const iterator for the arrangement vertices. */
  Vertex_const_iterator vertices_end() const
  {
    return (Vertex_const_iterator (_dcel().vertices_end(),
                                   _dcel().vertices_end(),
                                   _Is_concrete_vertex (&m_topol_traits))); 
  }
  //@}

  /// \name Traversal functions for the arrangement halfedges.
  //@{

  /*! Get an iterator for the first halfedge in the arrangement. */
  Halfedge_iterator halfedges_begin() 
  { 
    return (Halfedge_iterator (_dcel().halfedges_begin(),
                               _dcel().halfedges_end(),
                               _Is_valid_halfedge (&m_topol_traits))); 
  }

  /*! Get a past-the-end iterator for the arrangement halfedges. */
  Halfedge_iterator halfedges_end()
  {
    return (Halfedge_iterator (_dcel().halfedges_end(),
                               _dcel().halfedges_end(),
                               _Is_valid_halfedge (&m_topol_traits))); 
  }

  /*! Get a const iterator for the first halfedge in the arrangement. */
  Halfedge_const_iterator halfedges_begin() const
  { 
    return (Halfedge_const_iterator (_dcel().halfedges_begin(),
                                     _dcel().halfedges_end(),
                                     _Is_valid_halfedge (&m_topol_traits))); 
  }
  
  /*! Get a past-the-end const iterator for the arrangement halfedges. */
  Halfedge_const_iterator halfedges_end() const
  {
    return (Halfedge_const_iterator (_dcel().halfedges_end(),
                                     _dcel().halfedges_end(),
                                     _Is_valid_halfedge (&m_topol_traits))); 
  }
  //@}

  /// \name Traversal functions for the arrangement edges.
  //@{

  /*! Get an iterator for the first edge in the arrangement. */
  Edge_iterator edges_begin() 
  { 
    return (Edge_iterator (_dcel().edges_begin(),
                           _dcel().edges_end(),
                           _Is_valid_halfedge (&m_topol_traits))); 
  }

  /*! Get a past-the-end iterator for the arrangement edges. */
  Edge_iterator edges_end()
  {
    return (Edge_iterator (_dcel().edges_end(),
                           _dcel().edges_end(),
                           _Is_valid_halfedge (&m_topol_traits))); 
  }

  /*! Get a const iterator for the first edge in the arrangement. */
  Edge_const_iterator edges_begin() const
  { 
    return (Edge_const_iterator (_dcel().edges_begin(),
                                 _dcel().edges_end(),
                                 _Is_valid_halfedge (&m_topol_traits))); 
  }
  
  /*! Get a past-the-end const iterator for the arrangement edges. */
  Edge_const_iterator edges_end() const
  {
    return (Edge_const_iterator (_dcel().edges_end(),
                                 _dcel().edges_end(),
                                 _Is_valid_halfedge (&m_topol_traits))); 
  }
  //@}

  /// \name Traversal functions for the arrangement faces.
  //@{

  /*! Get an iterator for the first face in the arrangement. */
  Face_iterator faces_begin() 
  { 
    return (Face_iterator (_dcel().faces_begin(),
                           _dcel().faces_end(),
                           _Is_valid_face (&m_topol_traits))); 
  }

  /*! Get a past-the-end iterator for the arrangement faces. */
  Face_iterator faces_end()
  {
    return (Face_iterator (_dcel().faces_end(),
                           _dcel().faces_end(),
                           _Is_valid_face (&m_topol_traits))); 
  }

  /*! Get a const iterator for the first face in the arrangement. */
  Face_const_iterator faces_begin() const
  { 
    return (Face_const_iterator (_dcel().faces_begin(),
                                 _dcel().faces_end(),
                                 _Is_valid_face (&m_topol_traits))); 
  }
  
  /*! Get a past-the-end const iterator for the arrangement faces. */
  Face_const_iterator faces_end() const
  {
    return (Face_const_iterator (_dcel().faces_end(),
                                 _dcel().faces_end(),
                                 _Is_valid_face (&m_topol_traits))); 
  }

  //! reference_face (const version).
  /*! The function returns a reference face of the arrangement.
      All reference faces of arrangements of the same type have a common 
      point.
      \return A const handle to the reference face.
  */
  Face_const_handle reference_face() const
  {
    return _const_handle_for(this->topology_traits()->reference_face());
  }
  
  //! reference_face (non-const version).
  /*! The function returns a reference face of the arrangement.
      All reference faces of arrangements of the same type have a common 
      point.
      \return A handle to the reference face.
  */
  Face_handle reference_face()
  {
    return _handle_for(this->topology_traits()->reference_face());
  }

  //@}

  /// \name Traversal functions for the unbounded faces of the arrangement.
  //@{

  /*! Get an iterator for the first unbounded face in the arrangement. */
  Unbounded_face_iterator unbounded_faces_begin() 
  { 
    return Unbounded_face_iterator(_dcel().faces_begin(),
                                   _dcel().faces_end(),
                                   _Is_unbounded_face (&m_topol_traits)); 
  }

  /*! Get a past-the-end iterator for the unbounded arrangement faces. */
  Unbounded_face_iterator unbounded_faces_end()
  {
    return Unbounded_face_iterator(_dcel().faces_end(),
                                   _dcel().faces_end(),
                                   _Is_unbounded_face (&m_topol_traits)); 
  }

  /*! Get a const iterator for the first unbounded face in the arrangement. */
  Unbounded_face_const_iterator unbounded_faces_begin() const
  { 
    return Unbounded_face_const_iterator(_dcel().faces_begin(),
                                         _dcel().faces_end(),
                                         _Is_unbounded_face (&m_topol_traits));
  }
  
  /*! Get a past-the-end const iterator for the unbounded arrangement faces. */
  Unbounded_face_const_iterator unbounded_faces_end() const
  {
    return Unbounded_face_const_iterator(_dcel().faces_end(),
                                         _dcel().faces_end(),
                                         _Is_unbounded_face (&m_topol_traits));
  }

  /*! Get the fictitious face (non-const version). */
  Face_handle fictitious_face ()
  {
    // The fictitious contains all other faces in a single hole inside it.
    return
      Face_handle(const_cast<DFace*>(this->topology_traits()->initial_face()));
  }
  
  /*! Get the unbounded face (const version). */
  Face_const_handle fictitious_face () const
  {
    // The fictitious contains all other faces in a single hole inside it.
    return DFace_const_iter (this->topology_traits()->initial_face());
  }
  //@}
  
  /// \name Casting away constness for handle types.
  //@{
  Vertex_handle non_const_handle (Vertex_const_handle vh)
  {
    DVertex    *p_v = (DVertex*) &(*vh);
    return (Vertex_handle (p_v));
  }

  Halfedge_handle non_const_handle (Halfedge_const_handle hh)
  {
    DHalfedge  *p_he = (DHalfedge*) &(*hh);
    return (Halfedge_handle (p_he));
  }

  Face_handle non_const_handle (Face_const_handle fh)
  {
    DFace      *p_f = (DFace*) &(*fh);
    return (Face_handle (p_f));
  }
  //@}

  /// \name Specilaized insertion functions.
  //@{

  /*!
   * Insert a point that forms an isolated vertex in the interior of a given
   * face.
   * \param p The given point.
   * \param f The face into which we insert the new isolated vertex.
   * \return A handle for the isolated vertex that has been created.
   */
  Vertex_handle insert_in_face_interior (const Point_2& p,
                                         Face_handle f);

  /*!
   * Insert an x-monotone curve into the arrangement as a new hole (inner
   * component) inside the given face.
   * \param cv The given x-monotone curve.
   * \param f The face into which we insert the new hole.
   * \return A handle for one of the halfedges corresponding to the inserted
   *         curve, directed (lexicographically) from left to right.
   */
  Halfedge_handle insert_in_face_interior (const X_monotone_curve_2& cv, 
                                           Face_handle f);

  /*!
   * Insert an x-monotone curve into the arrangement, such that its left
   * endpoint corresponds to a given arrangement vertex.
   * \param cv The given x-monotone curve.
   * \param v The given vertex.
   * \param f The face that contains v (in case it has no incident edges).
   * \pre The left endpoint of cv is incident to the vertex v.
   * \return A handle for one of the halfedges corresponding to the inserted
   *         curve, whose target is the new vertex.
   */
  Halfedge_handle insert_from_left_vertex (const X_monotone_curve_2& cv, 
                                           Vertex_handle v,
                                           Face_handle f = Face_handle());

  /*!
   * Insert an x-monotone curve into the arrangement, such that its left
   * endpoints corresponds to a given arrangement vertex, given the exact
   * place for the curve in the circular list around this vertex.
   * \param cv The given x-monotone curve.
   * \param prev The reference halfedge. We should represent cv as a pair
   *             of edges, one of them should become prev's successor.
   * \pre The target vertex of prev is cv's left endpoint.
   * \return A handle for one of the halfedges corresponding to the inserted
   *         curve, whose target is the new vertex that was created.
   */
  Halfedge_handle insert_from_left_vertex (const X_monotone_curve_2& cv,
                                           Halfedge_handle prev);

  /*!
   * Insert an x-monotone curve into the arrangement, such that its right
   * endpoint corresponds to a given arrangement vertex.
   * \param cv The given x-monotone curve.
   * \param v The given vertex.
   * \param f The face that contains v (in case it has no incident edges).
   * \pre The right endpoint of cv is incident to the vertex v.
   * \return A handle for one of the halfedges corresponding to the inserted
   *         curve, whose target is the new vertex.
   */
  Halfedge_handle insert_from_right_vertex (const X_monotone_curve_2& cv, 
                                            Vertex_handle v,
                                            Face_handle f = Face_handle());

  /*! 
   * Insert an x-monotone curve into the arrangement, such that its right
   * endpoints corresponds to a given arrangement vertex, given the exact
   * place for the curve in the circular list around this vertex.

   * \param cv The given x-monotone curve.
   * \param prev The reference halfedge. We should represent cv as a pair
   *             of edges, one of them should become prev's successor.
   * \pre The target vertex of prev is cv's right endpoint.
   * \return A handle for one of the halfedges corresponding to the inserted
   *         curve, whose target is the new vertex that was created.
   */
  Halfedge_handle insert_from_right_vertex (const X_monotone_curve_2& cv,
                                            Halfedge_handle prev);
  
  /*! 
   * Insert an x-monotone curve into the arrangement, such that both its
   * endpoints correspond to given arrangement vertices.
   * \param cv The given x-monotone curve.
   * \param v1 The first vertex.
   * \param v2 The second vertex.
   * \param f The face that contains v1 and v2
   *          (in case both have no incident edges).
   * \pre v1 and v2 corresponds to cv's endpoints.
   * \return A handle for one of the halfedges corresponding to the inserted
   *         curve directed from v1 to v2.
   */
  Halfedge_handle insert_at_vertices (const X_monotone_curve_2& cv, 
                                      Vertex_handle v1, 
                                      Vertex_handle v2,
                                      Face_handle f = Face_handle());

  /*! 
   * Insert an x-monotone curve into the arrangement, such that both its
   * endpoints correspond to given arrangement vertices, given the exact
   * place for the curve in one of the circular lists around a vertex.
   * \param cv The given x-monotone curve.
   * \param prev1 The reference halfedge for the first vertex.
   * \param v2 The second vertex.
   * \pre The target vertex of prev1 and v2 corresponds to cv's endpoints.
   * \return A handle for one of the halfedges corresponding to the inserted
   *         curve directed from prev1 to v2.
   */
  Halfedge_handle insert_at_vertices (const X_monotone_curve_2& cv, 
                                      Halfedge_handle h1, 
                                      Vertex_handle v2);

  /*!
   * Insert an x-monotone curve into the arrangement, such that both its
   * endpoints correspond to given arrangement vertices, given the exact
   * place for the curve in both circular lists around these two vertices.
   * \param cv the given curve.
   * \param prev1 The reference halfedge for the first vertex.
   * \param prev2 The reference halfedge for the second vertex.
   * \pre The target vertices of prev1 and prev2 are cv's endpoints.
   * \return A handle for one of the halfedges corresponding to the inserted
   *         curve directed from prev1's target to prev2's target.
   */
  Halfedge_handle insert_at_vertices (const X_monotone_curve_2 & cv,
                                      Halfedge_handle prev1, 
                                      Halfedge_handle prev2);

  //@}

  /// \name Vertex manipulation functions.
  //@{

  /*!
   * Replace the point associated with the given vertex.
   * \param v The vertex to modify.
   * \param p The point that should be associated with the edge.
   * \pre p is geometrically equivalent to the current point
   *      associated with v.
   * \return A handle for a the modified vertex (same as v).
   */
  Vertex_handle modify_vertex (Vertex_handle v,
                               const Point_2& p);

  /*!
   * Remove an isolated vertex from the interior of a given face.
   * \param v The vertex to remove.
   * \pre v is an isolated vertex (it has no incident halfedges).
   * \return A handle for the face containing v.
   */
  Face_handle remove_isolated_vertex (Vertex_handle v);
  
  ///@}

  /// \name Halfedge manipulation functions.
  //@{

  /*!
   * Replace the x-monotone curve associated with the given edge.
   * \param e The edge to modify.
   * \param cv The curve that should be associated with the edge.
   * \pre cv is geometrically equivalent to the current curve
   *      associated with e.
   * \return A handle for a the modified halfedge (same as e).
   */
  Halfedge_handle modify_edge (Halfedge_handle e, 
                               const X_monotone_curve_2& cv);

  /*!
   * Split a given edge into two, and associate the given x-monotone
   * curves with the split edges.
   * \param e The edge to split (one of the pair of twin halfegdes).
   * \param cv1 The curve that should be associated with the first split edge.
   * \param cv2 The curve that should be associated with the second split edge.

   * \pre cv1's source and cv2's target equal the endpoints of the curve
   *      currently assoicated with e (respectively), and cv1's target equals
   *      cv2's target, and this is the split point (ot vice versa).
   * \return A handle for the halfedge whose source is the source of the the
   *         original halfedge e, and whose target is the split point.
   */
  Halfedge_handle split_edge (Halfedge_handle e,
                              const X_monotone_curve_2& cv1, 
                              const X_monotone_curve_2& cv2);

  /*!
   * Merge two edges to form a single edge, and associate the given x-monotone
   * curve with the merged edge.
   * \param e1 The first edge to merge (one of the pair of twin halfegdes).
   * \param e2 The second edge to merge (one of the pair of twin halfegdes).
   * \param cv The curve that should be associated with merged edge.
   * \return A handle for the merged halfedge.
   */
  Halfedge_handle merge_edge (Halfedge_handle e1, 
                              Halfedge_handle e2, 
                              const X_monotone_curve_2& cv);              

  /*!
   * Remove an edge from the arrangement.
   * \param e The edge to remove (one of the pair of twin halfegdes).
   * \param remove_source Should the source vertex of e be removed if it
   *                      becomes isolated (true by default).
   * \param remove_target Should the target vertex of e be removed if it
   *                      becomes isolated (true by default).
   * \return A handle for the remaining face.
   */
  Face_handle remove_edge (Halfedge_handle e,
                           bool remove_source = true,
                           bool remove_target = true);
  
  //@}

protected:

  /// \name Allocating and de-allocating points and curves.
  //@{

  /*! Is one of the given x and y parameter spaces open?
   * These parameter spaces are typically associated with a particular curve
   * end.
   * \param ps_x The parameter space in x.
   * \param ps_y The parameter space in y.
   */
  inline bool is_open(Arr_parameter_space ps_x, Arr_parameter_space ps_y)
    const
  {
    return
      (((ps_x != ARR_INTERIOR) && (m_boundary_types[ps_x] == ARR_OPEN)) ||
       ((ps_y != ARR_INTERIOR) && (m_boundary_types[ps_y] == ARR_OPEN)));
  }
  
  /*! Initialize the boundary_types array */
  inline void init_boundary_types()
  {
    init_boundary_side(ARR_LEFT_BOUNDARY, Arr_left_side_category());
    init_boundary_side(ARR_BOTTOM_BOUNDARY, Arr_bottom_side_category());
    init_boundary_side(ARR_TOP_BOUNDARY, Arr_top_side_category());
    init_boundary_side(ARR_RIGHT_BOUNDARY, Arr_right_side_category());
  }

  /*! Initialize the boundary_types array */
  void init_boundary_side(Arr_parameter_space ps, Arr_oblivious_side_tag)
  {
    m_boundary_types[ps] = ARR_OBLIVIOUS;
  }
    
  /*! Initialize the boundary_types array */
  void init_boundary_side(Arr_parameter_space ps, Arr_open_side_tag)
  {
    m_boundary_types[ps] = ARR_OPEN;
  }

  /*! Initialize the boundary_types array */
  void init_boundary_side(Arr_parameter_space ps, Arr_closed_side_tag)
  {
    m_boundary_types[ps] = ARR_CLOSED;
  }

  /*! Initialize the boundary_types array */
  void init_boundary_side(Arr_parameter_space ps, Arr_contracted_side_tag)
  {
    m_boundary_types[ps] = ARR_CONTRACTION;
  }

  /*! Initialize the boundary_types array */
  void init_boundary_side(Arr_parameter_space ps, 
                          Arr_identified_side_tag)
  {
    m_boundary_types[ps] = ARR_IDENTIFICATION;
  }

  /*! Allocate a new point. */
  Point_2 *_new_point (const Point_2& pt)
  {
    Point_2   *p_pt = m_points_alloc.allocate (1);

    m_points_alloc.construct (p_pt, pt);
    return (p_pt);
  }

  /*! De-allocate a point. */
  void _delete_point (Point_2& pt)
  {
    Point_2   *p_pt = &pt;

    m_points_alloc.destroy (p_pt);
    m_points_alloc.deallocate (p_pt, 1);
  }

  /*! Allocate a new curve. */
  X_monotone_curve_2 *_new_curve (const X_monotone_curve_2& cv)
  {
    X_monotone_curve_2   *p_cv = m_curves_alloc.allocate (1);

    m_curves_alloc.construct (p_cv, cv);
    return (p_cv);
  }

  /*! De-allocate a curve. */
  void _delete_curve (X_monotone_curve_2& cv)
  {
    X_monotone_curve_2   *p_cv = &cv;

    m_curves_alloc.destroy (p_cv);
    m_curves_alloc.deallocate (p_cv, 1);
  }
  //@}

  /// \name Converting handles to pointers (for the arrangement accessor).
  //@{

  /*! Access the DCEL (non-const version). */
  inline Dcel& _dcel ()
  {
    return (m_topol_traits.dcel());
  }

  /*! Access the DCEL (const version). */
  inline const Dcel& _dcel () const
  {
    return (m_topol_traits.dcel());
  }

  /*! Convert a vertex handle to a pointer to a DCEL vertex. */
  inline DVertex* _vertex (Vertex_handle vh) const
  {
    return (&(*vh));
  }

  /*! Convert a constant vertex handle to a pointer to a DCEL vertex. */
  inline const DVertex* _vertex (Vertex_const_handle vh) const
  {
    return (&(*vh));
  }

  /*! Convert a halfedge handle to a pointer to a DCEL halfedge. */
  inline DHalfedge* _halfedge (Halfedge_handle hh) const
  {
    return (&(*hh));
  }

  /*! Convert a constant halfedge handle to a pointer to a DCEL halfedge. */
  inline const DHalfedge* _halfedge (Halfedge_const_handle hh) const
  {
    return (&(*hh));
  }

  /*! Convert a face handle to a pointer to a DCEL face. */
  inline DFace* _face (Face_handle fh) const
  {
    return (&(*fh));
  }

  /*! Convert a constant face handle to a pointer to a DCEL face. */
  inline const DFace* _face (Face_const_handle fh) const
  {
    return (&(*fh));
  }
  //@}

  /// \name Converting pointers to handles (for the arrangement accessor).
  //@{

  /*! Convert a pointer to a DCEL vertex to a vertex handle. */
  Vertex_handle _handle_for (DVertex *v)
  {
    return (Vertex_handle (v));
  }

  /*! Convert a pointer to a DCEL vertex to a constant vertex handle. */
  Vertex_const_handle _const_handle_for (const DVertex *v) const
  {
    return (Vertex_const_handle (v));
  }

  /*! Convert a pointer to a DCEL halfedge to a halfedge handle. */
  Halfedge_handle _handle_for (DHalfedge *he)
  {
    return (Halfedge_handle (he));
  }


  /*! Convert a pointer to a DCEL halfedge to a constant halfedge handle. */
  Halfedge_const_handle _const_handle_for (const DHalfedge *he) const
  {
    return (Halfedge_const_handle (he));
  }

  /*! Convert a pointer to a DCEL face to a face handle. */
  Face_handle _handle_for (DFace *f)
  {
    return (Face_handle (f));
  }

  /*! Convert a pointer to a DCEL face to a constant face handle. */
  Face_const_handle _const_handle_for (const DFace *f) const
  {
    return (Face_const_handle (f));
  }
  //@}

  /// \name Auxiliary (protected) functions.
  //@{
   
  /*!
   * Locate the place for the given curve around the given vertex.
   * \param v The given arrangement vertex.
   * \param cv The given x-monotone curve.
   * \param ind Whether we refer to the minimal or maximal end of cv.
   * \return A pointer to a halfedge whose target is v, where cv should be
   *         inserted between this halfedge and the next halfedge around this
   *         vertex (in a clockwise order).
   *         A NULL return value indicates a precondition violation.
   */
  DHalfedge* _locate_around_vertex (DVertex *v,
                                    const X_monotone_curve_2& cv,
                                    Arr_curve_end ind) const;

  /*!
   * Compute the distance (in halfedges) between two halfedges.
   * \param e1 The source halfedge.
   * \param e2 The destination halfedge.
   * \pre e1 and e2 belong to the same connected component
   * \return The number of halfedges along the component boundary between the
   *         two halfedges.
   */
  unsigned int _halfedge_distance (const DHalfedge *e1,
                                   const DHalfedge *e2) const;

  /*!
   * Compare two vertices lexicographically, while taking care of boundary
   * conditions (for the special usage of _find_leftmost_vertex() alone!).
   * \param v1 The first vertex.
   * \param v2 The second vertex.
   * \return The comparison result.
   * \pre Both vertices are not at infinity.
   */
  Comparison_result _compare_vertices_xy (const DVertex *v1,
                                          const DVertex *v2) const
  {
    return (_compare_vertices_xy_impl(v1, v2, Are_all_sides_oblivious_tag()));

  }

  Comparison_result
  _compare_vertices_xy_impl (const DVertex *v1,
                             const DVertex *v2,
                             Arr_all_sides_oblivious_tag) const
  {
    return (m_geom_traits->compare_xy_2_object() (v1->point(), v2->point()));
  }

  Comparison_result
  _compare_vertices_xy_impl (const DVertex *v1,
                             const DVertex *v2,
                             Arr_not_all_sides_oblivious_tag) const;
  
  /*!
   * Locate the leftmost vertex on the a given sequence defined by two
   * halfedges. This sequence is still an open loop, but it will soon be closed
   * by the insertion of the given curve: The first vertex we consider is the
   * target vertex of the first halfedge, and the last vertex we consider is
   * the source vertex of the second halfedge, such that the new curve will
   * connect these two vertices.
   * \param he_before The first halfedge; we start from its target vertex.
   * \param he_after The second halfedge; we stop at its source vertex.
   * \param cv The curve we are about to insert.
   * \param is_perimetric Output: Is the halfedge sequence perimetric.
   * \return A pair comprising the leftmost vertex found, along with the
   *         lowest halfedge incident to this vertex (or NULL if this halfedge
   *         should correspond to the unexisting curve cv).
   */
  std::pair <const DVertex*, const DHalfedge*>
  _find_leftmost_vertex_on_open_loop (const DHalfedge *he_before,
                                      const DHalfedge *he_after,
                                      const X_monotone_curve_2& cv,
                                      bool& is_perimetric) const;

  /*!
   * Locate the leftmost vertex on the a given sequence, defined by an anchor
   * halfedge and its twin, which forms a closed loop (i.e., the anchor's twin
   * is reachable from the anchor halfedge).
   * \param he_anchor The halfedge defining the closed loop.
   * \param is_perimetric Output: Is the halfedge sequence perimetric.
   * \param at_infinity Output: Does the path contain a vertex at infinity.
   * \return A pair comprising the leftmost vertex found, along with an index
   *         indicating the number of dicontinuity crossing with respect to
   *         the input halfedge.
   */
  std::pair<int, const DVertex*>
  _find_leftmost_vertex_on_closed_loop (const DHalfedge *he_anchor,
                                        bool& is_perimetric,
                                        bool& at_infinity) const;

  /*!
   * Given two predecessor halfedges that will be used for inserting a
   * new halfedge pair (prev1 will be the predecessor of the halfedge he1,
   * and prev2 will be the predecessor of its twin he2), such that the
   * insertion will create a new face that forms a hole inside an existing
   * face, determine whether he1 will be incident to this new hole.
   * \param prev1 The first predecessor halfedge.
   * \param prev2 The second predecessor halfedge.
   * \param cv The x-monotone curve we use to connect prev1's target and
   *           prev2's target vertex.
   * \pre prev1 and prev2 belong to the same inner CCB.
   * \return true if he1 (and prev1) lies in the interior of the face we
   *         are about to create, false otherwise - in which case he2
   *         (and prev2) must be incident to this new face.
   */
  bool _is_inside_new_face (const DHalfedge *prev1,
                            const DHalfedge *prev2,
                            const X_monotone_curve_2& cv) const;

  /*!
   * Move a given outer CCB from one face to another.
   * \param from_face The face currently containing the component.
   * \param to_face The face into which we should move the component.
   * \param he A halfedge lying on the outer component.
   */
  void _move_outer_ccb (DFace *from_face, DFace *to_face, DHalfedge *he);

  /*!
   * Move a given inner CCB (hole) from one face to another.
   * \param from_face The face currently containing the component.
   * \param to_face The face into which we should move the component.
   * \param he A halfedge lying on the inner component.
   */
  void _move_inner_ccb (DFace *from_face, DFace *to_face, DHalfedge *he);

  /*!
   * Insert the given vertex as an isolated vertex inside the given face.
   * \param f The face that should contain the isolated vertex.
   * \param v The isolated vertex.
   */
  void _insert_isolated_vertex (DFace *f, DVertex *v);

  /*!
   * Move a given isolated vertex from one face to another.
   * \param from_face The face currently containing the isolated vertex.
   * \param to_face The face into which we should move the isolated vertex.
   * \param v The isolated vertex.
   */
  void _move_isolated_vertex (DFace *from_face, DFace *to_face, DVertex *v);

  /*!
   * Create a new vertex and associate it with the given point.
   * \param p The point.
   * \return A pointer to the newly created vertex.
   */
  DVertex* _create_vertex (const Point_2& p);

  /*!
   * Create a new boundary vertex.
   * \param cv The curve incident to the boundary.
   * \param ind The relevant curve-end.
   * \param bx The boundary condition in x.
   * \param by The boundary condition in y.
   * \pre Either bx or by does not equal ARR_INTERIOR.
   * \return A pointer to the newly created vertex.
   */
  DVertex* _create_boundary_vertex (const X_monotone_curve_2& cv,
                                    Arr_curve_end ind,
                                    Arr_parameter_space bx,
                                    Arr_parameter_space by);

  /*!
   * Locate the DCEL features that will be used for inserting the given curve
   * end, which has a boundary condition, and set a proper vertex there.
   * \param f The face that contains the curve end.
   * \param cv The x-monotone curve.
   * \param ind The curve end.
   * \param bx The boundary condition at the x-coordinate.
   * \param by The boundary condition at the y-coordinate.
   * \param p_pred Output: The predecessor halfedge around this vertex
   *                       (may be NULL, if no such halfedge exists).
   * \return The vertex that corresponds to the curve end.
   */
  DVertex* _place_and_set_curve_end (DFace *f,
                                     const X_monotone_curve_2& cv,
                                     Arr_curve_end ind,
                                     Arr_parameter_space bx,
                                     Arr_parameter_space by,
                                     DHalfedge **p_pred);
  
  /*!
   * Insert an x-monotone curve into the arrangement, such that both its
   * endpoints correspond to free arrangement vertices (newly created vertices
   * or existing isolated vertices), so a new inner CCB is formed in the face
   * that contains the two vertices.
   * \param cv The given x-monotone curve.
   * \param f The face containing the two end vertices.
   * \param v1 The free vertex that corresponds to the left endpoint of cv.
   * \param v2 The free vertex that corresponds to the right endpoint of cv.
   * \param res The comparison result of the points associated with v1 and v2.
   * \return A pointer to one of the halfedges corresponding to the inserted
   *         curve, directed from v1 to v2.
   */
  DHalfedge* _insert_in_face_interior (const X_monotone_curve_2& cv,
                                       DFace *f,
                                       DVertex *v1, DVertex *v2,
                                       Comparison_result res);

  /*! 
   * Insert an x-monotone curve into the arrangement, such that one of its
   * endpoints corresponds to a given arrangement vertex, given the exact
   * place for the curve in the circular list around this vertex. The other
   * endpoint corrsponds to a free vertex (a newly created vertex or an
   * isolated vertex).
   * \param cv The given x-monotone curve.
   * \param prev The reference halfedge. We should represent cv as a pair
   *             of edges, one of them should become prev's successor.
   * \param v The free vertex that corresponds to the other endpoint.
   * \param res The comparison result of the points associated with prev's
   *            target and v.
   * \return A pointer to one of the halfedges corresponding to the inserted
   *         curve, whose target is the vertex v.
   */
  DHalfedge* _insert_from_vertex (const X_monotone_curve_2& cv,
                                  DHalfedge *prev,
                                  DVertex *v,
                                  Comparison_result res);

  /*!
   * Insert an x-monotone curve into the arrangement, where the end vertices
   * are given by the target points of two given halfedges.
   * The two halfedges should be given such that in case a new face is formed,
   * it will be the incident face of the halfedge directed from the first
   * vertex to the second vertex.
   * \param cv the given curve.
   * \param prev1 The reference halfedge for the first vertex.
   * \param prev2 The reference halfedge for the second vertex.
   * \param res The comparison result of the points associated with prev1's
   *            target vertex and prev2's target vertex.
   * \param new_face Output - whether a new face has been created.
   * \return A pointer to one of the halfedges corresponding to the inserted
   *         curve directed from prev1's target to prev2's target.
   *         In case a new face has been created, it is given as the incident
   *         face of this halfedge.
   */
  DHalfedge* _insert_at_vertices (const X_monotone_curve_2& cv,
                                  DHalfedge *prev1, 
                                  DHalfedge *prev2,
                                  Comparison_result res,
                                  bool& new_face);

  /*!
   * Relocate all inner CCBs and isolated vertices to their proper position,
   * immediately after a face has split due to the insertion of a new halfedge.
   * \param new_he The new halfedge that caused the split, such that the new
   *               face lies to its left and the old face to its right.
   */
  void _relocate_in_new_face (DHalfedge *new_he);

  /*!
   * Relocate all inner CCBs to their proper position,
   * immediately after a face has split due to the insertion of a new halfedge.
   * \param new_he The new halfedge that caused the split, such that the new
   *               face lies to its left and the old face to its right.
   */
  void _relocate_inner_ccbs_in_new_face (DHalfedge *new_he);

  /*!
   * Relocate all  vertices to their proper position,
   * immediately after a face has split due to the insertion of a new halfedge.
   * \param new_he The new halfedge that caused the split, such that the new
   *               face lies to its left and the old face to its right.
   */
  void _relocate_isolated_vertices_in_new_face (DHalfedge *new_he);

  /*!
   * Replace the point associated with the given vertex.
   * \param v The vertex to modify.
   * \param p The point that should be associated with the edge.
   */
  void _modify_vertex (DVertex *v, 
                       const Point_2& p);

  /*!
   * Replace the x-monotone curve associated with the given edge.
   * \param e The edge to modify.
   * \param cv The curve that should be associated with the edge.
   */
  void _modify_edge (DHalfedge *he, 
                     const X_monotone_curve_2& cv);

  /*!
   * Check if the given vertex represents one of the ends of a given curve.
   * \param v The vertex.
   * \param cv The curve.
   * \param ind Whether we refer to the minimal or maximal end of cv.
   * \return Whether v represents the left (or right) end of cv.
   */
  bool _are_equal (const DVertex *v,
                   const X_monotone_curve_2& cv, Arr_curve_end ind) const;

  /*!
   * Split a given edge into two at a given point, and associate the given
   * x-monotone curves with the split edges.
   * \param e The edge to split (one of the pair of twin halfegdes).
   * \param p The split point.
   * \param cv1 The curve that should be associated with the first split edge,
   *            whose source equals e's source and its target is p.
   * \param cv2 The curve that should be associated with the second split edge,
   *            whose source is p and its target equals e's target.
   * \return A pointer to the first split halfedge, whose source equals the
   *         source of e, and whose target is the split point.
   */
  DHalfedge* _split_edge (DHalfedge *e,
                          const Point_2& p,
                          const X_monotone_curve_2& cv1, 
                          const X_monotone_curve_2& cv2);

  /*!
   * Split a given edge into two at a given vertex, and associate the given
   * x-monotone curves with the split edges.
   * \param e The edge to split (one of the pair of twin halfegdes).
   * \param v The split vertex.
   * \param cv1 The curve that should be associated with the first split edge,
   *            whose source equals e's source and its target is v.
   * \param cv2 The curve that should be associated with the second split edge,
   *            whose source is v and its target equals e's target.
   * \return A pointer to the first split halfedge, whose source equals the
   *         source of e, and whose target is v.
   */
  DHalfedge* _split_edge (DHalfedge *e,
                          DVertex *v,
                          const X_monotone_curve_2& cv1,
                          const X_monotone_curve_2& cv2);

  /*!
   * Remove a pair of twin halfedges from the arrangement.
   * \param e One of the halfedges to be removed.
   * \param remove_source Should the source vertex of e be removed if it
   *                      becomes isolated.
   * \param remove_target Should the target vertex of e be removed if it
   *                      becomes isolated.
   * \pre In case the removal causes the creation of a new inner CCB (hole),
   *      e should point at this hole.
   * \return A pointer to the remaining face.
   */
  DFace *_remove_edge (DHalfedge *e,
                       bool remove_source, bool remove_target);

  /*!
   * Remove a vertex in case it becomes redundant after the deletion of an
   * incident edge.
   * \param v The vertex.
   * \param f The face that contains v (in case it becomes isolated).
   */
  void _remove_vertex_if_redundant (DVertex *v, DFace *f);

  /*!
   * Remove an isolated vertex from the interior of its face (but not from
   * the DCEL).
   * \param v The isolated vertex to remove.
   */
  void _remove_isolated_vertex (DVertex* v);
  //@}

  /// \name Auxiliary (protected) functions for validity checking.
  //@{

  /*! Check the validity of a given vertex. */
  bool _is_valid (Vertex_const_handle v) const;

  /*! Check the validity of a given halfedge. */
  bool _is_valid (Halfedge_const_handle he) const;

  /*! Check the validity of a given face. */
  bool _is_valid (Face_const_handle f) const;

  /*! Check the validity of an outer CCB. */
  bool _is_outer_ccb_valid (const DOuter_ccb *oc,
                            const DHalfedge *first) const;

  /*! Check the validity of an inner CCB. */
  bool _is_inner_ccb_valid (const DInner_ccb *ic,
                            const DHalfedge *first) const;

  /*!
   * Check that all vertices are unique (no two vertices with the same 
   * geometric point.
   */
  bool _are_vertices_unique() const;
  
  /*! Check that the curves around a given vertex are ordered clockwise. */
  bool _are_curves_ordered_cw_around_vertrex (Vertex_const_handle v) const;
  
  //@}

protected:

  /// \name Managing and notifying the arrangement observers.
  //@{

  /*!
   * Register a new observer (so it starts receiving notifications).
   * \param p_obs A pointer to the observer object.
   */
  void _register_observer (Observer *p_obs)
  {
    m_observers.push_back (p_obs);
  }

  /*!
   * Unregister a new observer (so it stops receiving notifications).
   * \param p_obs A pointer to the observer object.
   * \return Whether the observer was successfully unregistered.
   */
  bool _unregister_observer (Observer *p_obs)
  {
    Observers_iterator   iter;
    Observers_iterator   end = m_observers.end();

    for (iter = m_observers.begin(); iter != end; ++iter)
    {
      if ((*iter) == p_obs)
      {
        // Remove the p_ob pointer from the list of observers.
        m_observers.erase (iter);
        return (true);
      }
    }

    // If we reached here, the observer was not registered.
    return (false);
  }

protected:

  /* Notify the observers on global arrangement operations: */

  void _notify_before_assign (const Self& arr)
  {
    Observers_iterator   iter;
    Observers_iterator   end = m_observers.end();

    for (iter = m_observers.begin(); iter != end; ++iter)
      (*iter)->before_assign (arr);
  }

  void _notify_after_assign ()
  {
    Observers_rev_iterator   iter;
    Observers_rev_iterator   end = m_observers.rend();

    for (iter = m_observers.rbegin(); iter != end; ++iter)
      (*iter)->after_assign();
  }

  void _notify_before_clear ()
  {
    Observers_iterator   iter;
    Observers_iterator   end = m_observers.end();

    for (iter = m_observers.begin(); iter != end; ++iter)
      (*iter)->before_clear();
  }

  void _notify_after_clear ()
  {
    Observers_rev_iterator   iter;
    Observers_rev_iterator   end = m_observers.rend();

    for (iter = m_observers.rbegin(); iter != end; ++iter)
      (*iter)->after_clear ();
  }

  void _notify_before_global_change ()
  {
    Observers_iterator   iter;

    Observers_iterator   end = m_observers.end();

    for (iter = m_observers.begin(); iter != end; ++iter)
      (*iter)->before_global_change();

  }

  void _notify_after_global_change ()
  {
    Observers_rev_iterator   iter;
    Observers_rev_iterator   end = m_observers.rend();

    for (iter = m_observers.rbegin(); iter != end; ++iter)
      (*iter)->after_global_change();
  }

  /* Notify the observers on local changes in the arrangement: */

  void _notify_before_create_vertex (const Point_2& p)
  {
    Observers_iterator   iter;
    Observers_iterator   end = m_observers.end();

    for (iter = m_observers.begin(); iter != end; ++iter)
      (*iter)->before_create_vertex (p);
  }

  void _notify_after_create_vertex (Vertex_handle v)
  {
    Observers_rev_iterator   iter;
    Observers_rev_iterator   end = m_observers.rend();

    for (iter = m_observers.rbegin(); iter != end; ++iter)
      (*iter)->after_create_vertex (v);
  }

  void _notify_before_create_boundary_vertex (const X_monotone_curve_2& cv,
                                              Arr_curve_end ind,
                                              Arr_parameter_space bx,
                                              Arr_parameter_space by)
  {
    Observers_iterator   iter;
    Observers_iterator   end = m_observers.end();

    for (iter = m_observers.begin(); iter != end; ++iter)
      (*iter)->before_create_boundary_vertex (cv, ind, bx, by);
  }

  void _notify_after_create_boundary_vertex (Vertex_handle v)
  {
    Observers_rev_iterator   iter;
    Observers_rev_iterator   end = m_observers.rend();

    for (iter = m_observers.rbegin(); iter != end; ++iter)
      (*iter)->after_create_boundary_vertex (v);
  }

  void _notify_before_create_edge (const X_monotone_curve_2& c,
                                   Vertex_handle v1, Vertex_handle v2)
  {
    Observers_iterator   iter;
    Observers_iterator   end = m_observers.end();

    for (iter = m_observers.begin(); iter != end; ++iter)
      (*iter)->before_create_edge (c, v1, v2);
  }

  void _notify_after_create_edge (Halfedge_handle e)
  {
    Observers_rev_iterator   iter;
    Observers_rev_iterator   end = m_observers.rend();

    for (iter = m_observers.rbegin(); iter != end; ++iter)
      (*iter)->after_create_edge (e);
  }

  void _notify_before_modify_vertex (Vertex_handle v,
                                     const Point_2& p)
  {
    Observers_iterator   iter;
    Observers_iterator   end = m_observers.end();

    for (iter = m_observers.begin(); iter != end; ++iter)
      (*iter)->before_modify_vertex (v, p);
  }

  void _notify_after_modify_vertex (Vertex_handle v)
  {
    Observers_rev_iterator   iter;
    Observers_rev_iterator   end = m_observers.rend();

    for (iter = m_observers.rbegin(); iter != end; ++iter)
      (*iter)->after_modify_vertex (v);
  }

  void _notify_before_modify_edge (Halfedge_handle e,
                                   const X_monotone_curve_2& c)
  {
    Observers_iterator   iter;
    Observers_iterator   end = m_observers.end();

    for (iter = m_observers.begin(); iter != end; ++iter)
      (*iter)->before_modify_edge (e, c);
  }

  void _notify_after_modify_edge (Halfedge_handle e)
  {
    Observers_rev_iterator   iter;
    Observers_rev_iterator   end = m_observers.rend();

    for (iter = m_observers.rbegin(); iter != end; ++iter)
      (*iter)->after_modify_edge (e);
  }

  void _notify_before_split_edge (Halfedge_handle e,
                                  Vertex_handle v,
                                  const X_monotone_curve_2& c1,
                                  const X_monotone_curve_2& c2)
  {
    Observers_iterator   iter;
    Observers_iterator   end = m_observers.end();

    for (iter = m_observers.begin(); iter != end; ++iter)
      (*iter)->before_split_edge (e, v, c1, c2);
  }

  void _notify_after_split_edge (Halfedge_handle e1,
                                 Halfedge_handle e2)
  {
    Observers_rev_iterator   iter;
    Observers_rev_iterator   end = m_observers.rend();

    for (iter = m_observers.rbegin(); iter != end; ++iter)
      (*iter)->after_split_edge (e1, e2);
  }

  void _notify_before_split_fictitious_edge (Halfedge_handle e,
                                             Vertex_handle v)
  {
    Observers_iterator   iter;
    Observers_iterator   end = m_observers.end();

    for (iter = m_observers.begin(); iter != end; ++iter)
      (*iter)->before_split_fictitious_edge (e, v);
  }

  void _notify_after_split_fictitious_edge (Halfedge_handle e1,
                                            Halfedge_handle e2)
  {
    Observers_rev_iterator   iter;
    Observers_rev_iterator   end = m_observers.rend();

    for (iter = m_observers.rbegin(); iter != end; ++iter)
      (*iter)->after_split_fictitious_edge (e1, e2);
  }

  void _notify_before_split_face (Face_handle f,
                                  Halfedge_handle e)
  {
    Observers_iterator   iter;
    Observers_iterator   end = m_observers.end();

    for (iter = m_observers.begin(); iter != end; ++iter)
      (*iter)->before_split_face (f, e);
  }

  void _notify_after_split_face (Face_handle f,
                                 Face_handle new_f,
                                 bool is_hole)
  {
    Observers_rev_iterator   iter;
    Observers_rev_iterator   end = m_observers.rend();

    for (iter = m_observers.rbegin(); iter != end; ++iter)
      (*iter)->after_split_face (f, new_f, is_hole);
  }

  void _notify_before_split_outer_ccb (Face_handle f,
                                       Ccb_halfedge_circulator h,
                                       Halfedge_handle e)
  {
    Observers_iterator   iter;
    Observers_iterator   end = m_observers.end();

    for (iter = m_observers.begin(); iter != end; ++iter)
      (*iter)->before_split_outer_ccb (f, h, e);
  }

  void _notify_after_split_outer_ccb (Face_handle f,
                                      Ccb_halfedge_circulator h1,
                                      Ccb_halfedge_circulator h2)
  {
    Observers_rev_iterator   iter;
    Observers_rev_iterator   end = m_observers.rend();

    for (iter = m_observers.rbegin(); iter != end; ++iter)
      (*iter)->after_split_outer_ccb (f, h1, h2);
  }

  void _notify_before_split_inner_ccb (Face_handle f,
                                       Ccb_halfedge_circulator h,
                                       Halfedge_handle e)
  {
    Observers_iterator   iter;
    Observers_iterator   end = m_observers.end();

    for (iter = m_observers.begin(); iter != end; ++iter)
      (*iter)->before_split_inner_ccb (f, h, e);
  }

  void _notify_after_split_inner_ccb (Face_handle f,
                                      Ccb_halfedge_circulator h1,
                                      Ccb_halfedge_circulator h2)
  {
    Observers_rev_iterator   iter;
    Observers_rev_iterator   end = m_observers.rend();

    for (iter = m_observers.rbegin(); iter != end; ++iter)
      (*iter)->after_split_inner_ccb (f, h1, h2);
  }

  void _notify_before_add_outer_ccb (Face_handle f,
                                     Halfedge_handle e)
  {
    Observers_iterator   iter;
    Observers_iterator   end = m_observers.end();

    for (iter = m_observers.begin(); iter != end; ++iter)
      (*iter)->before_add_outer_ccb (f, e);
  }

  void _notify_after_add_outer_ccb (Ccb_halfedge_circulator h)
  {
    Observers_rev_iterator   iter;
    Observers_rev_iterator   end = m_observers.rend();

    for (iter = m_observers.rbegin(); iter != end; ++iter)
      (*iter)->after_add_outer_ccb (h);
  }

  void _notify_before_add_inner_ccb (Face_handle f,
                                     Halfedge_handle e)
  {
    Observers_iterator   iter;
    Observers_iterator   end = m_observers.end();

    for (iter = m_observers.begin(); iter != end; ++iter)
      (*iter)->before_add_inner_ccb (f, e);
  }

  void _notify_after_add_inner_ccb (Ccb_halfedge_circulator h)
  {
    Observers_rev_iterator   iter;
    Observers_rev_iterator   end = m_observers.rend();

    for (iter = m_observers.rbegin(); iter != end; ++iter)
      (*iter)->after_add_inner_ccb (h);
  }

  void _notify_before_add_isolated_vertex (Face_handle f,
                                           Vertex_handle v)
  {
    Observers_iterator   iter;
    Observers_iterator   end = m_observers.end();

    for (iter = m_observers.begin(); iter != end; ++iter)
      (*iter)->before_add_isolated_vertex (f, v);
  }

  void _notify_after_add_isolated_vertex (Vertex_handle v)
  {
    Observers_rev_iterator   iter;
    Observers_rev_iterator   end = m_observers.rend();

    for (iter = m_observers.rbegin(); iter != end; ++iter)
      (*iter)->after_add_isolated_vertex (v);
  }

  void _notify_before_merge_edge (Halfedge_handle e1,
                                  Halfedge_handle e2,
                                  const X_monotone_curve_2& c)
  {
    Observers_iterator   iter;
    Observers_iterator   end = m_observers.end();

    for (iter = m_observers.begin(); iter != end; ++iter)
      (*iter)->before_merge_edge (e1, e2, c);
  }

  void _notify_after_merge_edge (Halfedge_handle e)
  {
    Observers_rev_iterator   iter;
    Observers_rev_iterator   end = m_observers.rend();

    for (iter = m_observers.rbegin(); iter != end; ++iter)
      (*iter)->after_merge_edge (e);
  }

  void _notify_before_merge_fictitious_edge (Halfedge_handle e1,
                                             Halfedge_handle e2)
  {
    Observers_iterator   iter;
    Observers_iterator   end = m_observers.end();

    for (iter = m_observers.begin(); iter != end; ++iter)
      (*iter)->before_merge_fictitious_edge (e1, e2);
  }

  void _notify_after_merge_fictitious_edge (Halfedge_handle e)
  {
    Observers_rev_iterator   iter;
    Observers_rev_iterator   end = m_observers.rend();

    for (iter = m_observers.rbegin(); iter != end; ++iter)
      (*iter)->after_merge_fictitious_edge (e);
  }

  void _notify_before_merge_face (Face_handle f1,
                                  Face_handle f2,
                                  Halfedge_handle e)
  {
    Observers_iterator   iter;
    Observers_iterator   end = m_observers.end();

    for (iter = m_observers.begin(); iter != end; ++iter)
      (*iter)->before_merge_face (f1, f2, e);
  }

  void _notify_after_merge_face (Face_handle f)
  {
    Observers_rev_iterator   iter;
    Observers_rev_iterator   end = m_observers.rend();

    for (iter = m_observers.rbegin(); iter != end; ++iter)
      (*iter)->after_merge_face (f);
  }

  void _notify_before_merge_outer_ccb (Face_handle f,
                                       Ccb_halfedge_circulator h1,
                                       Ccb_halfedge_circulator h2,
                                       Halfedge_handle e)
  {
    Observers_iterator   iter;
    Observers_iterator   end = m_observers.end();

    for (iter = m_observers.begin(); iter != end; ++iter)
      (*iter)->before_merge_outer_ccb (f, h1, h2, e);
  }

  void _notify_after_merge_outer_ccb (Face_handle f,
                                      Ccb_halfedge_circulator h)
  {
    Observers_rev_iterator   iter;
    Observers_rev_iterator   end = m_observers.rend();

    for (iter = m_observers.rbegin(); iter != end; ++iter)
      (*iter)->after_merge_outer_ccb (f, h);
  }

  void _notify_before_merge_inner_ccb (Face_handle f,
                                       Ccb_halfedge_circulator h1,
                                       Ccb_halfedge_circulator h2,
                                       Halfedge_handle e)
  {
    Observers_iterator   iter;
    Observers_iterator   end = m_observers.end();

    for (iter = m_observers.begin(); iter != end; ++iter)
      (*iter)->before_merge_inner_ccb (f, h1, h2, e);
  }

  void _notify_after_merge_inner_ccb (Face_handle f,
                                      Ccb_halfedge_circulator h)
  {
    Observers_rev_iterator   iter;
    Observers_rev_iterator   end = m_observers.rend();

    for (iter = m_observers.rbegin(); iter != end; ++iter)
      (*iter)->after_merge_inner_ccb (f, h);
  }

  void _notify_before_move_outer_ccb (Face_handle from_f,
                                      Face_handle to_f,
                                      Ccb_halfedge_circulator h)
  {
    Observers_iterator   iter;
    Observers_iterator   end = m_observers.end();

    for (iter = m_observers.begin(); iter != end; ++iter)
      (*iter)->before_move_outer_ccb (from_f, to_f, h);
  }

  void _notify_after_move_outer_ccb (Ccb_halfedge_circulator h)
  {
    Observers_rev_iterator   iter;
    Observers_rev_iterator   end = m_observers.rend();

    for (iter = m_observers.rbegin(); iter != end; ++iter)
      (*iter)->after_move_outer_ccb (h);
  }

  void _notify_before_move_inner_ccb (Face_handle from_f,
                                      Face_handle to_f,
                                      Ccb_halfedge_circulator h)
  {
    Observers_iterator   iter;
    Observers_iterator   end = m_observers.end();

    for (iter = m_observers.begin(); iter != end; ++iter)
      (*iter)->before_move_inner_ccb (from_f, to_f, h);
  }

  void _notify_after_move_inner_ccb (Ccb_halfedge_circulator h)
  {
    Observers_rev_iterator   iter;
    Observers_rev_iterator   end = m_observers.rend();

    for (iter = m_observers.rbegin(); iter != end; ++iter)
      (*iter)->after_move_inner_ccb (h);
  }

  void _notify_before_move_isolated_vertex (Face_handle from_f,
                                            Face_handle to_f,
                                            Vertex_handle v)
  {
    Observers_iterator   iter;
    Observers_iterator   end = m_observers.end();

    for (iter = m_observers.begin(); iter != end; ++iter)
      (*iter)->before_move_isolated_vertex (from_f, to_f, v);
  }


  void _notify_after_move_isolated_vertex (Vertex_handle v)
  {
    Observers_rev_iterator   iter;
    Observers_rev_iterator   end = m_observers.rend();

    for (iter = m_observers.rbegin(); iter != end; ++iter)
      (*iter)->after_move_isolated_vertex (v);
  }

  void _notify_before_remove_vertex (Vertex_handle v)
  {
    Observers_iterator   iter;
    Observers_iterator   end = m_observers.end();

    for (iter = m_observers.begin(); iter != end; ++iter)
      (*iter)->before_remove_vertex (v);
  }

  void _notify_after_remove_vertex ()
  {
    Observers_rev_iterator   iter;
    Observers_rev_iterator   end = m_observers.rend();

    for (iter = m_observers.rbegin(); iter != end; ++iter)
      (*iter)->after_remove_vertex ();
  }

  void _notify_before_remove_edge (Halfedge_handle e)
  {
    Observers_iterator   iter;
    Observers_iterator   end = m_observers.end();

    for (iter = m_observers.begin(); iter != end; ++iter)
      (*iter)->before_remove_edge (e);
  }

  void _notify_after_remove_edge ()
  {
    Observers_rev_iterator   iter;
    Observers_rev_iterator   end = m_observers.rend();

    for (iter = m_observers.rbegin(); iter != end; ++iter)
      (*iter)->after_remove_edge ();
  }

  void _notify_before_remove_outer_ccb (Face_handle f,
                                        Ccb_halfedge_circulator h)
  {
    Observers_iterator   iter;
    Observers_iterator   end = m_observers.end();

    for (iter = m_observers.begin(); iter != end; ++iter)
      (*iter)->before_remove_outer_ccb (f, h);
  }

  void _notify_after_remove_outer_ccb (Face_handle f)
  {
    Observers_rev_iterator   iter;
    Observers_rev_iterator   end = m_observers.rend();

    for (iter = m_observers.rbegin(); iter != end; ++iter)
      (*iter)->after_remove_outer_ccb (f);
  }

  void _notify_before_remove_inner_ccb (Face_handle f,
                                        Ccb_halfedge_circulator h)
  {
    Observers_iterator   iter;
    Observers_iterator   end = m_observers.end();

    for (iter = m_observers.begin(); iter != end; ++iter)
      (*iter)->before_remove_inner_ccb (f, h);
  }

  void _notify_after_remove_inner_ccb (Face_handle f)
  {
    Observers_rev_iterator   iter;
    Observers_rev_iterator   end = m_observers.rend();

    for (iter = m_observers.rbegin(); iter != end; ++iter)
      (*iter)->after_remove_inner_ccb (f);
  }
  //@}

};

//-----------------------------------------------------------------------------
// Declarations of the various global insertion and removal functions.
//-----------------------------------------------------------------------------

// In some compilers there is a template deduction disambiguity between this
// function and the following function receiving two InputIterator.
// For now the solution is to add a dummy variable at the end (referring
// to point-location). Maybe the proper solution is to use boost::enable_if
// together with appropriate tag.
/*!
 * Insert a curve or x-monotone curve into the arrangement (incremental 
 * insertion).
 * The inserted curve can be x-monotone (or not) and may intersect the
 * existing arrangement.
 * \param arr The arrangement.
 * \param cv The curve to be inserted.
 * \param pl A point-location object associated with the arrangement.
 */
template <class GeomTraits, class TopTraits, class Curve, class PointLocation>
void insert (Arrangement_on_surface_2<GeomTraits, TopTraits>& arr,
	     const Curve& c, const PointLocation& pl,
	     typename PointLocation::Point_2* = 0);

/*!
 * Insert a curve or x-monotone curve into the arrangement (incremental
 * insertion).
 * The inserted curve can be x-monotone (or not) and may intersect the
 * existing arrangement. The default "walk" point-location strategy is used
 * for the curve insertion.
 * \param arr The arrangement.
 * \param cv The curve to be inserted.
 */
template <class GeomTraits, class TopTraits, class Curve>
void insert (Arrangement_on_surface_2<GeomTraits, TopTraits>& arr,
                   const Curve& c);

/*!
 * Insert a range of curves or x-monotone curves into the arrangement 
 * (aggregated insertion). 
 * The inserted curves may intersect one another and may also intersect the 
 * existing arrangement.
 * \param arr The arrangement.
 * \param begin An iterator for the first curve in the range.
 * \param end A past-the-end iterator for the curve range.
 * \pre The value type of the iterators must be Curve_2.
 */
template <class GeomTraits, class TopTraits, class InputIterator>
void insert (Arrangement_on_surface_2<GeomTraits, TopTraits>& arr,
                    InputIterator begin, InputIterator end);

/*!
 * Insert an x-monotone curve into the arrangement (incremental insertion)
 * when the location of the left endpoint of the curve is known and is
 * given as an isertion hint.
 * The inserted x-monotone curve may intersect the existing arrangement.
 * \param arr The arrangement.
 * \param cv The x-monotone curve to be inserted.
 * \param obj An object that represents the location of cv's left endpoint
 *            in the arrangement.
 */

template <class GeomTraits, class TopTraits>
void insert (Arrangement_on_surface_2<GeomTraits, TopTraits>& arr,
             const typename GeomTraits::X_monotone_curve_2& c,
             const Object& obj);

/*!
 * Insert an x-monotone curve into the arrangement, such that the curve
 * interior does not intersect with any existing edge or vertex in the
 * arragement (incremental insertion).
 * \param arr The arrangement.
 * \param c The x-monotone curve to be inserted.
 * \param pl A point-location object associated with the arrangement.
 * \pre The interior of c does not intersect any existing edge or vertex.
 * \return A handle for one of the new halfedges corresponding to the inserted
 *         curve, directed (lexicographically) from left to right.
 */
template <class GeomTraits, class TopTraits, class PointLocation>
typename Arrangement_on_surface_2<GeomTraits, TopTraits>::Halfedge_handle
insert_non_intersecting_curve
(Arrangement_on_surface_2<GeomTraits, TopTraits>& arr,
 const typename GeomTraits::X_monotone_curve_2& c,
 const PointLocation& pl);

/*!
 * Insert an x-monotone curve into the arrangement, such that the curve
 * interior does not intersect with any existing edge or vertex in the
 * arragement (incremental insertion). The default point-location strategy
 * is used for the curve insertion.
 * \param arr The arrangement.
 * \param c The x-monotone curve to be inserted.
 * \pre The interior of c does not intersect any existing edge or vertex.

 * \return A handle for one of the new halfedges corresponding to the inserted
 *         curve, directed (lexicographically) from left to right.
 */
template <class GeomTraits, class TopTraits>
typename Arrangement_on_surface_2<GeomTraits, TopTraits>::Halfedge_handle
insert_non_intersecting_curve
(Arrangement_on_surface_2<GeomTraits, TopTraits>& arr,
 const typename GeomTraits::X_monotone_curve_2& c);

/*!
 * Insert a range of pairwise interior-disjoint x-monotone curves into
 * the arrangement, such that the curve interiors do not intersect with
 * any existing edge or vertex in the arragement (aggregated insertion).
 * \param arr The arrangement.
 * \param begin An iterator for the first x-monotone curve in the range.
 * \param end A past-the-end iterator for the x-monotone curve range.
 * \pre The value type of the iterators must be X_monotone_curve_2.
 *      The curves in the range are pairwise interior-disjoint, and their
 *      interiors do not intersect any existing edge or vertex.
 */
template <class GeomTraits, class TopTraits, class InputIterator>
void insert_non_intersecting_curves 
(Arrangement_on_surface_2<GeomTraits, TopTraits>& arr,
 InputIterator begin, InputIterator end);

/*!
 * Remove an edge from the arrangement. In case it is possible to merge
 * the edges incident to the end-vertices of the removed edge after its
 * deletion, the function performs these merges as well.
 * \param arr The arrangement.
 * \param e The edge to remove (one of the pair of twin halfegdes).
 * \return A handle for the remaining face.
 */
template <class GeomTraits, class TopTraits>
typename Arrangement_on_surface_2<GeomTraits, TopTraits>::Face_handle
remove_edge (Arrangement_on_surface_2<GeomTraits, TopTraits>& arr,
             typename Arrangement_on_surface_2<GeomTraits,
                                               TopTraits>::Halfedge_handle e);

/*!
 * Insert a vertex that corresponds to a given point into the arrangement.
 * The inserted point may lie on any existing arrangement feature.
 * \param arr The arrangement.
 * \param p The point to be inserted.
 * \param pl A point-location object associated with the arrangement.
 * \return A handle to the vertex that corresponds to the given point.
 */
template <class GeomTraits, class TopTraits, class PointLocation>
typename Arrangement_on_surface_2<GeomTraits, TopTraits>::Vertex_handle
insert_point (Arrangement_on_surface_2<GeomTraits, TopTraits>& arr,
              const typename GeomTraits::Point_2& p,
              const PointLocation& pl);

/*!
 * Insert a vertex that corresponds to a given point into the arrangement.
 * The inserted point may lie on any existing arrangement feature.
 * \param arr The arrangement.
 * \param p The point to be inserted.
 * \return A handle to the vertex that corresponds to the given point.
 */
template <class GeomTraits, class TopTraits>
typename Arrangement_on_surface_2<GeomTraits, TopTraits>::Vertex_handle
insert_point (Arrangement_on_surface_2<GeomTraits, TopTraits>& arr,
              const typename GeomTraits::Point_2& p);

/*!
 * Remove a vertex from the arrangement.
 * \param arr The arrangement.
 * \param v The vertex to remove.
 * \return Whether the vertex has been removed or not.
 */
template <class GeomTraits, class TopTraits>
bool
remove_vertex (Arrangement_on_surface_2<GeomTraits, TopTraits>& arr,
               typename Arrangement_on_surface_2<GeomTraits,
                                                 TopTraits>::Vertex_handle v);


/*!
 * Check the validity of the arrangement. In particular, check that the
 * edegs are disjoint-interior, and the holes are located in their proper
 * position.
 * \param arr The arrangement.
 * \return Whether the arrangement is valid.
 */
template <class GeomTraits, class TopTraits>
bool is_valid (const Arrangement_on_surface_2<GeomTraits, TopTraits>& arr);

/*!
 * Compute the zone of the given x-monotone curve in the existing arrangement.
 * Meaning, it output the arrangment's vertices, edges and faces that the 
 * x-monotone curve intersects.
 * \param arr The arrangement.
 * \param c The x-monotone curve that its zone was computed.
 * \param oi Output iterator of CGAL::Object to insert the zone elements to.
 * \param pi The point location strategy that is used to locate the starting
 * point.
 * \return The output iterator that the curves were inserted to.
 */
template <class GeomTraits, class TopTraits, 
  class OutputIterator, class PointLocation>
OutputIterator zone (Arrangement_on_surface_2<GeomTraits, TopTraits>& arr, 
                     const typename GeomTraits::X_monotone_curve_2& c,
                     OutputIterator oi,
                     const PointLocation& pl);

/*!
 * Compute the zone of the given x-monotone curve in the existing arrangement.
 * Overloaded version with no point location object - the walk point-location
 * strategy is used as default.
 * \param arr The arrangement.
 * \param c The x-monotone curve that its zone was computed.
 * \param oi Output iterator of CGAL::Object to insert the zone elements to.
 * \return The output iterator that the curves were inserted to.
 */
template <class GeomTraits, class TopTraits, class OutputIterator>
OutputIterator zone (Arrangement_on_surface_2<GeomTraits, TopTraits>& arr, 
                     const typename GeomTraits::X_monotone_curve_2& c,
                     OutputIterator oi);

/*!
 * Checks if the given curve/x-monotone curve intersects the existing arrangement.
 * \param arr The arrangement.
 * \param c The curve/x-monotone curve.
 * \param pi The point location strategy that is used to locate the starting
 * point.
 * \return True if the curve intersect the arrangement, false otherwise.
 */
template <class GeomTraits, class TopTraits, class Curve, class PointLocation>
bool do_intersect (Arrangement_on_surface_2<GeomTraits, TopTraits>& arr, 
                   const Curve& c, const PointLocation& pl);

/*!
 * Checks if the given curve/x-monotone curve intersects the existing arrangement.
 * Overloaded version with no point location object - the walk point-location
 * strategy is used as default.
 * \param arr The arrangement.
 * \param c The x-monotone curve/curve.
 * \return True if the curve intersect the arrangement, false otherwise.
 */
template <class GeomTraits, class TopTraits, class Curve>
bool do_intersect (Arrangement_on_surface_2<GeomTraits, TopTraits>& arr, 
                   const Curve& c);


CGAL_END_NAMESPACE

// The function definitions can be found under:
#include <CGAL/Arrangement_2/Arrangement_on_surface_2_impl.h>
#include <CGAL/Arrangement_2/Arrangement_on_surface_2_global.h>

#endif

