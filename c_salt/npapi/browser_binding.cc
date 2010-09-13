// Copyright 2010 The Ginsu Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#include "c_salt/npapi/browser_binding.h"

#include "c_salt/instance.h"

namespace c_salt {
namespace npapi {

bool BrowserBinding::HasMethod(NPIdentifier name) const {
  ScopedNPIdToStringConverter np_str(name);
  return scripting_bridge_->HasScriptMethod(np_str.string_value());
}

void BrowserBinding::Invalidate() {
  return scripting_bridge_->Invalidate();
}

bool BrowserBinding::Invoke(NPIdentifier name,
                            const NPVariant* args,
                            uint32_t arg_count,
                            NPVariant* return_value) {
  ScopedNPIdToStringConverter np_str(name);
  return scripting_bridge_->InvokeScriptMethod(np_str.string_value(),
                                               args,
                                               arg_count,
                                               return_value);
}

bool BrowserBinding::HasProperty(NPIdentifier name) const {
  ScopedNPIdToStringConverter np_str(name);
  return scripting_bridge_->HasScriptProperty(np_str.string_value());
}

bool BrowserBinding::GetProperty(NPIdentifier name, NPVariant* return_value)
    const {
  ScopedNPIdToStringConverter np_str(name);
  SharedVariant value(new Variant());
  bool success = scripting_bridge_->GetScriptProperty(np_str.string_value(),
                                                      value);
  ConvertVariantToNPVariant(*value, return_value);
  return success;
}

bool BrowserBinding::SetProperty(NPIdentifier name,
                                 const NPVariant& np_value) {
  ScopedNPIdToStringConverter np_str(name);
  SharedVariant value(CreateVariantFromNPVariant(np_value));
  return scripting_bridge_->SetScriptProperty(np_str.string_value(), value);
}

bool BrowserBinding::RemoveProperty(NPIdentifier name) {
  ScopedNPIdToStringConverter np_str(name);
  return scripting_bridge_->RemoveScriptProperty(np_str.string_value());
}

void BrowserBinding::ConvertVariantToNPVariant(const Variant& value,
                                               NPVariant* np_value) const {
  switch (value.variant_type()) {
  case Variant::kNullVariantType:
    NULL_TO_NPVARIANT(*np_value);
    break;
  case Variant::kBoolVariantType:
    BOOLEAN_TO_NPVARIANT(value.BoolValue(), *np_value);
    break;
  case Variant::kInt32VariantType:
    INT32_TO_NPVARIANT(value.Int32Value(), *np_value);
    break;
  case Variant::kDoubleVariantType:
    DOUBLE_TO_NPVARIANT(value.DoubleValue(), *np_value);
    break;
  case Variant::kStringVariantType:
    {
      std::string str_val = value.StringValue();
      uint32_t length = str_val.size();
      NPUTF8* utf8_string = reinterpret_cast<NPUTF8*>(NPN_MemAlloc(length+1));
      memcpy(utf8_string, str_val.c_str(), length);
      utf8_string[length] = '\0';
      STRINGN_TO_NPVARIANT(utf8_string, length, *np_value);
    }
    break;
  case Variant::kObjectVariantType:
    // NOTIMPLEMENTED
    NULL_TO_NPVARIANT(*np_value);
    break;
  default:
    // NOTREADCHED
    assert(false);
    break;
  }
}

Variant* BrowserBinding::CreateVariantFromNPVariant(
    const NPVariant& np_value) const {
  Variant* value = NULL;
  switch (np_value.type) {
  case NPVariantType_Null:
  case NPVariantType_Void:
    value = new Variant();
    break;
  case NPVariantType_Bool:
    value = new Variant(np_value.value.boolValue);
    break;
  case NPVariantType_Int32:
    value = new Variant(np_value.value.intValue);
    break;
  case NPVariantType_Double:
    value = new Variant(np_value.value.doubleValue);
    break;
  case NPVariantType_String:
    {
      const NPString& np_string = np_value.value.stringValue;
      std::string string_value(
          static_cast<const char*>(np_string.UTF8Characters),
                                   np_string.UTF8Length);
      value = new Variant(string_value);
    }
    break;
  case NPVariantType_Object:
    // NOTIMPLEMENTED
    value = NULL;
    break;
  default:
    break;
  }
  return value;
}

// Helper functions for dereferencing the bridging object.
static inline BrowserBinding* cast_browser_binding(NPObject* np_object) {
  assert(np_object);
  return static_cast<BrowserBinding*>(np_object);
}

// The browser-facing entry points that represent the bridge's class methods.
// These are the function wrappers that the browser actually calls.
NPObject* Allocate(NPP npp, NPClass* npclass) {
  return new BrowserBinding(npp);
}

void Deallocate(NPObject* object) {
  delete cast_browser_binding(object);
}

void Invalidate(NPObject* object) {
  cast_browser_binding(object)->Invalidate();
}

bool HasMethod(NPObject* object, NPIdentifier name) {
  return cast_browser_binding(object)->HasMethod(name);
}

bool Invoke(NPObject* object, NPIdentifier name,
            const NPVariant* args,
            uint32_t arg_count,
            NPVariant* return_value) {
  return cast_browser_binding(object)->Invoke(
      name, args, arg_count, return_value);
}

bool HasProperty(NPObject* object, NPIdentifier name) {
  return cast_browser_binding(object)->HasProperty(name);
}

bool GetProperty(NPObject* object, NPIdentifier name, NPVariant* result) {
  return cast_browser_binding(object)->GetProperty(name, result);
}

bool SetProperty(NPObject* object, NPIdentifier name, const NPVariant* value) {
  return cast_browser_binding(object)->SetProperty(name, *value);
}

bool RemoveProperty(NPObject* object, NPIdentifier name) {
  return cast_browser_binding(object)->RemoveProperty(name);
}

void BrowserBinding::Retain() {
  NPN_RetainObject(this);
}

static NPClass bridge_class = {
  NP_CLASS_STRUCT_VERSION,
  c_salt::npapi::Allocate,
  c_salt::npapi::Deallocate,
  c_salt::npapi::Invalidate,
  c_salt::npapi::HasMethod,
  c_salt::npapi::Invoke,
  NULL,  // InvokeDefault is not implemented
  c_salt::npapi::HasProperty,
  c_salt::npapi::GetProperty,
  c_salt::npapi::SetProperty,
  c_salt::npapi::RemoveProperty
};

BrowserBinding* BrowserBinding::CreateBrowserBinding(
    const c_salt::Instance& instance) {
  return static_cast<BrowserBinding*>(
      NPN_CreateObject(instance.npp_instance(), &bridge_class));
}

}  // namespace npapi
}  // namespace c_salt
