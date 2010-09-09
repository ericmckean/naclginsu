// Copyright 2010 The Ginsu Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#ifndef C_SALT_BOOST_VARIANT_VISITORS_H_
#define C_SALT_BOOST_VARIANT_VISITORS_H_

#include <string>

#include "boost/shared_ptr.hpp"
#include "boost/variant/static_visitor.hpp"
#include "c_salt/scripting_bridge.h"

namespace c_salt {

// The following set of ToXVisitor classes each implement the boost Static
// Visitor concept, as described here:
// http://live.boost.org/doc/libs/1_41_0/doc/html/variant/reference.html
//   #variant.concepts.static-visitor
// Each class ToXVisitor can be passed as a static visitor to the
// boost::apply_visitor function.  ToXVisitor converts the value held by the
// boost variant in to type X.  E.g., ToStringVisitor converts the value to a
// std::string.
class ToStringVisitor : public boost::static_visitor<std::string> {
 public:
  std::string operator()(bool value) const;
  std::string operator()(int32_t value) const;
  std::string operator()(double value) const;
  // In this case, we can just pass the same const-reference out, since it
  // exists in the variant.  In all other cases, we have to copy because we are
  // creating a std::string on the stack.
  const std::string& operator()(const std::string& value) const;
  std::string operator()(const boost::shared_ptr<ScriptingBridge>& value) const;
};

class ToScriptingBridgeVisitor
  : public boost::static_visitor<boost::shared_ptr<ScriptingBridge> > {
 public:
  template <class T>
  boost::shared_ptr<ScriptingBridge> operator()(T value) const {
    // This is a catch all for types other than shared_ptr to ScriptingBridge,
    // none of which can be converted to shared_ptr<ScriptingBridge>, so we
    // just return a default-initialized shared_ptr.
    return boost::shared_ptr<ScriptingBridge>();
  }
  boost::shared_ptr<ScriptingBridge> operator()(
      const boost::shared_ptr<ScriptingBridge>& value) const;
};

class ToBoolVisitor : public boost::static_visitor<bool> {
 public:
  bool operator()(bool value) const;
  bool operator()(int32_t value) const;
  bool operator()(double value) const;
  bool operator()(const std::string& value) const;
  bool operator()(const boost::shared_ptr<ScriptingBridge>& value) const;
};

class ToIntVisitor : public boost::static_visitor<int32_t> {
 public:
  int32_t operator()(bool value) const;
  int32_t operator()(int32_t value) const;
  int32_t operator()(double value) const;
  int32_t operator()(const std::string& value) const;
  int32_t operator()(const boost::shared_ptr<ScriptingBridge>& value) const;
};

class ToDoubleVisitor : public boost::static_visitor<double> {
 public:
  double operator()(bool value) const;
  double operator()(int32_t value) const;
  double operator()(double value) const;
  double operator()(const std::string& value) const;
  double operator()(const boost::shared_ptr<ScriptingBridge>& value) const;
};

}  // namespace c_salt

#endif  // C_SALT_BOOST_VARIANT_VISITORS_H_
