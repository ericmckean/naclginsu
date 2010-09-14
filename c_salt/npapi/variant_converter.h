// Copyright 2010 The Ginsu Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#ifndef C_SALT_NPAPI_VARIANT_CONVERTER_H_
#define C_SALT_NPAPI_VARIANT_CONVERTER_H_

#include <nacl/npruntime.h>

#include "c_salt/variant_ptrs.h"

// TODO(dmichael):  Add support for converting NPObject* to ScriptableObject
// #include "c_salt/npapi/browser_binding.h"

namespace c_salt {

namespace npapi {

// VariantConverter is a class that knows how to convert NPVariants to and
// from c_salt::Variants.
class VariantConverter {
 public:
  VariantConverter();

  SharedVariant CreateVariantFromNPVariant(const NPVariant& np_var) const;
  void ConvertVariantToNPVariant(const c_salt::Variant& c_salt_var,
                                 NPVariant* np_var) const;
 private:
  // Note there is no state right now, but I expect that we will need to have
  // the BrowserBinding to support converting NPObject* to ScriptableObject
};

}  // namespace npapi
}  // namespace c_salt

#endif  // C_SALT_NPAPI_VARIANT_CONVERTER_H_
