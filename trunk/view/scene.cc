// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "view/scene.h"

#include "osg/Geode"
#include "osg/Geometry"

namespace {
osg::Node* CreateSimpleTestModel() {
  osg::Vec3Array* vtx_array = new osg::Vec3Array;
  vtx_array->setName("osg_Vertex");
  vtx_array->push_back(osg::Vec3(0.0,0.0,0.0));
  vtx_array->push_back(osg::Vec3(0.0,0.0,1.0));
  vtx_array->push_back(osg::Vec3(1.0,0.0,0.0));
  vtx_array->push_back(osg::Vec3(1.0,0.0,1.0));

  char vtx_shader_src[] =
      "uniform mat4 osg_ModelViewProjectionMatrix;\n"
      "attribute vec4 osg_Vertex;\n"
      "void main(void)\n"
      "{\n"
      "  gl_Position = osg_ModelViewProjectionMatrix * osg_Vertex;\n"
      "}\n";
  char frag_shader_src[] =
      "void main(void)\n"
      "{\n"
      "  gl_FragColor = vec4(1.0, 1.0, 0.0, 1.0);\n"
      "}\n";
  osg::Program* program = new osg::Program;
  program->addShader(new osg::Shader(osg::Shader::VERTEX, vtx_shader_src));
  program->addShader(new osg::Shader(osg::Shader::FRAGMENT, frag_shader_src));
  program->addBindAttribLocation("osg_Vertex", 0);

  osg::Geometry* geometry = new osg::Geometry;
  geometry->setUseDisplayList(false);
  geometry->setUseVertexBufferObjects(true);
  geometry->setVertexAttribArray(0, vtx_array);
  geometry->setVertexAttribBinding(0, osg::Geometry::BIND_PER_VERTEX);
  geometry->getOrCreateStateSet()->setAttribute(program);
  geometry->addPrimitiveSet(new osg::DrawArrays(GL_TRIANGLE_STRIP, 0, 4));

  osg::Geode* geode = new osg::Geode;
  geode->addDrawable(geometry);
  return geode;
}
}  // namespace

namespace ginsu {
namespace view {

Scene::Scene() {
}

Scene::~Scene() {
}

void Scene::Init() {
  osg::ref_ptr<osg::Group> root = new osg::Group;
  root->addChild(CreateSimpleTestModel());

  root_ = root;
}

void Scene::Update() {
}

const osg::BoundingSphere& Scene::GetBound() const {
  return root_->getBound();
}

}  // namespace view
}  // namespace ginsu
