// Copyright 2010 The Ginsu Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#include <iostream>
#include <string>

#include "c_salt/notification_center.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using c_salt::NotificationCenter;

namespace {
const char* kTestNotification = "Test Notification";
const char* kTestNotification2 = "Test Notification 2";
}  // namespace

// A dummy class that implements a notification response method.  This method
// is expected to be called from a notification.
class Observer {
 public:
  Observer() : call_count_(0) {}
  void NotificationHandler(void* notification) {
    ++call_count_;
  }

  int call_count() const {
    return call_count_;
  }

 private:
  int call_count_;
};

class NotificationCenterTest : public ::testing::Test {
 protected:
  virtual void SetUp() {}
  NotificationCenter* default_center_;
};

// Verify the default center is the same all the time.
TEST_F(NotificationCenterTest, DefaultCenter) {
  NotificationCenter* default_center_1 = NotificationCenter::DefaultCenter();
  NotificationCenter* default_center_2 = NotificationCenter::DefaultCenter();
  EXPECT_EQ(default_center_1, default_center_2);
}

// Add an observer and verify that it gets called when a notification is posted.
TEST_F(NotificationCenterTest, PostNotification) {
  Observer observer;
  NotificationCenter center;
  center.AddObserver(kTestNotification,
                     &observer,
                     &Observer::NotificationHandler);
  center.PostNotification(kTestNotification, NULL);
  EXPECT_EQ(1, observer.call_count());
}

// Add a couple of observers and verify that they both get called when a
// notification is posted.
TEST_F(NotificationCenterTest, Post2Notifications) {
  Observer observer;
  Observer observer2;
  NotificationCenter center;
  center.AddObserver(kTestNotification,
                     &observer,
                     &Observer::NotificationHandler);
  center.AddObserver(kTestNotification,
                     &observer2,
                     &Observer::NotificationHandler);
  center.PostNotification(kTestNotification, NULL);
  EXPECT_EQ(1, observer.call_count());
  EXPECT_EQ(1, observer2.call_count());
}

// Verify that observers can be added after an initial post.
TEST_F(NotificationCenterTest, AddObserver) {
  Observer observer;
  NotificationCenter center;
  center.AddObserver(kTestNotification,
                     &observer,
                     &Observer::NotificationHandler);
  center.AddObserver(kTestNotification2,
                     &observer,
                     &Observer::NotificationHandler);
  center.PostNotification(kTestNotification, NULL);
  center.PostNotification(kTestNotification2, NULL);
  EXPECT_EQ(2, observer.call_count());
  Observer observer2;
  center.AddObserver(kTestNotification,
                     &observer2,
                     &Observer::NotificationHandler);
  center.PostNotification(kTestNotification, NULL);
  EXPECT_EQ(3, observer.call_count());
  EXPECT_EQ(1, observer2.call_count());
}

// Verify that observers can be removed.  This depends on the AddObserver test.
TEST_F(NotificationCenterTest, RemoveObserver) {
  Observer observer;
  Observer observer2;
  NotificationCenter center;
  center.AddObserver(kTestNotification,
                     &observer,
                     &Observer::NotificationHandler);
  center.AddObserver(kTestNotification,
                     &observer2,
                     &Observer::NotificationHandler);
  center.AddObserver(kTestNotification2,
                     &observer,
                     &Observer::NotificationHandler);
  center.PostNotification(kTestNotification, NULL);
  center.PostNotification(kTestNotification2, NULL);
  EXPECT_EQ(2, observer.call_count());
  EXPECT_EQ(1, observer2.call_count());
  // Remove |observer| from the notifications it observes.
  center.RemoveObserver(&observer, &Observer::NotificationHandler);
  center.PostNotification(kTestNotification, NULL);
  // There are no more observers of |kTestNotification2|.
  center.PostNotification(kTestNotification2, NULL);
  EXPECT_EQ(2, observer.call_count());  // Should not increment.
  EXPECT_EQ(2, observer2.call_count());  // Still observes |kTestNotification|.
  // Remove all remaining observers of |kTestNotification|.
  center.RemoveObserverFromNotification(&observer2,
                                        &Observer::NotificationHandler,
                                        kTestNotification);
  center.PostNotification(kTestNotification, NULL);
  EXPECT_EQ(2, observer.call_count());  // Should not increment.
  EXPECT_EQ(2, observer2.call_count());  // Should not increment.
}

// Remove a notification, verify that all its observers get disconnected.
TEST_F(NotificationCenterTest, RemoveNotification) {
  Observer observer;
  Observer observer2;
  NotificationCenter center;
  center.AddObserver(kTestNotification,
                     &observer,
                     &Observer::NotificationHandler);
  center.AddObserver(kTestNotification,
                     &observer2,
                     &Observer::NotificationHandler);
  center.PostNotification(kTestNotification, NULL);
  // Make sure the connections are made and functioning.
  EXPECT_EQ(1, observer.call_count());
  EXPECT_EQ(1, observer2.call_count());
  center.RemoveNotification(kTestNotification);
  // Make sure no further handling of |kTestNotification| happens.
  center.PostNotification(kTestNotification, NULL);
  EXPECT_EQ(1, observer.call_count());  // Should not increment.
  EXPECT_EQ(1, observer2.call_count());
}

