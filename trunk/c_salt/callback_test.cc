// Copyright 2010 The Ginsu Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#include "c_salt/callback.h"

#include <cassert>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#include "boost/variant.hpp"
#include "c_salt/npapi/npvariant_converter.h"
#include "c_salt/npapi/npapi_method_callback.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {
// Use boost::variant for my variant type in the test.  Any variant will do.
typedef boost::variant<int32_t, double, bool, std::string, void*> MyVariant;
typedef std::vector<MyVariant> MyVariantVector;

// A boost static visitor to visit MyVariant and print it.
class VariantPrintVisitor : public boost::static_visitor<void> {
 private:
  std::ostream* stream_;
 public:
  explicit VariantPrintVisitor(std::ostream* stream) : stream_(stream) {}
  void operator()(void* ptr) {
    // Save the format of the stream before setting it to print hex
    std::ios_base::fmtflags saved_format_flags(stream_->flags());
    *stream_ << "Pointer: "
             << std::showbase  // Prefix integral types with their base.
             << std::hex       // Show integral types in hex.
             << reinterpret_cast<unsigned int>(ptr);
    // Reset the format flags.
    stream_->flags(saved_format_flags);
  }
  void operator()(bool b) {
    *stream_ << "Bool: " << (b ? "true" : "false");
  }
  void operator()(const std::string& s) {
    *stream_ << "String: " << s;
  }
  void operator()(double d) {
    *stream_ << "Double: " << d;
  }
  void operator()(int32_t i) {
    *stream_ << "Int: " << i;
  }
};

// An overload of the stream insertion operator to print variants (using the
// visitor defined above)
std::ostream& operator<<(std::ostream& s, const MyVariant& v) {
  VariantPrintVisitor visitor(&s);
  boost::apply_visitor(visitor, v);
  return s;
}

// An overload for streaming a vector of streamable types.
template <class T>
std::ostream& operator<<(std::ostream& s, const std::vector<T>& vector) {
  typename std::vector<T>::const_iterator iter(vector.begin()),
                                          last(vector.end()-1);
  for (; iter != last; ++iter) {
    s << *iter << ", ";
  }
  s << *iter << "\n";
}

// MyConverter just wraps the conversion to/from a variant.  If the types match,
// it succeeds.  If the types do not match, it fails.  It is possible to make
// your converter handle conversions more permissively, but for the purposes of
// testing callback.h, I'll keep it simple.
class MyConverter {
 public:
  template <class ValueType>
  void operator()(MyVariant* var, ValueType value) {
    *var = value;
  }
  template <class ValueType>
  bool operator()(ValueType* value, const MyVariant& var) {
    // See if the type of the variant is actually ValueType.
    const ValueType* var_value = boost::get<ValueType>(&var);
    if (var_value) {
      // It is the right type, so we can convert easily.
      *value = *var_value;
      return true;
    }
    return false;
  }
};

// FakeCallback is a class that provides a member function with a signature
// defined by a given template parameter.  It allows us to set a value for it
// to return when called, and it stores a vector of variants of the arguments
// it was passed so that we can inspect it and ensure that the correct
// parameters were passed.
// The actual implementation is below, spread between FakeCallback
// specializations, FakeCallbackReturnHandler, and FakeCallbackBase:
//      /------------------\
//      | FakeCallbackBase |  < Defines common things for FakeCallbacks
//      \------------------/
//               ^
//               | inherits from
//  /-------------------------------\
//  | FakeCallbackReturnHandlerBase |  < Handles void returns
//  \-------------------------------/
//               ^
//               | inherits from
//       /--------------\
//       | FakeCallback |  < Defines Func, sets passed_params_vector_ based
//       \--------------/    on the arguments it is passed.
//
template <class Signature>
struct FakeCallback;

// All things common to FakeCallback specializations
class FakeCallbackBase {
 protected:
  MyVariantVector passed_params_vector_;
  MyVariant value_to_return_;
  FakeCallbackBase() {}
  ~FakeCallbackBase() {}
 public:
  void Clear() {
    passed_params_vector_.clear();
  }
  void set_value_to_return(const MyVariant& var) {
    value_to_return_ = var;
  }
  const MyVariantVector& passed_params_vector() {
    return passed_params_vector_;
  }
};

// FakeCallbackReturnHandler is specialized on return type.  For void return
// type, we don't return anything.  This one is the default implementation for
// non-void return types.  We just return return_val.
template <class ReturnType>
class FakeCallbackReturnHandler : public FakeCallbackBase {
 public:
  ReturnType value_to_return() {
    MyConverter converter;
    ReturnType return_val;
    bool success = converter(&return_val, value_to_return_);
    return return_val;
  }
};
// The specialization for void return types.  Don't return anything.
template <>
class FakeCallbackReturnHandler<void> : public FakeCallbackBase {
 public:
  void value_to_return() {}
};

template <class Signature>
struct FakeCallback;

template <class Ret>
struct FakeCallback<Ret()> : public FakeCallbackReturnHandler<Ret> {
  Ret Func() {
    this->passed_params_vector_.clear();
    return this->value_to_return();
  }
};
template <class Ret, class Arg1>
struct FakeCallback<Ret(Arg1)> : public FakeCallbackReturnHandler<Ret> {
  Ret Func(Arg1 a1) {
    this->passed_params_vector_.clear();
    this->passed_params_vector_.push_back(a1);
    return this->value_to_return();
  }
};
template <class Ret, class Arg1, class Arg2>
struct FakeCallback<Ret(Arg1, Arg2)>
  : public FakeCallbackReturnHandler<Ret> {
  Ret Func(Arg1 a1, Arg2 a2) {
    this->passed_params_vector_.clear();
    this->passed_params_vector_.push_back(a1);
    this->passed_params_vector_.push_back(a2);
    return this->value_to_return();
  }
};
template <class Ret, class Arg1, class Arg2, class Arg3>
struct FakeCallback<Ret(Arg1, Arg2, Arg3)>
  : public FakeCallbackReturnHandler<Ret> {
  Ret Func(Arg1 a1, Arg2 a2, Arg3 a3) {
    this->passed_params_vector_.clear();
    this->passed_params_vector_.push_back(a1);
    this->passed_params_vector_.push_back(a2);
    this->passed_params_vector_.push_back(a3);
    return this->value_to_return();
  }
};
template <class Ret, class Arg1, class Arg2, class Arg3, class Arg4>
struct FakeCallback<Ret(Arg1, Arg2, Arg3, Arg4)>
  : public FakeCallbackReturnHandler<Ret> {
  Ret Func(Arg1 a1, Arg2 a2, Arg3 a3, Arg4 a4) {
    this->passed_params_vector_.clear();
    this->passed_params_vector_.push_back(a1);
    this->passed_params_vector_.push_back(a2);
    this->passed_params_vector_.push_back(a3);
    this->passed_params_vector_.push_back(a4);
    return this->value_to_return();
  }
};
template <class Ret, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5>
struct FakeCallback<Ret(Arg1, Arg2, Arg3, Arg4, Arg5)>
  : public FakeCallbackReturnHandler<Ret> {
  Ret Func(Arg1 a1, Arg2 a2, Arg3 a3, Arg4 a4, Arg5 a5) {
    this->passed_params_vector_.clear();
    this->passed_params_vector_.push_back(a1);
    this->passed_params_vector_.push_back(a2);
    this->passed_params_vector_.push_back(a3);
    this->passed_params_vector_.push_back(a4);
    this->passed_params_vector_.push_back(a5);
    return this->value_to_return();
  }
};
template <class Ret, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5,
          class Arg6>
struct FakeCallback<Ret(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6)>
  : public FakeCallbackReturnHandler<Ret> {
  Ret Func(Arg1 a1, Arg2 a2, Arg3 a3, Arg4 a4, Arg5 a5, Arg6 a6) {
    this->passed_params_vector_.clear();
    this->passed_params_vector_.push_back(a1);
    this->passed_params_vector_.push_back(a2);
    this->passed_params_vector_.push_back(a3);
    this->passed_params_vector_.push_back(a4);
    this->passed_params_vector_.push_back(a5);
    this->passed_params_vector_.push_back(a6);
    return this->value_to_return();
  }
};


// The actual implementation of InvokeAndCheckEquivalence.  This is sort of a
// hack to make the type of FakeCallback<Signature>::Func available through
// function template type deduction.
// See InvokeAndCheckEquivalence below for information on what the function
// does.
template <class Signature, class ReturnType, class MemFunSignature>
void InvokeAndCheckEquivalenceImpl(const MyVariantVector& parameters_to_pass,
                               ReturnType expected_return_value,
                               FakeCallback<Signature> callback_mock,
                               MemFunSignature func_ptr) {
  // Create an appropriate FunctionInvoker that can handle the type of
  // FakeCallback<Signature>::Func.
  c_salt::FunctionInvoker<MemFunSignature> invoker(&callback_mock, func_ptr);
  callback_mock.set_value_to_return(expected_return_value);
  // Remember the expected return value (as a variant)
  MyVariant expected_return_value_var = expected_return_value;
  // actual_return_value_var will be set when we invoke the function.
  MyVariant actual_return_value_var;
  MyConverter converter;
  invoker.Invoke(parameters_to_pass.begin(),
                 parameters_to_pass.end(),
                 &actual_return_value_var,
                 converter);
  // The value written to actual_return_value_var should match the expected
  // value we told callback_mock to return.
  EXPECT_EQ(expected_return_value_var, actual_return_value_var);
  // The parameters that callback_mock received should match the ones we passed.
  // Along the way:
  //  -Invoke converted the vector of variants in to the individual parameters
  //   of the real C++ types necessary for passing to FakeCallback::Func
  //  -Invoke called FakeCallback::Func with those arguments (via a
  //   boost::function)
  //  -FakeCallback::Func took the real arguments and pushed them in to its
  //   passed_params_vector.
  // All that should result in callback_mock.passed_params_vector() matching
  // the values we sent in at the outset in parameters_to_pass.
  EXPECT_EQ(parameters_to_pass, callback_mock.passed_params_vector());
}

// Use a FunctionInvoker to invoke Func on the given FakeCallback, passing
// parameters_to_pass.  Set the FakeCallback to return expected_return_value.
// Then, use gtest macros to make sure that the FakeCallback was passed the
// correct parameters and that its return value was converted properly also.
template <class Signature, class ReturnType>
void InvokeAndCheckEquivalence(const MyVariantVector& parameters_to_pass,
                               ReturnType expected_return_value,
                               FakeCallback<Signature> callback_mock) {
  InvokeAndCheckEquivalenceImpl(parameters_to_pass,
                                expected_return_value,
                                callback_mock,
                                &FakeCallback<Signature>::Func);
}

}  // unnamed namespace

// Test framework.  We just put some values we want to test for each type
// in to vectors.
class CallbackTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    int32_t_values_.push_back(0);
    int32_t_values_.push_back(std::numeric_limits<int32_t>::max());
    int32_t_values_.push_back(std::numeric_limits<int32_t>::min());

    bool_values_.push_back(false);
    bool_values_.push_back(true);

    string_values_.push_back("");
    string_values_.push_back("Hello world!");

    double_values_.push_back(0.0);
    double_values_.push_back(std::numeric_limits<double>::infinity());
    double_values_.push_back(std::numeric_limits<double>::max());
    double_values_.push_back(std::numeric_limits<double>::min());
  }

  // These vectors define what values we will test for each possible parameter
  // type.
  std::vector<int32_t> int32_t_values_;
  std::vector<bool> bool_values_;
  std::vector<double> double_values_;
  std::vector<std::string> string_values_;
};

using std::string;  // So we can refer to it as just string for the macros

// Check that all values we want to test for the given type are returned
// properly.
#define TEST_RETURN_TYPE(TYPE) \
{ \
  MyVariantVector params; \
  FakeCallback<TYPE()> mock; \
  std::vector<TYPE>::const_iterator iter(this->TYPE ## _values_.begin()), \
                                    the_end(this->TYPE ## _values_.end()); \
  for (; iter != the_end; ++iter) { \
    InvokeAndCheckEquivalence(params, *iter, mock); \
  } \
}

// Test that each type is returned properly.
TEST_F(CallbackTest, EmptyParamsCheckReturn) {
  TEST_RETURN_TYPE(bool);  //NOLINT - Lint thinks this is a function declaration
  TEST_RETURN_TYPE(double);  //NOLINT
  TEST_RETURN_TYPE(int32_t);  //NOLINT
  TEST_RETURN_TYPE(string);  //NOLINT
  {
    MyVariantVector params;
    FakeCallback<void* ()> mock;
    InvokeAndCheckEquivalence(params, static_cast<void*>(NULL), mock);
    InvokeAndCheckEquivalence(params,
                              reinterpret_cast<void*>(0xdeadbeef),
                              mock);
  }
}

// Check that all values we want to test for the given type are passed properly
// to 1-param functions.
#define TEST_PARAM_TYPE1(TYPE) \
{ \
  MyVariantVector params(1); \
  FakeCallback<bool(TYPE)> mock; /* NOLINT - thinks that's a C-style cast */ \
  std::vector<TYPE>::const_iterator iter(this->TYPE ## _values_.begin()), \
                                    the_end(this->TYPE ## _values_.end()); \
  for (; iter != the_end; ++iter) { \
    params[0] = *iter; \
    InvokeAndCheckEquivalence(params, false, mock); \
  } \
}

// Check that all values we want to test for the given types are passed properly
// to 2-param functions.
#define TEST_PARAM_TYPE2(TYPE1, TYPE2) \
{ \
  MyVariantVector params(2); \
  FakeCallback<bool(TYPE1, TYPE2)> mock; /* NOLINT: thinks C-style cast */ \
  std::vector<TYPE1>::const_iterator iter1(this->TYPE1 ## _values_.begin()), \
                                    the_end1(this->TYPE1 ## _values_.end()); \
  for (; iter1 != the_end1; ++iter1) { \
    params[0] = *iter1; \
    std::vector<TYPE2>::const_iterator iter2(this->TYPE2 ## _values_.begin()), \
                                      the_end2(this->TYPE2 ## _values_.end()); \
    for (; iter2 != the_end2; ++iter2) { \
      params[1] = *iter2; \
      InvokeAndCheckEquivalence(params, false, mock); \
    } \
  } \
}

TEST_F(CallbackTest, CheckParamsThorough) {
  // Test 1-parameter calls for all types (but void*)
  TEST_PARAM_TYPE1(bool);  // NOLINT - Lint thinks these are function
                           //          declarations with unnamed parameters.
  TEST_PARAM_TYPE1(double);  // NOLINT
  TEST_PARAM_TYPE1(int32_t);  // NOLINT
  TEST_PARAM_TYPE1(string);  // NOLINT
  // Test 2-parameter calls for all pairs of types (but void*)
  TEST_PARAM_TYPE2(bool, bool);  // NOLINT
  TEST_PARAM_TYPE2(bool, double);  // NOLINT
  TEST_PARAM_TYPE2(bool, int32_t);  // NOLINT
  TEST_PARAM_TYPE2(bool, string);  // NOLINT
  TEST_PARAM_TYPE2(double, bool);  // NOLINT
  TEST_PARAM_TYPE2(double, double);  // NOLINT
  TEST_PARAM_TYPE2(double, int32_t);  // NOLINT
  TEST_PARAM_TYPE2(double, string);  // NOLINT
  TEST_PARAM_TYPE2(int32_t, bool);  // NOLINT
  TEST_PARAM_TYPE2(int32_t, double);  // NOLINT
  TEST_PARAM_TYPE2(int32_t, int32_t);  // NOLINT
  TEST_PARAM_TYPE2(int32_t, string);  // NOLINT
  TEST_PARAM_TYPE2(string, bool);  // NOLINT
  TEST_PARAM_TYPE2(string, double);  // NOLINT
  TEST_PARAM_TYPE2(string, int32_t);  // NOLINT
  TEST_PARAM_TYPE2(string, string);  // NOLINT
  // Test 1-argument parameters of void* type
  {
    MyVariantVector params(1);
    FakeCallback<bool(void*)> mock;  // NOLINT - this is NOT a C-style cast
    params[0] = static_cast<void*>(NULL);
    InvokeAndCheckEquivalence(params, false, mock);
    params[0] = reinterpret_cast<void*>(0xdeadbeef);
    InvokeAndCheckEquivalence(params, false, mock);
  }
}

// Try a few arbitrarily chosen invocations, making sure to cover
// each number of arguments.
TEST_F(CallbackTest, CheckParamsSpotTesting) {
  {
    MyVariantVector params(3);
    FakeCallback<bool(double, int32_t, std::string)> mock3;  // NOLINT: not cast
    params[0] = 3.1415;
    params[1] = static_cast<int32_t>(42);
    params[2] = std::string("\"Beware of bugs in the above code; I have only "
                            "proved it correct, not tried it.\" -Knuth");
    InvokeAndCheckEquivalence(params, false, mock3);

    // Add another param, this time a bool.
    FakeCallback<bool(double, int32_t, std::string, bool)> mock4; // NOLINT
    params.push_back(false);
    InvokeAndCheckEquivalence(params, false, mock4);

    // Add another param, this time another int32_t.
    FakeCallback<bool(double,  // NOLINT - this is not a C-style cast
                      int32_t,
                      std::string,
                      bool,
                      int32_t)> mock5;
    params.push_back(static_cast<int32_t>(5));
    InvokeAndCheckEquivalence(params, false, mock5);

    // Add another param, this time another double.
    FakeCallback<bool(double,  // NOLINT - this is not a C-style cast
                      int32_t,
                      std::string,
                      bool,
                      int32_t,
                      double)>
      mock6;
    params.push_back(3.0e6);
    InvokeAndCheckEquivalence(params, false, mock6);
  }
}

// Try a few arbitrarily chosen invocations, making sure to cover
// each number of arguments.
TEST_F(CallbackTest, CheckConstRefParams) {
  {
    MyVariantVector params(3);
    FakeCallback<bool(const double&, // NOLINT - this is not a C-style cast
                      const int32_t&,
                      const std::string&)> mock3;
    params[0] = 3.1415;
    params[1] = static_cast<int32_t>(42);
    params[2] = std::string("\"Beware of bugs in the above code; I have only "
                            "proved it correct, not tried it.\" -Knuth");
    InvokeAndCheckEquivalence(params, false, mock3);

    // Add another param, this time a bool.
    FakeCallback<bool(const double&,  // NOLINT - this is not a C-style cast
                      const int32_t&,
                      const std::string&,
                      const bool&)> mock4;
    params.push_back(false);
    InvokeAndCheckEquivalence(params, false, mock4);

    // Add another param, this time another int32_t.
    FakeCallback<bool(const double&,  // NOLINT - this is not a C-style cast
                      const int32_t&,
                      const std::string&,
                      const bool&,
                      const int32_t&)> mock5;
    params.push_back(static_cast<int32_t>(5));
    InvokeAndCheckEquivalence(params, false, mock5);

    // Add another param, this time another double.
    FakeCallback<bool(const double&,  // NOLINT - this is not a C-style cast
                       const int32_t&,
                       const std::string&,
                       const bool&,
                       const int32_t&,
                       const double&)>
      mock6;
    params.push_back(3.0e6);
    InvokeAndCheckEquivalence(params, false, mock6);
  }
}
