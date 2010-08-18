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

TEST_F(PartialDSTest, TestMakeVertex) {
  PartialDSTest::PEMesh::VertexHandle v;
  v = mesh_->MakeVertex();
  ASSERT_TRUE(v != NULL);
  ASSERT_EQ(mesh_->vertices().size(), 1);
  mesh_->DestroyVertex(v);
  ASSERT_EQ(mesh_->vertices().size(), 0);
}

}  // namespace
