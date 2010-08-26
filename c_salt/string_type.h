// Copyright 2010 The Native Client Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#ifndef C_SALT_STRING_TYPE_H_
#define C_SALT_STRING_TYPE_H_

#include <string>

#include "boost/noncopyable.hpp"
#include "c_salt/type.h"

namespace c_salt {

class StringType : public Type, public boost::noncopyable {
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

  // Creates a copy of the internal string in the browser's memory which is
  // referenced by |np_var|.  The caller is responsible for deleting the memory
  // associated with |np_var| using NPN_MemFree().  If |np_var| is NULL, this
  // method does nothing and returns |false|.
  bool CreateNPVariantCopy(NPVariant* np_var) const;

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
};

}  // namespace c_salt

#endif  // C_SALT_STRING_TYPE_H_
