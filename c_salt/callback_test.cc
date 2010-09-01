// Copyright 2010 Google Inc. All Rights Reserved.
// Author: dmichael@google.com (Dave Michael)
#include "c_salt/callback.h"

#include <nacl/npruntime.h>

#include <iostream>
#include <string>

#include "c_salt/instance.h"
#include "c_salt/module.h"
#include "c_salt/npapi/npvariant_converter.h"
#include "c_salt/npapi/npapi_method_callback.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace c_salt;  // NOLINT
using namespace c_salt::npapi;  // NOLINT

namespace {
  NPObject* const kInvalidNPObjectPointer =
                           reinterpret_cast<NPObject*>(0xdeadbeef);
  template <class T>
  struct ValueMaker;

  template <>
  struct ValueMaker<std::string> {
    static std::string Make() { return "Hello World!"; }
  };
  template <>
  struct ValueMaker<int32_t> {
    static int Make() { return 1ul; }
  };
  template <>
  struct ValueMaker<double> {
    static double Make() { return 3.14; }
  };
  template <>
  struct ValueMaker<bool> {
    static bool Make() { return true; }
  };
  template <>
  struct ValueMaker<NPObject*> {
    static NPObject* Make() {
      return kInvalidNPObjectPointer;
    }
  };

  struct NPArgMakerBase {
    virtual ~NPArgMakerBase() {}
    NPArgMakerBase() : args(NULL), num_args(0) {}
    NPVariant* args;
    uint32_t num_args;
    NPVariantConverter converter;
  };
  template <class Signature>
  struct NPArgMaker : public NPArgMakerBase {};

  template <class RET, class ARG1>
  struct NPArgMaker<RET(ARG1)> : public NPArgMakerBase {
    explicit NPArgMaker(ARG1 a1) {
      num_args = 1u;
      args = new NPVariant[1];
      converter(args, a1);
    }
  };
  template <class RET, class ARG1, class ARG2>
  struct NPArgMaker<RET(ARG1, ARG2)> : public NPArgMakerBase {
    NPArgMaker(ARG1 a1, ARG2 a2) {
      num_args = 2u;
      args = new NPVariant[2];
      converter(args, a1);
      converter(args+1, a2);
    }
  };
  template <class RET, class ARG1, class ARG2, class ARG3>
  struct NPArgMaker<RET(ARG1, ARG2, ARG3)> : public NPArgMakerBase {
    NPArgMaker(ARG1 a1, ARG2 a2, ARG3 a3) {
      num_args = 3u;
      args = new NPVariant[3];
      converter(args, a1);
      converter(args+1, a2);
      converter(args+2, a3);
    }
  };
  template <class RET, class ARG1, class ARG2, class ARG3, class ARG4>
  struct NPArgMaker<RET(ARG1, ARG2, ARG3, ARG4)> : public NPArgMakerBase {
    NPArgMaker(ARG1 a1, ARG2 a2, ARG3 a3, ARG4 a4) {
      num_args = 4u;
      args = new NPVariant[4];
      converter(args, a1);
      converter(args+1, a2);
      converter(args+2, a3);
      converter(args+3, a4);
    }
  };

  template <class Signature>
  struct NPInvoker;

  template <class RET>
  struct NPInvoker<RET(*)()> {
    static RET InvokeWithArgs(NPAPIMethodCallbackExecutor* mce) {
      NPVariant retval;
      NPVariant* retval_ptr(&retval);
      NPArgMaker<RET()> arg_maker;
      mce->Execute(arg_maker.args, arg_maker.num_args, &retval);
      RET real_retval;
      NPVariantConverter converter;
      converter(&real_retval, retval);
      return real_retval;
    }
    static void Invoke(NPAPIMethodCallbackExecutor* mce) {
      std::cout << "Returned " << InvokeWithArgs(mce) << std::endl;
    }
  };
  template <class RET, class ARG1>
  struct NPInvoker<RET(*)(ARG1)> {
    static RET InvokeWithArgs(NPAPIMethodCallbackExecutor* mce,
                              ARG1 a1) {
      NPVariant retval;
      NPVariant* retval_ptr(&retval);
      NPArgMaker<RET(ARG1)>arg_maker(a1);
      mce->Execute(arg_maker.args, arg_maker.num_args, &retval);
      RET real_retval;
      NPVariantConverter converter;
      converter(&real_retval, retval);
      return real_retval;
    }
    static void Invoke(NPAPIMethodCallbackExecutor* mce) {
      std::cout << "Returned " << InvokeWithArgs(mce,
                                                 ValueMaker<ARG1>::Make())

          << std::endl;
    }
  };
  template <class RET, class ARG1, class ARG2>
  struct NPInvoker<RET(*)(ARG1, ARG2)> {
    static RET InvokeWithArgs(NPAPIMethodCallbackExecutor* mce,
                              ARG1 a1,
                              ARG2 a2) {
      NPVariant retval;
      NPVariant* retval_ptr(&retval);
      NPArgMaker<RET(ARG1, ARG2)>arg_maker(a1, a2);
      mce->Execute(arg_maker.args, arg_maker.num_args, &retval);
      RET real_retval;
      NPVariantConverter converter;
      converter(&real_retval, retval);
      return real_retval;
    }
    static void Invoke(NPAPIMethodCallbackExecutor* mce) {
      std::cout << "Returned " << InvokeWithArgs(mce,
                                                 ValueMaker<ARG1>::Make(),
                                                 ValueMaker<ARG2>::Make())
          << std::endl;
    }
  };
  template <class RET, class ARG1, class ARG2, class ARG3>
  struct NPInvoker<RET(*)(ARG1, ARG2, ARG3)> {
    static RET InvokeWithArgs(NPAPIMethodCallbackExecutor* mce,
                              ARG1 a1,
                              ARG2 a2,
                              ARG3 a3) {
      NPVariant retval;
      NPVariant* retval_ptr(&retval);
      NPArgMaker<RET(ARG1, ARG2, ARG3)>arg_maker(a1, a2, a3);
      mce->Execute(arg_maker.args, arg_maker.num_args, &retval);
      RET real_retval;
      NPVariantConverter converter;
      converter(&real_retval, retval);
      return real_retval;
    }
    static void Invoke(NPAPIMethodCallbackExecutor* mce) {
      std::cout << "Returned " << InvokeWithArgs(mce,
                                                 ValueMaker<ARG1>::Make(),
                                                 ValueMaker<ARG2>::Make(),
                                                 ValueMaker<ARG3>::Make())
          << std::endl;
    }
  };
  template <class RET, class ARG1, class ARG2, class ARG3, class ARG4>
  struct NPInvoker<RET(*)(ARG1, ARG2, ARG3, ARG4)> {
    static RET InvokeWithArgs(NPAPIMethodCallbackExecutor* mce,
                              ARG1 a1,
                              ARG2 a2,
                              ARG3 a3,
                              ARG4 a4) {
      NPVariant retval;
      NPVariant* retval_ptr(&retval);
      NPArgMaker<RET(ARG1, ARG2, ARG3, ARG4)>arg_maker(a1, a2, a3, a4);
      mce->Execute(arg_maker.args, arg_maker.num_args, &retval);
      RET real_retval;
      NPVariantConverter converter;
      converter(&real_retval, retval);
      return real_retval;
    }
    static void Invoke(NPAPIMethodCallbackExecutor* mce) {
      std::cout << "Returned " << InvokeWithArgs(mce,
                                                 ValueMaker<ARG1>::Make(),
                                                 ValueMaker<ARG2>::Make(),
                                                 ValueMaker<ARG3>::Make(),
                                                 ValueMaker<ARG4>::Make())
          << std::endl;
    }
  };

  template <class Signature>
  struct CallbackMock;

  template <class RET>
  struct CallbackMock<RET(*)()> {
    RET Func() {
      std::cout << "CallbackMock (0 args) invoked.\n";
      return ValueMaker<RET>::Make();
    }
  };
  template <class RET, class ARG1>
  struct CallbackMock<RET(*)(ARG1)> {
    RET Func(ARG1 a1) {
      std::cout << "CallbackMock (1 args) invoked with "
                << a1
                << ".\n";
      return ValueMaker<RET>::Make();
    }
  };
  template <class RET, class ARG1, class ARG2>
  struct CallbackMock<RET(*)(ARG1, ARG2)> {
    RET Func(ARG1 a1, ARG2 a2) {
      std::cout << "CallbackMock (2 args) invoked with "
                << a1
                << ", "
                << a2
                << ".\n";
      return ValueMaker<RET>::Make();
    }
  };
  template <class RET, class ARG1, class ARG2, class ARG3>
  struct CallbackMock<RET(*)(ARG1, ARG2, ARG3)> {
    RET Func(ARG1 a1, ARG2 a2, ARG3 a3) {
      std::cout << "CallbackMock (3 args) invoked with "
                << a1
                << ", "
                << a2
                << ", "
                << a3
                << ".\n";
      return ValueMaker<RET>::Make();
    }
  };
  template <class RET, class ARG1, class ARG2, class ARG3, class ARG4>
  struct CallbackMock<RET(*)(ARG1, ARG2, ARG3, ARG4)> {
    RET Func(ARG1 a1, ARG2 a2, ARG3 a3, ARG4 a4) {
      std::cout << "CallbackMock (4 args) invoked with "
                << a1
                << ", "
                << a2
                << ", "
                << a3
                << ", "
                << a4
                << ".\n";
      return ValueMaker<RET>::Make();
    }
  };
}

// A helper function that creates an appropriate NPAPIMethodCallbackExecutor
// that, when invoked via its NPAPI-based Execute function, will convert the
// call in to an appropriate application-level native C++ call.
template <class T, class Signature>
NPAPIMethodCallbackExecutor* MakeNPAPIMethodCallbackExecutor(T* target,
                                                   Signature method) {
  return new NPAPIMethodCallbackExecutorImpl<Signature>(target,
                                                                  method);
}

#define TEST_SIGNATURE(SIGTYPE) \
{\
  CallbackMock<SIGTYPE> cb;\
  NPAPIMethodCallbackExecutor* mce(\
         MakeNPAPIMethodCallbackExecutor(&cb, \
                                    &CallbackMock<SIGTYPE>::Func));\
  NPInvoker<SIGTYPE>::Invoke(mce);\
  delete mce;\
}

class CallbackTest : public ::testing::Test {
 protected:
  virtual void SetUp() {}
};

TEST_F(CallbackTest, Signatures) {
  // TODO(dmichael): This is cheating...  for now, I just want to see it's
  // working but I need to turn it in to a real automated test.
  // Note that lint thinks these are function declarations and expects to see
  // parameter names, but they are in fact just function pointer types, where
  // parameters would not make sense.
  TEST_SIGNATURE(std::string(*)());  // NOLINT
  TEST_SIGNATURE(int32_t(*)(double));  // NOLINT
  TEST_SIGNATURE(double(*)(bool, int32_t));  // NOLINT
  TEST_SIGNATURE(bool(*)(std::string, double, int32_t, bool));  // NOLINT
  TEST_SIGNATURE(bool(*)(NPObject*, int32_t, double, bool));  // NOLINT
  // void return not currently supported
  // TEST_SIGNATURE(void(*)(std::string, double, int32_t, bool));
  //
}

//  Try passing parameters other than the expected ones.
TEST_F(CallbackTest, InvokeWithUnexpectedParameters) {
  // Lint thinks these are C-style casts, but they're not.  They're typedefs
  // of function pointer types.
  typedef bool(*Signature)(std::string, int32_t, double, bool);  // NOLINT
  CallbackMock<Signature> mock;
  NPAPIMethodCallbackExecutor* mce(
      MakeNPAPIMethodCallbackExecutor(&mock,
                                      &CallbackMock<Signature>::Func));
  typedef bool(*IntSignature)(int32_t, int32_t, int32_t, int32_t);  //NOLINT
  NPInvoker<IntSignature>::Invoke(mce);  // NOLINT
  typedef bool(*DoubleSignature)(double, double, double, double);  //NOLINT
  NPInvoker<DoubleSignature>::Invoke(mce);
  typedef bool(*BoolSignature)(bool, bool, bool, bool);  //NOLINT
  NPInvoker<BoolSignature>::Invoke(mce);
}

//  Try const-ref parameters, and passing types other than the expected ones.
TEST_F(CallbackTest, InvokeWithConstRefParameters) {
  // Lint thinks these are C-style casts, but they're not.  They're typedefs
  // of function pointer types.
  typedef bool(*Signature)(const std::string&, // NOLINT
                           const int32_t&,
                           const double&,
                           const bool&);
  CallbackMock<Signature> mock;
  NPAPIMethodCallbackExecutor* mce(
      MakeNPAPIMethodCallbackExecutor(&mock,
                                 &CallbackMock<Signature>::Func));
  typedef bool(*IntSignature)(int32_t, int32_t, int32_t, int32_t);  //NOLINT
  NPInvoker<IntSignature>::Invoke(mce);  // NOLINT
  typedef bool(*DoubleSignature)(double, double, double, double);  //NOLINT
  NPInvoker<DoubleSignature>::Invoke(mce);
  typedef bool(*BoolSignature)(bool, bool, bool, bool);  //NOLINT
  NPInvoker<BoolSignature>::Invoke(mce);
}

class MyInstance : public c_salt::Instance {
 public:
  explicit MyInstance(const NPP& instance) : c_salt::Instance(instance) {}
};

// This is the connection to the c_salt Module machinery.  The Module singleton
// calls CreateInstance to make new copies of each in-browser instance of
// Ginsu.
class TestModule : public c_salt::Module {
 public:
  virtual c_salt::Instance* CreateInstance(const NPP& instance) {
    return new MyInstance(instance);
  }
};

namespace c_salt {
Module* CreateModule() {
  return new TestModule();
}
}  // namespace c_salt
