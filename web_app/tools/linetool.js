// Copyright 2010 The Ginsu Authors.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

/**
 * @fileoverview The Line Tool
 */

goog.provide('ginsu.tools.LineTool');

goog.require('goog.math.Coordinate');
goog.require('goog.math.Size');
goog.require('goog.math.Vec3');
goog.require('ginsu.events.EventType');
goog.require('ginsu.tools.Tool');

/**
 * Constructor for the LineTool object.
 * @extends {ginsu.tools.Tool}
 */
ginsu.tools.LineTool = function() {
  // Use the event name as the id, so that it will match the ids of various
  // UI elements like toolbar buttons.
  ginsu.tools.Tool.call(this, ginsu.events.EventType.LINE);
};
goog.inherits(ginsu.tools.LineTool, ginsu.tools.Tool);

/**
 * Method to start up the LineTool.
 * @param {!goog.math.Coordinate} startPoint 2D-point, usually the mouse-down
 *     point.
 * @param {!goog.math.Size} frameSize 2D-point representing the size of the
 *     element that encloses the virtual LineTool.
 */
ginsu.tools.LineTool.prototype.startAtPointInFrame =
    function(startPoint, frameSize) {
  window.console.log('startPoint: ' + startPoint + ' frameSize: ' + frameSize);
};

/**
 * Handle the drag START event.
 * @param {!ginsu.tools.ViewController} view The view controller that called
 *     this method.
 * @param {!goog.fx.DragEvent} dragStartEvent The START event that
 *     triggered this handler.
 */
ginsu.tools.LineTool.prototype.handleStartDrag =
    function(controller, dragStartEvent) {
  window.console.log('dragStartEvent: ' + dragStartEvent);
};

/**
 * Handle the drag DRAG event.
 * @param {!ginsu.controllers.ViewController} view The view controller that called
 *     this method.
 * @param {!goog.fx.DragEvent} dragEvent The DRAG event that triggered
 *     this handler.
 */
ginsu.tools.LineTool.prototype.handleDrag =
    function(controller, dragEvent) {
  window.console.log('dragEvent: ' + dragEvent.clientX + ' ' + dragEvent.clientY);
};

/**
 * Handle the drag END event.
 * @param {!ginsu.controllers.ViewController} view The view controller that called
 *     this method.
 * @param {!goog.fx.DragEvent} dragEndEvent The drag END event that
 *     triggered this handler.
 */
ginsu.tools.LineTool.prototype.handleEndDrag =
    function(controller, dragEndEvent) {
  window.console.log('dragEndEvent: ' + dragEndEvent);
};
