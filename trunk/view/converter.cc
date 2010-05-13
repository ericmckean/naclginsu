// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "view/converter.h"

#include "model/component.h"
#include "osg/Geometry"

namespace ginsu {
namespace view {

Converter::Converter(osg::Geometry* node) : node_(node) {}

void Converter::BeginTriangleData(const TriangleData& triangles) {
  // TODO(gwink): Strat gathering vertex data; grab primitive format here.
}

void Converter::AddVertex(const Vertex& vertex) {
  // TODO(gwink): accumulate vertices.
}

void Converter::EndTriangleData() {
  // TODO(gwink): Form vertex-array primitive and add to geometry node.
}

}  // namespace view
}  // namespace ginsu
