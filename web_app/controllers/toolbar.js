// Copyright 2010 The Ginsu Authors.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

/**
 * @fileoverview  Implement the Ginsu toolbar, which is a view that
 * contains the tool buttons.  This class manages the buttons as radio-style
 * buttons; that is, only one can be active at a time.  Activation and
 * deactivation events are dispatched by the tools themselves; this class only
 * manages the UI.
 */

goog.provide('ginsu.controllers.Toolbar');

goog.require('goog.Disposable');
goog.require('goog.dom');
goog.require('goog.events');
goog.require('goog.ui.Component');
goog.require('goog.ui.Container');
goog.require('goog.ui.Control');
goog.require('goog.ui.SelectionModel');
goog.require('goog.ui.Toolbar');
goog.require('goog.ui.ToolbarToggleButton');
goog.require('ginsu.events.Event');
goog.require('ginsu.events.EventType');

/**
 * Creates a new Toolbar instance.  The Toolbar implements a radio-button
 * behavior for its buttons, meaning only one can be selected at a time.  When
 * the button selection changes, an Event of type ACTION is fired for the
 * selected button.
 * @param {?goog.ui.ToolbarRenderer} opt_renderer Renderer used to render or
 *     decorate the container; defaults to {@link goog.ui.ToolbarRenderer}.
 * @param {?goog.dom.DomHelper} opt_domHelper DOM helper, used for document
 *     interaction.
 * @constructor
 * @extends {goog.ui.Toolbar}
 */
ginsu.controllers.Toolbar = function(opt_renderer, opt_domHelper) {
  // Create a toolbar whose orientation is horizontal.
  goog.ui.Toolbar.call(this, opt_renderer,
      goog.ui.Container.Orientation.HORIZONTAL, opt_domHelper);

  /**
   * The selection model to handle radio-button behavior of the toolbar.
   * @type {goog.ui.SelectionModel}
   * @private
   */
  this.selectionModel_ = new goog.ui.SelectionModel();
  // Anonymous function params for setSelectionHandler():
  // @param {Object} button The item to be selected or deselected.
  // @param {Boolean} select Whether to select or deselect |button|.
  this.selectionModel_.setSelectionHandler(function(button, select) {
    if (button) {
      button.setChecked(select);
    }
  });
};
goog.inherits(ginsu.controllers.Toolbar, goog.ui.Toolbar);

/**
 * Override of disposeInternal() to dispose of retained objects.
 * @override
 */
ginsu.controllers.Toolbar.prototype.disposeInternal = function() {
  ginsu.controllers.Toolbar.superClass_.disposeInternal.call(this);
  this.selectionModel_.clear();
  this.selectionModel_ = null;
};

/**
 * Performs post-exit processing of the toolbar, ensuring that any listeners
 * are removed and any other housekeeping is performed.
 */
ginsu.controllers.Toolbar.prototype.exitDocument = function() {
  ginsu.controllers.Toolbar.superClass_.exitDocument.call(this);
  // Stop listening to all the buttons.
  this.forEachChild(function(child, index) {
      goog.events.unlisten(child, goog.ui.Component.EventType.ACTION,
          this.handleButtonSelection_, false, this);
    }, this);
};

/**
 * Builds the DOM elements which represent the toolbar.
 */
ginsu.controllers.Toolbar.prototype.createDom = function() {
  this.setId('GrayskullToolbar');
  this.element_ = goog.dom.createDom('div',
      {'class': 'goog-container goog-container-horizontal'});
  // Should be unfocusable, otherwise Chrome draws an orange box around it when
  // you click on any button.
  this.setFocusable(false);
};

/**
 * Add a toggle button with id |buttonId| to the toolbar using the given DOM
 * content to display the button.  Throws an error if either |content| or
 * |toolId| is null or undefined.
 * @param {!goog.ui.ControlContent} content The DOM content used to display
 *     the button.
 * @param {string} toolId The id of the tool associated with this button.
 * @return {goog.ui.ToolbarToggleButton} The newly created button.
 */
ginsu.controllers.Toolbar.prototype.addToggleButton =
    function(content, toolId) {
  if (!content || !toolId) {
    // TODO(dspringer): we need an error class, and this should be localized.
    throw new Error('Invalid argument to addToggleButton: null or undefined');
  }
  button = new goog.ui.ToolbarToggleButton(content);
  button.setId(toolId);
  this.addChild(button, true);
  // Wire the button up to the SelectionModel to get the radio button behavior.
  button.setAutoStates(goog.ui.Component.State.CHECKED, false);
  this.selectionModel_.addItem(button);
  goog.events.listen(button, goog.ui.Component.EventType.ACTION,
      this.handleButtonSelection_, false, this);
  return button;
};

/**
 * Handler method for ACTION events from the toolbar buttons.  These events are
 * dispatched whenever a tool button is clicked (selected).  Update the UI to
 * reflect the new selection, then forward the ACTION event.
 * @param {goog.events.Event} actionEvent The event object instance that
 *     encapsulates this event.
 * @private
 */
ginsu.controllers.Toolbar.prototype.handleButtonSelection_ =
    function(actionEvent) {
  this.selectionModel_.setSelectedItem(actionEvent.target);
  actionEvent.stopPropagation();
  this.dispatchEvent(new ginsu.events.Event(ginsu.events.EventType.ACTION, this,
      actionEvent.target.getId()));
};

/**
 * Handler method for tool ACTION events.  Triggered when a tool gets activated
 * or deactivated.
 * @param {ginsu.events.Event} toolEvent The event object instance that
 *     encapsulates this event.
 */
ginsu.controllers.Toolbar.prototype.handleActionEvent = function(toolEvent) {
  // If this event was caused by a newly-activated tool, then update the UI
  // to reflect it.
  var activeTool = toolEvent.target.activeTool();
  if (activeTool.toolId() == toolEvent.eventId()) {
    var button = this.getChild(toolEvent.eventId());
    if (button) {
      this.selectionModel_.setSelectedItem(button);
    }
  }
};
