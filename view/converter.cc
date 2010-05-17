// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "view/converter.h"

#include "model/component.h"
#include "osg/Geometry"

namespace {
enum AttribIndex {
  kAttribIndexVertex,
  kAttribIndexNormal,
};
}  // namespace

namespace ginsu {
namespace view {

Converter::Converter(osg::Geometry* node) : node_(node) {
}

void Converter::Convert(const ginsu::model::Component& component) {
  vertex_array_ = new osg::Vec3Array;
  vertex_array_->setName("osg_Vertex");
  normal_array_ = new osg::Vec3Array;
  normal_array_->setName("osg_Normal");

  // Tesselate component faces and collect all vertices into vertex_array_.
  Tessellate(component);

  node_->setUseDisplayList(false);
  node_->setUseVertexBufferObjects(true);
  node_->setVertexAttribArray(kAttribIndexVertex, vertex_array_);
  node_->setVertexAttribBinding(kAttribIndexVertex,
      osg::Geometry::BIND_PER_VERTEX);
  node_->setVertexAttribArray(kAttribIndexNormal, normal_array_);
  node_->setVertexAttribNormalize(kAttribIndexNormal, true);
  node_->setVertexAttribBinding(kAttribIndexNormal,
      osg::Geometry::BIND_PER_VERTEX);
}

void Converter::BeginTriangleData(const TriangleData& tri_data) {
  normal_.set(tri_data.normal_x, tri_data.normal_y, tri_data.normal_z);

  GLenum mode = 0;
  switch (tri_data.flavor) {
    case kTriangles:
      mode = GL_TRIANGLES;
      break;
    case kTriangleStrip:
      mode = GL_TRIANGLE_STRIP;
      break;
    case kTriangleFan:
      mode = GL_TRIANGLE_FAN;
      break;
    default:
      assert(false);
  }
  primitive_set_ = new osg::DrawArrays(mode);
  primitive_set_->setFirst(vertex_array_->size());
}

void Converter::AddVertex(const Vertex& vertex) {
  normal_array_->push_back(normal_);
  vertex_array_->push_back(osg::Vec3(vertex.x, vertex.y, vertex.z));
}

void Converter::EndTriangleData() {
  GLsizei count = vertex_array_->size() - primitive_set_->getFirst();
  if (count > 0) {
    primitive_set_->setCount(count);
    node_->addPrimitiveSet(primitive_set_.get());
  }
}

}  // namespace view
}  // namespace ginsu
