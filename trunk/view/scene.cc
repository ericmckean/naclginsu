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
      "uniform mat3 osg_NormalMatrix;\n"
      "attribute vec4 osg_Vertex;\n"
      "attribute vec3 osg_Normal;\n"
      "varying vec4 color;\n"
      "void main(void) {\n"
      "  vec3 normal_eye = normalize(osg_NormalMatrix * osg_Normal);\n"
      "  float normal_comp = max(normal_eye.z, 0.0);\n"
      "  float color_comp = (0.8 * normal_comp) + 0.2;\n"
      "  color = vec4(color_comp, color_comp, color_comp, 1.0);\n"
      "  gl_Position = osg_ModelViewProjectionMatrix * osg_Vertex;\n"
      "}\n";
  char frag_shader_src[] =
      "precision highp float;\n"
      "varying vec4 color;\n"
      "void main(void) {\n"
      "  gl_FragColor = color;\n"
      "}\n";

  osg::Program* program = new osg::Program;
  program->addShader(new osg::Shader(osg::Shader::VERTEX, vtx_shader_src));
  program->addShader(new osg::Shader(osg::Shader::FRAGMENT, frag_shader_src));
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
