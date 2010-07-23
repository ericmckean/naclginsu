// Copyright 2010 The Ginsu Authors.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

/**
 * @fileoverview  Handle keyboard shortcuts by using the mechanism in
 * goog.ui.KeyboardShortcutHandler, then transforming the resulting SHORTCUT
 * events into Ginsu ACTION events.
 * .
 */

goog.provide('ginsu.controllers.KeyboardHandler');

goog.require('goog.dom');
goog.require('goog.events.EventTarget');
goog.require('goog.object');
goog.require('goog.ui.KeyboardShortcutHandler');
goog.require('ginsu.events.Dragger');
goog.require('ginsu.events.EventType');

/**
 * Constructor for the KeyboardHandler class.  This class listens for the
 * SHORTCUT events generated through the normal KeyboardShortcutHandler
 * mechanism, then transforms and re-dispatches those events as grayskull
 * ACTION events.
 * @param {?goog.events.EventTarget} opt_element The DOM element that receives
 *     the keyboard events.  If undefined, the |document| element is used.
 * @constructor
 * @extends {goog.ui.KeyboardShortcutHandler}
 */
ginsu.controllers.KeyboardHandler = function(opt_element) {
  goog.ui.KeyboardShortcutHandler.call(this, opt_element || document);
  goog.events.listen(this,
      goog.ui.KeyboardShortcutHandler.EventType.SHORTCUT_TRIGGERED,
      this.handleKeyboard_, false, this);
};
goog.inherits(ginsu.controllers.KeyboardHandler,
              goog.ui.KeyboardShortcutHandler);

/**
 * Override of disposeInternal() to unhook all the listeners.
 * @override
 */
ginsu.controllers.KeyboardHandler.prototype.disposeInternal = function() {
  goog.events.unlisten(this,
      goog.ui.KeyboardShortcutHandler.EventType.SHORTCUT_TRIGGERED,
      this.handleKeyboard_, false, this);
  ginsu.controllers.KeyboardHandler.superClass_.disposeInternal.call(this);
};

/**
 * Handle keyboard shortcut events.  These get transformed into grayskull app
 * events and re-dispatched.
 * @param {!goog.ui.KeyboardShortcutEvent} keyboardEvent The SHORTCUT event
 *     that triggered this handler.
 * @private
 */
ginsu.controllers.KeyboardHandler.prototype.handleKeyboard_ =
    function(keyboardEvent) {
  var eventClone = goog.object.clone(keyboardEvent);
  keyboardEvent.stopPropagation();
  keyboardEvent.preventDefault();
  this.dispatchEvent(new ginsu.events.Event(ginsu.events.EventType.ACTION, this,
      keyboardEvent.identifier));
};
