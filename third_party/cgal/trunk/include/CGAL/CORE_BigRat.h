// Copyright (c) 2006-2008 Max-Planck-Institute Saarbruecken (Germany).
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
// $URL: svn+ssh://scm.gforge.inria.fr/svn/cgal/branches/CGAL-3.5-branch/Number_types/include/CGAL/CORE_BigRat.h $
// $Id: CORE_BigRat.h 47264 2008-12-08 06:25:14Z hemmer $
//
//
// Author(s)     : Michael Hemmer   <hemmer@mpi-inf.mpg.de>


#ifndef CGAL_CORE_BIGRAT_H
#define CGAL_CORE_BIGRAT_H

#include <CGAL/number_type_basic.h>
#include <CGAL/CORE_coercion_traits.h>
#include <CGAL/CORE_Expr.h> // used for To_interval-functor

//#if defined(CGAL_CORE_BIGRAT_NUMER_DENOM_ARE_MEMBERS)
//  #define CGAL_CORE_NUMERATOR(X) ((X).numerator())
//  #define CGAL_CORE_DENOMINATOR(X) ((X).denominator())
//#elif defined(CGAL_CORE_BIGRAT_NUMER_DENOM_ARE_NONMEMBERS)
  #define CGAL_CORE_NUMERATOR(X) (numerator((X)))
  #define CGAL_CORE_DENOMINATOR(X) (denominator((X)))
//#else

CGAL_BEGIN_NAMESPACE

//
// Algebraic structure traits
//
template <> class Algebraic_structure_traits< CORE::BigRat >
  : public Algebraic_structure_traits_base< CORE::BigRat,
                                            Field_tag >  {
  public:
    typedef Tag_true            Is_exact;
    typedef Tag_false           Is_numerical_sensitive;

    // BigRat are always normalized, so no special simplify-functor is needed

    // Nothing new...
};




//
// Real embeddable traits
//
template <> class Real_embeddable_traits< CORE::BigRat >
  : public INTERN_RET::Real_embeddable_traits_base< CORE::BigRat , CGAL::Tag_true > {
  public:

    class Abs
      : public std::unary_function< Type, Type > {
      public:
        Type operator()( const Type& x ) const {
          return CORE::abs( x );
        }
    };

    class Sgn
      : public std::unary_function< Type, ::CGAL::Sign > {
      public:
        ::CGAL::Sign operator()( const Type& x ) const {
          return (::CGAL::Sign) CORE::sign( x );
        }
    };

    class Compare
      : public std::binary_function< Type, Type,
                                Comparison_result > {
      public:
        Comparison_result operator()( const Type& x,
                                            const Type& y ) const {
          return CGAL::sign( ::CORE::cmp(x,y));
        }
    };

    class To_double
      : public std::unary_function< Type, double > {
      public:
        double operator()( const Type& x ) const {
          // this call is required to get reasonable values for the double
          // approximation
          return x.doubleValue();
        }
    };

    class To_interval
      : public std::unary_function< Type, std::pair< double, double > > {
      public:
        std::pair<double, double> operator()( const Type& x_ ) const {
            CORE::Expr x(x_);
            std::pair<double,double> result;
            x.doubleInterval(result.first, result.second);
            CGAL_expensive_assertion(result.first  <= x);
            CGAL_expensive_assertion(result.second >= x);
            return result;
        }
    };
};

/*! \ingroup NiX_Fraction_traits_spec
 *  \brief Specialization of Fraction_traits for ::leda::rational
 */
template <>
class Fraction_traits< CORE::BigRat > {
public:
    typedef CORE::BigRat Type;
    typedef ::CGAL::Tag_true Is_fraction;
    typedef CORE::BigInt Numerator_type;
    typedef Numerator_type Denominator_type;

    typedef Algebraic_structure_traits< Numerator_type >::Gcd Common_factor;

    class Decompose {
    public:
        typedef Type first_argument_type;
        typedef Numerator_type& second_argument_type;
        typedef Numerator_type& third_argument_type;
        void operator () (
                const Type& rat,
                Numerator_type& num,
                Numerator_type& den) {
            num = CGAL_CORE_NUMERATOR(rat);
            den = CGAL_CORE_DENOMINATOR(rat);
        }
    };

    class Compose {
    public:
        typedef Numerator_type first_argument_type;
        typedef Numerator_type second_argument_type;
        typedef Type result_type;
        Type operator ()(
                const Numerator_type& num ,
                const Numerator_type& den ) {
            return Type(num, den);
        }
    };
};

template <class F>
class Output_rep< ::CORE::BigRat, F> {
    const ::CORE::BigRat& t;
public:
    //! initialize with a const reference to \a t.
    Output_rep( const ::CORE::BigRat& tt) : t(tt) {}
    //! perform the output, calls \c operator\<\< by default.
    std::ostream& operator()( std::ostream& out) const {
        switch (get_mode(out)) {
        case IO::PRETTY:{
            if(CGAL_CORE_DENOMINATOR(t) == ::CORE::BigRat(1))
                return out <<CGAL_CORE_NUMERATOR(t);
            else
                return out << CGAL_CORE_NUMERATOR(t)
                           << "/"
                           << CGAL_CORE_DENOMINATOR(t);
            //break; // unreachable
        }

        default:
            return out << CGAL_CORE_NUMERATOR(t)
                       << "/"
                       << CGAL_CORE_DENOMINATOR(t);
        }
    }
};

template <>
struct Needs_parens_as_product< ::CORE::BigRat >{
    bool operator()( ::CORE::BigRat t){
        if (CGAL_CORE_DENOMINATOR(t) != 1 )
            return true;
        else
            return needs_parens_as_product(CGAL_CORE_NUMERATOR(t)) ;
    }
};

template <>
class Output_rep< ::CORE::BigRat, Parens_as_product_tag > {
    const ::CORE::BigRat& t;
public:
    // Constructor
    Output_rep( const ::CORE::BigRat& tt) : t(tt) {}
    // operator
    std::ostream& operator()( std::ostream& out) const {
        Needs_parens_as_product< ::CORE::BigRat > needs_parens_as_product;
        if (needs_parens_as_product(t))
            return out <<"("<< oformat(t) <<")";
        else
            return out << oformat(t);
    }
};

// Benchmark_rep specialization 
template<>
class Benchmark_rep< CORE::BigRat > {
    const CORE::BigRat& t;
public:
    //! initialize with a const reference to \a t.
    Benchmark_rep( const CORE::BigRat& tt) : t(tt) {}
    //! perform the output, calls \c operator\<\< by default.
    std::ostream& operator()( std::ostream& out) const { 
            out << "Rational(" << numerator(t) << "," << denominator(t) << ")";
            return out;
    }
    
    static std::string get_benchmark_name() {
        return "Rational";
    }
};

CGAL_END_NAMESPACE

//since types are included by CORE_coercion_traits.h:
#include <CGAL/CORE_Expr.h>
#include <CGAL/CORE_BigInt.h>
#include <CGAL/CORE_BigRat.h>
#include <CGAL/CORE_BigFloat.h>

#endif // CGAL_CORE_BIGRAT_H
