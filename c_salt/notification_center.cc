// Copyright 2010 The Ginsu Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#include "c_salt/notification_center.h"

namespace c_salt {

// The class singleton and its mutex.
// TODO(c_salt_authors): This mutex should really be some kind of scoped
// mutex class, such as boost::mutex and boost::lock_guard.
static pthread_mutex_t notification_center_mutex = PTHREAD_MUTEX_INITIALIZER;
static NotificationCenter* notification_center = NULL;

NotificationCenter* NotificationCenter::DefaultCenter() {
  pthread_mutex_lock(&notification_center_mutex);
  if (notification_center == NULL) {
    notification_center = new NotificationCenter();
  }
  pthread_mutex_unlock(&notification_center_mutex);
  return notification_center;
}

NotificationCenter::NotificationCenter() {
  pthread_mutex_init(&mutex_, NULL);
}

NotificationCenter::~NotificationCenter() {
  pthread_mutex_destroy(&mutex_);
}

void NotificationCenter::RemoveNotification(
    const std::string& notification_name) {
  pthread_mutex_lock(&mutex_);
  NotificationDict::iterator it = notifications_.find(notification_name);
  if (it != notifications_.end()) {
    notifications_.erase(it);
  }
  pthread_mutex_unlock(&mutex_);
}

void NotificationCenter::PostNotification(const std::string& notification_name,
                                          void* notification_data) {
  pthread_mutex_lock(&mutex_);
  const NotificationDict::const_iterator it =
      notifications_.find(notification_name);
  SharedNotification notification_signal;
  if (it != notifications_.end() && !(it->second)->empty()) {
    // Make a local copy of the notification signal, in the event that the
    // notification gets removed before the signal can be fired (which will
    // make the iterator invalid).
    notification_signal = it->second;
  }
  // Signal the observers.  boost makes all this thread-safe, so it is ok to
  // unlock the mutex before signaling the observers.  Note that it's possible
  // for another thread to disconnect the signal before this gets reached, in
  // which case the signal will not fire.
  pthread_mutex_unlock(&mutex_);
  if (notification_signal != NULL) {
    (*notification_signal)(notification_data);
  }
}

}  // namespace ginsu
