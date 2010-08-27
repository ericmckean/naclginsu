// Copyright (c) 2010 The Ginsu Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ginsu/ginsu_module.h"

// Return the Ginsu Module.
namespace c_salt {
Module* CreateModule() {
  return new ginsu::GinsuModule();
}
}  // namespace c_salt
