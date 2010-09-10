// Copyright 2010 The Ginsu Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#ifndef C_SALT_CONVERTING_VISITOR_H_
#define C_SALT_CONVERTING_VISITOR_H_

#include <cctype>
#include <cmath>
#include <limits>
#include <sstream>
#include <string>

#include "boost/shared_ptr.hpp"
#include "boost/variant/static_visitor.hpp"

class ScriptingBridge;

namespace c_salt {

// The following set of ConvertingVisitor template specializations implement the
// Visitor concept, as described here:
// http://live.boost.org/doc/libs/1_41_0/doc/html/variant/reference.html
//   #variant.concepts.static-visitor
// Each instantiation of ConvertingVisitor can be passed as a static visitor to
// the boost::apply_visitor function.  ConvertingVisitor<TargetType> converts
// the value held by the boost variant in to type TargetType.
//
// This one is the default, which has no implementation.  We only have
// specializations implement ConvertingVisitor, as we want to have very
// fine-grained control of what each conversion does depending on the type.
template <class TargetType>
class ConvertingVisitor;

template <>
class ConvertingVisitor<std::string>
  : public boost::static_visitor<std::string> {
 public:
  std::string operator()(bool value) const;
  std::string operator()(int32_t value) const;
  std::string operator()(double value) const;
  // In this case, we can just pass the same const-reference out, since it
  // exists in the variant.  In all other cases, we have to copy because we are
  // creating a std::string on the stack.
  const std::string& operator()(const std::string& value) const;
  std::string operator()(const boost::shared_ptr<ScriptingBridge>& value) const;
};

template <>
class ConvertingVisitor<boost::shared_ptr<ScriptingBridge> >
  : public boost::static_visitor<boost::shared_ptr<ScriptingBridge> > {
 public:
  template <class T>
  boost::shared_ptr<ScriptingBridge> operator()(T value) const {
    // This is a catch all for types other than shared_ptr to ScriptingBridge,
    // none of which can be converted to shared_ptr<ScriptingBridge>, so we
    // just return a default-initialized shared_ptr.
    return boost::shared_ptr<ScriptingBridge>();
  }
  boost::shared_ptr<ScriptingBridge> operator()(
      const boost::shared_ptr<ScriptingBridge>& value) const;
};

template <>
class ConvertingVisitor<bool> : public boost::static_visitor<bool> {
 public:
  bool operator()(bool value) const;
  bool operator()(int32_t value) const;
  bool operator()(double value) const;
  bool operator()(const std::string& value) const;
  bool operator()(const boost::shared_ptr<ScriptingBridge>& value) const;
};

template <>
class ConvertingVisitor<int32_t> : public boost::static_visitor<int32_t> {
 public:
  int32_t operator()(bool value) const;
  int32_t operator()(int32_t value) const;
  int32_t operator()(double value) const;
  int32_t operator()(const std::string& value) const;
  int32_t operator()(const boost::shared_ptr<ScriptingBridge>& value) const;
};

template <>
class ConvertingVisitor<double> : public boost::static_visitor<double> {
 public:
  double operator()(bool value) const;
  double operator()(int32_t value) const;
  double operator()(double value) const;
  double operator()(const std::string& value) const;
  double operator()(const boost::shared_ptr<ScriptingBridge>& value) const;
};

std::string ConvertingVisitor<std::string>::operator()(bool value) const {
  return std::string(value ? "true" : "false");
}
std::string ConvertingVisitor<std::string>::operator()(double value) const {
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
std::string ConvertingVisitor<std::string>::operator()(int32_t value) const {
    std::ostringstream stream;
    stream << value;
    return stream.str();
}
// In this case, we can just pass the same const-reference out, since it
// exists in the variant.  In all other cases, we have to copy because we are
// creating a std::string on the stack.
const std::string&
ConvertingVisitor<std::string>::operator()(const std::string& value) const {
  return value;
}
std::string ConvertingVisitor<std::string>::operator()(
    const boost::shared_ptr<ScriptingBridge>& value) const {
  // TODO(dspringer, dmichael):  Should we bother converting it to a string,
  //                             a-la JSON?
  return std::string();
}

boost::shared_ptr<ScriptingBridge>
ConvertingVisitor<boost::shared_ptr<ScriptingBridge> >::operator()(
      const boost::shared_ptr<ScriptingBridge>& value) const {
    return value;
}

bool ConvertingVisitor<bool>::operator()(bool value) const {
  return value;
}
bool ConvertingVisitor<bool>::operator()(int32_t value) const {
  return (value != 0);
}
bool ConvertingVisitor<bool>::operator()(double value) const {
  return (std::fabs(value) < std::numeric_limits<double>::epsilon());
}
bool ConvertingVisitor<bool>::operator()(const std::string& value) const {
  if (value.empty()) return false;
  int ch = std::tolower(value[0]);
  return ch == 'y' || ch == '1' || ch == 't';
}
bool ConvertingVisitor<bool>::operator()(
    const boost::shared_ptr<ScriptingBridge>& value) const {
  return static_cast<bool>(value);  // Return true if value is not null.
}

int32_t ConvertingVisitor<int32_t>::operator()(bool value) const {
  return value ? 1 : 0;
}
int32_t ConvertingVisitor<int32_t>::operator()(int32_t value) const {
  return value;
}
int32_t ConvertingVisitor<int32_t>::operator()(double value) const {
  return static_cast<int32_t>(value);
}
int32_t ConvertingVisitor<int32_t>::operator()(const std::string& value) const {
  std::istringstream input_stream(value);
  int32_t int_value(0);
  // This may fail, in which case int_value remains 0.
  input_stream >> int_value;
  return int_value;
}
int32_t ConvertingVisitor<int32_t>::operator()(
    const boost::shared_ptr<ScriptingBridge>& value) const {
  return 0;
}

double ConvertingVisitor<double>::operator()(bool value) const {
  return value ? 1.0 : 0.0;
}
double ConvertingVisitor<double>::operator()(int32_t value) const {
  // Truncate value.  E.g., 3.5 -> 3, and -3.5 -> -3.
  // We don't use floor because it always converts to the largest
  // integer _less_than_or_equal_ to the given double.  E.g.,
  // -3.5 -> -4
  return static_cast<double>(value);
}
double ConvertingVisitor<double>::operator()(double value) const {
  return value;
}
double ConvertingVisitor<double>::operator()(const std::string& value) const {
  std::istringstream input_stream(value);
  double double_value(0.0);
  // This may fail, in which case double_value remains 0.0.
  input_stream >> double_value;
  return double_value;
}
double ConvertingVisitor<double>::operator()(
    const boost::shared_ptr<ScriptingBridge>& value) const {
  return 0.0;
}

}  // namespace c_salt

#endif  // C_SALT_CONVERTING_VISITOR_H_
