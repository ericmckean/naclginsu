// Copyright 2010 The Ginsu Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#ifndef C_SALT_CALLBACK_H_
#define C_SALT_CALLBACK_H_

// C Headers
#include <cstring>
// C++ Headers
#include <string>

// Other
#include "boost/function.hpp"
#include "boost/bind.hpp"
#include <nacl/npruntime.h>
#include "c_salt/basic_macros.h"


namespace c_salt {

class ScriptingBridge;

// Templates used to support method call-backs when a method or property is
// accessed from the browser code.

// Class suite used to publish a method name to JavaScript.  Typical use is
// like this:
//     bridge->AddMethodNamed("calculate", this, &Calculator::Calculate);
//
//  Where bridge is a ScriptingBridge*.

// Pure virtual class that provides the interface for invoking a method given
// NPAPI arguments and the ScriptingBridge which owns the callback.  Clients
// should generally not use this interface directly;  it simply allows c_salt
// code to invoke methods generically.  See MethodCallbackExecutorImpl (and
// related classes) below for further details on how this is achieved.
class MethodCallbackExecutor {
 public:
  virtual ~MethodCallbackExecutor() {}

  virtual bool Execute(ScriptingBridge* bridge,
                       const NPVariant* args,
                       uint32_t arg_count,
                       NPVariant* return_value) = 0;
};


// The ::c_salt::c_salt_private namespace is used for code that is internal to c_salt, and
// for which clients should have no expectation of stability.
namespace c_salt_private {
// A simple traitsy class to turn a member function type in to a 
// non-member type.  If you need more arguments, just expand on the pattern.
// Usage is:
//   typedef MakeNonMemFun<void(Foo::*)(int,bool)>::type NonMemType;
// In this case, NonMemType will be a typedef for void(*)(int,bool).  I.e., a
// non-member version of the pointer-to-member-function type that was provided,
// with the same return and argument types.  
// TODO(dmichael):  This might be useful in other generic programming contexts,
// especially if it also worked as a "do-nothing" for non-member function types.
// TODO(dmichael):  Would variadic template args help here?  Or macro magic?
// Some way to make for less copy/paste code to add more arguments.
template <class MemFunSignature>
struct MakeNonMemFun;

// Partial specializations that implement the MakeNonMemFun capability for
// various numbers of arguments.
template <class RET, class T>
struct MakeNonMemFun<RET(T::*)()> { typedef RET(type)(); };
template <class RET, class T, class ARG1>
struct MakeNonMemFun<RET(T::*)(ARG1)> { typedef RET(type)(ARG1); };
template <class RET, class T, class ARG1, class ARG2>
struct MakeNonMemFun<RET(T::*)(ARG1, ARG2)> { typedef RET(type)(ARG1, ARG2); };
template <class RET, class T, class ARG1, class ARG2, class ARG3>
struct MakeNonMemFun<RET(T::*)(ARG1, ARG2, ARG3)> {
  typedef RET(type)(ARG1, ARG2, ARG3);
};
template <class RET, class T, class ARG1, class ARG2, class ARG3, class ARG4>
struct MakeNonMemFun<RET(T::*)(ARG1, ARG2, ARG3, ARG4)> {
  typedef RET(type)(ARG1, ARG2, ARG3, ARG4);
};
template <class RET, class T, class ARG1, class ARG2, class ARG3, class ARG4,
         class ARG5>
struct MakeNonMemFun<RET(T::*)(ARG1, ARG2, ARG3, ARG4, ARG5)> {
  typedef RET(type)(ARG1, ARG2, ARG3, ARG4, ARG5);
};
template <class RET, class T, class ARG1, class ARG2, class ARG3, class ARG4,
         class ARG5, class ARG6>
struct MakeNonMemFun<RET(T::*)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6)> {
  typedef RET(type)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6);
};

// Handle Marshalling of types in to NPVariants.  This is primarily used for
// turning the real return type in to an NPVARIANT for NPAPI.
// TODO(dmichael):  Could I have done this with simple overloading instead?
// TODO(dmichael):  I'm probably not supporting by-reference or by-pointer.
// Should I?
template <class T>
class Marshaller;

// Full specializations for handling all the types that are supported by NPAPI.
template <>
class Marshaller<bool> {
 public:
  // take the given bool value and put it in the given NPVariant.
  static void put(NPVariant* return_val, bool value) {
    BOOLEAN_TO_NPVARIANT(value, *return_val);
  }
};
template <>
class Marshaller<int32_t> {
 public:
  static void put(NPVariant* return_val, int32_t value) {
    INT32_TO_NPVARIANT(value, *return_val);
  }
};
template <>
class Marshaller<double> {
 public:
  static void put(NPVariant* return_val, double value) {
    DOUBLE_TO_NPVARIANT(value, *return_val);
  }
};
// Allow floats for returns, but convert to double for NPAPI.
// TODO(dmichael)  I could allow "upgrades" in general...  e.g.,
// it's safe to accept long doubles as args or 64-bit ints, because we can
// always safely convert the NPAPI allowed types.  Or I can not support any, for
// simplicity, since it doesn't really buy you anything ultimately.
template <>
class Marshaller<float> {
 public:
  static void put(NPVariant* return_val, float value) {
    DOUBLE_TO_NPVARIANT(static_cast<double>(value), *return_val);
  }
};
template <>
class Marshaller<std::string> {
 public:
  static void put(NPVariant* return_val, const std::string& str) {
    // Note: string_copy will be freed later on by the brower, so it needs to
    // be allocated here with NPN_MemAlloc()
    const std::string::size_type str_length(str.length()+1);
    char* string_copy = reinterpret_cast<char*>(NPN_MemAlloc(str_length));
    std::strncpy(string_copy, str.c_str(), str_length);
    STRINGN_TO_NPVARIANT(string_copy, str_length-1, *return_val);
  }
};
template <>
class Marshaller<NPObject*> {
 public:
  static void put(NPVariant* return_val, NPObject* value) {
    OBJECT_TO_NPVARIANT(value, *return_val);
  }
};

// Template magic for unmarshalling depending on type.  This is used when
// unpacking the NPVariant array and turning it in to real C++ arguments.
template <class T>
class Unmarshaller;

// This specialization is extra-special.  It handles forwarding the scripting
// bridge.  This is different from "normal" arguments that come from args.
// We won't advance the args pointer, we'll just pass the bridge along.  This
// is what allows a client to accept the ScriptingBridge if they want it, and
// they can take it anywhere in their argument list.
template <>
class Unmarshaller<ScriptingBridge*> {
 public:
  static ScriptingBridge* get(NPVariant** /*args*/,
                  ScriptingBridge* bridge,
                  bool* /*error*/) {
    return bridge;
  }
};
template <>
class Unmarshaller<bool> {
 public:
  // Get a boolean value (note that this is a <bool> specialization of
  // Unmarshaller) out of args and return it.
  // args - A pointer to an array of NPVariants.  This is an inout parameter.
  //        If the Unmarshalling succeeds, we advance *args so that the next
  //        time it is passed, it will point to the next argument.
  // bridge - A pointer to the ScriptingBridge with whom this callback is
  //          registered.  It is not used in "normal" Unmarshall
  //          specializations.  It exists only so that the callback handler can
  //          have ScriptingBridge* in their argument list if they wish to
  //          receive the ScriptingBridge*.
  // error - An out parameter that will be true if and only if the next value in
  //         args did not match the expected type (in this case, bool).  If the
  //         error bool is true, the return value is meaningless and should be
  //         ignored.
  static bool get(NPVariant** args,
                  ScriptingBridge* /*bridge*/,
                  bool* error) {
    if (!NPVARIANT_IS_BOOLEAN(**args)) {
      *error = true;
      return false;
    }
    ++(*args);  // advance args to the next argument.  Note it's dangerous to do
             // this directly in the macro, since if the macro is done poorly,
             // args this could happen more than once.  Hence, we do it once
             // before to be safe.
    return NPVARIANT_TO_BOOLEAN(*((*args)-1u));
  }
};
template <>
class Unmarshaller<int32_t> {
 public:
  static int32_t get(NPVariant** args,
                     ScriptingBridge* /*bridge*/,
                     bool* error) {
    if (!NPVARIANT_IS_INT32(**args)) {
      *error = true;
      return 0;
    }
    ++(*args);
    return NPVARIANT_TO_INT32(*( (*args)-1u));
  }
};
template <>
class Unmarshaller<double> {
 public:
  static double get(NPVariant** args,
                    ScriptingBridge* bridge,
                    bool* error) {
    if (!NPVARIANT_IS_DOUBLE(**args)) {
      *error = true;
      return 0.0;
    }
    ++(*args);
    return NPVARIANT_TO_DOUBLE(*((*args)-1u));
  }
};
template <>
class Unmarshaller<NPObject*> {
 public:
  static NPObject* get(NPVariant** args,
                       ScriptingBridge* bridge,
                       bool* error) {
    if (!NPVARIANT_IS_OBJECT(**args)) {
      *error = true;
      return NULL;
    }
    ++(*args);
    return NPVARIANT_TO_OBJECT(*((*args)-1u));
  }
};
template <>
class Unmarshaller<std::string> {
 public:
  static std::string get(NPVariant** args,
                         ScriptingBridge* bridge,
                         bool* error) {
    if (!NPVARIANT_IS_STRING(**args)) {
      *error = true;
      return "";
    }
    ++(*args);
    NPString np_string(NPVARIANT_TO_STRING(*((*args)-1u)));
    return std::string(np_string.UTF8Characters, np_string.UTF8Length);
  }
};

// Class with partial specializations for unmarshalling NPVariants and passing
// them to client-defined methods.
template <class Signature>
class MethodInvoker;

template <class RET>
class MethodInvoker<RET()> {
 public:
  static bool Invoke(const boost::function<RET()>& func,
                    ScriptingBridge* bridge,
                    NPVariant* args,
                    uint32_t arg_count,
                    NPVariant* return_value) {
    if (arg_count != 0u) {
      // Argument count does not match.
      return false;
    }
    // TODO(dmichael) this probably won't work with void return types
    // Invoke the function and marshall the real return value in to the
    // NPVariant (return_value).
    Marshaller<RET>::put(return_value, func());

    return true;
  }
};

template <class RET, class ARG1>
class MethodInvoker<RET(ARG1)> {
 public:
  static bool Invoke(boost::function<RET(ARG1)> func,
                    ScriptingBridge* bridge,
                    NPVariant* args,
                    uint32_t arg_count,
                    NPVariant* return_value) {
    if (arg_count != 1u) {
      // Argument count does not match.
      return false;
    }
    // Determine if there were errors unmarshalling.
    // (this might be a perfect time for exceptions :-/)
    bool error(false);
    // Note that I have to get each arg 1 at a time to ensure the Unmarshall
    // calls are done in order.  E.g., if I wrote something more like:
    //   g( f(args), f(args) );
    // The calls to f() could occur in any order, and the parameters could
    // get out of order.  Doing it this way also makes breaking on error easier.
    // See specializations for 2+ args to get a better illustration of the
    // pattern.
    const ARG1& arg1(Unmarshaller<ARG1>::get(&args, bridge, &error));
    if (error) return false;
    Marshaller<RET>::put(return_value, func(arg1));
    return true;
  }
};
// From here on out I'll leave out comments to keep it concise...  see above
// for explanations of what's going on.  TODO(dmichael) Explain what's going
// on :-)
template <class RET, class ARG1, class ARG2>
class MethodInvoker<RET(ARG1, ARG2)> {
 public:
  static bool Invoke(boost::function<RET(ARG1, ARG2)> func,
                    ScriptingBridge* bridge,
                    NPVariant* args,
                    uint32_t arg_count,
                    NPVariant* return_value) {
    if (arg_count != 2u) return false;
    bool error(false);
    const ARG1& arg1(Unmarshaller<ARG1>::get(&args, bridge, &error));
    if (error) return false;
    const ARG2& arg2(Unmarshaller<ARG2>::get(&args, bridge, &error));
    if (error) return false;
    Marshaller<RET>::put(return_value, func(arg1, arg2));
    return true;
  }
};
template <class RET, class ARG1, class ARG2, class ARG3>
class MethodInvoker<RET(ARG1, ARG2, ARG3)> {
 public:
  static bool Invoke(boost::function<RET(ARG1, ARG2, ARG3)> func,
                    ScriptingBridge* bridge,
                    NPVariant* args,
                    uint32_t arg_count,
                    NPVariant* return_value) {
    if (arg_count != 3u) return false;
    bool error(false);
    const ARG1& arg1(Unmarshaller<ARG1>::get(&args, bridge, &error));
    if (error) return false;
    const ARG2& arg2(Unmarshaller<ARG2>::get(&args, bridge, &error));
    if (error) return false;
    const ARG3& arg3(Unmarshaller<ARG3>::get(&args, bridge, &error));
    if (error) return false;
    Marshaller<RET>::put(return_value, func(arg1, arg2, arg3));
    return true;
  }
};
template <class RET, class ARG1, class ARG2, class ARG3, class ARG4>
class MethodInvoker<RET(ARG1, ARG2, ARG3, ARG4)> {
 public:
  static bool Invoke(boost::function<RET(ARG1, ARG2, ARG3, ARG4)> func,
                    ScriptingBridge* bridge,
                    NPVariant* args,
                    uint32_t arg_count,
                    NPVariant* return_value) {
    if (arg_count != 4u) return false;
    bool error(false);
    const ARG1& arg1(Unmarshaller<ARG1>::get(&args, bridge, &error));
    if (error) return false;
    const ARG2& arg2(Unmarshaller<ARG2>::get(&args, bridge, &error));
    if (error) return false;
    const ARG3& arg3(Unmarshaller<ARG3>::get(&args, bridge, &error));
    if (error) return false;
    const ARG4& arg4(Unmarshaller<ARG4>::get(&args, bridge, &error));
    if (error) return false;
    Marshaller<RET>::put(return_value, func(arg1, arg2, arg3, arg4));
    return true;
  }
};
template <class RET, class ARG1, class ARG2, class ARG3, class ARG4, class ARG5>
class MethodInvoker<RET(ARG1, ARG2, ARG3, ARG4, ARG5)> {
 public:
  static bool Invoke(boost::function<RET(ARG1, ARG2, ARG3, ARG4, ARG5)> func,
                    ScriptingBridge* bridge,
                    NPVariant* args,
                    uint32_t arg_count,
                    NPVariant* return_value) {
    if (arg_count != 5u) return false;
    bool error(false);
    const ARG1& arg1(Unmarshaller<ARG1>::get(&args, bridge, &error));
    if (error) return false;
    const ARG2& arg2(Unmarshaller<ARG2>::get(&args, bridge, &error));
    if (error) return false;
    const ARG3& arg3(Unmarshaller<ARG3>::get(&args, bridge, &error));
    if (error) return false;
    const ARG4& arg4(Unmarshaller<ARG4>::get(&args, bridge, &error));
    if (error) return false;
    const ARG5& arg5(Unmarshaller<ARG5>::get(&args, bridge, &error));
    if (error) return false;
    Marshaller<RET>::put(return_value, func(arg1, arg2, arg3, arg4, arg5));
    return true;
  }
};
template <class RET, class ARG1, class ARG2, class ARG3, class ARG4, class ARG5,
         class ARG6>
class MethodInvoker<RET(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6)> {
 public:
  static bool Invoke(
      boost::function<RET(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6)> func,
      ScriptingBridge* bridge,
      NPVariant* args,
      uint32_t arg_count,
      NPVariant* return_value) {
    if (arg_count != 6u) return false;
    bool error(false);
    const ARG1& arg1(Unmarshaller<ARG1>::get(&args, bridge, &error));
    if (error) return false;
    const ARG2& arg2(Unmarshaller<ARG2>::get(&args, bridge, &error));
    if (error) return false;
    const ARG3& arg3(Unmarshaller<ARG3>::get(&args, bridge, &error));
    if (error) return false;
    const ARG4& arg4(Unmarshaller<ARG4>::get(&args, bridge, &error));
    if (error) return false;
    const ARG5& arg5(Unmarshaller<ARG5>::get(&args, bridge, &error));
    if (error) return false;
    const ARG6& arg6(Unmarshaller<ARG6>::get(&args, bridge, &error));
    if (error) return false;
    Marshaller<RET>::put(return_value, func(arg1, arg2, arg3, arg4, arg5,
                                            arg6));
    return true;
  }
};

// BindingBase is a helper class to provide an appropriately typed
// boost::function member variable, with a constructor to allow binding a
// target-class instance & method and assigning them to that boost::function.
// This allows MethodCallbackExecutorImpl below to own, initialize, and invoke
// the right kind of boost::function without being overly complicated by partial
// specialization itself.
template <class T, class MemFunSignature>
class BindingBase;
template <class T, class U, class RET>
class BindingBase<T, RET(U::*)()> {
 protected:
  BindingBase(T* instance, RET(U::*method)())
      : function_(boost::bind(method, instance)) {
  }
  boost::function<RET()> function_;
};
template <class T, class U, class RET, class ARG1>
class BindingBase<T, RET(U::*)(ARG1)> {
 protected:
  BindingBase(T* instance, RET(U::*method)(ARG1))
      : function_(boost::bind(method, instance, _1)) {
  }
  boost::function<RET(ARG1)> function_;
};
template <class T, class U, class RET, class ARG1, class ARG2>
class BindingBase<T, RET(U::*)(ARG1, ARG2)> {
 protected:
  BindingBase(T* instance, RET(U::*method)(ARG1, ARG2))
      : function_(boost::bind(method, instance, _1, _2)) {}
  boost::function<RET(ARG1, ARG2)> function_;
};
template <class T, class U, class RET, class ARG1, class ARG2, class ARG3>
class BindingBase<T, RET(U::*)(ARG1, ARG2, ARG3)> {
 protected:
  BindingBase(T* instance, RET(U::*method)(ARG1, ARG2, ARG3))
      : function_(boost::bind(method, instance, _1, _2, _3)) {}
  boost::function<RET(ARG1, ARG2, ARG3)> function_;
};
template <class T, class U, class RET, class ARG1, class ARG2, class ARG3,
          class ARG4>
class BindingBase<T, RET(U::*)(ARG1, ARG2, ARG3, ARG4)> {
 protected:
  BindingBase(T* instance, RET(U::*method)(ARG1, ARG2, ARG3, ARG4))
      : function_(boost::bind(method, instance, _1, _2, _3, _4)) {}
  boost::function<RET(ARG1, ARG2, ARG3, ARG4)> function_;
};
template <class T, class U, class RET, class ARG1, class ARG2, class ARG3,
          class ARG4, class ARG5>
class BindingBase<T, RET(U::*)(ARG1, ARG2, ARG3, ARG4, ARG5)> {
 protected:
  BindingBase(T* instance, RET(U::*method)(ARG1, ARG2, ARG3, ARG4, ARG5))
      : function_(boost::bind(method, instance, _1, _2, _3, _4, _5)) {}
  boost::function<RET(ARG1, ARG2, ARG3, ARG4, ARG5)> function_;
};
template <class T, class U, class RET, class ARG1, class ARG2, class ARG3,
          class ARG4, class ARG5, class ARG6>
class BindingBase<T, RET(U::*)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6)> {
 protected:
  BindingBase(T* instance, RET(U::*method)(ARG1, ARG2, ARG3, ARG4, ARG5,
                                           ARG6))
      : function_(boost::bind(method, instance, _1, _2, _3, _4, _5, _6)) {}
  boost::function<RET(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6)> function_;
};
// TODO(dmichael) Add support for more nums of args
}  // c_salt_private

// MethodCallbackExecutorImpl is a class template that implements the
// MethodCallbackExecutor interface by calling an arbitrary boost::function and
// automatically handling marshalling/unmarshalling of the arguments and return
// type to bridge the gap between the NPAPI method invocation and the invocation
// of a real C++ method on a client-defined class.
template <class T, class MemFunSignature>
class MethodCallbackExecutorImpl : public MethodCallbackExecutor,
            public ::c_salt::c_salt_private::BindingBase<T, MemFunSignature> {
              //TODO(dmichael) - there's no reason for
              //MethodCallbackExecutorImpl to only support member functions.
              //The helper function template below (MakeMethodCallbackExecutor)
              //should handle deciding what kind of function you have and
              //creating an appropriate MethodCallbackExecutorImpl.
 public:
  typedef bool (T::*Method)(ScriptingBridge* bridge,
                            const NPVariant* args,
                            uint32_t arg_count,
                            NPVariant* return_value);
  typedef typename ::c_salt::c_salt_private::MakeNonMemFun<MemFunSignature>::type Signature;

  MethodCallbackExecutorImpl(T* instance, MemFunSignature method)
      : ::c_salt::c_salt_private::BindingBase<T, MemFunSignature>(instance,
                                                                  method) {} 
  virtual ~MethodCallbackExecutorImpl() {}

  virtual bool Execute(ScriptingBridge* bridge,
                       const NPVariant* args,
                       uint32_t arg_count,
                       NPVariant* return_value) {
    return ::c_salt::c_salt_private::MethodInvoker<Signature>::Invoke(
        ::c_salt::c_salt_private::BindingBase<T,MemFunSignature>::function_, bridge, const_cast<NPVariant*>(args), arg_count, return_value);
    //TODO(dmichael) shouldn't need to const cast.  Come up with a different way
    //to advance args or something
  }
};

// A helper function that creates an appropriate MethodCallbackExecutor that,
// when invoked via its NPAPI-based Execute function, will convert the call in
// to an appropriate application-level native C++ call.
template <class T, class Signature>
MethodCallbackExecutor* MakeMethodCallbackExecutor(T* target,
                                                   Signature method) {
  return new ::c_salt::MethodCallbackExecutorImpl<T, Signature>(target, method);
}

// Class suite used to publish properties to the browser code.  Usage is
// similar to MethodCallback.
class PropertyAccessorCallbackExecutor {
 public:
  virtual bool Execute(ScriptingBridge* bridge,
                       NPVariant* return_value) = 0;
};

template <class T>
class PropertyAccessorCallback : public PropertyAccessorCallbackExecutor {
 public:
  typedef bool (T::*Method)(ScriptingBridge* bridge,
                            NPVariant* return_value);

  PropertyAccessorCallback(T* instance, Method method)
      : instance_(instance), method_(method) {}
  virtual bool Execute(ScriptingBridge* bridge,
                       NPVariant* return_value) {
    // Use "this->" to force C++ to look inside our templatized base class; see
    // Effective C++, 3rd Ed, item 43, p210 for details.
    return ((this->instance_)->*(this->method_))(bridge, return_value);
  }

 private:
  T* instance_;
  Method method_;
};

class PropertyMutatorCallbackExecutor {
 public:
  virtual bool Execute(ScriptingBridge* bridge,
                       const NPVariant* value) = 0;
};

template <class T>
class PropertyMutatorCallback : public PropertyMutatorCallbackExecutor {
 public:
  typedef bool (T::*Method)(ScriptingBridge* bridge,
                            const NPVariant* return_value);

  PropertyMutatorCallback(T* instance, Method method)
      : instance_(instance), method_(method) {}
  virtual bool Execute(ScriptingBridge* bridge,
                       const NPVariant* value) {
    // Use "this->" to force C++ to look inside our templatized base class; see
    // Effective C++, 3rd Ed, item 43, p210 for details.
    return ((this->instance_)->*(this->method_))(bridge, value);
  }

 private:
  T* instance_;
  Method method_;
};

}  // namespace c_salt

#endif  // C_SALT_CALLBACK_H_
