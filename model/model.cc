// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "model/model.h"
#include "model/component.h"
#include "model/mesh.h"

namespace ginsu {
namespace model {

Model::Model() {
}

Model::~Model() {
  Clear();
}

void Model::AddComponent(Component* component) {
  if (component != NULL) {
    components_.push_back(ComponentItem(component));
  }
}

void Model::Clear() {
  components_.clear();
}

}  // namespace model
}  // namespace ginsu
