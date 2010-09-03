// Copyright 2010 The Ginsu Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#include "c_salt/string_type.h"

#include <cmath>
#include <limits>

#include "c_salt/bool_type.h"
#include "c_salt/double_type.h"
#include "c_salt/int32_type.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using c_salt::BoolType;
using c_salt::DoubleType;
using c_salt::Int32Type;
using c_salt::StringType;
using c_salt::Type;

namespace {
const char* kTestBoolTrueString = "TRUE";
const double kTestDoubleValue = 3.141592653589793;
const char* kTestDoubleAsString = "3.1415926535897";
const int32_t kTestIntValue = 42;
const char* kTestIntAsString = "42";
}  // namespace

class StringTypeTest : public testing::Test {
};

// Make sure well-known string values convert correctly to bool types, and
// bool types convert to the known string values.
TEST_F(StringTypeTest, TypeConvertBool) {
  BoolType bool_type(true);
  // Convert bool to string and back.
  Type* str_from_bool =
      Type::CreateFromTypeWithTypeId(bool_type, Type::kStringTypeId);
  EXPECT_EQ(0, strncmp(str_from_bool->string_value().c_str(),
                       kTestBoolTrueString,
                       strlen(kTestBoolTrueString)));
  Type* bool_from_str =
      Type::CreateFromTypeWithTypeId(*str_from_bool, Type::kBoolTypeId);
  EXPECT_TRUE(bool_from_str->bool_value());
  // Step 2: Convert certain well-known strings to bools.
  StringType str_true("true");
  EXPECT_TRUE(str_true.bool_value());
  StringType str_yes("Yes");
  EXPECT_TRUE(str_yes.bool_value());
  StringType str_1("1");
  EXPECT_TRUE(str_1.bool_value());
  StringType str_0("0");
  EXPECT_FALSE(str_0.bool_value());
  StringType str_false("hello?");
  EXPECT_FALSE(str_false.bool_value());
}

// Make sure the coinversion from a double type works both ways.
TEST_F(StringTypeTest, TypeConvertDouble) {
  DoubleType dbl_type(kTestDoubleValue);
  // Convert double to string and back.
  Type* str_from_dbl =
      Type::CreateFromTypeWithTypeId(dbl_type, Type::kStringTypeId);
  EXPECT_EQ(0, strncmp(str_from_dbl->string_value().c_str(),
                       kTestDoubleAsString,
                       strlen(kTestDoubleAsString)));
  Type* dbl_from_str =
      Type::CreateFromTypeWithTypeId(*str_from_dbl, Type::kDoubleTypeId);
  EXPECT_TRUE(std::fabs(dbl_from_str->double_value() - kTestDoubleValue) <
              std::numeric_limits<double>::epsilon());
}

// Make sure the coinversion from an int type works both ways.
TEST_F(StringTypeTest, TypeConvertInt) {
  Int32Type int_type(kTestIntValue);
  // Convert int to string and back.
  Type* str_from_int =
      Type::CreateFromTypeWithTypeId(int_type, Type::kStringTypeId);
  EXPECT_EQ(kTestIntAsString, str_from_int->string_value());
  Type* int_from_str =
      Type::CreateFromTypeWithTypeId(*str_from_int, Type::kInt32TypeId);
  EXPECT_EQ(kTestIntValue, int_from_str->int32_value());
}
