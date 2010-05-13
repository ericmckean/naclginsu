// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GINSU_VIEW_CONVERTER_H_
#define GINSU_VIEW_CONVERTER_H_

#include "model/tessellator.h"

namespace osg {
class Geometry;
}

namespace ginsu {
namespace view {

// Convert a model Component instance to triangle data for viewing in the
// scene graph. Usage:
//   Converter converter(node_to_receive_triangle_arrays);
//   converte.Tessellate(component);
class Converter : model::Tessellator {
 public:
  Converter(osg::Geometry* node);

 protected:
  virtual void BeginTriangleData(const TriangleData& triangles);
  virtual void AddVertex(const Vertex& vertex);
  virtual void EndTriangleData();

 private:
  osg::Geometry* node_;
};

}  // namespace view
}  // namespace ginsu
#endif  // GINSU_VIEW_CONVERTER_H_
