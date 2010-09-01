// Copyright 2010 Google Inc. All Rights Reserved.
// Author: dmichael@google.com (Dave Michael)

#include "c_salt/npapi/npvariant_converter.h"

#include <cassert>
#include <cstring>
#include <sstream>

namespace c_salt {
namespace npapi {

void NPVariantConverter::operator() (NPVariant* var, const NPObject* value) {
  // Would like to use const NPObject*, but the macro doesn't support it.
  // Try to maintain const-correctness in PPAPI.
  NPObject* mutable_value = const_cast<NPObject*>(value);
  OBJECT_TO_NPVARIANT(mutable_value, *var);
}

void NPVariantConverter::operator() (NPVariant* var, const std::string& value) {
  const std::string::size_type str_length(value.length()+1);
  char* string_copy = reinterpret_cast<char*>(NPN_MemAlloc(str_length));
  std::strncpy(string_copy, value.c_str(), str_length);
  STRINGN_TO_NPVARIANT(string_copy, str_length-1, *var);
}

void NPVariantConverter::operator() (NPVariant* var, int32_t value) {
  INT32_TO_NPVARIANT(value, *var);
}

void NPVariantConverter::operator() (NPVariant* var, double value) {
  DOUBLE_TO_NPVARIANT(value, *var);
}

void NPVariantConverter::operator() (NPVariant* var, bool value) {
  BOOLEAN_TO_NPVARIANT(value, *var);
}

bool NPVariantConverter::operator() (NPObject** value, const NPVariant& var) {
  assert(value);
  if (NULL == value) return false;
  switch (var.type) {
    case NPVariantType_Void:
    case NPVariantType_Null: {
      *value = NULL;
      return true;
      break;
    }
    case NPVariantType_Object: {
      *value = NPVARIANT_TO_OBJECT(var);
      return true;
      break;
    }
    case NPVariantType_String:
    case NPVariantType_Bool:
    case NPVariantType_Int32:
    case NPVariantType_Double:
    default: {
      *value = NULL;
      return false;
    }
  }
  // Should never get here:
  return false;
}

bool NPVariantConverter::operator() (std::string* value, const NPVariant& var) {
  if (NULL == value) return false;
  switch (var.type) {
    case NPVariantType_Void:
    case NPVariantType_Null: {
      // In these cases, the default string we are given is okay, so we do
      // nothing to it, and return true.
      return true;
      break;
    }
    case NPVariantType_String: {
      NPString np_string(NPVARIANT_TO_STRING(var));
      *value = std::string(np_string.UTF8Characters, np_string.UTF8Length);
      return true;
    }
    case NPVariantType_Bool: {
      if (NPVARIANT_TO_BOOLEAN(var)) {
        *value = "true";
      } else {
        *value = "false";
      }
      return true;
    }
    case NPVariantType_Int32: {
      std::ostringstream stream;
      stream << NPVARIANT_TO_INT32(var);
      *value = stream.str();
      return true;
    }
    case NPVariantType_Double: {
      std::ostringstream stream;
      stream << NPVARIANT_TO_DOUBLE(var);
      *value = stream.str();
      return true;
    }
    // TODO(dmichael): Is there a way to support turning NPObject in to a
    // string?  Maybe sometimes?  Probably best to wait until Pepper v2 to
    // do that kind of thing.
    case NPVariantType_Object:
    default: {
      return false;
    }
  }
  // Should never get here:
  return false;
}

}  // namespace npapi
}  // namespace c_salt
