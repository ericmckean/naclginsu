// Copyright (c) 2010 The Ginsu Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GINSU_MODEL_MODEL_H_
#define GINSU_MODEL_MODEL_H_

#include "model/component.h"

namespace ginsu {
namespace model {

class Model {
 private:
  std::vector<Component> components_;
};

}  // namespace model
}  // namespace ginsu
#endif  // GINSU_MODEL_MODEL_H_
