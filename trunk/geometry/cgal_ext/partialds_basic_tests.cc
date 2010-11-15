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
    PEMesh::EnableExhaustiveMode(true);
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

TEST_F(PartialDSTest, TestCreateRegion) {
  PartialDSTest::PEMesh::RegionHandle r;
  r = mesh_->CreateEmptyRegion();
  ASSERT_TRUE(r != NULL);
  ASSERT_EQ(mesh_->regions().size(), 1);
  mesh_->DeleteEmptyRegion(r);
  ASSERT_EQ(mesh_->regions().size(), 0);
}

TEST_F(PartialDSTest, TestCreateIsolatedVertex) {
  PartialDSTest::PEMesh::RegionHandle r;
  r = mesh_->CreateEmptyRegion();
  ASSERT_TRUE(r != NULL);

  PartialDSTest::PEMesh::VertexHandle v;
  PartialDSTest::PEMesh::ShellHandle s;
  mesh_->CreateIsolatedVertex(r, &v, &s);
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

TEST_F(PartialDSTest, TestCreateWireEdge) {
  PartialDSTest::PEMesh::RegionHandle r;
  r = mesh_->CreateEmptyRegion();
  ASSERT_TRUE(r != NULL);

  PartialDSTest::PEMesh::VertexHandle v;
  PartialDSTest::PEMesh::ShellHandle s;
  mesh_->CreateIsolatedVertex(r, &v, &s);
  ASSERT_TRUE(v != NULL);
  PartialDSTest::PEMesh::EdgeHandle e1;
  e1 = mesh_->CreateWireEdgeAndVertex(s, v);
  ASSERT_TRUE(e1 != NULL);
  ASSERT_TRUE(mesh_->ValidateEdge(e1));
  ASSERT_TRUE(mesh_->ValidateVertex(e1->start_pvertex()->vertex()));
  ASSERT_TRUE(mesh_->ValidateVertex(e1->end_pvertex()->vertex()));
  PartialDSTest::PEMesh::PEdgeConstHandle pe1 = e1->parent_pedge();
  ASSERT_TRUE(mesh_->ValidatePEdge(pe1));
  PartialDSTest::PEMesh::LoopConstHandle loop1 = pe1->parent_loop();
  ASSERT_TRUE(mesh_->ValidateLoop(loop1));
  
  PartialDSTest::PEMesh::EdgeHandle e2;
  e2 = mesh_->CreateWireEdgeAndVertex(s, v);
  ASSERT_TRUE(e2 != NULL);
  ASSERT_TRUE(mesh_->ValidateEdge(e2));
  ASSERT_TRUE(mesh_->ValidateVertex(e2->start_pvertex()->vertex()));
  ASSERT_TRUE(mesh_->ValidateVertex(e2->end_pvertex()->vertex()));
  PartialDSTest::PEMesh::PEdgeHandle pe2 = e2->parent_pedge();
  ASSERT_TRUE(mesh_->ValidatePEdge(pe2));
  PartialDSTest::PEMesh::LoopHandle loop2 = pe2->parent_loop();
  ASSERT_TRUE(mesh_->ValidateLoop(loop2));
  PartialDSTest::PEMesh::FaceHandle face2 = loop2->parent_face();
  ASSERT_TRUE(mesh_->ValidateFace(face2));
  ASSERT_TRUE(mesh_->ValidatePFace(face2->parent_pface()));

  // This fails because we can't add a vertex to a degenerate loop.
  // TODO(gwink): unit-test CreateWireEdgeInLoop when we can create real faces.
  // PartialDSTest::PEMesh::EdgeHandle e3;
  // e3 = mesh_->CreateWireEdgeInLoop(loop2, pe2->start_pvertex());
  // ASSERT_TRUE(e3 != NULL);

  mesh_->DeleteWireEdgeAndVertex(e2, e2->end_pvertex()->vertex());
  ASSERT_TRUE(mesh_->ValidateVertex(e1->start_pvertex()->vertex()));
  ASSERT_TRUE(mesh_->ValidateVertex(e1->end_pvertex()->vertex()));
  mesh_->DeleteWireEdgeAndVertex(e1, e1->end_pvertex()->vertex());
  ASSERT_TRUE(mesh_->ValidateVertex(v));
  mesh_->DeleteIsolatedVertex(v);
  mesh_->DeleteEmptyRegion(r);
}

TEST_F(PartialDSTest, TestSplitEdgeCreateVertex) {
  // Setup: create a wire edge.
  // TODO(gwink): re-enable this test once we can create non-wire edges.
//  PartialDSTest::PEMesh::RegionHandle r;
//  r = mesh_->CreateEmptyRegion();
//  ASSERT_TRUE(r != NULL);
//  PartialDSTest::PEMesh::VertexHandle v;
//  v = mesh_->CreateIsolatedVertex(r);
//  ASSERT_TRUE(v != NULL);
//  PartialDSTest::PEMesh::EdgeHandle e1;
//  e1 = mesh_->CreateWireEdgeAndVertex(r->outer_shell(), v);
//
//  PartialDSTest::PEMesh::VertexHandle w;
//  w = mesh_->SplitEdgeCreateVertex(e1);
//  ASSERT_TRUE(mesh_->ValidateVertex(w));
//  ASSERT_TRUE(mesh_->ValidateVertex(v));
//  ASSERT_TRUE(mesh_->ValidateEdge(e1));
//  
//  mesh_->DeleteVertexJoinEdge(w, e1);
//  ASSERT_TRUE(mesh_->ValidateVertex(v));
//  ASSERT_TRUE(mesh_->ValidateEdge(e1));
}

TEST_F(PartialDSTest, TestMakeEdgeCycle) {
  PartialDSTest::PEMesh::RegionHandle r;
  r = mesh_->CreateEmptyRegion();
  ASSERT_TRUE(r != NULL);

  PartialDSTest::PEMesh::VertexHandle v[3];
  PartialDSTest::PEMesh::EdgeHandle e[3];
  PartialDSTest::PEMesh::ShellHandle shell;

  // Build a triangle of edges.
  mesh_->CreateIsolatedVertex(r, &v[0], &shell);
  e[0] = mesh_->CreateWireEdgeAndVertex(shell, v[0]);
  v[1] = e[0]->end_pvertex()->vertex();
  e[1] = mesh_->CreateWireEdgeAndVertex(shell, v[1]);
  v[2] = e[1]->end_pvertex()->vertex();
  // Deliberately orienting e[2] against e[0] & e[1].
  e[2] = mesh_->MakeEdgeCycle(shell, v[0], v[2]);
  
  ASSERT_TRUE(mesh_->ValidateEdge(e[0]));
  ASSERT_TRUE(mesh_->ValidateEdge(e[1]));
  ASSERT_TRUE(mesh_->ValidateEdge(e[2]));
  ASSERT_TRUE(mesh_->ValidateVertex(v[0]));
  ASSERT_TRUE(mesh_->ValidateVertex(v[1]));
  ASSERT_TRUE(mesh_->ValidateVertex(v[2]));

  // Validate the cycle.
  std::vector<PartialDSTest::PEMesh::EdgeHandle> cycle;
  cycle.push_back(e[0]);
  cycle.push_back(e[1]);
  cycle.push_back(e[2]);
  ASSERT_TRUE(PartialDSTest::PEMesh::ValidateEdgeCycle(cycle));

  // Take things apart, validating after each step.
  mesh_->DeleteEdgeCycle(e[2]);
  ASSERT_TRUE(mesh_->ValidateEdge(e[0]));
  ASSERT_TRUE(mesh_->ValidateEdge(e[1]));
  ASSERT_TRUE(mesh_->ValidateVertex(v[0]));
  ASSERT_TRUE(mesh_->ValidateVertex(v[1]));
  ASSERT_TRUE(mesh_->ValidateVertex(v[2]));

  mesh_->DeleteWireEdgeAndVertex(e[1], v[2]);
  ASSERT_TRUE(mesh_->ValidateEdge(e[0]));
  ASSERT_TRUE(mesh_->ValidateVertex(v[0]));
  ASSERT_TRUE(mesh_->ValidateVertex(v[1]));

  mesh_->DeleteWireEdgeAndVertex(e[0], v[1]);
  ASSERT_TRUE(mesh_->ValidateVertex(v[0]));

  mesh_->DeleteIsolatedVertex(v[0]);
  ASSERT_TRUE(r->IsEmpty());
  mesh_->DeleteEmptyRegion(r);
}
}  // namespace
