// Copyright 2010 The Ginsu Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#ifndef C_SALT_NOTIFICATION_PTRS_H_
#define C_SALT_NOTIFICATION_PTRS_H_

// A convenience wrapper for a shared Notification pointer
// type.  As more smart pointer types are needed, add them here.

#include "boost/shared_ptr.hpp"
#include "boost/signals2.hpp"

namespace c_salt {

typedef boost::signals2::signal<void (void*)> Notification;
typedef boost::shared_ptr<Notification> SharedNotification;

}  // namespace c_salt

#endif  // C_SALT_NOTIFICATION_PTRS_H_
