// Copyright (c) 2006  Tel-Aviv University (Israel).
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
// $URL: svn+ssh://scm.gforge.inria.fr/svn/cgal/branches/CGAL-3.5-branch/Envelope_2/include/CGAL/envelope_2.h $
// $Id: envelope_2.h 37894 2007-04-03 18:32:11Z efif $
//
// Author(s)     : Ron Wein   <wein@post.tau.ac.il>

#ifndef CGAL_ENVELOPE_2_H
#define CGAL_ENVELOPE_2_H

/*! \file
 * Global functions for computing lower and upper envelopes of curves in the
 * plane.
 */

#include <CGAL/Envelope_2/Env_divide_and_conquer_2.h>

CGAL_BEGIN_NAMESPACE

/*!
 * Compute the lower envelope of a range of curves.
 * \param begin An iterator for the first curve.
 * \param end A past-the-end iterator for the curves.
 * \param diag Output: The minimization diagram.
 * \pre The value-type of the iterator is Traits::Curve_2.
 */
template <class InputIterator, class EnvelopeDiagram>
void lower_envelope_2 (InputIterator begin, InputIterator end,
                       EnvelopeDiagram& diag)
{
  typedef typename EnvelopeDiagram::Traits_2                       Traits_2;
  typedef Envelope_divide_and_conquer_2<Traits_2, EnvelopeDiagram> Envelope_2;

  Envelope_2      env;

  env.insert_curves (begin, end,
                     true,         // Lower envelope.
                     diag);

  return;
}

/*!
 * Compute the upper envelope of a range of curves.
 * \param begin An iterator for the first curve.
 * \param end A past-the-end iterator for the curves.
 * \param diag Output: The maximization diagram.
 * \pre The value-type of the iterator is Traits::Curve_2.
 */
template <class InputIterator, class EnvelopeDiagram>
void upper_envelope_2 (InputIterator begin, InputIterator end,
                       EnvelopeDiagram& diag)
{
  typedef typename EnvelopeDiagram::Traits_2                       Traits_2;
  typedef Envelope_divide_and_conquer_2<Traits_2, EnvelopeDiagram> Envelope_2;

  Envelope_2      env;

  env.insert_curves (begin, end,
                     false,         // Upper envelope.
                     diag);

  return;
}

/*!
 * Compute the lower envelope of a range of x-monotone curves.
 * \param begin An iterator for the first x-monotone curve.
 * \param end A past-the-end iterator for the x-monotone curves.
 * \param diag Output: The minimization diagram.
 * \pre The value-type of the iterator is Traits::X_monotone_curve_2.
 */
template <class InputIterator, class EnvelopeDiagram>
void lower_envelope_x_monotone_2 (InputIterator begin, InputIterator end,
                                  EnvelopeDiagram& diag)
{
  typedef typename EnvelopeDiagram::Traits_2                       Traits_2;
  typedef Envelope_divide_and_conquer_2<Traits_2, EnvelopeDiagram> Envelope_2;

  Envelope_2      env;

  env.insert_x_monotone_curves (begin, end,
                                true,              // Lower envelope.
                                diag);

  return;
}

/*!
 * Compute the upper envelope of a range of x-monotone curves.
 * \param begin An iterator for the first x-monotone curve.
 * \param end A past-the-end iterator for the x-monotone curves.
 * \param diag Output: The maximization diagram.
 * \pre The value-type of the iterator is Traits::X_monotone_curve_2.
 */
template <class InputIterator, class EnvelopeDiagram>
void upper_envelope_x_monotone_2 (InputIterator begin, InputIterator end,
                                  EnvelopeDiagram& diag)
{
  typedef typename EnvelopeDiagram::Traits_2                       Traits_2;
  typedef Envelope_divide_and_conquer_2<Traits_2, EnvelopeDiagram> Envelope_2;

  Envelope_2      env;

  env.insert_x_monotone_curves (begin, end,
                                false,          // Upper envelope.
                                diag);

  return;
}

CGAL_END_NAMESPACE

#endif
