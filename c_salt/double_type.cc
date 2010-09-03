// Copyright 2010 The Ginsu Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#include "c_salt/double_type.h"

#include <cassert>
#include <limits>
#include <sstream>

namespace c_salt {

const size_t DoubleType::kMaxStrLength = 32;

DoubleType::DoubleType(double double_value)
    : Type(kDoubleTypeId), double_value_(double_value) {
}

DoubleType::~DoubleType() {
}

bool DoubleType::ConvertToNPVariant(NPVariant* np_var) const {
  assert(np_var);
  if (np_var == NULL)
    return false;
  DOUBLE_TO_NPVARIANT(double_value(), *np_var);
  return true;
}

std::string DoubleType::string_value() const {
  std::stringstream str_val;
  // Try to get enough places after the decimal so that converting back
  // to a double type preserves as much of the precision as possible.
  str_val.precision(std::numeric_limits<double>::digits10);
  str_val.setf(std::ios::fixed, std::ios::floatfield);
  str_val << double_value();
  return str_val.str();
}

}  // namespace c_salt
