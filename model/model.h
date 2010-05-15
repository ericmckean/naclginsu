// Copyright (c) 2010 The Ginsu Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GINSU_MODEL_MODEL_H_
#define GINSU_MODEL_MODEL_H_

#include <vector>
#include "boost/shared_ptr.hpp"

namespace ginsu {
namespace model {

class Component;

class Model {
 public:
  typedef boost::shared_ptr<Component> ComponentItem;
  typedef std::vector<ComponentItem>::const_iterator const_iterator;

  Model();
  ~Model();

  // Add a component at the end of the list.
  void AddComponent(Component* component);

  // Iterate over components in model.
  const_iterator begin_component() const { return components_.begin(); }
  const_iterator end_component() const { return components_.end(); }

  // Demo only.
  void DemoComputeIntersection();
  
 protected:
  // Remove all geometry from the model.
  void Clear();

 private:
  std::vector<ComponentItem> components_;
};

}  // namespace model
}  // namespace ginsu
#endif  // GINSU_MODEL_MODEL_H_
