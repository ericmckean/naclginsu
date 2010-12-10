// Copyright 2010 The Ginsu Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#ifndef VIEW_OPENGL_VIEW_H_
#define VIEW_OPENGL_VIEW_H_

#include "boost/noncopyable.hpp"
#include "c_salt/opengl_context_ptrs.h"

namespace c_salt {
class Instance;
class Notification;
class OpenGLContext;
}  // namespace c_salt

namespace ginsu {
namespace view {

// Provide the drawing support for an OpenGLContext.  Becomes an observer of
// OpenGLContext for notifications about intializing and rendering. Note that
// this class doesn't do much by default; all it does is maintain an
// association with an OpenGLContext.  To do any rendering or OpenGL graphics,
// you need to write a subclass of OpenGLView and implement RenderOpenGL().
// You can perform one-time OpenGL initializations in InitializeOpenGL() and
// perform clean-up in ReleaseOpenGL().  You should also implement
// ResizeOpenGL() to handle changing 2D viewports (usually you will modify the
// projection stack in this method).
class OpenGLView : public boost::noncopyable {
 public:
  // Ctor creates a default OpenGLContext.  You can change the context by
  // calling the SetOpenGLContext() method.
  explicit OpenGLView(const c_salt::Instance& instance);

  // Dtor releases the associated context, which in turn might cause
  // ReleaseOpenGL() to get called (if |this| holds the last reference to the
  // context).
  virtual ~OpenGLView();

  // This method is called by the c_salt rendering pipeline exactly once when
  // |context| first made the current context.  You do not call this method
  // directly. Subclasses can specialize this to perform OpenGL set-up code.
  // This can include compiling and loading shaders, etc.  Before this call is
  // made, the |context| is guaranteed to be the current context and is ready
  // to process OpenGL drawing commands.
  virtual void InitializeOpenGL(const c_salt::OpenGLContext& context) = 0;

  // This method is called exactly once by the associated context when it is
  // about to be deallocated.  You do not call this method directly.  Subclasses
  // can specialize this method to do graphics shut-down procedures.
  virtual void ReleaseOpenGL(const c_salt::OpenGLContext& context) = 0;

  // Subclasses specialize this code to draw OpenGL.  The associated context
  // calls this method; you do not call it directly.  |context| is guaranteed
  // to be current before this call is made.
  virtual void RenderOpenGL(const c_salt::OpenGLContext& context) = 0;

  // Called by the c_salt rendering pipeline whenever the size of the view
  // changes.  Subclasses can specialize this method to recompute viewport-
  // dependent things like the perspective projection transform.  Then this
  // method is called, width() and height() will return the new viewport size
  // values.  You do not need to call this method directly, it is called by
  // the c_salt rendering pipeline.
  virtual void ResizeViewport() = 0;

  // Call this to indicate that the view needs to be refreshed.  This in turn
  // tells the associated context to issue a repaint request.  If |flag| is
  // |false|, then this view will be skipped during rendering.
  void SetNeedsRedraw(bool flag);

  // Clamp the size to 1x1 pixels.  Call ResizeViewport() once the new size
  // information has ben set.
  void SetSize(int32_t width, int32_t height);

  // Accessor and mutator for the OpenGL context.  Setting the context clears
  // and releases the old one, then attaches this view to the new one.  The
  // first time a newly attached context is made current, the InitializeOpenGL()
  // method will be called once.
  c_salt::SharedOpenGLContext opengl_context() const {
    return context_;
  }

  // The ctor creates a default OpenGLContext associated with this OpenGLView
  // instance.  This method will release the default OpenGLContext and
  // replace it with |context|.  Releasing the default OpenGLContext might
  // cause the ReleaseOpenGL() method to get called with the old context, if
  // |this| holds the last reference ot the context.
  void SetOpenGLContext(const c_salt::SharedOpenGLContext& context);

  int32_t width() const {
    return width_;
  }
  int32_t height() const {
    return height_;
  }

 private:
  // Notificaiton handlers.  These wrap the notificaitons and turn them into
  // calls to the subclass API.
  void OpenGLDidInitialize(const c_salt::Notification& notification);
  void OpenGLWillDelete(const c_salt::Notification& notification);
  void OpenGLDoRender(const c_salt::Notification& notification);

  int32_t width_;
  int32_t height_;
  bool needs_redraw_;
  c_salt::SharedOpenGLContext context_;
};

}  // namespace view
}  // namespace ginsu

#endif  // VIEW_OPENGL_VIEW_H_
