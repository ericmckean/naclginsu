// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "view/converter.h"

#include "model/component.h"
#include "osg/Geometry"

namespace ginsu {
namespace view {

Converter::Converter(osg::Geometry* node) : node_(node) {
}

void Converter::Convert(const ginsu::model::Component& component) {
  vertex_array_ = new osg::Vec3Array;
  vertex_array_->setName("osg_Vertex");

  // Tesselate component faces and collect all vertices into vertex_array_.
  Tessellate(component);

  node_->setUseDisplayList(false);
  node_->setUseVertexBufferObjects(true);
  node_->setVertexAttribArray(0, vertex_array_);
  node_->setVertexAttribBinding(0, osg::Geometry::BIND_PER_VERTEX);
  node_->addPrimitiveSet(
      new osg::DrawArrays(GL_TRIANGLES, 0, vertex_array_->size()));
}

void Converter::BeginTriangleData(const TriangleData& triangles) {
}

void Converter::AddVertex(const Vertex& vertex) {
  vertex_array_->push_back(osg::Vec3(vertex.x, vertex.y, vertex.z));
}

void Converter::EndTriangleData() {
}

}  // namespace view
}  // namespace ginsu
