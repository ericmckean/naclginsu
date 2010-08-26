// Copyright (c) 2010 The Ginsu Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GINSU_GINSU_H_
#define GINSU_GINSU_H_

#include <time.h>
#include <nacl/nacl_npapi.h>
#include <nacl/npruntime.h>
#include <nacl/npapi_extensions.h>
#include <pgl/pgl.h>

#include <map>
#include <string>

#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include "c_salt/instance.h"
#include "c_salt/type.h"

namespace ginsu {

namespace model {
class Model;
}  // namespace model
namespace view {
class View;
}  // namespace view

// The main class for the Ginsu application.
class Ginsu : public c_salt::Instance {
 public:
  Ginsu();
  virtual ~Ginsu();

  // Specialization of c_salt::Instance informal protocol.
  virtual bool InstanceDidLoad(const NPP instance, int width, int height);
  virtual void WindowDidChangeSize(const NPP instance, int width, int height);
  virtual void InitializeMethods(c_salt::ScriptingBridge* bridge);

  // Exposed to the browser via the ScriptingBridge.  The ScriptingBridge
  // classes don't handle const params yet.
  // TODO(dspringer): change to const std::string& when this is available.
  int32_t GetValueForKey(std::string key);
  // Sets |value| for the property named |key|, but only if |key| already
  // exists in the property dictionary.
  bool SetValueForKey(std::string key, int32_t value);

 private:
  static void RepaintCallback(NPP npp, NPDeviceContext3D* context);
  static void TickCallback(void* data);
  void Paint();
  void Tick();

  // Returns true if anything was updated.
  bool UpdateAnimation();

  // The keys in this map are available to the Browser via Get|SetValueForKey().
  // The values are c_salt::Types, currently they are marshaled explicitly as
  // return types to the browser depending on the key value.
  typedef boost::shared_ptr<c_salt::Type> SharedTypePtr;
  typedef std::map<std::string, SharedTypePtr> PropertyDictionary;
  PropertyDictionary property_dictionary_;

  // TODO(dspringer): Move this into a c_salt View3D class.
  void CreateContext(const NPP instance);
  void DestroyContext(const NPP instance);
  NPP npp_instance_;  // Weak reference.
  NPDevice* device3d_;
  NPDeviceContext3D context3d_;
  PGLContext pgl_context_;

  boost::scoped_ptr<model::Model> model_;
  boost::scoped_ptr<view::View> view_;

  double last_update_;  // Time measured in milliseconds.
};

}  // namespace ginsu

#endif  // GINSU_GINSU_H_
