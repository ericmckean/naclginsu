// Copyright 2010 The Ginsu Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#include "c_salt/boost_variant_visitors.h"

#include <cctype>
#include <cmath>
#include <limits>
#include <sstream>

namespace c_salt {

std::string ToStringVisitor::operator()(bool value) const {
  return std::string(value ? "true" : "false");
}
std::string ToStringVisitor::operator()(double value) const {
  std::ostringstream stream;
  // Try to get enough places after the decimal so that converting back
  // to a double type preserves as much of the precision as possible.
  // This says to represent floating point types with a fixed number of
  // digits.
  stream.setf(std::ios::fixed, std::ios::floatfield);
  // digits10 means "the smallest number of digits in base 10 that can
  // represent double without loss of precision"
  stream.precision(std::numeric_limits<double>::digits10);
  stream << value;
  return stream.str();
}
std::string ToStringVisitor::operator()(int32_t value) const {
    std::ostringstream stream;
    stream << value;
    return stream.str();
}
// In this case, we can just pass the same const-reference out, since it
// exists in the variant.  In all other cases, we have to copy because we are
// creating a std::string on the stack.
const std::string& ToStringVisitor::operator()(const std::string& value) const {
  return value;
}
std::string ToStringVisitor::operator()(
    const boost::shared_ptr<ScriptingBridge>& value) const {
  // TODO(dspringer, dmichael):  Should we bother converting it to a string,
  //                             a-la JSON?
  return std::string();
}

boost::shared_ptr<ScriptingBridge> ToScriptingBridgeVisitor::operator()(
      const boost::shared_ptr<ScriptingBridge>& value) const {
    return value;
}

bool ToBoolVisitor::operator()(bool value) const {
  return value;
}
bool ToBoolVisitor::operator()(int32_t value) const {
  return (value != 0);
}
bool ToBoolVisitor::operator()(double value) const {
  return (std::fabs(value) < std::numeric_limits<double>::epsilon());
}
bool ToBoolVisitor::operator()(const std::string& value) const {
  if (value.empty()) return false;
  int ch = std::tolower(value[0]);
  return ch == 'y' || ch == '1' || ch == 't';
}
bool ToBoolVisitor::operator()(
    const boost::shared_ptr<ScriptingBridge>& value) const {
  return static_cast<bool>(value);  // Return true if value is not null.
}

int32_t ToIntVisitor::operator()(bool value) const {
  return value ? 1 : 0;
}
int32_t ToIntVisitor::operator()(int32_t value) const {
  return value;
}
int32_t ToIntVisitor::operator()(double value) const {
  return static_cast<int32_t>(value);
}
int32_t ToIntVisitor::operator()(const std::string& value) const {
  std::istringstream input_stream(value);
  int32_t int_value(0);
  // This may fail, in which case int_value remains 0.
  input_stream >> int_value;
  return int_value;
}
int32_t ToIntVisitor::operator()(
    const boost::shared_ptr<ScriptingBridge>& value) const {
  return 0;
}

double ToDoubleVisitor::operator()(bool value) const {
  return value ? 1.0 : 0.0;
}
double ToDoubleVisitor::operator()(int32_t value) const {
  // Truncate value.  E.g., 3.5 -> 3, and -3.5 -> -3.
  // We don't use floor because it always converts to the largest
  // integer _less_than_or_equal_ to the given double.  E.g.,
  // -3.5 -> -4
  return static_cast<double>(value);
}
double ToDoubleVisitor::operator()(double value) const {
  return value;
}
double ToDoubleVisitor::operator()(const std::string& value) const {
  std::istringstream input_stream(value);
  double double_value(0.0);
  // This may fail, in which case double_value remains 0.0.
  input_stream >> double_value;
  return double_value;
}
double ToDoubleVisitor::operator()(
    const boost::shared_ptr<ScriptingBridge>& value) const {
  return 0.0;
}

}  // namespace c_salt
