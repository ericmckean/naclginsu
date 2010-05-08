// Copyright (c) 2010 The Ginsu Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#ifndef GINSU_MODEL_TESSELLATOR_H_
#define GINSU_MODEL_TESSELLATOR_H_

#include "third_party/glu_tessellator/glu_tessellator.h"

namespace ginsu {
namespace model {
  
class Component;

// Component tessellator. Usage:
// 1. Subclass Tessellator and override BeginTriangleData, AddVertex and
//    EndTriangleData.
// 2. Invoke Tessellate(component) for each component to convert to triangles.
class Tessellator {
 public:
  // The tessellation comes in three flavors.
  enum Flavor {
    kTriangles,
    kTriangleStrip,
    kTriangleFan
  };

  // A vertex in the triangulation.
  struct Vertex {
    float x, y, z;
  };

  // Info about the face being triangulated.
  struct TriangleData {
    Flavor flavor;
    float normal_x, normal_y, normal_z;  // Normal vector of embedding plane
  };

  // Tessellate the given component. Will call the callbacks below to deliver
  // the tessellation data to the subclass.
  void Tessellate(const Component& component);

 protected:
  // Override these function in a subclass to catch the tessellated geometry.
  // The default implementation does nothing. Not that it is possible for
  // AddVertex to never be called. Such empty faces should simply be
  // discarded.
  virtual void BeginTriangleData(const TriangleData& triangles);
  virtual void AddVertex(const Vertex& vertex);
  virtual void EndTriangleData();

  // Callbacks for the glu tessellator.
  static void BeginGluCallback(GLenum which, void* user_data);
  static void EndGluCallback(void* user_data);
  static void ErrorGluCallback(GLenum errorCode, void* user_data);
  static void VertexGluCallback(void* vertex, void* user_data);

  // Create and configure a glu tessellator.
  static GLUtesselator* CreateGluTessellator();
};

}  // namespace model
}  // namespace ginsu
#endif  // GINSU_MODEL_TESSELLATOR_H_
