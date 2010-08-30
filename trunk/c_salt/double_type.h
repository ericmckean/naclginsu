// Copyright 2010 The Native Client Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#ifndef C_SALT_DOUBLE_TYPE_H_
#define C_SALT_DOUBLE_TYPE_H_

#include <cmath>

#include "c_salt/type.h"

namespace c_salt {

class DoubleType : public Type {
 public:
  explicit DoubleType(double double_value);
  virtual ~DoubleType();
  virtual bool ConvertToNPVariant(NPVariant* np_var) const;

  virtual bool bool_value() const {
    return double_value() != 0.0;
  }
  virtual int32_t int32_value() const {
    return static_cast<int32_t>(std::floor(double_value()) + 0.5);
  }

  virtual double double_value() const {
    return double_value_;
  }

 private:
  static const size_t kMaxStrLength;
  double double_value_;
};

}  // namespace c_salt

#endif  // C_SALT_DOUBLE_TYPE_H_
