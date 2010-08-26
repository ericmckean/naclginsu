// Copyright 2010 The Ginsu Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

// TODO(dspringer): This file will disappear when we migrate to Pepper V2.


#include <nacl/npupp.h>

#include "c_salt/instance.h"

using c_salt::Instance;
using c_salt::ScriptingBridge;

NPError NPP_New(NPMIMEType mime_type,
                NPP instance,
                uint16_t mode,
                int16_t argc,
                char* argn[],
                char* argv[],
                NPSavedData* saved) {
  extern void InitializePepperExtensions(NPP instance);
  if (instance == NULL) {
    return NPERR_INVALID_INSTANCE_ERROR;
  }

  InitializePepperExtensions(instance);

  // Build the attribute key/value map.
  // std::map<std::string, std::string> attribute_dict;
  // while (--argc) {
  //   attribute_dict[argn[argc]] = argv[argc];
  // }
  Instance* module_instance = Instance::CreateInstance(/* attribute_dict */);
  if (module_instance == NULL) {
    return NPERR_OUT_OF_MEMORY_ERROR;
  }
  instance->pdata = reinterpret_cast<void*>(module_instance);
  return NPERR_NO_ERROR;
}

NPError NPP_Destroy(NPP instance, NPSavedData** save) {
  if (instance == NULL) {
    return NPERR_INVALID_INSTANCE_ERROR;
  }
  Instance* module_instance = static_cast<Instance*>(instance->pdata);
  if (module_instance != NULL) {
    delete module_instance;
  }
  return NPERR_NO_ERROR;
}

// NPP_GetScriptableInstance returns the NPObject pointer that corresponds to
// NPPVpluginScriptableNPObject queried by NPP_GetValue() from the browser.
NPObject* NPP_GetScriptableInstance(NPP instance) {
  if (instance == NULL || instance->pdata == NULL) {
    return NULL;
  }

  Instance* module_instance = static_cast<Instance*>(instance->pdata);
  if (!module_instance) {
    return NULL;
  }
  return module_instance->CreateScriptingBridge(instance);
}

NPError NPP_GetValue(NPP instance, NPPVariable variable, void *value) {
  if (NPPVpluginScriptableNPObject == variable) {
    NPObject* scriptable_object = NPP_GetScriptableInstance(instance);
    if (scriptable_object == NULL)
      return NPERR_INVALID_INSTANCE_ERROR;
    *reinterpret_cast<NPObject**>(value) = scriptable_object;
    return NPERR_NO_ERROR;
  }
  return NPERR_INVALID_PARAM;
}

int16_t NPP_HandleEvent(NPP instance, void* event) {
  if (instance == NULL) {
    return 0;
  }
  Instance* module_instance = static_cast<Instance*>(instance->pdata);
  if (!module_instance)
    return 0;
  return module_instance->ReceiveEvent(
      *reinterpret_cast<const NPPepperEvent*>(event)) ? 1 : 0;
}

NPError NPP_SetWindow(NPP instance, NPWindow* window) {
  if (instance == NULL) {
    return NPERR_INVALID_INSTANCE_ERROR;
  }
  if (window == NULL) {
    return NPERR_GENERIC_ERROR;
  }
  Instance* module_instance = static_cast<Instance*>(instance->pdata);
  if (!module_instance)
    return NPERR_INVALID_INSTANCE_ERROR;
  // The first call to NPP_SetWindow indicates that the instance is all loaded
  // up and Pepper devices are ready for use.
  if (!module_instance->is_loaded()) {
    module_instance->InstanceDidLoad(instance, window->width, window->height);
    module_instance->set_is_loaded(true);
  }
  module_instance->WindowDidChangeSize(instance, window->width, window->height);
  return NPERR_NO_ERROR;
}

extern "C" {
NPError InitializePepperGateFunctions(NPPluginFuncs* plugin_funcs) {
  memset(plugin_funcs, 0, sizeof(*plugin_funcs));
  plugin_funcs->version = NPVERS_HAS_PLUGIN_THREAD_ASYNC_CALL;
  plugin_funcs->size = sizeof(*plugin_funcs);
  plugin_funcs->newp = NPP_New;
  plugin_funcs->destroy = NPP_Destroy;
  plugin_funcs->setwindow = NPP_SetWindow;
  plugin_funcs->event = NPP_HandleEvent;
  plugin_funcs->getvalue = NPP_GetValue;
  return NPERR_NO_ERROR;
}
}  // extern "C"
