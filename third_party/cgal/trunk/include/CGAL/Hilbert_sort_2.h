// Copyright (c) 2007  INRIA Sophia-Antipolis (France).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org); you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; version 2.1 of the License.
// See the file LICENSE.LGPL distributed with CGAL.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $URL: svn+ssh://scm.gforge.inria.fr/svn/cgal/branches/CGAL-3.6-branch/Spatial_sorting/include/CGAL/Hilbert_sort_2.h $
// $Id: Hilbert_sort_2.h 51456 2009-08-24 17:10:04Z spion $
//
// Author(s)     : Christophe Delage

#ifndef CGAL_HILBERT_SORT_2_H
#define CGAL_HILBERT_SORT_2_H

#include <CGAL/basic.h>
#include <functional>
#include <cstddef>
#include <CGAL/Hilbert_sort_base.h>

CGAL_BEGIN_NAMESPACE

namespace internal {
    template <class K, int x, bool up> struct Hilbert_cmp_2;

    template <class K, int x>
    struct Hilbert_cmp_2<K,x,true>
        : public std::binary_function<typename K::Point_2,
                                      typename K::Point_2, bool>
    {
        typedef typename K::Point_2 Point;
        K k;
        Hilbert_cmp_2 (const K &_k = K()) : k(_k) {}
        bool operator() (const Point &p, const Point &q) const
        { 
            return Hilbert_cmp_2<K,x,false> (k) (q, p);
        }
    };
    
    template <class K>
    struct Hilbert_cmp_2<K,0,false>
        : public std::binary_function<typename K::Point_2,
                                      typename K::Point_2, bool>
    {
        typedef typename K::Point_2 Point;
        K k;
        Hilbert_cmp_2 (const K &_k = K()) : k(_k) {}
        bool operator() (const Point &p, const Point &q) const
        { 
            return k.less_x_2_object() (p, q);
        }
    };
    
    template <class K>
    struct Hilbert_cmp_2<K,1,false>
        : public std::binary_function<typename K::Point_2,
                                      typename K::Point_2, bool>
    {
        typedef typename K::Point_2 Point;
        K k;
        Hilbert_cmp_2 (const K &_k = K()) : k(_k) {}
        bool operator() (const Point &p, const Point &q) const
        { 
            return k.less_y_2_object() (p, q);
        }
    };
}

template <class K>
class Hilbert_sort_2
{
public:
    typedef K Kernel;
    typedef typename Kernel::Point_2 Point;
    
private:
    Kernel _k;
    std::ptrdiff_t _limit;

    template <int x, bool up> struct Cmp : public internal::Hilbert_cmp_2<Kernel,x,up>
    { Cmp (const Kernel &k) : internal::Hilbert_cmp_2<Kernel,x,up> (k) {} };

public:
    Hilbert_sort_2 (const Kernel &k = Kernel(), std::ptrdiff_t limit = 1)
        : _k(k), _limit (limit)
    {}

    template <int x, bool upx, bool upy, class RandomAccessIterator>
    void sort (RandomAccessIterator begin, RandomAccessIterator end) const
    {
        const int y = (x + 1) % 2;
        if (end - begin <= _limit) return;

        RandomAccessIterator m0 = begin, m4 = end;

        RandomAccessIterator m2 = internal::hilbert_split (m0, m4, Cmp< x,  upx> (_k));
        RandomAccessIterator m1 = internal::hilbert_split (m0, m2, Cmp< y,  upy> (_k));
        RandomAccessIterator m3 = internal::hilbert_split (m2, m4, Cmp< y, !upy> (_k));

        sort<y, upy, upx> (m0, m1);
        sort<x, upx, upy> (m1, m2);
        sort<x, upx, upy> (m2, m3);
        sort<y,!upy,!upx> (m3, m4);
    }

    template <class RandomAccessIterator>
    void operator() (RandomAccessIterator begin, RandomAccessIterator end) const
    {
        sort <0, false, false> (begin, end);
    }
};

CGAL_END_NAMESPACE

#endif//CGAL_HILBERT_SORT_2_H
