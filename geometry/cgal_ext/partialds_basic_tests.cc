// Copyright (c) 2010 The Ginsu Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <CGAL/Simple_cartesian.h>
#include <gtest/gtest.h>
#include "geometry/cgal_ext/partialds.h"

namespace {

using ginsu::geometry::PartialDS;

class PartialDSTest : public ::testing::Test {
 protected:
  typedef CGAL::Simple_cartesian<double> Kernel;
  typedef PartialDS<Kernel> PEMesh;

  PartialDSTest() : mesh_(NULL) {}

  virtual void SetUp() {
    mesh_ = new PEMesh();
  }

  virtual void TearDown() {
    delete mesh_;
    mesh_ = NULL;
  }

 protected:
  PEMesh* mesh_;
};

TEST_F(PartialDSTest, SimpleModelTest) {
  ASSERT_TRUE(mesh_ != NULL);
}

TEST_F(PartialDSTest, TestMakeRegion) {
  PartialDSTest::PEMesh::RegionHandle r;
  r = mesh_->CreateEmptyRegion();
  ASSERT_TRUE(r != NULL);
  ASSERT_EQ(mesh_->regions().size(), 1);
  mesh_->DeleteEmptyRegion(r);
  ASSERT_EQ(mesh_->regions().size(), 0);
}

TEST_F(PartialDSTest, MakeIsolatedVertex) {
  PartialDSTest::PEMesh::RegionHandle r;
  r = mesh_->CreateEmptyRegion();
  ASSERT_TRUE(r != NULL);

  PartialDSTest::PEMesh::VertexHandle v;
  v = mesh_->CreateIsolatedVertex(r);
  ASSERT_TRUE(v != NULL);
  mesh_->DeleteIsolatedVertex(v);
  ASSERT_TRUE(mesh_->ValidateVertex(v));
  PartialDSTest::PEMesh::PVertexConstHandle pv = v->parent_pvertex();
  ASSERT_TRUE(mesh_->ValidatePVertex(pv));
  PartialDSTest::PEMesh::EdgeConstHandle e = pv->parent_edge();
  ASSERT_TRUE(mesh_->ValidateEdge(e));
  PartialDSTest::PEMesh::PEdgeConstHandle pe = e->parent_pedge();
  ASSERT_TRUE(mesh_->ValidatePEdge(pe));
  PartialDSTest::PEMesh::LoopConstHandle loop = pe->parent_loop();
  ASSERT_TRUE(mesh_->ValidateLoop(loop));

  // TODO(gwink): this will fail since the mesh is not empty.
  mesh_->DeleteEmptyRegion(r);
}

}  // namespace
