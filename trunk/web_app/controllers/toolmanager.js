// Copyright 2010 The Ginsu Authors.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

/**
 * @fileoverview  Implement the tool manager.  This class keeps a stack of
 * tools, the active tool is the top of this stack.  A stack is used so things
 * like the orbit tool can be temporarily activated with (say) the middle
 * mouse button then the original tool restored (with a pop) when the button is
 * released.
 *
 * The fact that tools are managed with a stack is hidden from the API (instead
 * of 'push' and 'pop' there are 'activate' and 'deactivate').  This is so that
 * if a stack is no longer the best implementation it can change easily without
 * affecting consumers of this class.
 */

goog.provide('ginsu.controllers.ToolManager');

goog.require('goog.Disposable');
goog.require('goog.structs.Map');
goog.require('ginsu.events.Event');
goog.require('ginsu.events.EventType');

/**
 * This class maintains a stack of tools.  A tool is a UI element that has
 * certain behavior, for example the push-pull tool.  Tool activation happens
 * outside this class, either through various UI widgets (like the toolbar) or
 * via other user events, such as pressing the middle mouse button (which
 * pushes the orbit tool).  The GCViewController owns an instance of this class.
 * @constructor
 * @extends {goog.events.EventTarget}
 */
ginsu.controllers.ToolManager = function() {
  goog.events.EventTarget.call(this);

  /**
   * The tool registry, a dictionary whose keys are the tool IDs and values are
   * the tool objects.
   * @type {goog.structs.Map.<string, ginsu.controllers.Tool>}
   * @private
   */
  this.toolDictionary_ = new goog.structs.Map();

  /**
   * The tool stack used to manage the currently active tool.
   * @type {Array.<Object>}
   * @private
   */
  this.toolStack_ = [];
};
goog.inherits(ginsu.controllers.ToolManager, goog.events.EventTarget);

/**
 * Override of disposeInternal() to dispose of retained objects.
 * @override
 */
ginsu.controllers.ToolManager.prototype.disposeInternal = function() {
  ginsu.controllers.ToolManager.superClass_.disposeInternal.call(this);
  // Deactivate all the tools, then clear out the stack.
  goog.array.forEach(this.toolStack_, function(tool) {
    if (tool.deactivate)
      tool.deactivate();
  }, this);
  this.toolStack_.length = 0;
  this.toolDictionary_.clear();
  this.toolDictionary_ = null;
};

/**
 * Add a tool to the internal dictionary.  If a tool with the same key already
 * exists, then the old tool is removed and the new one put in its place.
 * The key is the tool's id.
 * @param {!ginsu.tools.Tool} tool The tool to add to the dictionary.
 *     Cannot be null.
 */
ginsu.controllers.ToolManager.prototype.addTool = function(tool) {
  this.toolDictionary_.set(tool.toolId(), tool);
};

/**
 * Remove a tool to the internal dictionary.  If the tool does not exist, this
 * method does nothing.  The key is the tool's id.  The tool is silently
 * removed from the tool stack; note that if the tool is the active tool no
 * deactivate methods are called.  It is the caller's responsibility to
 * deactivate a tool before removing it.
 * @param {!ginsu.tools.Tool} tool The tool to remove from the dictionary.
 *     Cannot be null.
 */
ginsu.controllers.ToolManager.prototype.removeTool = function(tool) {
  goog.array.remove(this.toolStack_, tool);
  this.toolDictionary_.remove(tool.toolId());
};

/**
 * Get the currently active tool.  This is just the tool on the top of the
 * stack.  Returns null if the stack is empty.
 * @return {Object} The active tool.
 */
ginsu.controllers.ToolManager.prototype.activeTool = function() {
  var stackSize = this.toolStack_.length;
  if (stackSize > 0) {
    return this.toolStack_[stackSize - 1];
  }
  return null;
};

/**
 * Method to activate a tool.  This deactivates the current tool (top-of-stack)
 * then pushes |tool| onto the stack, and then activates it.  Tool activation
 * and deactivation is done by calling the activate() and deactivate() methods
 * on the tool, if the tool responds to those methods.
 * Throws an error if |tool| is null or undefined.
 * @param {string} toolId The id of the tool to make active.
 */
ginsu.controllers.ToolManager.prototype.activateToolWithId = function(toolId) {
  // Make sure the id is valid and the tool has been registered.
  if (!toolId) {
    // TODO(dspringer): we need a GSError object.
    throw new
        Error('ToolManager.activateToolWithId() called with null toolId');
  }
  if (!this.toolDictionary_.containsKey(toolId)) {
    // TODO(dspringer): we need a GSError object.
    throw new Error(
        "ToolManager.activateToolWithId() called with unregistered tool: " +
        toolId + "'");
  }
  var activeTool = this.activeTool();
  if (activeTool) {
    activeTool.deactivate();
  }
  // Push and activate the new tool.  Note that the containsKey() check above
  // means that the tool exists in the dictionary, so no need to check for a
  // null object nere.
  var tool = this.toolDictionary_.get(toolId);
  this.toolStack_.push(tool);
  tool.activate();
  // Dispatch an ACTION GSEvent with the tool's ID.
  this.dispatchEvent(new ginsu.events.Event(ginsu.events.EventType.ACTION, this,
      tool.toolId()));
};

/**
 * Method to deactivate a tool.  This pops the stack then sends the deactivate()
 * message to the just-popped tool, then sends the activate() message to the
 * new tool on the top of the stack (if there is one).
 * @param {?string} opt_toolId The id of the tool to make deactive, can be null.
 * @return {Object} the former tool on the top of the stack, can be null.
 */
ginsu.controllers.ToolManager.prototype.deactivateToolWithId = function(opt_toolId) {
  // Pop and deactivate the current tool.
  var oldActiveTool = null;
  if (this.toolStack_.length > 0) {
    oldActiveTool = this.toolStack_.pop();
    oldActiveTool.deactivate();
  }
  // Activate the new top-of-stack tool if there is one.
  var activeTool = this.activeTool();
  if (activeTool) {
    activeTool.activate();
  }
  // Dispatch an ACTION GSEvent with the tool's ID.
  if (oldActiveTool) {
    this.dispatchEvent(new ginsu.events.Event(ginsu.events.EventType.ACTION,
        this, oldActiveTool.toolId()));
  }
  return oldActiveTool;
};

/**
 * Handler method for tool ACTION events.  Triggered when a tool gets activated
 * or deactivated.
 * @param {ginsu.events.Event} toolEvent The event object instance that
 *     encapsulates this event.
 */
ginsu.controllers.ToolManager.prototype.handleActionEvent = function(toolEvent) {
  // Activate the registered tool with the same id as |toolEvent|.
  this.activateToolWithId(toolEvent.eventId());
};

/**
 * Handle the drag START event: tell the active tool to start its drag sequence
 * if it responds to the drag handling methods.
 * TODO(dspringer): We might want to make our own GSEvent class to handle these
 * events and add more info to simplfy tool programming.
 * @param {!goog.fx.DragEvent} dragStartEvent The START event that
 *     triggered this handler.
 */
ginsu.controllers.ToolManager.prototype.handleDragStartEvent =
    function(dragStartEvent) {
  var activeTool = this.activeTool();
  // Tools are not required to implement the drag handling informal protocol.
  if (activeTool && activeTool.handleStartDrag) {
    activeTool.handleStartDrag(dragStartEvent.target, dragStartEvent);
  }
};

/**
 * Handle the DRAG event: tell the active tool to do its drag processing
 * if it responds to the drag handling methods.
 * @param {!goog.fx.DragEvent} dragEvent The DRAG event that triggered this
 *     handler.
 */
ginsu.controllers.ToolManager.prototype.handleDragEvent = function(dragEvent) {
  var activeTool = this.activeTool();
  // Tools are not required to implement the drag handling informal protocol.
  if (activeTool && activeTool.handleDrag) {
    activeTool.handleDrag(dragEvent.target, dragEvent);
  }
};

/**
 * Handle the drag END event: tell the active tool to end its drag processing
 * if it responds to the drag handling methods.
 * @param {!goog.fx.DragEvent} dragEndEvent The END event that triggered this
 *     handler.
 */
ginsu.controllers.ToolManager.prototype.handleDragEndEvent =
    function(dragEndEvent) {
  var activeTool = this.activeTool();
  // Tools are not required to implement the drag handling informal protocol.
  if (activeTool && activeTool.handleEndDrag) {
    activeTool.handleEndDrag(dragEndEvent.target, dragEndEvent);
  }
};
