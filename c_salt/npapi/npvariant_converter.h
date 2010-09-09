// Copyright 2010 The Ginsu Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#ifndef C_SALT_NPAPI_NPVARIANT_CONVERTER_H_
#define C_SALT_NPAPI_NPVARIANT_CONVERTER_H_

#include <nacl/npruntime.h>

#include <cstring>
#include <map>
#include <sstream>
#include <string>
#include <vector>

//#include "c_salt/npapi/browser_binding.h"

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
  explicit NPVariantConverter(NPP instance, NPObject* window_object);

  // Set var based on value.
  void operator() (NPVariant* var, const NPObject* value);
  void operator() (NPVariant* var, const std::string& value);
  void operator() (NPVariant* var, int32_t value);
  void operator() (NPVariant* var, double value);
  void operator() (NPVariant* var, bool value);
  template <class T>
  void operator() (NPVariant* var, const std::vector<T>& vec);
  // TODO(dmichael): Add map support.
  // template <class T>
  // void operator() (NPVariant* var, const std::map<T>& dictionary);

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
  template <class T>
  bool operator() (std::vector<T>* vec, const NPVariant& var);
  // TODO(dmichael): Add map support.
  // template <class T>
  // bool operator() (std::map<T>* dictionary, const NPVariant& var);

 private:
  // TODO(dmichael):  Change to BrowserBinding (when it includes WindowObject).
  NPP instance_;
  NPObject* window_object_;
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

// Convert a given vector in to an NPVariant.  The vector must consist of types
// that are themselves convertible (e.g., int32_t, or vector<bool>, etc).
template <class T>
void NPVariantConverter::operator() (NPVariant* var,
                                     const std::vector<T>& vec) {
  // Initialize the return value.
  VOID_TO_NPVARIANT(*var);
  // Ask the browser to create a new JavaScript array object.
  NPVariant variant;
  NPString npstr;
  npstr.UTF8Characters = "new Array();";
  npstr.UTF8Length = static_cast<uint32_t>(std::strlen(npstr.UTF8Characters));
  if (!NPN_Evaluate(instance_, window_object_, &npstr, &variant) ||
      !NPVARIANT_IS_OBJECT(variant)) {
    return;
  }
  // Set the properties, each array subscript has its own property id.
  NPObject* array_object = NPVARIANT_TO_OBJECT(variant);
  if (array_object) {
    typename std::vector<T>::const_iterator iter(vec.begin()),
                                            the_end(vec.end());
    for (int32_t index = 0; iter != the_end; ++iter, ++index) {
      NPVariant array_value;
      // call the appropriate operator() to convert the object to a variant
      (*this)(&array_value, *iter);
      NPN_SetProperty(instance_,
                      array_object,
                      NPN_GetIntIdentifier(index),
                      &array_value);
    }
    (*this)(var, array_object);
    return;
  }
  return;  // TODO(dmichael):  How to deal with errors here?  Does it matter?
}

// Convert the NPVariant in to a vector.  The NPVariant var must be an NPObject.
// If var represents a JavaScript array with contiguous indices [0, n-1), then
// it will be marshalled in to vector of size n exactly as expected.
//
// Behavior in special cases:
// If var contains any non-integer identifiers, they will be ignored.  Any
// indices in the range [0, max_index] (where max_index is the greatest index
// in the var array) which don't appear as an identifier in the JavaScript array
// will have default-initialized objects in the resultant vector.
template <class T>
bool NPVariantConverter::operator() (std::vector<T>* vec,
                                     const NPVariant& var) {
  // Unpack the array object.  This is done by enumerating the identifiers on
  // the array; the identifiers are the array subscripts.
  bool success = false;
  NPIdentifier* identifier_array = NULL;
  uint32_t element_count = 0;
  // Convert the var in to an NPObject
  NPObject* array_object = NULL;
  (*this)(&array_object, var);
  if (NPN_Enumerate(instance_,
                    array_object,
                    &identifier_array,
                    &element_count)) {
    // For any object whose identifier maps to an integer properly (if this was
    // really an array used properly, that should always be the case), we will
    // place that property in the vector at that index.  Properties that do not
    // have an integral identifier will not appear in the vector.  Any "holes"
    // in the vector (indices for which there is not identifier/property) will
    // be default initialized.  If we encounter identifiers >= element_count,
    // we will grow the vector so that we can place them at the appropriate
    // index.
    //
    uint32_t max_index = -1;  // The biggest index we have seen so far.
    // The likely case is that the identifiers go from [0..element_count), so
    // we will size the vector appropriately.  Note however that we may find
    // that the vector does not contain all those identifiers, so before we are
    // done, we will resize to the max_index + 1.
    std::vector<T> vec_temp(element_count);
    for (uint32_t j = 0; j < element_count; ++j) {
      // NPN_Enumerate enumerates identifiers for properties AND methods.
      // Furthermore, the identifiers may be strings or integers.  We only want
      // identifiers for properties which are also integers.  So for each
      // identifier, only add it if it is an integer (i.e., not a string) and
      // if it refers to a property.
      if ((NPN_HasProperty(instance_, array_object, identifier_array[j]))
          && (!NPN_IdentifierIsString(identifier_array[j]))) {
        int32_t real_index = NPN_IntFromIdentifier(identifier_array[j]);
        if (real_index >= 0) {
          NPVariant array_elem;
          VOID_TO_NPVARIANT(array_elem);
          if (NPN_GetProperty(instance_,
                              array_object,
                              identifier_array[j],
                              &array_elem)) {
            // If the vector is not yet big enough to contain this index, then
            // grow it.  This should only happen if the JavaScript array
            // has gaps or doesn't start from 0.
            if (real_index >= vec_temp.size()) {
              vec_temp.resize(real_index + 1);  // To contain index i, we need
                                                 // to cover the range 0..i,
                                                 // which has i+1 elements.
            }
            // Call the appropriate operator() to convert to the vector
            // element.
            // Note we have to use a temporary here to handle bools, since
            // vector<bool>::operator[] doesn't actually return a bool&.
            // TODO(dmichael):  We can optimize this to put values straight in
            // to the vector for all other types with a little template
            // metaprogramming.
            T temp;
            // (*this)(&(vec_temp[real_index]), array_elem);  // <-what we could
                                   // do if vector<bool> wasn't a specialization
            (*this)(&temp, array_elem);
            vec_temp[real_index] = temp;
          }  // end if NPN_GetProperty succeeds
        }  // end if real_index is non-negative
      }  // end if identifier_array[j] is integral and represents a property
    }  // end for each identifier
    // Now we have converted all the identifiers we could in to the vector
    // successfully.  (Note:  May want to eventually account for bad indices
    // and failed element conversions with some kind of conversion-exception
    // handler hook.)
    // Swap our temporary vector with the real out-parameter vector, now that
    // everything has succeeded.  This ensures that if there was any failure
    // to convert, the original vector we were passed remains unmodified.
    vec->swap(vec_temp);
    success = true;
    NPN_MemFree(identifier_array);
  }  // end if NPN_Enumerate succeeded
  return success;
}

}  // namespace npapi
}  // namespace c_salt

#endif  // C_SALT_NPAPI_NPVARIANT_CONVERTER_H_
