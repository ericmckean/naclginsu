// Copyright 2010 The Ginsu Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#include "c_salt/string_type.h"

#include <cassert>
#include <cstdlib>
#include <cstring>

namespace c_salt {

StringType::StringType(const NPString& np_string)
    : Type(kStringTypeId),
      string_value_(static_cast<const char*>(np_string.UTF8Characters),
                    np_string.UTF8Length) {
}

StringType::~StringType() {
}

bool StringType::ConvertToNPVariant(NPVariant* np_var) const {
  assert(np_var);
  if (np_var == NULL)
    return false;
  NULL_TO_NPVARIANT(*np_var);
  uint32_t length = string_value_.size();
  NPUTF8* utf8_string = reinterpret_cast<NPUTF8*>(NPN_MemAlloc(length+1));
  memcpy(utf8_string, string_value_.c_str(), length);
  utf8_string[length] = '\0';
  STRINGN_TO_NPVARIANT(utf8_string, length, *np_var);
  return true;
}

const StringType::StringCompareResult StringType::Compare(
    const StringType& other_string) const {
  return Compare(other_string.string_value());
}

const StringType::StringCompareResult StringType::Compare(
    const std::string& other_string) const {
  // Use strncmp to guard against buffer overruns.
  const char* c_str = string_value_.c_str();
  const char* other_c_str = other_string.c_str();
  int compare_result = std::strncmp(c_str, other_c_str, string_value_.size());
  return compare_result < 0 ? kStringLexicallyLessThan :
         compare_result > 0 ? kStringLexicallyGreaterThan :
                              kStringLexicallyEqual;
}

bool StringType::bool_value() const {
  if (string_value_.size() == 0)
    return false;
  int ch = std::tolower(string_value_.at(0));
  return ch == 'y' || ch == '1' || ch == 't';
}

int32_t StringType::int32_value() const {
  return string_value_.size() ? std::atol(string_value_.c_str()) : 0;
}

double StringType::double_value() const {
  return string_value_.size() ? std::atof(string_value_.c_str()) : 0.0;
}

}  // namespace c_salt
