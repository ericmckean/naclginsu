// Copyright (c) 2010 The Ginsu Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

//#include "model/model.h"
#include "osg/Geometry"
#include "osgViewer/Viewer"

namespace {
osg::Node* CreateSimpleTestModel()
{
  osg::Group* group = new osg::Group;

  osg::Geode* geode = new osg::Geode;
  group->addChild(geode);

  osg::Geometry* geometry = new osg::Geometry;
  geode->addDrawable(geometry);

  osg::Vec3Array* vertices = new osg::Vec3Array;
  vertices->push_back(osg::Vec3(0.0,0.0,0.0));
  vertices->push_back(osg::Vec3(0.0,0.0,1.0));
  vertices->push_back(osg::Vec3(1.0,0.0,0.0));
  vertices->push_back(osg::Vec3(1.0,0.0,1.0));
  geometry->setVertexArray(vertices);
  geometry->addPrimitiveSet(new osg::DrawArrays(GL_TRIANGLE_STRIP, 0, 4));

  char vtx_shader_src[] =
      "void main(void)\n"
      "{\n"
      "    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
      "}\n";
  char frag_shader_src[] =
      "void main(void)\n"
      "{\n"
      "    gl_FragColor = vec4(1.0,1.0,0.0,1.0); \n"
      "}\n";
  osg::Program* program = new osg::Program;
  program->addShader(new osg::Shader(osg::Shader::VERTEX, vtx_shader_src));
  program->addShader(new osg::Shader(osg::Shader::FRAGMENT, frag_shader_src));
  geometry->getOrCreateStateSet()->setAttribute(program);

  return group;
}
}  // namespace

// Instantiate platform-specific GraphicsWindow implementation.
#if defined(__APPLE__)
    #if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_5
        USE_GRAPICSWINDOW_IMPLEMENTATION(Cocoa)
    #else
        USE_GRAPICSWINDOW_IMPLEMENTATION(Carbon)
    #endif
#else
    USE_GRAPHICSWINDOW()
#endif

int main(int argc, char* argv[]) {
  //ginsu::model::Model model;

  osg::ArgumentParser arguments(&argc, argv);
  osgViewer::Viewer viewer(arguments);

  osg::ref_ptr<osg::Node> scene_data = CreateSimpleTestModel();
  viewer.setSceneData(scene_data);

  viewer.realize();
  return viewer.run();
}
