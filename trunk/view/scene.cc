// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "view/scene.h"

#include "model/component.h"
#include "model/model.h"
#include "osg/Geode"
#include "osg/Geometry"
#include "view/converter.h"

using ginsu::model::Component;
using ginsu::model::Model;

namespace {
osg::Program* BuildFaceShader() {
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
  return program;
}

osg::Node* BuildComponentNode(const Component& component,
                              osg::Program* face_shader) {
  osg::Geometry* geometry = new osg::Geometry;
  ginsu::view::Converter converter(geometry);
  converter.Convert(component);
  geometry->getOrCreateStateSet()->setAttribute(face_shader);

  osg::Geode* geode = new osg::Geode;
  geode->addDrawable(geometry);
  return geode;
}
}  // namespace

namespace ginsu {
namespace view {

Scene::Scene(Model* model) : model_(model) {
}

Scene::~Scene() {
}

void Scene::Init() {
  face_shader_ = BuildFaceShader();

  osg::ref_ptr<osg::Group> root = new osg::Group;
  for (Model::const_iterator iter = model_->begin_component();
       iter != model_->end_component(); ++iter) {
    osg::Node* node = BuildComponentNode(*(iter->get()), face_shader_.get());
    root->addChild(node);
  }

  root_ = root;
}

void Scene::Update() {
}

const osg::BoundingSphere& Scene::GetBound() const {
  return root_->getBound();
}

}  // namespace view
}  // namespace ginsu
