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
  Converter(osg::Geometry* face_geom, osg::Geometry* edge_geom);

  void Convert(const ginsu::model::Component& component);

 protected:
  virtual void BeginTriangleData(const TriangleData& tri_data);
  virtual void AddVertex(const Vertex& vertex);
  virtual void EndTriangleData();
  virtual void EdgeFlag(bool flag);

 private:
  osg::Geometry* face_geom_;
  osg::Geometry* edge_geom_;

  // Data for constructing face node.
  struct FaceData {
    osg::Vec3 normal_;
    osg::ref_ptr<osg::DrawArrays> primitive_set_;
    osg::ref_ptr<osg::Vec3Array> vertex_array_;
    osg::ref_ptr<osg::Vec3Array> normal_array_;
  } face_data_;

  // Data for constructing edge node.
  struct EdgeData {
    bool edge_flag_;
    osg::ref_ptr<osg::Vec3Array> vertex_array_;
  } edge_data_;
};

}  // namespace view
}  // namespace ginsu
#endif  // GINSU_VIEW_CONVERTER_H_
