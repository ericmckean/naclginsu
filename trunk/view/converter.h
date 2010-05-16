// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GINSU_VIEW_CONVERTER_H_
#define GINSU_VIEW_CONVERTER_H_

#include "model/tessellator.h"
#include "osg/Array"

namespace osg {
class DrawArrays;
class Geometry;
}

namespace ginsu {
namespace view {

// Convert a model Component instance to triangle data for viewing in the
// scene graph. Usage:
//   Converter converter(node_to_receive_triangle_arrays);
//   converter.Tessellate(component);
class Converter : protected model::Tessellator {
 public:
  Converter(osg::Geometry* node);

  void Convert(const ginsu::model::Component& component);

 protected:
  virtual void BeginTriangleData(const TriangleData& tri_data);
  virtual void AddVertex(const Vertex& vertex);
  virtual void EndTriangleData();

 private:
  osg::Geometry* node_;

  osg::ref_ptr<osg::DrawArrays> primitive_set_;
  osg::ref_ptr<osg::Vec3Array> vertex_array_;
};

}  // namespace view
}  // namespace ginsu
#endif  // GINSU_VIEW_CONVERTER_H_
