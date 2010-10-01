// Copyright (c) 2010 The FakeInstance Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <cstdio>

#include "c_salt/integration_tests/fake_instance.h"

FakeInstance::FakeInstance(const NPP& npp_instance)
    : c_salt::Instance(npp_instance),
      method_tester_(new MethodTester()) {
  std::printf("FakeInstance constructor;  creating MethodScriptTest\n");
  this->CreateScriptingBridgeForObject(method_tester_);
}

FakeInstance::~FakeInstance() {
}

void FakeInstance::InitializeMethods(c_salt::ScriptingBridge* bridge) {
  std::printf("Adding methods in FakeInstance\n");
  bridge->AddMethodNamed("getMethodTester",
                         this,
                         &FakeInstance::GetMethodTester);
}

void FakeInstance::InitializeProperties(c_salt::ScriptingBridge* bridge) {
  // No properties (yet)
}

boost::shared_ptr<MethodTester> FakeInstance::GetMethodTester() {
  return method_tester_;
}
