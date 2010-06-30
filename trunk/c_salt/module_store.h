// Copyright 2010 The Native Client SDK Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#ifndef C_SALT_MODULE_STORE_
#define C_SALT_MODULE_STORE_

#include <nacl/nacl_npapi.h>
#include <nacl/npruntime.h>
#include <nacl/npapi_extensions.h>

#include "c_salt/basic_macros.h"

namespace c_salt {

class Module;

// Implements a singleton class that holds all the module instances, and maps
// the instances to NPP instances in the browser.  One of these is created
// when the .nexe is first loaded and destroyed when the last .nexe is removed.
class ModuleStore {
 public:
  // Return the singleton, creating it if needed.
  ModuleStore* SharedModuleStore();

  void AddModuleForInstance(Module* module, NPP instance);
  void RemoveModule(Module* module);

  NPP InstanceForModule(const Module* module) const;
  Module* ModuleForInstance(const NPP instance) const;

 private:
  DISALLOW_IMPLICIT_CONSTRUCTORS(ModuleStore);
};

}  // namespace c_salt

#endif  // C_SALT_MODULE_STORE_
