// Copyright 2010 Google Inc. All Rights Reserved.
// Author: dmichael@google.com (Dave Michael)
#include "c_salt/callback.h"

#include <iostream>

/* #include <stdio.h>
#include "common/google.h"
#include "common/commandlineflags.h"
#include "common/logging.h" 
#include "testing/googletest.h" */

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "c_salt/module.h"

using namespace c_salt;
using namespace c_salt::c_salt_private;

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
struct ValueMaker<ScriptingBridge*> {
  static ScriptingBridge* Make() {
    return reinterpret_cast<ScriptingBridge*>(0xdeadbeef);
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
  NPArgMaker(ARG1 a1) {
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
  static RET InvokeWithArgs(MethodCallbackExecutor* mce, ScriptingBridge* sb) {
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
  static RET InvokeWithArgs(MethodCallbackExecutor* mce, ScriptingBridge* sb, ARG1 a1) {
    NPVariant retval;
    NPVariant* retval_ptr(&retval);
    NPArgMaker<RET (ARG1)> arg_maker(a1);
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
  static RET InvokeWithArgs(MethodCallbackExecutor* mce, ScriptingBridge* sb, ARG1 a1, ARG2 a2) {
    NPVariant retval;
    NPVariant* retval_ptr(&retval);
    NPArgMaker<RET (ARG1, ARG2)> arg_maker(a1, a2);
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
  static RET InvokeWithArgs(MethodCallbackExecutor* mce, ScriptingBridge* sb, ARG1 a1, ARG2 a2, ARG3 a3) {
    NPVariant retval;
    NPVariant* retval_ptr(&retval);
    NPArgMaker<RET (ARG1, ARG2, ARG3)> arg_maker(a1, a2, a3);
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
  static RET InvokeWithArgs(MethodCallbackExecutor* mce, ScriptingBridge* sb, ARG1 a1, ARG2 a2, ARG3 a3, ARG4 a4) {
    NPVariant retval;
    NPVariant* retval_ptr(&retval);
    NPArgMaker<RET (ARG1, ARG2, ARG3, ARG4)> arg_maker(a1, a2, a3, a4);
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

#define TEST_SIGNATURE(SIGTYPE) \
{\
  CallbackMock<SIGTYPE> cb;\
  MethodCallbackExecutor* mce(MakeMethodCallbackExecutor(&cb,\
                                                &CallbackMock<SIGTYPE>::Func));\
  ScriptingBridge* sb(ValueMaker<ScriptingBridge*>::Make());\
  NPInvoker<SIGTYPE>::Invoke(mce, sb);\
}

class CallbackTest : public ::testing::Test {
 protected:
  virtual void SetUp() {}
};

TEST_F(CallbackTest, TestIt) {
    // TODO This is cheating...  for now, I just want to see it's working
    // but I need to turn it in to a real automated test
    //
    TEST_SIGNATURE(std::string(*)());
    TEST_SIGNATURE(int32_t(*)(double));
    TEST_SIGNATURE(double(*)(bool,int32_t));
    TEST_SIGNATURE(bool(*)(std::string, double, int32_t, bool));
    // void return not currently supported
    //TEST_SIGNATURE(void(*)(std::string, double, int32_t, bool));
}

class MyModule : public c_salt::Module
{};
c_salt::Module* c_salt::Module::CreateModule() { return new MyModule; }
