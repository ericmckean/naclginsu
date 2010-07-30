// Copyright 2010 The Native Client Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#ifndef C_SALT_STRING_TYPE_H_
#define C_SALT_STRING_TYPE_H_

#include "boost/scoped_ptr.hpp"
#include "c_salt/basic_macros.h"
#include "c_salt/type.h"

#include <string>

namespace c_salt {

class StringType : public Type {
 public:
  // Possible results from the Compare methods.
  typedef enum {
    kStringLexicallyEqual,
    kStringLexicallyLessThan,
    kStringLexicallyGreaterThan
  } StringCompareResult;

  // Ctor makes a copy of string in local memory.  If the string ever needs to
  // be sent back to the browser, use CreateNPStringCopy();
  explicit StringType(const NPString& np_string);
  virtual ~StringType();

  // Creates a copy of the internal string in the browser's memory.  The caller
  // is responsible for deleting the memory.  The string pointer must be freed
  // using NPN_MemFree().
  bool CreateNPVariantCopy(NPVariant& np_var) const;

  // Compare |other_string| with this one.
  // TODO(dspringer): Flesh this out to have anchoring, case-insensitivity,
  // etc.
  const StringCompareResult Compare(const StringType& other_string) const;
  const StringCompareResult Compare(const std::string& other_string) const;

  virtual bool bool_value() const;
  virtual int32_t int32_value() const;
  virtual double double_value() const;

 private:
  // Private accessor for the internal string, this is provided
  // TODO(dspringer): This seems dangerous and wrong, but short of wrapping
  // all of the std::string API this will have to do for now.
  const std::string& string_value() const {
    return string_value_;
  }

  std::string string_value_;
  DISALLOW_COPY_AND_ASSIGN(StringType);
};

}  // namespace c_salt

#endif  // C_SALT_STRING_TYPE_H_
