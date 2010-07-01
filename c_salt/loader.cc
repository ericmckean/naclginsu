// Copyright 2010 The Ginsu Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#include <nacl/npupp.h>
#include <pgl/pgl.h>

// These functions are called when module code is first loaded, and when the
// module code text gets unloaded.  They must use C-style linkage.

extern "C" {

NPError NP_GetEntryPoints(NPPluginFuncs* plugin_funcs) {
  // Defined in npp_gate.cc
  extern NPError InitializePepperGateFunctions(NPPluginFuncs* plugin_funcs);
  return InitializePepperGateFunctions(plugin_funcs);
}

NPError NP_Initialize(NPNetscapeFuncs* browser_functions,
                      NPPluginFuncs* plugin_functions) {
  NPError np_err = NP_GetEntryPoints(plugin_functions);
  if (NPERR_NO_ERROR == np_err) {
    // Perform any special initialization here.
    pglInitialize();
  }
  return np_err;
}
NPError NP_Shutdown() {
  // Perform any specialized shout-down procedures here, then delete the
  // singleton.
  pglTerminate();
  return NPERR_NO_ERROR;
}

}  // extern "C"
