// Copyright (c) 2005  INRIA (France).
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
// $URL: svn+ssh://scm.gforge.inria.fr/svn/cgal/branches/CGAL-3.5-branch/Surface_mesh_parameterization/include/CGAL/Convertible_filter_iterator.h $
// $Id: Convertible_filter_iterator.h 44130 2008-07-12 21:58:52Z spion $
//
//
// Author(s)     : Laurent Saboret, Pierre Alliez, Bruno Levy


#ifndef CGAL_CONVERTIBLE_FILTER_ITERATOR_H
#define CGAL_CONVERTIBLE_FILTER_ITERATOR_H

#include <CGAL/iterator.h>

CGAL_BEGIN_NAMESPACE


/// This class inherits from Filter_iterator<> +
/// adds a conversion to handle/const handle.
/// See Filter_iterator<> documentation.

template<class Iterator,        ///< Internal iterator.
         class Predicate,       ///< Predicate to filter out elements.
         class ConstHandle,     ///< Const-handle type to convert to.
         class Handle = void*   ///< Non-const-handle type to convert to (void*=none).
>
class Convertible_filter_iterator
    : public Filter_iterator<Iterator, Predicate>
{
    typedef Filter_iterator<Iterator, Predicate>    Base;
    typedef Convertible_filter_iterator             Self;

public:

  /// CREATION
  /// --------

    Convertible_filter_iterator() {}
    Convertible_filter_iterator(Base base)
        : Base(base) {}
    Convertible_filter_iterator(Iterator e, const Predicate& p)
        : Base(e,p) {}
    Convertible_filter_iterator(Iterator e, const Predicate& p, Iterator c)
        : Base(e,p,c) {}

    Convertible_filter_iterator(const Self& it) : Base(it) {}
    Self& operator=(const Self& it) { Base::operator=(it); return *this; }

  /// OPERATIONS Forward Category
  /// ---------------------------

    bool  operator==(Nullptr_t ptr) const { return (const Base&)*this == ptr; }
    bool  operator!=(Nullptr_t ptr) const { return ! (*this == ptr); }
    bool  operator==(const Self& it) const { return (const Base&)*this == it; }
    bool  operator!=(const Self& it) const { return ! (*this == it); }

    Self& operator++()     { Base::operator++(); return *this; }
    Self  operator++(int)  { Self tmp(*this); ++(*this); return tmp; }

  /// OPERATIONS Bidirectional Category
  /// ---------------------------------

    Self& operator--()     { Base::operator--(); return *this; }
    Self  operator--(int)  { Self tmp(*this); --(*this); return tmp; }

  /// EXTRA CASTS
  /// -----------

    operator Handle()               { return Base::operator->(); }
    operator ConstHandle() const    { return Base::operator->(); }
};


CGAL_END_NAMESPACE

#endif //CGAL_CONVERTIBLE_FILTER_ITERATOR_H
