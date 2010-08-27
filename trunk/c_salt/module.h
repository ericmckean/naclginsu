// Copyright 2010 The Ginsu Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#ifndef C_SALT_MODULE_H_
#define C_SALT_MODULE_H_

#include "boost/noncopyable.hpp"
#include "c_salt/scripting_bridge.h"

namespace c_salt {

// The base class for a c_salt-based Native Client module.  Pepper makes this
// a global object, and it exists as long as the module code is loaded.  This
// class is repsonsible for creating Instances (see c_salt/instance.h).

// TODO(dspringer): This becomes a subclass of pp::Module when Pepper v2
// becomes available.
class Instance;

class Module : public boost::noncopyable {
 public:
  // The Module is a singleton.
  static Module& GetModuleSingleton();
  static void CleanUp();

  // Create and return an instance of the module.  The subclass can create a
  // specific class instance.
  virtual Instance* CreateInstance() = 0;

 protected:
  Module();
  virtual ~Module();

 private:
  static Module* module_singleton_;
};

// Implemented by the module.  Creates the Module associated with this Native
// Client module.  This function is called once when the singleton Module gets
// created.
Module* CreateModule();

}  // namespace c_salt

#endif  // C_SALT_MODULE_H_
