// Copyright 2010 The Ginsu Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#ifndef C_SALT_VARIANT_H_
#define C_SALT_VARIANT_H_

#include <string>

#include "boost/shared_ptr.hpp"
#include "boost/variant.hpp"
#include "c_salt/converting_visitor.h"

namespace c_salt {

class ScriptingBridge;

// A polymorphic type container.  You can always get the value as any of the
// intrisic types, no matter what the underlying storage actually is.  For
// example, if you create a Variant using the string "3.14159", you can get
// its double_value(), which will be 3.14159.  The value is immutable.  Note:
// making copies of Varaint instances using the copy constructor might cause a
// round trip to the browser in order to perform reference counting.

class Variant {
 public:

  typedef enum {
    kNullVariantType,
    kBoolVariantType,
    kInt32VariantType,
    kDoubleVariantType,
    kStringVariantType,
    kObjectVariantType
  } VariantType;

  // Create a new Variant with the corresponding underlying storage.  The caller
  // is responsible for deleting the resulting Type.
  Variant() : variant_type_(kNullVariantType) {}
  explicit Variant(bool bool_value)
      : variant_type_(kBoolVariantType), value_(bool_value) {}
  explicit Variant(int32_t int32_value)
      : variant_type_(kInt32VariantType), value_(int32_value) {}
  explicit Variant(double double_value)
      : variant_type_(kDoubleVariantType), value_(double_value) {}
  // Creates a deep copy of |string_value|, this object instance manages the
  // string's memory.
  explicit Variant(const std::string& string_value)
      : variant_type_(kStringVariantType), value_(string_value) {}
  explicit Variant(const char* string_value)
      : variant_type_(kStringVariantType), value_(string_value) {}
  // Hold a shared reference to |object_value|.
  explicit Variant(const boost::shared_ptr<ScriptingBridge>& object_value)
      : variant_type_(kObjectVariantType), value_(object_value) {}

  // TODO(dspringer,dmichael): It might be worthwhile to make a no-copy
  // version of the std::string ctor variant(), but that requires a lot more
  // thought.  For reference, see, e.g. CFStringCreateWithCharactersNoCopy()
  // here:
  // http://developer.apple.com/library/mac/#documentation/CoreFoundation
  //     /Reference/CFStringRef/Reference/reference.html
  // static Variant* CreateWithStringNoCopy(const std::string* string_value);

  // The dtor potentially does a round-trip to the browser in the event that
  // the underlying storage type is ref counted.
  virtual ~Variant();

  VariantType variant_type() const {
    return variant_type_;
  }

  // Always succeeds.  Conversion rules:
  //  Convert from higher precision to lower (e.g. double to int): the
  //    higher precision value is truncated towards 0.
  //  Convert number to string: the formatted equivalent of printf() with %d or
  //    %g is used.
  //  Convert bool to number: true -> 1, false -> 0.
  //  Convert number to bool: non-0 -> true, 0 within epsilon -> false.
  //  Convert bool to string: true -> "true", false -> "false".
  //  Convert string to bool: if the string starts with 't', 'y' or '1' (case
  //    insensitive) -> true; if it starts with anything else -> false.
  //  Convert string to number: any format recognized by strol() or strtod()
  //    (see the POSIX man mages for these functions for more details) is
  //    converted to a corresponding numeric value; strings that cannot be
  //    converted this way produce a 0 (the double value is exactly 0.0).
  template <class T>
  T GetValue() const {
    ConvertingVisitor<T> visitor;
    return boost::apply_visitor(visitor, value_);
  }

  // Convenience wrappers.
  virtual bool BoolValue() const {
    return GetValue<bool>();
  }
  virtual int32_t Int32Value() const {
    return GetValue<int32_t>();
  }
  virtual double DoubleValue() const {
    return GetValue<double>();
  }
  virtual std::string StringValue() const {
    return GetValue<std::string>();
  }
  // TODO(dspringer, dmichael): ObjectValue()?

  // TODO(dspringer, dmichael): Add other more specific accessors that give
  // more information about possible conversions and conversion errors.
  /*
  template <class T>
  T GetValue(bool* did_convert) const;
  */

 private:
  VariantType variant_type_;
  boost::variant<bool,
                 int32_t,
                 double,
                 std::string,
                 boost::shared_ptr<ScriptingBridge> > value_;
};

typedef boost::shared_ptr<Variant> SharedVariant;

}  // namespace c_salt

#endif  // C_SALT_VARIANT_H_
