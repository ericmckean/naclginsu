// Copyright 2010 The Ginsu Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.
#include "c_salt/npapi/npvariant_converter.h"

#include <nacl/npruntime.h>

#include <limits>
#include <string>
#include <vector>

#include "c_salt/instance.h"
#include "c_salt/module.h"
#include "gtest/gtest.h"

namespace {

// An overload for streaming a vector of streamable types.
template <class T>
std::ostream& operator<<(std::ostream& s, const std::vector<T>& vector) {
  typename std::vector<T>::const_iterator iter(vector.begin()),
                                          last(vector.end()-1);
  for (; iter != last; ++iter) {
    s << *iter << ", ";
  }
  s << *iter << "\n";
  return s;
}

class NPVariantConverterTest : public ::testing::Test {
 protected:
  virtual void SetUp() {}
};

template <class T>
void CheckConversion(T value) {
  NPVariant var;
  T out;
  c_salt::npapi::NPVariantConverter converter(NULL, NULL);
      // TODO(dmichael) - this will not work;  have to test through the browser
      // most likely, since we need JavaScript + NPAPI browser support
  converter(&var, value);
  converter(&out, var);
  EXPECT_EQ(value, out);
}

TEST_F(NPVariantConverterTest, SameType) {
  // strings
  CheckConversion(std::string(""));
  CheckConversion(std::string("Hello World!"));
  CheckConversion(std::string("3.14"));
  CheckConversion(std::string("42"));

  // int32_t
  CheckConversion(static_cast<int32_t>(-1));
  CheckConversion(static_cast<int32_t>(0));
  CheckConversion(static_cast<int32_t>(1));
  CheckConversion(static_cast<int32_t>(42));
  CheckConversion(std::numeric_limits<int32_t>::max());
  CheckConversion(std::numeric_limits<int32_t>::min());

  // double
  CheckConversion(static_cast<double>(0));
  CheckConversion(3.1415);
  CheckConversion(std::numeric_limits<double>::max());
  CheckConversion(std::numeric_limits<double>::min());
  CheckConversion(std::numeric_limits<double>::infinity());

  // bool
  CheckConversion(true);
  CheckConversion(false);

#if 0
  // The vector stuff doesn't work without a browser so NPVariantConverter can
  // do JavaScript evaluation, etc.
  // vector of strings
  std::vector<std::string> string_vec;
  CheckConversion(string_vec);
  string_vec.push_back("");
  string_vec.push_back("Hello world!");
  string_vec.push_back("37");
  string_vec.push_back("3.1415");
  string_vec.push_back("true");
  CheckConversion(string_vec);

  // vector of ints
  std::vector<int32_t> int_vec;
  CheckConversion(int_vec);
  int_vec.push_back(0);
  int_vec.push_back(42);
  int_vec.push_back(-310);
  CheckConversion(int_vec);

  // vector of doubles
  std::vector<double> double_vec;
  CheckConversion(double_vec);
  double_vec.push_back(0.0);
  double_vec.push_back(3.1415);
  double_vec.push_back(-1.7);
  CheckConversion(double_vec);

  // vector of bools
  std::vector<bool> bool_vec;
  CheckConversion(bool_vec);
  bool_vec.push_back(true);
  bool_vec.push_back(false);
  bool_vec.push_back(true);
  CheckConversion(bool_vec);

  // vector of vector of ints
  // [0] :
  // [1] : 1
  // [2] : 1, 2
  // [3] : 1, 2, 3
  // etc up to [9]
  std::vector< std::vector<int32_t> > int_vec_vec;
  CheckConversion(int_vec_vec);
  for (size_t i = 0; i < 10; ++i) {
    std::vector<int32_t> inner_vector;
    for (size_t j = 0; j < 10; ++j) {
      inner_vector.push_back(static_cast<int32_t>(j));
    }
    int_vec_vec.push_back(inner_vector);
  }
  CheckConversion(int_vec_vec);
#endif
}

}  // unnamed namespace

class MyInstance : public c_salt::Instance {
 public:
  explicit MyInstance(const NPP& instance) : c_salt::Instance(instance) {}
};

class TestModule : public c_salt::Module {
 public:
  virtual c_salt::Instance* CreateInstance(const NPP& instance) {
    return new MyInstance(instance);
  }
};
