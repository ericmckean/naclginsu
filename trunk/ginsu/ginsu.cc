// Copyright (c) 2010 The Ginsu Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ginsu/plugin.h"

#if __GNUC__ >= 4
#define EXPORT __attribute__ ((visibility("default")))
#else
#define EXPORT
#endif  // GNUC

namespace ginsu {
// NPP entry points.
NPError NPP_New(NPMIMEType pluginType,
                NPP instance,
                uint16 mode,
                int16 argc, char* argn[], char* argv[],
                NPSavedData* saved) {
  if (g_browser->version < NPVERS_HAS_PLUGIN_THREAD_ASYNC_CALL) {
    return NPERR_INCOMPATIBLE_VERSION_ERROR;
  }

  Plugin* plugin = static_cast<Plugin*>(
      g_browser->createobject(instance, Plugin::GetPluginClass()));
  instance->pdata = plugin;
  plugin->New(pluginType, argc, argn, argv);

  return NPERR_NO_ERROR;
}

NPError NPP_Destroy(NPP instance, NPSavedData** save) {
  Plugin* plugin = static_cast<Plugin*>(instance->pdata);
  if (plugin) g_browser->releaseobject(plugin);

  return NPERR_NO_ERROR;
}

NPError NPP_SetWindow(NPP instance, NPWindow* window) {
  Plugin* plugin = static_cast<Plugin*>(instance->pdata);
  if (plugin) plugin->SetWindow(*window);

  return NPERR_NO_ERROR;
}

NPError NPP_NewStream(NPP instance,
                      NPMIMEType type,
                      NPStream* stream,
                      NPBool seekable,
                      uint16* stype) {
  *stype = NP_ASFILEONLY;
  return NPERR_NO_ERROR;
}

NPError NPP_DestroyStream(NPP instance, NPStream* stream, NPReason reason) {
  return NPERR_NO_ERROR;
}

void NPP_StreamAsFile(NPP instance, NPStream* stream, const char* fname) {
}

int32 NPP_Write(NPP instance,
                NPStream* stream,
                int32 offset,
                int32 len,
                void* buffer) {
  return 0;
}

int32 NPP_WriteReady(NPP instance, NPStream* stream) {
  return 0;
}

void NPP_Print(NPP instance, NPPrint* platformPrint) {
}

int16 NPP_HandleEvent(NPP instance, void* event) {
  Plugin* plugin = static_cast<Plugin*>(instance->pdata);

  if (plugin)
    return plugin->HandleEvent(*static_cast<NPPepperEvent*>(event));

  return 0;
}

void NPP_URLNotify(NPP instance, const char* url, NPReason reason,
                   void* notify_data) {
}

NPError NPP_GetValue(NPP instance, NPPVariable variable, void* value) {
  NPError err = NPERR_NO_ERROR;

  switch (variable) {
#if defined(OS_LINUX)
    case NPPVpluginNameString:
      *(static_cast<const char**>(value)) = "Ginsu";
      break;
    case NPPVpluginDescriptionString:
      *(static_cast<const char**>(value)) =
          "3D modeling application in the web browser";
      break;
    case NPPVpluginNeedsXEmbed:
      *(static_cast<NPBool*>(value)) = TRUE;
      break;
#endif
    case NPPVpluginScriptableNPObject: {
      void** v = static_cast<void**>(value);
      Plugin* plugin = static_cast<Plugin*>(instance->pdata);
      // Return value is expected to be retained
      g_browser->retainobject(plugin);
      *v = plugin;
      break;
    }
    default:
      err = NPERR_GENERIC_ERROR;
      break;
  }

  return err;
}

NPError NPP_SetValue(NPP instance, NPNVariable variable, void* value) {
  return NPERR_GENERIC_ERROR;
}
}  // namespace ginsu

// NP entry points
extern "C" {
EXPORT NPError API_CALL NP_GetEntryPoints(NPPluginFuncs* plugin_funcs) {
  plugin_funcs->version = NPVERS_HAS_PLUGIN_THREAD_ASYNC_CALL;
  plugin_funcs->size = sizeof(plugin_funcs);
  plugin_funcs->newp = ginsu::NPP_New;
  plugin_funcs->destroy = ginsu::NPP_Destroy;
  plugin_funcs->setwindow = ginsu::NPP_SetWindow;
  plugin_funcs->newstream = ginsu::NPP_NewStream;
  plugin_funcs->destroystream = ginsu::NPP_DestroyStream;
  plugin_funcs->asfile = ginsu::NPP_StreamAsFile;
  plugin_funcs->writeready = ginsu::NPP_WriteReady;
  plugin_funcs->write = ginsu::NPP_Write;
  plugin_funcs->print = ginsu::NPP_Print;
  plugin_funcs->event = ginsu::NPP_HandleEvent;
  plugin_funcs->urlnotify = ginsu::NPP_URLNotify;
  plugin_funcs->getvalue = ginsu::NPP_GetValue;
  plugin_funcs->setvalue = ginsu::NPP_SetValue;

  return NPERR_NO_ERROR;
}

EXPORT NPError API_CALL NP_Initialize(NPNetscapeFuncs* browser_funcs
#if defined(OS_LINUX)
                                     , NPPluginFuncs* plugin_funcs
#endif  // OS_LINUX
                                     ) {
  ginsu::g_browser = browser_funcs;
  pglInitialize();

#if defined(OS_LINUX)
  NP_GetEntryPoints(plugin_funcs);
#endif  // OS_LINUX
  return NPERR_NO_ERROR;
}

EXPORT void API_CALL NP_Shutdown() {
  pglTerminate();
}

#if defined(OS_LINUX)
EXPORT NPError API_CALL NP_GetValue(NPP instance, NPPVariable variable,
                                    void* value) {
  return ginsu::NPP_GetValue(instance, variable, value);
}

EXPORT char* API_CALL NP_GetMIMEDescription() {
  return "pepper-application/ginsu:3D modeling application";
}
#endif  // OS_LINUX
}  // extern "C"
