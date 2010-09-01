// Copyright 2010 Google Inc. All Rights Reserved.
// Author: dmichael@google.com (Dave Michael)

#ifndef C_SALT_NPAPI_NPVARIANT_CONVERTER_H_
#define C_SALT_NPAPI_NPVARIANT_CONVERTER_H_

#include <nacl/npruntime.h>

#include <sstream>
#include <string>

namespace c_salt {
namespace npapi {

// NPVariantConverter is a functor that knows how to convert NPVariants to and
// from C++ types.  Functions are of two basic forms:
//   (A)  void operator(NPVariant* var, CppType x);
//   (B)  bool operator(CppType* x, const NPVariant& var);
// Type (A) functions set var's type & value to match the value stored in x.
//   The type of var before the function call is irrelevant;  it is set to
//   whatever is appropriate based on CppType.
// Type (B) functions set the value of x to match the value pulled from var.
//   If var is not bound to the same type as CppType, then the function attempts
//   to convert the value of x to that type.  If there is no viable conversion,
//   the function returns false.
// The pattern is that the argument that will be changed is on the left, and
// the argument that is a pure in-parameter is on the right.
class NPVariantConverter {
 public:

  // Set var based on value.
  void operator() (NPVariant* var, const NPObject* value);
  void operator() (NPVariant* var, const std::string& value);
  void operator() (NPVariant* var, int32_t value);
  void operator() (NPVariant* var, double value);
  void operator() (NPVariant* var, bool value);

  // Set value to var, if var represents an NPObject*.  Otherwise, return false.
  // If value is null, return false.
  bool operator() (NPObject** value, const NPVariant& var);
  // Convert var to any numeric type (or, really, any type that supports
  // deserialization from istringstream will probably compile.
  // Return false if the conversion fails (e.g., "Hello world"->double should
  // fail and return false).
  // TODO(dmichael):  This function currently allows some conversions that are
  // unsafe, or may have some ambiguity.  E.g., signed->unsigned,
  // int32_t->char, bool->double.
  // If value is null, return false.
  template <class Number>
  bool operator() (Number* value, const NPVariant& var);
  // Convert var to value, which is a std::string.
  bool operator() (std::string* value, const NPVariant& var);
};

// Convert to any numeric type (or, really, any type that supports
// deserialization from istringstream, currently).
// TODO(dmichael): Maybe use NaCl's checked_cast or similar to ensure we don't
// allow unsafe conversions.
template <class Number>
bool NPVariantConverter::operator() (Number* value, const NPVariant& var) {
  if (NULL == value) return false;
  switch (var.type) {
    case NPVariantType_Void:
    case NPVariantType_Null: {
      *value = 0;
      return true;
      break;
    }
    case NPVariantType_String: {
      NPString np_string(NPVARIANT_TO_STRING(var));
      std::string str(np_string.UTF8Characters, np_string.UTF8Length);
      std::istringstream input_stream(str);
      if (input_stream >> *value) {
        return true;
      } else {
        return false;
      }
    }
    case NPVariantType_Bool: {
      *value = static_cast<Number>(NPVARIANT_TO_BOOLEAN(var));
      return true;
    }
    case NPVariantType_Int32: {
      *value = static_cast<Number>(NPVARIANT_TO_INT32(var));
      return true;
    }
    case NPVariantType_Double: {
      *value = static_cast<Number>(NPVARIANT_TO_DOUBLE(var));
      return true;
    }
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

#endif  // C_SALT_NPAPI_NPVARIANT_CONVERTER_H_
