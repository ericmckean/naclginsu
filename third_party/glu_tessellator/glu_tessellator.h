// Copyright (c) 2010 The Ginsu Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#ifndef GINSU_TESSELLATOR_GLU_TESSELLATOR_H_
#define GINSU_TESSELLATOR_GLU_TESSELLATOR_H_

#ifdef __cplusplus
extern "C" {
#endif

// These are the essential defines and typedefs needed to build and use the 
// tessellator in Ginsu.
#define GLAPIENTRY
#define GLU_TESS_MAX_COORD 1.0e150
#define GLU_TESS_BEGIN                       100100
#define GLU_TESS_VERTEX                      100101
#define GLU_TESS_END                         100102
#define GLU_TESS_ERROR                       100103
#define GLU_TESS_EDGE_FLAG                   100104
#define GLU_TESS_COMBINE                     100105
#define GLU_TESS_BEGIN_DATA                  100106
#define GLU_TESS_VERTEX_DATA                 100107
#define GLU_TESS_END_DATA                    100108
#define GLU_TESS_ERROR_DATA                  100109
#define GLU_TESS_EDGE_FLAG_DATA              100110
#define GLU_TESS_COMBINE_DATA                100111

#define GLU_TESS_WINDING_ODD                 100130
#define GLU_TESS_WINDING_NONZERO             100131
#define GLU_TESS_WINDING_POSITIVE            100132
#define GLU_TESS_WINDING_NEGATIVE            100133
#define GLU_TESS_WINDING_ABS_GEQ_TWO         100134
#define GLU_TESS_WINDING_RULE                100140
#define GLU_TESS_BOUNDARY_ONLY               100141
#define GLU_TESS_TOLERANCE                   100142
#define GLU_TESS_MISSING_BEGIN_POLYGON       100151
#define GLU_TESS_MISSING_BEGIN_CONTOUR       100152
#define GLU_TESS_MISSING_END_POLYGON         100153
#define GLU_TESS_MISSING_END_CONTOUR         100154
#define GLU_TESS_COORD_TOO_LARGE             100155
#define GLU_TESS_NEED_COMBINE_CALLBACK       100156

#define GLU_INVALID_ENUM                     100900
#define GLU_INVALID_VALUE                    100901
#define GLU_OUT_OF_MEMORY                    100902

#define GL_TRIANGLES                         0x0004
#define GL_TRIANGLE_STRIP                    0x0005
#define GL_TRIANGLE_FAN                      0x0006

typedef unsigned int GLenum;
typedef double GLdouble;
typedef float GLfloat;
typedef unsigned char GLboolean;

// The tessellator and associated types & functions.
#ifdef __cplusplus
class GLUtesselator;
typedef void (*CallbackFunc )();
#else
typedef struct GLUtesselator GLUtesselator;
typedef void (*CallbackFunc )(void);
#endif

extern GLUtesselator* gluNewTess();
extern void gluDeleteTess (GLUtesselator* tess);
extern void gluTessBeginContour(GLUtesselator* tess);
extern void gluTessBeginPolygon(GLUtesselator* tess, void* data);
extern void gluTessCallback(GLUtesselator* tess, GLenum which, CallbackFunc);
extern void gluTessEndContour(GLUtesselator* tess);
extern void gluTessEndPolygon(GLUtesselator* tess);
extern void gluTessNormal (GLUtesselator* tess,
                           GLdouble valueX, GLdouble valueY, GLdouble valueZ);
extern void gluTessProperty(GLUtesselator* tess, GLenum which, GLdouble data);
extern void gluTessVertex(GLUtesselator* tess, GLdouble *location,
                          void* data);

#ifdef __cplusplus
}
#endif

#endif  // GINSU_TESSELLATOR_GLU_TESSELLATOR_H_
