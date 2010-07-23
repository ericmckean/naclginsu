// Copyright 2010 The Ginsu Authors.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

/**
 * @fileoverview  Implement the view controller class, ViewController, that
 * owns the Ginsu plugin and wraps JavaScript bridge calls to it.  This
 * class also handles certain UI interactions, such as mouse drags and keyboard
 * shortcuts.
 */

goog.provide('ginsu.controllers.ViewController');

goog.require('goog.Disposable');
goog.require('goog.dom');
goog.require('goog.events.EventTarget');
goog.require('goog.fx.DragEvent');
goog.require('goog.object');
goog.require('goog.style');
goog.require('ginsu.events.Dragger');
goog.require('ginsu.events.EventType');

/**
 * Constructor for the ViewController class.  This class encapsulates the
 * Ginsu plugin in a view.  It also produces some UI events, such as mouse
 * drags.  Objects, like tools, that respond to those events can then query the
 * plugin.
 * @param {!Object} element The DOM element that represents a ViewController.
 *     If undefined, an error is thrown.  The element is expected to conform to
 *     the Ginsu JavaScript bridge API (it has methods getValueForKey() and
 *     setValueForKey()).
 * @constructor
 * @extends {goog.events.EventTarget}
 */
ginsu.controllers.ViewController = function(element) {
  goog.events.EventTarget.call(this);
  /**
   * The element containing the Ginsu NaCl module that corresponds to
   * this object instance.  If null or undefined, an exception is thrown.
   * @type {Element}
   * @private
   */
  if (!element) {
    throw new Error('ViewController() requries a valid element');
  }
  // The container is the containing DOM element.
  this.container_ = element;
  
  /* TODO(dspringer): Enbale this error checking once it's implemented in Ginsu.
  if (typeof this.container_['getValueForKey'] != 'function' ||
      typeof this.container_['setValueForKey'] != 'function') {
    throw new Error(
        'ViewController() Element does not implement getValueForKey() ' +
        'and setValueForKey()');
  }
  */

  /**
   * Mouse drag event object.
   * @type {ginsu.events.Dragger}
   * @private
   */
  this.dragListener_ = new ginsu.events.Dragger(element);
  // Hook up a Dragger and listen to the drag events coming from it, then
  // reprocess the events as Ginsu DRAG events.
  goog.events.listen(this.dragListener_, goog.fx.Dragger.EventType.START,
      this.handleStartDrag_, false, this);
  goog.events.listen(this.dragListener_, goog.fx.Dragger.EventType.END,
      this.handleEndDrag_, false, this);
  goog.events.listen(this.dragListener_, goog.fx.Dragger.EventType.DRAG,
      this.handleDrag_, false, this);
};
goog.inherits(ginsu.controllers.ViewController, goog.events.EventTarget);

/**
 * Override of disposeInternal() to unhook all the listeners and dispose
 * of retained objects.
 * @override
 */
ginsu.controllers.ViewController.prototype.disposeInternal = function() {
  ginsu.controllers.ViewController.superClass_.disposeInternal.call(this);
  goog.events.unlisten(this.dragListener_, goog.fx.Dragger.EventType.START,
      this.handleStartDrag_, false, this);
  goog.events.unlisten(this.dragListener_, goog.fx.Dragger.EventType.DRAG,
      this.handleDrag_, false, this);
  goog.events.unlisten(this.dragListener_, goog.fx.Dragger.EventType.END,
      this.handleEndDrag_, false, this);
  this.dragListener_ = null;
  this.container_ = null;
};

/**
 * Method to get the bounding frame rectangle for the DOM container.
 * @return {Object} The bounding frame expressed in the coordinate space
 *     of this view.
 */
ginsu.controllers.ViewController.prototype.frame = function() {
  var containerSize = goog.style.getSize(this.container_);
  return {x: 0, y: 0, width: containerSize.width, height: containerSize.height};
};

/**
 * Method to get a value for a key from the Ginsu plugin.  This method
 * forwards the request to the Ginsu plugin via the JavaScript bridge.
 * Throws an exception if the key does not represent a property defined in
 * the Ginsu plugin.
 * @param {!String} key The name of the property in the Ginsu plugin.
 *     Cannot be null or undefined.
 * @return {!Object} The value for the property.  Can be null.
 */
ginsu.controllers.ViewController.prototype.getValueForKey = function(key) {
  return this.container_.getValueForKey(key);
};

/**
 * Method to get a value for a key from the Ginsu plugin.  This method
 * forwards the request to the Ginsu plugin via the JavaScript bridge.
 * Throws an exception if the key does not represent a property defined in
 * the Ginsu plugin.
 * @param {!Object} value The value for the property.  Can be null, but must be
 *     defined.
 * @param {!String} key The name of the property in the Ginsu plugin.
 *     Cannot be null or undefined.
 */
ginsu.controllers.ViewController.prototype.setValueForKey = function(key, value) {
  this.container_.setValueForKey(key, value);
};

/**
 * Handle the drag START event: dispatch a DRAG_START event from this view
 * controller.
 * @param {!goog.fx.DragEvent} dragStartEvent The START event that
 *     triggered this handler.
 * @private
 */
ginsu.controllers.ViewController.prototype.handleStartDrag_ =
    function(dragStartEvent) {
  // Transform the event into a DRAG_START that comes from this target.
  var dragEventClone = goog.object.clone(dragStartEvent);
  dragStartEvent.stopPropagation();
  dragEventClone.target = this;
  dragEventClone.type = ginsu.events.EventType.DRAG_START;
  this.dispatchEvent(dragEventClone);
};

/**
 * Handle the DRAG event: dispatch a DRAG event from this view controller.
 * @param {!goog.fx.DragEvent} dragEvent The DRAG event that triggered this
 *     handler.
 * @private
 */
ginsu.controllers.ViewController.prototype.handleDrag_ = function(dragEvent) {
  var dragEventClone = goog.object.clone(dragEvent);
  dragEvent.stopPropagation();
  dragEventClone.target = this;
  dragEventClone.type = ginsu.events.EventType.DRAG;
  this.dispatchEvent(dragEventClone);
};

/**
 * Handle the drag END event: dispatch a DRAG_END event from this view
 * controller.
 * @param {!goog.fx.DragEvent} dragEndEvent The END event that triggered this
 *     handler.
 * @private
 */
ginsu.controllers.ViewController.prototype.handleEndDrag_ =
    function(dragEndEvent) {
  var dragEventClone = goog.object.clone(dragEndEvent);
  dragEndEvent.stopPropagation();
  dragEventClone.target = this;
  dragEventClone.type = ginsu.events.EventType.DRAG_END;
  this.dispatchEvent(dragEventClone);
};

/**
 * Handle keyboard shortcut events.  These get transformed into Ginsu app
 * events and re-dispatched.
 * @param {!goog.ui.KeyboardShortcutEvent} keyboardEvent The SHORTCUT event
 * that triggered this handler.
 * @private
 */
ginsu.controllers.ViewController.prototype.handleKeyboard_ =
    function(keyboardEvent) {
  var eventClone = goog.object.clone(keyboardEvent);
  keyboardEvent.stopPropagation();
  this.dispatchEvent(new ginsu.events.Event(ginsu.events.EventType.ACTION, this,
      keyboardEvent.identifier));
};
