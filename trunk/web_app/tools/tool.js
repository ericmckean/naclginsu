// Copyright 2010 The Ginsu Authors.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

/**
 * @fileoverview Main superclass for all the tools.  Each tool has an id, and
 * fires an event when activated or deactivated.
 */

goog.provide('ginsu.tools.Tool');

goog.require('goog.events.EventTarget');

/**
 * Constructor for the GSTool class.  This provides a default ID ('gs_tool')
 * and some default implementations for the informal GSTool protocol.
 * @param {?string} opt_id The id for this tool.  If null or undefined, the
 *     new tool instance gets the default id 'gs_tool'.
 * @constructor
 * @extends {goog.events.EventTarget}
 */
ginsu.tools.Tool = function(opt_id) {
  goog.events.EventTarget.call(this);

  /**
   * The tool id.  Initialized to be |opt_id| or the default 'gs_tool'.
   * @type {string}
   * @private
   */
  this.toolId_ = opt_id || ginsu.tools.Tool.DEFAULT_TOOL_ID;
};
goog.inherits(ginsu.tools.Tool, goog.events.EventTarget);

/**
 * The default tool id.  This is the id for tools created using the default
 * constructor.
 * @type {string}
 */
ginsu.tools.Tool.DEFAULT_TOOL_ID = 'gs_tool';

/**
 * Override of disposeInternal() to dispose of retained objects.
 * @override
 */
ginsu.tools.Tool.prototype.disposeInternal = function() {
  ginsu.tools.Tool.superClass_.disposeInternal.call(this);
  this.toolId_ = null;
};

/**
 * Accessor for the |toolId_| property.
 * @return {string} The id of this tool instance, guaranteed not to be null.
 */
ginsu.tools.Tool.prototype.toolId = function() {
  return this.toolId_;
};

/**
 * Mutator for the |toolID_| property.
 * @param {string} toolId The new tool id.  If null or undefined, this method
 *     has no effect and silently returns.
 */
ginsu.tools.Tool.prototype.setToolId = function(toolId) {
  if (!toolId)
    return;
  this.toolId_ = toolId;
};

/**
 * Default activate method.  This posts an activation event with the tool ID
 * as the name.
 */
ginsu.tools.Tool.prototype.activate = function() {
  this.dispatchEvent(
      new ginsu.events.Event(ginsu.tools.Tool.EventType.ACTIVATE,
      this, this.toolId()));
};

/**
 * Default deactivate method.  This posts a deactivation event with the tool ID
 * as the name.
 */
ginsu.tools.Tool.prototype.deactivate = function() {
  this.dispatchEvent(
      new ginsu.events.Event(ginsu.tools.Tool.EventType.DEACTIVATE, this,
      this.toolId()));
};

/**
 * Event types dispatched by a tool during activation and deactivation.
 * @enum {string}
 */
ginsu.tools.Tool.EventType = {
  ACTIVATE: 'gstool_activate',
  DEACTIVATE: 'gstool_deactivate'
};
