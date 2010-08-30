// Copyright 2010 The Ginsu Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#include "c_salt/bool_type.h"

#include <cassert>

namespace c_salt {

BoolType::BoolType(bool bool_value)
    : Type(kBoolTypeId), bool_value_(bool_value) {
}

BoolType::~BoolType() {
}

bool BoolType::ConvertToNPVariant(NPVariant* np_var) const {
  assert(np_var != NULL);
  if (np_var == NULL)
    return false;
  BOOLEAN_TO_NPVARIANT(bool_value(), *np_var);
  return true;
}

}  // namespace c_salt
