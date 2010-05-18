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

Converter::Converter(osg::Geometry* face_geom,
                     osg::Geometry* edge_geom)
    : face_geom_(face_geom),
      edge_geom_(edge_geom) {
  memset(&face_data_, 0, sizeof(face_data_));
  memset(&edge_data_, 0, sizeof(edge_data_));
}

void Converter::Convert(const ginsu::model::Component& component) {
  // At least one of them should be non-null.
  assert((face_geom_ != NULL) || (edge_geom_ != NULL));

  if (face_geom_ != NULL) {
    face_data_.vertex_array_ = new osg::Vec3Array;
    face_data_.vertex_array_->setName("osg_Vertex");
    face_data_.normal_array_ = new osg::Vec3Array;
    face_data_.normal_array_->setName("osg_Normal");
  }
  if (edge_geom_ != NULL) {
    edge_data_.vertex_array_ = new osg::Vec3Array;
    edge_data_.vertex_array_->setName("osg_Vertex");
  }

  // Tesselate component faces and collect all vertices into vertex_array_.
  Tessellate(component);

  if (face_geom_ != NULL) {
    face_geom_->setUseDisplayList(false);
    face_geom_->setUseVertexBufferObjects(true);
    face_geom_->setVertexAttribArray(kAttribIndexVertex,
        face_data_.vertex_array_);
    face_geom_->setVertexAttribBinding(kAttribIndexVertex,
        osg::Geometry::BIND_PER_VERTEX);
    face_geom_->setVertexAttribArray(kAttribIndexNormal,
        face_data_.normal_array_);
    face_geom_->setVertexAttribNormalize(kAttribIndexNormal, true);
    face_geom_->setVertexAttribBinding(kAttribIndexNormal,
        osg::Geometry::BIND_PER_VERTEX);
  }
  if (edge_geom_ != NULL) {
    edge_geom_->setUseDisplayList(false);
    edge_geom_->setUseVertexBufferObjects(true);
    edge_geom_->setVertexAttribArray(kAttribIndexVertex,
        edge_data_.vertex_array_);
    edge_geom_->setVertexAttribBinding(kAttribIndexVertex,
        osg::Geometry::BIND_PER_VERTEX);
    edge_geom_->addPrimitiveSet(
        new osg::DrawArrays(GL_LINES, 0, edge_data_.vertex_array_->size()));
  }
}

void Converter::BeginTriangleData(const TriangleData& tri_data) {
  if (face_geom_ == NULL)
    return;

  face_data_.normal_.set(tri_data.normal_x,
                         tri_data.normal_y,
                         tri_data.normal_z);

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
  face_data_.primitive_set_ = new osg::DrawArrays(mode);
  face_data_.primitive_set_->setFirst(face_data_.vertex_array_->size());
}

void Converter::AddVertex(const Vertex& vertex) {
  if (face_geom_ != NULL) {
    face_data_.normal_array_->push_back(face_data_.normal_);
    face_data_.vertex_array_->push_back(
        osg::Vec3(vertex.x, vertex.y, vertex.z));
  }
  if ((edge_geom_ != NULL) && edge_data_.edge_flag_) {
    edge_data_.vertex_array_->push_back(
        osg::Vec3(vertex.x, vertex.y, vertex.z));
  }
}

void Converter::EndTriangleData() {
  if (face_geom_ == NULL)
    return;

  GLsizei count = face_data_.vertex_array_->size() -
      face_data_.primitive_set_->getFirst();
  if (count > 0) {
    face_data_.primitive_set_->setCount(count);
    face_geom_->addPrimitiveSet(face_data_.primitive_set_);
  }
}

void Converter::EdgeFlag(bool flag) {
  edge_data_.edge_flag_ = flag;
}

}  // namespace view
}  // namespace ginsu
