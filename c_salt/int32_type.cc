// Copyright 2010 The Ginsu Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#include "c_salt/int32_type.h"

#include <cassert>

namespace c_salt {

const size_t Int32Type::kMaxStrLength = 32;

Int32Type::Int32Type(int32_t int32_value)
    : Type(kInt32TypeId), int32_value_(int32_value) {
}

Int32Type::~Int32Type() {
}

bool Int32Type::ConvertToNPVariant(NPVariant* np_var) const {
  assert(np_var);
  if (np_var == NULL)
    return false;
  INT32_TO_NPVARIANT(int32_value(), *np_var);
  return true;
}

}  // namespace c_salt
