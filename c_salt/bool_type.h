// Copyright 2010 The Native Client Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#ifndef C_SALT_BOOL_TYPE_H_
#define C_SALT_BOOL_TYPE_H_

#include <string>

#include "c_salt/type.h"

namespace c_salt {

class BoolType : public Type {
 public:
  explicit BoolType(bool bool_value);
  virtual ~BoolType();
  virtual bool ConvertToNPVariant(NPVariant* np_var) const;

  virtual int32_t int32_value() const {
    return bool_value() ? 1 : 0;
  }
  virtual double double_value() const {
    return bool_value() ? 1.0 : 0.0;
  }

  virtual bool bool_value() const {
    return bool_value_;
  }

  virtual std::string string_value() const {
    return bool_value() ? "TRUE" : "FALSE";
  }

 private:
  bool bool_value_;
};

}  // namespace c_salt
#endif  // C_SALT_BOOL_TYPE_H_
