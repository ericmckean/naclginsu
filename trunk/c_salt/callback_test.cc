// Copyright 2010 Google Inc. All Rights Reserved.
// Author: dmichael@google.com (Dave Michael)
#include "c_salt/callback.h"

#include <nacl/npruntime.h>

#include <iostream>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "c_salt/instance.h"

using namespace c_salt;  // NOLINT
using namespace c_salt::c_salt_private;  // NOLINT

namespace {
  NPObject* const kInvalidNPObjectPointer =
                           reinterpret_cast<NPObject*>(0xdeadbeef);
  ScriptingBridge* const kInvalidScriptingBridgePointer =
                           reinterpret_cast<ScriptingBridge*>(0xbeefdead);
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
  template <>
  struct ValueMaker<ScriptingBridge*> {
    static ScriptingBridge* Make() {
      return kInvalidScriptingBridgePointer;
    }
  };

  struct NPArgMakerBase {
    virtual ~NPArgMakerBase() {}
    NPArgMakerBase() : args(NULL), num_args(0) {}
    NPVariant* args;
    uint32_t num_args;
  };
  template <class Signature>
  struct NPArgMaker : public NPArgMakerBase {};

  template <class RET, class ARG1>
  struct NPArgMaker<RET(ARG1)> : public NPArgMakerBase {
    explicit NPArgMaker(ARG1 a1) {
      num_args = 1u;
      args = new NPVariant[1];
      Marshaller<ARG1>::put(args, a1);
    }
  };
  template <class RET, class ARG1, class ARG2>
  struct NPArgMaker<RET(ARG1, ARG2)> : public NPArgMakerBase {
    NPArgMaker(ARG1 a1, ARG2 a2) {
      num_args = 2u;
      args = new NPVariant[2];
      Marshaller<ARG1>::put(args, a1);
      Marshaller<ARG2>::put(args+1, a2);
    }
  };
  template <class RET, class ARG1, class ARG2, class ARG3>
  struct NPArgMaker<RET(ARG1, ARG2, ARG3)> : public NPArgMakerBase {
    NPArgMaker(ARG1 a1, ARG2 a2, ARG3 a3) {
      num_args = 3u;
      args = new NPVariant[3];
      Marshaller<ARG1>::put(args, a1);
      Marshaller<ARG2>::put(args+1, a2);
      Marshaller<ARG3>::put(args+2, a3);
    }
  };
  template <class RET, class ARG1, class ARG2, class ARG3, class ARG4>
  struct NPArgMaker<RET(ARG1, ARG2, ARG3, ARG4)> : public NPArgMakerBase {
    NPArgMaker(ARG1 a1, ARG2 a2, ARG3 a3, ARG4 a4) {
      num_args = 4u;
      args = new NPVariant[4];
      Marshaller<ARG1>::put(args, a1);
      Marshaller<ARG2>::put(args+1, a2);
      Marshaller<ARG3>::put(args+2, a3);
      Marshaller<ARG4>::put(args+3, a4);
    }
  };

  template <class Signature>
  struct NPInvoker;

  template <class RET>
  struct NPInvoker<RET(*)()> {
    static RET InvokeWithArgs(MethodCallbackExecutor* mce,
                              ScriptingBridge* sb) {
      NPVariant retval;
      NPVariant* retval_ptr(&retval);
      NPArgMaker<RET()> arg_maker;
      mce->Execute(sb, arg_maker.args, arg_maker.num_args, &retval);
      bool dummy;
      return Unmarshaller<RET>::get(&retval_ptr, sb, &dummy);
    }
    static void Invoke(MethodCallbackExecutor* mce, ScriptingBridge* sb) {
      std::cout << "Returned " << InvokeWithArgs(mce, sb) << std::endl;
    }
  };
  template <class RET, class ARG1>
  struct NPInvoker<RET(*)(ARG1)> {
    static RET InvokeWithArgs(MethodCallbackExecutor* mce,
                              ScriptingBridge* sb,
                              ARG1 a1) {
      NPVariant retval;
      NPVariant* retval_ptr(&retval);
      NPArgMaker<RET(ARG1)>arg_maker(a1);
      mce->Execute(sb, arg_maker.args, arg_maker.num_args, &retval);
      bool dummy;
      return Unmarshaller<RET>::get(&retval_ptr, sb, &dummy);
    }
    static void Invoke(MethodCallbackExecutor* mce, ScriptingBridge* sb) {
      std::cout << "Returned " << InvokeWithArgs(mce,
                                                 sb,
                                                 ValueMaker<ARG1>::Make())

          << std::endl;
    }
  };
  template <class RET, class ARG1, class ARG2>
  struct NPInvoker<RET(*)(ARG1, ARG2)> {
    static RET InvokeWithArgs(MethodCallbackExecutor* mce,
                              ScriptingBridge* sb,
                              ARG1 a1,
                              ARG2 a2) {
      NPVariant retval;
      NPVariant* retval_ptr(&retval);
      NPArgMaker<RET(ARG1, ARG2)>arg_maker(a1, a2);
      mce->Execute(sb, arg_maker.args, arg_maker.num_args, &retval);
      bool dummy;
      return Unmarshaller<RET>::get(&retval_ptr, sb, &dummy);
    }
    static void Invoke(MethodCallbackExecutor* mce, ScriptingBridge* sb) {
      std::cout << "Returned " << InvokeWithArgs(mce,
                                                 sb,
                                                 ValueMaker<ARG1>::Make(),
                                                 ValueMaker<ARG2>::Make())
          << std::endl;
    }
  };
  template <class RET, class ARG1, class ARG2, class ARG3>
  struct NPInvoker<RET(*)(ARG1, ARG2, ARG3)> {
    static RET InvokeWithArgs(MethodCallbackExecutor* mce,
                              ScriptingBridge* sb,
                              ARG1 a1,
                              ARG2 a2,
                              ARG3 a3) {
      NPVariant retval;
      NPVariant* retval_ptr(&retval);
      NPArgMaker<RET(ARG1, ARG2, ARG3)>arg_maker(a1, a2, a3);
      mce->Execute(sb, arg_maker.args, arg_maker.num_args, &retval);
      bool dummy;
      return Unmarshaller<RET>::get(&retval_ptr, sb, &dummy);
    }
    static void Invoke(MethodCallbackExecutor* mce, ScriptingBridge* sb) {
      std::cout << "Returned " << InvokeWithArgs(mce,
                                                 sb,
                                                 ValueMaker<ARG1>::Make(),
                                                 ValueMaker<ARG2>::Make(),
                                                 ValueMaker<ARG3>::Make())
          << std::endl;
    }
  };
  template <class RET, class ARG1, class ARG2, class ARG3, class ARG4>
  struct NPInvoker<RET(*)(ARG1, ARG2, ARG3, ARG4)> {
    static RET InvokeWithArgs(MethodCallbackExecutor* mce,
                              ScriptingBridge* sb,
                              ARG1 a1,
                              ARG2 a2,
                              ARG3 a3,
                              ARG4 a4) {
      NPVariant retval;
      NPVariant* retval_ptr(&retval);
      NPArgMaker<RET(ARG1, ARG2, ARG3, ARG4)>arg_maker(a1, a2, a3, a4);
      mce->Execute(sb, arg_maker.args, arg_maker.num_args, &retval);
      bool dummy;
      return Unmarshaller<RET>::get(&retval_ptr, sb, &dummy);
    }
    static void Invoke(MethodCallbackExecutor* mce, ScriptingBridge* sb) {
      std::cout << "Returned " << InvokeWithArgs(mce,
                                                 sb,
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

#define TEST_SIGNATURE(SIGTYPE) \
{\
  CallbackMock<SIGTYPE> cb;\
  MethodCallbackExecutor* mce(\
         MakeMethodCallbackExecutor(&cb, \
                                    &CallbackMock<SIGTYPE>::Func));\
  ScriptingBridge* sb(ValueMaker<ScriptingBridge*>::Make());\
  NPInvoker<SIGTYPE>::Invoke(mce, sb);\
}

class CallbackTest : public ::testing::Test {
 protected:
  virtual void SetUp() {}
};

TEST_F(CallbackTest, TestIt) {
    // TODO(dmichael): This is cheating...  for now, I just want to see it's
    // working but I need to turn it in to a real automated test.  Note that
    // lint wants to see arg names in these function calls, which are not
    // necessary here.
    TEST_SIGNATURE(std::string(*)());  // NOLINT
    TEST_SIGNATURE(int32_t(*)(double));  // NOLINT
    TEST_SIGNATURE(double(*)(bool, int32_t));  // NOLINT
    TEST_SIGNATURE(bool(*)(std::string, double, int32_t, bool));  // NOLINT
    TEST_SIGNATURE(bool(*)(NPObject*, int32_t, double, bool));  // NOLINT
    // void return not currently supported
    // TEST_SIGNATURE(void(*)(std::string, double, int32_t, bool));
    //
    NPObject npo;
}

class MyInstance : public c_salt::Instance {
};

c_salt::Instance* c_salt::Instance::CreateInstance() { return new MyInstance; }
