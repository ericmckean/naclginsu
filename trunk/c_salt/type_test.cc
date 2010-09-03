// Copyright 2010 The Ginsu Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#include <nacl/npruntime.h>

#include <cmath>
#include <iostream>
#include <limits>
#include <string>

#include "c_salt/booL_type.h"
#include "c_salt/double_type.h"
#include "c_salt/int32_type.h"
#include "c_salt/object_type.h"
#include "c_salt/string_type.h"
#include "c_salt/type.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace c_salt;  // NOLINT

namespace {
const double kTestDoubleValue = 3.141592653589793;
// |kTestTruncatedIntValue| is the integer equivalent to
// floor(kTestDoubleValue).
const int32_t kTestTruncatedIntValue = 3;
const char* kTestDoubleAsString = "3.1415926535897";
const char* kTestString = "this is a test string";
const int32_t kTestIntValue = 42;
const char* kTestIntAsString = "42";
}  // namespace

// TODO(dspringer): Flesh out these tests if we decide to keep c_salt::Type.
class TypeTest : public ::testing::Test {
 protected:
  virtual void SetUp() {}
};

// Create a Type from an NPString NPVariant.  Tests all the typed return
// values.
TEST_F(TypeTest, CreateFromStrNPVariant) {
  NPString np_string = { static_cast<const NPUTF8*>(kTestString),
                         strlen(kTestString) };
  NPVariant np_str_var;
  np_str_var.type = NPVariantType_String;
  np_str_var.value.stringValue = np_string;
  Type* str_type = Type::CreateFromNPVariant(np_str_var);
  EXPECT_EQ(kTestString, str_type->string_value());
  EXPECT_TRUE(str_type->bool_value());
  EXPECT_EQ(0, str_type->int32_value());
  EXPECT_TRUE(std::fabs(str_type->double_value()) <
              std::numeric_limits<double>::epsilon());
}

// Create a Type from an int32 NPVariant.  Tests all the typed return
// values.
TEST_F(TypeTest, CreateFromIntNPVariant) {
  NPVariant np_int_var;
  np_int_var.type = NPVariantType_Int32;
  np_int_var.value.intValue = kTestIntValue;
  Type* str_int_type = Type::CreateFromNPVariant(np_int_var);
  EXPECT_EQ(kTestIntAsString, str_int_type->string_value());
  EXPECT_TRUE(str_int_type->bool_value());
  EXPECT_EQ(kTestIntValue, str_int_type->int32_value());
  EXPECT_TRUE(std::fabs(str_int_type->double_value() - kTestIntValue) <
              std::numeric_limits<double>::epsilon());
}

// Create a Type from an double NPVariant.  Tests all the typed return
// values.
TEST_F(TypeTest, CreateFromDoubleNPVariant) {
  NPVariant np_dbl_var;
  np_dbl_var.type = NPVariantType_Double;
  np_dbl_var.value.doubleValue = kTestDoubleValue;
  Type* str_dbl_type = Type::CreateFromNPVariant(np_dbl_var);
  EXPECT_EQ(0, strncmp(str_dbl_type->string_value().c_str(),
                       kTestDoubleAsString,
                       strlen(kTestDoubleAsString)));
  EXPECT_TRUE(str_dbl_type->bool_value());
  EXPECT_EQ(kTestTruncatedIntValue, str_dbl_type->int32_value());
  EXPECT_TRUE(std::fabs(str_dbl_type->double_value() - kTestDoubleValue) <
              std::numeric_limits<double>::epsilon());
}

// Create a Type from an bool NPVariant.  Tests all the typed return
// values.
TEST_F(TypeTest, CreateFromBoolNPVariant) {
  NPVariant np_bool_var;
  np_bool_var.type = NPVariantType_Bool;
  np_bool_var.value.boolValue = true;
  Type* str_bool_type = Type::CreateFromNPVariant(np_bool_var);
  EXPECT_EQ("TRUE", str_bool_type->string_value());
  EXPECT_TRUE(str_bool_type->bool_value());
  EXPECT_EQ(1, str_bool_type->int32_value());
  EXPECT_TRUE(std::fabs(str_bool_type->double_value() - 1) <
              std::numeric_limits<double>::epsilon());
}

// Make sure that type conversion works.
TEST_F(TypeTest, CreateFromTypeWithTypeId) {
  DoubleType double_type(kTestDoubleValue);
  // Step 1: convert double to string and back.
  Type* str_from_dbl =
      Type::CreateFromTypeWithTypeId(double_type, Type::kStringTypeId);
  EXPECT_EQ(0, strncmp(str_from_dbl->string_value().c_str(),
                       kTestDoubleAsString,
                       strlen(kTestDoubleAsString)));
  Type* dbl_from_str =
      Type::CreateFromTypeWithTypeId(*str_from_dbl, Type::kDoubleTypeId);
  EXPECT_TRUE(std::fabs(dbl_from_str->double_value() - kTestDoubleValue) <
              std::numeric_limits<double>::epsilon());
  // Step 2: convert from double to double.  This should preserve the value.
  Type* dbl_from_dbl =
      Type::CreateFromTypeWithTypeId(double_type, Type::kDoubleTypeId);
  EXPECT_TRUE(std::fabs(dbl_from_dbl->double_value() - kTestDoubleValue) <
              std::numeric_limits<double>::epsilon());
  // Step 3: convert from double to bool, int.  Expect precision loss.
  Type* int_from_dbl =
      Type::CreateFromTypeWithTypeId(double_type, Type::kInt32TypeId);
  EXPECT_EQ(kTestTruncatedIntValue, int_from_dbl->int32_value());
  Type* bool_from_dbl =
      Type::CreateFromTypeWithTypeId(double_type, Type::kBoolTypeId);
  EXPECT_TRUE(bool_from_dbl->bool_value());
}
