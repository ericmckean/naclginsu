// Copyright 2010 The Ginsu Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#include "view/opengl_view.h"

#include <algorithm>
#include <string>

#include "c_salt/instance.h"
#include "c_salt/notification_center.h"
#include "c_salt/opengl_context.h"

namespace ginsu {
namespace view {

OpenGLView::OpenGLView(const c_salt::Instance& instance)
    : width_(1), height_(1), needs_redraw_(true) {
  c_salt::SharedOpenGLContext
      shared_context(new c_salt::OpenGLContext(instance));
  SetOpenGLContext(shared_context);
}

OpenGLView::~OpenGLView() {
}

void OpenGLView::SetOpenGLContext(const c_salt::SharedOpenGLContext& context) {
  if (context_ != NULL) {
    c_salt::NotificationCenter* default_center =
        c_salt::NotificationCenter::DefaultCenter(context_->instance());
    default_center->RemoveSubscriberFromNotification(
        this, c_salt::OpenGLContext::kInitializeOpenGLContextNotification);
    default_center->RemoveSubscriberFromNotification(
        this, c_salt::OpenGLContext::kDeleteOpenGLContextNotification);
    default_center->RemoveSubscriberFromNotification(
      this, c_salt::OpenGLContext::kRenderOpenGLContextNotification);
  }
  context_ = context;  // Might cause a kDeleteOpenGLContext notification.
  if (context_ != NULL) {
    c_salt::NotificationCenter* default_center =
        c_salt::NotificationCenter::DefaultCenter(context_->instance());
    // Observe the context's notifications.
    const std::string& context_name = context_->instance_name();
    default_center->AddSubscriber(
        c_salt::OpenGLContext::kInitializeOpenGLContextNotification,
        this,
        &OpenGLView::OpenGLDidInitialize,
        context_name);
    default_center->AddSubscriber(
        c_salt::OpenGLContext::kDeleteOpenGLContextNotification,
        this,
        &OpenGLView::OpenGLWillDelete,
        context_name);
    default_center->AddSubscriber(
        c_salt::OpenGLContext::kRenderOpenGLContextNotification,
        this,
        &OpenGLView::OpenGLDoRender,
        context_name);
  }
}

void OpenGLView::SetNeedsRedraw(bool flag) {
  needs_redraw_ = flag;
  if (needs_redraw_) {
    // Signal the associated context to issue a repaint request.
    context_->RenderContext();
  }
}

void OpenGLView::SetSize(int32_t width, int32_t height) {
  width_ = std::max(static_cast<int32_t>(1), width);
  height_ = std::max(static_cast<int32_t>(1), height);
  assert(context_.get());
  if (context_.get() && context_->is_valid())
    context_->MakeContextCurrent();
  ResizeViewport();
}

void OpenGLView::OpenGLDidInitialize(const c_salt::Notification& notification) {
  if (context_ == NULL)
    return;
  InitializeOpenGL(*(context_.get()));
}

void OpenGLView::OpenGLWillDelete(const c_salt::Notification& notification) {
  if (context_ == NULL)
    return;
  ReleaseOpenGL(*(context_.get()));
}

void OpenGLView::OpenGLDoRender(const c_salt::Notification& notification) {
  if (context_ == NULL)
    return;
  RenderOpenGL(*(context_.get()));
}

}  // namespace view
}  // namespace ginsu
