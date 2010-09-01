// Copyright 2010 The Ginsu Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#ifndef C_SALT_CALLBACK_H_
#define C_SALT_CALLBACK_H_

#include "boost/function.hpp"
#include "boost/bind.hpp"
#include "boost/static_assert.hpp"
#include "boost/type_traits/remove_const.hpp"
#include "boost/type_traits/remove_reference.hpp"

namespace c_salt {
// Templates used to support method call-backs when a method or property is
// accessed from the browser code.

// FunctionInvoker is a class template which holds a boost::function and
// provides a means for invoking that boost::function by providing a sequence
// of variant parameters.  Its purpose is to allow turning invocations from
// various APIs in to a more natural C++ form.  For example, it can be used to
// convert an NPAPI method invocation in to a conventional C++ function call,
// or similarly for PPAPI, SRPC, etc.
//
// This is the default implementation of FunctionInvoker.  Its methods are not
// implemented; it is left here for documentation purposes only.  Any real usage
// of FunctionInvoker must match one of the specializations defined later.  If
// a match is not found, it is likely because the user is attempting to create
// a function invoker with more arguments than we currently support.
template <class Signature>
class FunctionInvoker {
  // Constructor for member functions that binds the target object to the
  // FunctionInvoker's stored boost::function.
  template <class T>
  FunctionInvoker(T* target_object, Signature method);

  // Convert the given parameters (taken as a sequence of variants) in to the
  // C++ types required by the bound function.  Then, invoke the boost::function
  // owned by this FunctionInvoker, passing the arguments.  Convert the return
  // value to a variant.
  // params_begin and params_end are forward iterators for a sequence that
  // contains the arguments to be sent to the function.  Note that STL-style
  // iterators or pointers to the beginning and one-past-the-end of an array
  // are both supported.
  // return_val is an out-parameter.  The caller must provide a valid pointer
  // to an object of the appropriate variant type.
  // converter is a functor which provides conversion operators from the
  // variant type we are using to/from appropriate C++ types.  Conversion
  // operators in VariantConverter must be of the form:
  //   // Set var to value
  //   void operator() (VariantType* var, cpp_type value);
  //   // Set value based upon the contents of var.  Return true if successful,
  //   // false if there was a problem converting.
  //   bool operator() (cpp_type* value, VariantType var);
  //
  // For example, we could define a converter to/from a simple boost::variant as
  // follows:
  // class BoostConverter {
  //  public:
  //   void operator() (boost::variant<int, bool>* var, int value) {
  //     *var = value;
  //   }
  //   void operator() (boost::variant<int, bool>* var, bool value) {
  //     *var = value;
  //   }
  //   bool operator() (int* value, const boost::variant<int, bool>& var) {
  //     const int* var_int = boost::get<int>(&var);
  //     if (NULL != var_int) {
  //       *value = *var_int;
  //       return true;
  //     }
  //     return false;
  //   }
  //   bool operator() (bool* value, const boost::variant<int, bool>& var) {
  //     const bool* var_bool = boost::get<bool>(&var);
  //     if (NULL != var_bool) {
  //       *value = *var_bool;
  //       return true;
  //     }
  //     return false;
  //   }
  // };
  // Note that these methods could be implemented with a template;  as long as
  // C++ overloading rules can select a method unambigously, it should work.
  // If conversion of any of the parameters fails (i.e., the given converter
  // returns false for one of them), then Invoke does not invoke the bound
  // boost::function, and instead immediately returns (without modifying
  // return_val_variant).  Conversions to variants are assumed to always
  // succeed.  If a client attempts to register a function with a return type
  // that is not supported by the given converter functor, a compile error will
  // be generated.
  template <class VariantForwardIterator,
            class VariantType,
            class VariantConverter>
  bool Invoke(VariantForwardIterator params_begin,
              VariantForwardIterator params_end,
              VariantType* return_val_variant,
              VariantConverter converter);
};

// Define some macros temporarily so that we can generate code succinctly for
// our FunctionInvoker classes.  We need one specialization of FunctionInvoker
// for each number of arguments, and they are largely the same.  The macros
// abstract out the parts that are unique.
//
// Here is an example of what the code looks like after the preprocessor is
// finished with it, in this case for 2 arguments:
#if 0
template <class T, class RetType, class Arg1, class Arg2>
class FunctionInvoker<RetType(T::*)(Arg1, Arg2)> {
 public:
  typedef RetType ReturnType;
  typedef ReturnType (T::*MemFunSignature) (Arg1, Arg2);
  typedef ReturnType (*Signature) (Arg1, Arg2);
  typedef boost::function<ReturnType (Arg1, Arg2)> FunctionType;

  FunctionInvoker(T* target_object, MemFunSignature method)
      : function_(boost::bind(method, target_object, _1, _2)) {}
  template <class VariantForwardIterator,
            class VariantType,
            class VariantConverter>
  bool Invoke(VariantForwardIterator params_begin,
              VariantForwardIterator params_end,
              VariantType* return_val_variant,
              VariantConverter converter) {
    // Declare a local instance of the argument type.  If it's a const-ref
    // parameter, remove the const-ref part of the type so we can make a local
    // argument on the stack to pass.
    typedef typename boost::remove_reference<Arg1>::type NoRef1;
    typedef typename boost::remove_const<NoRef1>::type NoConstRef1;
    NoConstRef1 arg1;
    // Attempt to convert params_begin and advance it.  If conversion fails,
    // return false and don't invoke the function.
    if (!converter(&arg1, *(params_begin++))) return false;
    typedef typename boost::remove_reference<Arg2>::type NoRef2;
    typedef typename boost::remove_const<NoRef2>::type NoConstRef2;
    NoConstRef2 arg2;
    if (!converter(&arg2, *(params_begin++))) return false;
    ReturnType retval = function_(arg1, arg2);
    converter(return_val_variant, retval);
    return true;
  }
 private:
  FunctionType function_;
};
#endif
// A macro that fills in the beginning of the class definition, including
// typedefs that we (or our clients) might find useful.
#define FUNCTIONINVOKER_CLASS_DEFINITION_BEGIN(ARGLIST) \
class FunctionInvoker<RetType(T::*) ARGLIST> {\
public:\
  typedef RetType ReturnType;\
  typedef ReturnType (T::*MemFunSignature) ARGLIST;\
  typedef ReturnType (*Signature) ARGLIST;\
  typedef boost::function<ReturnType ARGLIST> FunctionType;\

// A macro for the constructor.  BIND_ARGLIST is of the form:
// (method, target_object ...)
// where the ... is a list of 0 or more bind placeholders (, _1, _2, etc).
#define FUNCTIONINVOKER_CONSTRUCTOR(BIND_ARGLIST) \
  FunctionInvoker(T* target_object, MemFunSignature method) : \
  function_(boost::bind BIND_ARGLIST ) {}
// end FUNCTIONINVOKER_CONSTRUCTOR

// A macro for the beginning of the Invoke function.
#define FUNCTIONINVOKER_INVOKE_BEGIN() \
  template <class VariantForwardIterator, \
            class VariantType, \
            class VariantConverter> \
  bool Invoke(VariantForwardIterator params_begin, \
              VariantForwardIterator params_end, \
              VariantType* return_val_variant, \
              VariantConverter converter) {
// end FUNCTIONINVOKER_INVOKE_BEGIN

// A portion of the invoker function that converts 1 argument, returning
// false if that conversion fails.  NUM is a positive integer.
#define FUNCTIONINVOKER_INVOKE_CONVERT_ARG(NUM) \
    typedef typename boost::remove_reference<Arg ## NUM>::type NoRef ## NUM; \
    typedef typename boost::remove_const<NoRef ## NUM>::type NoConstRef ## NUM;\
    NoConstRef ## NUM arg ## NUM; \
    if (!converter(&arg ## NUM, *(params_begin++))) return false;
// end FUNCTIONINVOKER_INVOKE_CONVERT_ARG

// A macro for the end of the Invoke function.
#define FUNCTIONINVOKER_INVOKE_END(ARGLIST) \
    ReturnType retval = function_ ARGLIST; \
    converter(return_val_variant, retval); \
    return true;\
  }
// end FUNCTIONINVOKER_INVOKE_END

// A macro for the end of the FunctionInvoker class.
#define FUNCTIONINVOKER_CLASS_DEFINITION_END() \
private: \
  FunctionType function_;\
};
// end FUNCTIONINVOKER_CLASS_DEFINITION_END

// Now we use the above macros to define FunctionInvoker partial specializations
// for each number of arguments which we want to support.
// 0 Args.  Note the lack of semicolons.  These macros are not written to allow
// for semicolons.
template <class T, class RetType>
FUNCTIONINVOKER_CLASS_DEFINITION_BEGIN(())
// Note that the parameter list here (and in INVOKE_END) must have additional
// parens around it.  This makes it appear as 1 argument to the preprocessor,
// and places the entire parameter list in the resultant code.
FUNCTIONINVOKER_CONSTRUCTOR((method, target_object))
FUNCTIONINVOKER_INVOKE_BEGIN()
FUNCTIONINVOKER_INVOKE_END(())
FUNCTIONINVOKER_CLASS_DEFINITION_END()
// 1 Args.
template <class T, class RetType, class Arg1>
FUNCTIONINVOKER_CLASS_DEFINITION_BEGIN((Arg1))
FUNCTIONINVOKER_CONSTRUCTOR((method, target_object, _1))
FUNCTIONINVOKER_INVOKE_BEGIN()
FUNCTIONINVOKER_INVOKE_CONVERT_ARG(1)
FUNCTIONINVOKER_INVOKE_END((arg1))
FUNCTIONINVOKER_CLASS_DEFINITION_END()
// 2 Args.
template <class T, class RetType, class Arg1, class Arg2>
FUNCTIONINVOKER_CLASS_DEFINITION_BEGIN((Arg1, Arg2))
FUNCTIONINVOKER_CONSTRUCTOR((method, target_object, _1, _2))
FUNCTIONINVOKER_INVOKE_BEGIN()
FUNCTIONINVOKER_INVOKE_CONVERT_ARG(1)
FUNCTIONINVOKER_INVOKE_CONVERT_ARG(2)
FUNCTIONINVOKER_INVOKE_END((arg1, arg2))
FUNCTIONINVOKER_CLASS_DEFINITION_END()
// 3 Args.
template <class T, class RetType, class Arg1, class Arg2, class Arg3>
FUNCTIONINVOKER_CLASS_DEFINITION_BEGIN((Arg1, Arg2, Arg3))
FUNCTIONINVOKER_CONSTRUCTOR((method, target_object, _1, _2, _3))
FUNCTIONINVOKER_INVOKE_BEGIN()
FUNCTIONINVOKER_INVOKE_CONVERT_ARG(1)
FUNCTIONINVOKER_INVOKE_CONVERT_ARG(2)
FUNCTIONINVOKER_INVOKE_CONVERT_ARG(3)
FUNCTIONINVOKER_INVOKE_END((arg1, arg2, arg3))
FUNCTIONINVOKER_CLASS_DEFINITION_END()
// 4 Args.
template <class T, class RetType, class Arg1, class Arg2, class Arg3
         , class Arg4>
FUNCTIONINVOKER_CLASS_DEFINITION_BEGIN((Arg1, Arg2, Arg3, Arg4))
FUNCTIONINVOKER_CONSTRUCTOR((method, target_object, _1, _2, _3, _4))
FUNCTIONINVOKER_INVOKE_BEGIN()
FUNCTIONINVOKER_INVOKE_CONVERT_ARG(1)
FUNCTIONINVOKER_INVOKE_CONVERT_ARG(2)
FUNCTIONINVOKER_INVOKE_CONVERT_ARG(3)
FUNCTIONINVOKER_INVOKE_CONVERT_ARG(4)
FUNCTIONINVOKER_INVOKE_END((arg1, arg2, arg3, arg4))
FUNCTIONINVOKER_CLASS_DEFINITION_END()
// 5 Args.
template <class T, class RetType, class Arg1, class Arg2, class Arg3
          , class Arg4, class Arg5>
FUNCTIONINVOKER_CLASS_DEFINITION_BEGIN((Arg1, Arg2, Arg3, Arg4, Arg5))
FUNCTIONINVOKER_CONSTRUCTOR((method, target_object, _1, _2, _3, _4, _5))
FUNCTIONINVOKER_INVOKE_BEGIN()
FUNCTIONINVOKER_INVOKE_CONVERT_ARG(1)
FUNCTIONINVOKER_INVOKE_CONVERT_ARG(2)
FUNCTIONINVOKER_INVOKE_CONVERT_ARG(3)
FUNCTIONINVOKER_INVOKE_CONVERT_ARG(4)
FUNCTIONINVOKER_INVOKE_CONVERT_ARG(5)
FUNCTIONINVOKER_INVOKE_END((arg1, arg2, arg3, arg4, arg5))
FUNCTIONINVOKER_CLASS_DEFINITION_END()
// 6 Args.
template <class T, class RetType, class Arg1, class Arg2, class Arg3
          , class Arg4, class Arg5, class Arg6>
FUNCTIONINVOKER_CLASS_DEFINITION_BEGIN((Arg1, Arg2, Arg3, Arg4, Arg5, Arg6))
FUNCTIONINVOKER_CONSTRUCTOR((method, target_object, _1, _2, _3, _4, _5, _6))
FUNCTIONINVOKER_INVOKE_BEGIN()
FUNCTIONINVOKER_INVOKE_CONVERT_ARG(1)
FUNCTIONINVOKER_INVOKE_CONVERT_ARG(2)
FUNCTIONINVOKER_INVOKE_CONVERT_ARG(3)
FUNCTIONINVOKER_INVOKE_CONVERT_ARG(4)
FUNCTIONINVOKER_INVOKE_CONVERT_ARG(5)
FUNCTIONINVOKER_INVOKE_CONVERT_ARG(6)
FUNCTIONINVOKER_INVOKE_END((arg1, arg2, arg3, arg4, arg5, arg6))
FUNCTIONINVOKER_CLASS_DEFINITION_END()

// Now clean up so these macros aren't exported outside this .h file.
#undef FUNCTIONINVOKER_CLASS_DEFINITION_BEGIN
#undef FUNCTIONINVOKER_CONSTRUCTOR
#undef FUNCTIONINVOKER_INVOKE_BEGIN
#undef FUNCTIONINVOKER_INVOKE_CONVERT_ARG
#undef FUNCTIONINVOKER_INVOKE_END
#undef FUNCTIONINVOKER_CLASS_DEFINITION_END

}  // namespace c_salt

#endif  // C_SALT_CALLBACK_H_
