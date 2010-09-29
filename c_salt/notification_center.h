// Copyright 2010 The Ginsu Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#ifndef C_SALT_NOTIFICATION_CENTER_H_
#define C_SALT_NOTIFICATION_CENTER_H_

#include <pthread.h>

#include <cstddef>
#include <map>
#include <string>

#include "boost/noncopyable.hpp"
#include "boost/signals2.hpp"
#include "c_salt/notification_ptrs.h"

namespace c_salt {
// NotificationCenter is a simple mechanism for broadcasting notifications
// within a process.
// Objects register with the NotificationCenter as observers of notifications
// using string identifiers to specify the notification of interest.  Observers
// must subsequently remove themselves as observers if they go out of scope
// before the NotificationCenter.
class NotificationCenter : public boost::noncopyable {
 public:
  // Return a singleton used by the whole app.  This call is mutex locked.
  static NotificationCenter* DefaultCenter();

  NotificationCenter();
  ~NotificationCenter();

  // Add an observer with it's method to invoke.
  // The observer must be created in the same thread as the NotificationCenter.
  // TODO(c_salt_authors):  Allow for a nicer observer function type (details
  // TBD).
  template <typename T>
  void AddObserver(const std::string& notification_name,
                   T* observer,
                   void (T::*mem_func)(void*));

  // Remove an observer from all its notifications in the dispatch table.
  template <typename T>
  void RemoveObserver(T* observer,
                      void (T::*mem_func)(void*));
  // Remove an observer from the specified notification.
  template <typename T>
  void RemoveObserverFromNotification(T* observer,
                                      void (T::*mem_func)(void*),
                                      const std::string& notification_name);
  // Remove all observers from a specified notification and then remove the
  // notification.  Future calls to PostNotification() to |notification_name|
  // will fail.
  void RemoveNotification(const std::string& notification_name);

  // Post a notification to be dispatched, specifying notification-specific
  // data.
  void PostNotification(const std::string& notification_name,
                        void* notification_data);

 private:
  typedef boost::signals2::connection Connection;
  typedef std::map<std::string, SharedNotification> NotificationDict;

  mutable pthread_mutex_t mutex_;
  NotificationDict notifications_;
};

template <typename T>
void NotificationCenter::AddObserver(const std::string& notification_name,
                                     T* observer,
                                     void (T::*mem_func)(void*)) {
  pthread_mutex_lock(&mutex_);
  SharedNotification notification_signal;
  std::pair<NotificationDict::iterator, bool> inserted =
    notifications_.insert(NotificationDict::value_type(notification_name,
                                                       notification_signal));
  // If this is a newly inserted notification signal, then reset the
  // value with a new signal object.
  if (inserted.second) {
    inserted.first->second.reset(new Notification());
  }
  // In all cases, copy the shared_ptr value to a local variable so that
  // other threads cannot invalidate it.
  notification_signal = inserted.first->second;
  // Boost handles all the thread safety for connecting, so unlock our mutex.
  pthread_mutex_unlock(&mutex_);
  notification_signal->connect(boost::bind(mem_func, observer, _1));
}

template <typename T>
void NotificationCenter::RemoveObserver(T* observer,
                                        void (T::*mem_func)(void*)) {
  pthread_mutex_lock(&mutex_);
  NotificationDict::iterator it = notifications_.begin();
  const NotificationDict::const_iterator end = notifications_.end();
  for (; it != end; ++it) {
    (it->second)->disconnect(boost::bind(mem_func, observer, _1));
  }
  pthread_mutex_unlock(&mutex_);
}

template <typename T>
void NotificationCenter::RemoveObserverFromNotification(
    T* observer,
    void (T::*mem_func)(void*),
    const std::string& notification_name) {
  pthread_mutex_lock(&mutex_);
  const NotificationDict::iterator it =
      notifications_.find(notification_name);
  if (it != notifications_.end()) {
    (it->second)->disconnect(boost::bind(mem_func, observer, _1));
  }
  pthread_mutex_unlock(&mutex_);
}

}  // namespace ginsu
#endif  // C_SALT_NOTIFICATION_CENTER_H_
