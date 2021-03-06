// Copyright (c) 2010 The Ginsu Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GINSU_GINSU_MODULE_H_
#define GINSU_GINSU_MODULE_H_

#include "c_salt/module.h"
#include "ginsu/ginsu.h"

namespace ginsu {
// This is the connection to the c_salt Module machinery.  The Module singleton
// calls CreateInstance to make new copies of each in-browser instance of
// Ginsu.
class GinsuModule : public c_salt::Module {
 public:
  virtual c_salt::Instance* CreateInstance(const NPP& npp_instance) {
    boost::shared_ptr<Ginsu> ginsu_ptr(new Ginsu(npp_instance));
    // This is slightly wacky;  Ginsu is both the instance and a
    // ScriptableNativeObject.  So we use it (as the instance) to create a
    // scripting bridge for itself.  The ScriptingBridge then takes ownership of
    // the new Ginsu instance.
    ginsu_ptr->CreateScriptingBridgeForObject(ginsu_ptr);
    return ginsu_ptr.get();
  }
};
}  // namespace ginsu

#endif  // GINSU_GINSU_MODULE_H_
