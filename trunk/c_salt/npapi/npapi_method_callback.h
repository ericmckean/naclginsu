// Copyright 2010 The Ginsu Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#ifndef C_SALT_NPAPI_NPAPI_METHOD_CALLBACK_H_
#define C_SALT_NPAPI_NPAPI_METHOD_CALLBACK_H_

#include <nacl/npruntime.h>

#include "c_salt/callback.h"
#include "c_salt/npapi/npvariant_converter.h"

namespace c_salt {
namespace npapi {

// Pure virtual class that provides the interface for invoking a method given
// NPAPI arguments.  Clients should generally not use this interface directly;
// it simply allows c_salt code to invoke methods generically.  See
// NPAPIMethodCallbackExecutorImpl (and related classes) below for further
// details on how this is achieved.
class NPAPIMethodCallbackExecutor {
 public:
  virtual ~NPAPIMethodCallbackExecutor() {}

  virtual bool Execute(const NPVariant* args,
                       uint32_t arg_count,
                       NPVariant* return_value,
                       NPP npp,
                       NPObject* window_object) = 0;
};

// NPAPIMethodCallbackExecutorImpl is a class template that implements the
// NPAPIMethodCallbackExecutor interface by calling an arbitrary boost::function
// and automatically handling marshalling/unmarshalling of the arguments and
// return type to bridge the gap between the NPAPI method invocation and the
// invocation of a real C++ method on a client-defined class.
template <class Signature>
class NPAPIMethodCallbackExecutorImpl : public NPAPIMethodCallbackExecutor {
 public:
  typedef typename ::c_salt::FunctionInvoker<Signature> FunctionInvokerType;

  template <class T>
  NPAPIMethodCallbackExecutorImpl(T* instance, Signature method)
      : function_invoker_(instance, method) {}
  virtual ~NPAPIMethodCallbackExecutorImpl() {}

  virtual bool Execute(const NPVariant* args,
                       uint32_t arg_count,
                       NPVariant* return_value,
                       NPP npp,
                       NPObject* window_object) {
    NPVariantConverter converter(npp, window_object);
    return function_invoker_.Invoke(args,
                                    args+arg_count,
                                    return_value,
                                    converter);
  }
 private:
  FunctionInvokerType function_invoker_;
};

}  // namespace npapi
}  // namespace c_salt

#endif  // C_SALT_NPAPI_NPAPI_METHOD_CALLBACK_H_
