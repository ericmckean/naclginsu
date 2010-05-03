// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GINSU_VIEW_VIEW_H_
#define GINSU_VIEW_VIEW_H_

#include "boost/scoped_ptr.hpp"

namespace ginsu {
namespace view {

class Scene;
class SceneView;

class View {
 public:
  View();
  ~View();

  void InitGL();
  void ReleaseGL();
  void SetWindowSize(int width, int height);
  void Draw();

 private:
  boost::scoped_ptr<Scene> scene_;
  boost::scoped_ptr<SceneView> scene_view_;
};

}  // namespace view
}  // namespace ginsu
#endif  // GINSU_VIEW_VIEW_H_
