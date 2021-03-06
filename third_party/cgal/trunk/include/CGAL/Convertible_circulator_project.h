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
// $URL: svn+ssh://scm.gforge.inria.fr/svn/cgal/branches/CGAL-3.5-branch/Surface_mesh_parameterization/include/CGAL/Convertible_circulator_project.h $
// $Id: Convertible_circulator_project.h 44130 2008-07-12 21:58:52Z spion $
//
//
// Author(s)     : Laurent Saboret, Pierre Alliez, Bruno Levy


#ifndef CGAL_CONVERTIBLE_CIRCULATOR_PROJECT_H
#define CGAL_CONVERTIBLE_CIRCULATOR_PROJECT_H

#include <CGAL/Circulator_project.h>

CGAL_BEGIN_NAMESPACE


/// This class inherits from Circulator_project<> +
/// adds a conversion to handle/const handle.
/// See Circulator_project<> documentation.

template<class C,               ///< Internal circulator.
         class Fct,             ///< Conversion functor.
         class Ref,
         class Ptr,
         class ConstHandle,     ///< Const-handle type to convert to.
         class Handle = void*   ///< Non-const-handle type to convert to (void*=none).
>
class Convertible_circulator_project
    : public Circulator_project<C, Fct, Ref, Ptr>
{
    typedef Circulator_project<C, Fct, Ref, Ptr>    Base;
    typedef Convertible_circulator_project          Self;

public:

  /// CREATION
  /// --------

    Convertible_circulator_project() {}
    Convertible_circulator_project(Base base) : Base(base) {}

    Convertible_circulator_project(const Self& cir) : Base(cir) {}
    Self& operator=(const Self& cir) { Base::operator=(cir); return *this; }

  /// OPERATIONS Forward Category
  /// ---------------------------

    bool  operator==(Nullptr_t ptr) const { return (const Base&)*this == ptr; }
    bool  operator!=(Nullptr_t ptr) const { return ! (*this == ptr); }
    bool  operator==(const Self& cir)    const { return (const Base&)*this == cir; }
    bool  operator!=(const Self& cir)    const { return ! (*this == cir); }

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

#endif //CGAL_CONVERTIBLE_CIRCULATOR_PROJECT_H
