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
  ASSERT_TRUE(mesh_->ValidateVertex(v));
  PartialDSTest::PEMesh::PVertexConstHandle pv = v->parent_pvertex();
  ASSERT_TRUE(mesh_->ValidatePVertex(pv));
  PartialDSTest::PEMesh::EdgeConstHandle e = pv->parent_edge();
  ASSERT_TRUE(mesh_->ValidateEdge(e));
  PartialDSTest::PEMesh::PEdgeConstHandle pe = e->parent_pedge();
  ASSERT_TRUE(mesh_->ValidatePEdge(pe));
  PartialDSTest::PEMesh::LoopConstHandle loop = pe->parent_loop();
  ASSERT_TRUE(mesh_->ValidateLoop(loop));

  mesh_->DeleteIsolatedVertex(v);
  mesh_->DeleteEmptyRegion(r);
}

TEST_F(PartialDSTest, MakeWireEdge) {
  PartialDSTest::PEMesh::RegionHandle r;
  r = mesh_->CreateEmptyRegion();
  ASSERT_TRUE(r != NULL);

  PartialDSTest::PEMesh::VertexHandle v;
  v = mesh_->CreateIsolatedVertex(r);
  ASSERT_TRUE(v != NULL);
  PartialDSTest::PEMesh::EdgeHandle e1;
  e1 = mesh_->CreateWireEdgeInShell(r->outer_shell(), v);
  ASSERT_TRUE(e1 != NULL);
  ASSERT_TRUE(mesh_->ValidateEdge(e1));
  ASSERT_TRUE(mesh_->ValidateVertex(e1->start_pvertex()->vertex()));
  ASSERT_TRUE(mesh_->ValidateVertex(e1->end_pvertex()->vertex()));
  PartialDSTest::PEMesh::PEdgeConstHandle pe1 = e1->parent_pedge();
  ASSERT_TRUE(mesh_->ValidatePEdge(pe1));
  PartialDSTest::PEMesh::LoopConstHandle loop1 = pe1->parent_loop();
  ASSERT_TRUE(mesh_->ValidateLoop(loop1));
  
  PartialDSTest::PEMesh::EdgeHandle e2;
  e2 = mesh_->CreateWireEdgeInShell(r->outer_shell(), v);
  ASSERT_TRUE(e2 != NULL);
  ASSERT_TRUE(mesh_->ValidateEdge(e2));
  ASSERT_TRUE(mesh_->ValidateVertex(e2->start_pvertex()->vertex()));
  ASSERT_TRUE(mesh_->ValidateVertex(e2->end_pvertex()->vertex()));
  PartialDSTest::PEMesh::PEdgeHandle pe2 = e2->parent_pedge();
  ASSERT_TRUE(mesh_->ValidatePEdge(pe2));
  PartialDSTest::PEMesh::LoopHandle loop2 = pe2->parent_loop();
  ASSERT_TRUE(mesh_->ValidateLoop(loop2));

  // This fails because we can't add a vertex to a degenerate loop.
  // TODO(gwink): unit-test CreateWireEdgeInLoop when we can create real faces.
  // PartialDSTest::PEMesh::EdgeHandle e3;
  // e3 = mesh_->CreateWireEdgeInLoop(loop2, pe2->start_pvertex());
  // ASSERT_TRUE(e3 != NULL);

  mesh_->DeleteWireEdgeAndVertex(e2);
  ASSERT_TRUE(mesh_->ValidateVertex(e1->start_pvertex()->vertex()));
  ASSERT_TRUE(mesh_->ValidateVertex(e1->end_pvertex()->vertex()));
  mesh_->DeleteWireEdgeAndVertex(e1);
  ASSERT_TRUE(mesh_->ValidateVertex(v));
  mesh_->DeleteIsolatedVertex(v);
  mesh_->DeleteEmptyRegion(r);
}

}  // namespace
