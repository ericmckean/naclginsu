/*
 *  test_vertex.cc
 *  ginsu
 *
 *  Created by Georges Winkenbach on 7/30/10.
 *  Copyright 2010 Google Inc. All rights reserved.
 *
 */

#include <CGAL/Simple_cartesian.h>
#include "test_vertex.h"
#include "partialds.h"

namespace ginsu {
namespace geometry {

typedef CGAL::Simple_cartesian<double>    Kernel;
typedef Kernel::Point_3                   Point_3;
typedef PartialDS<Kernel>                 Model;

static void test() {
  Model model;
  
}

}
}
