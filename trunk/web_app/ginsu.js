// Copyright 2010 The Ginsu Authors.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

/**
 * @fileoverview  The ginsu Application object.  This object instantiates a
 * Trackball object and connects it to the element named |ginsu_instance|.
 */

goog.provide('ginsu.Application');

goog.require('goog.Disposable');

// TODO(dspringer): Add these when ready.
// goog.require('tumbler.Dragger');
// goog.require('tumbler.Trackball');

/**
 * Constructor for the Application class.  Use the run() method to populate
 * the object with controllers and wire up the events.
 * @constructor
 * @extends {goog.Disposable}
 */
ginsu.Application = function() {
  goog.Disposable.call(this);
}
goog.inherits(ginsu.Application, goog.Disposable);

/**
 * The native module for the application.  This refers to the module loaded via
 * the <embed> tag.
 * @type {Element}
 * @private
 */
ginsu.Application.prototype.module_ = null;

/**
 * The trackball object.
 * @type {tumbler.Trackball}
 * @private
 */
ginsu.Application.prototype.trackball_ = null;

/**
 * The mouse-drag event object.
 * @type {tumbler.Dragger}
 * @private
 */
ginsu.Application.prototype.dragger_ = null;

/**
 * Place-holder to make the onload event handling process all work.
 */
var loadingGinsuApp_ = {};

/**
 * Override of disposeInternal() to dispose of retained objects.
 * @override
 */
ginsu.Application.prototype.disposeInternal = function() {
  this.terminate();
  ginsu.Application.superClass_.disposeInternal.call(this);
}

/**
 * Called by the module loading function once the module has been loaded.
 * @param {?Element} nativeModule The instance of the native module.
 */
ginsu.Application.prototype.moduleDidLoad = function(nativeModule) {
  this.module_ = nativeModule;
  alert('Ginsu loaded!');
  // TODO(dspringer): Wire all this up when ready.
  /*
  this.trackball_ = new tumbler.Trackball();
  this.dragger_ = new tumbler.Dragger(this.module_);
  this.dragger_.addDragListener(this.trackball_);
  */
}

/**
 * Asserts that cond is true; issues an alert and throws an Error otherwise.
 * @param {bool} cond The condition.
 * @param {String} message The error message issued if cond is false.
 */
ginsu.Application.prototype.assert = function(cond, message) {
  if (!cond) {
    message = "Assertion failed: " + message;
    alert(message);
    throw new Error(message);
  }
}

/**
 * The run() method starts and 'runs' the application.  The trackball object
 * is allocated and all the events get wired up.
 * @param {?String} opt_contentDivName The id of a DOM element in which to
 *     embed the Ginsu module.  If unspecified, defaults to
 *     DEFAULT_DIV_NAME.  The DOM element must exist.
 */
ginsu.Application.prototype.run = function(opt_contentDivName) {
  var contentDivName = opt_contentDivName || ginsu.Application.DEFAULT_DIV_NAME;
  var contentDiv = document.getElementById(contentDivName);
  this.assert(contentDiv, "Missing DOM element '" + contentDivName + "'");
  // Load the published .nexe.  This includes the 'nexes' attribute which
  // shows how to load multi-architecture modules.  Each entry in the
  // table is a key-value pair: the key is the runtime ('x86-32',
  // 'x86-64', etc.); the value is a URL for the desired NaCl module.
  var nexes = 'x86-32: ginsu_x86_32.nexe\n'
              + 'x86-64: ginsu_x86_64.nexe\n'
              + 'ARM: ginsu_arm.nexe ';
  // This assumes that the <div> containers for Ginsu modules each have a
  // unique name on the page.
  var uniqueModuleName = contentDivName + ginsu.Application.GINSU_MODULE_NAME;
  // This is a bit of a hack: when the |onload| event fires, |this| is set to
  // the DOM window object, *not* the <embed> element.  So, we keep a global
  // pointer to |this| because there is no way to make a closure here. See
  // http://code.google.com/p/nativeclient/issues/detail?id=693
  loadingGinsuApp_[uniqueModuleName] = this;
  var onLoadJS = "loadingGinsuApp_['"
	             + uniqueModuleName
	             + "'].moduleDidLoad(document.getElementById('"
	             + uniqueModuleName
	             + "'));"
  contentDiv.innerHTML = '<embed id="'
	                       + uniqueModuleName + '" '
	                    // + 'nexes="' + nexes + '" '
	                       + 'type="application/x-nacl-srpc" '
	                       + 'width="512" height="512" '
	                       + 'dimensions="3" '
	                       + 'onload="' + onLoadJS + '" />'
  // Note: this code is here to work around a bug in the Chrome Browser.
  // http://code.google.com/p/nativeclient/issues/detail?id=500
  document.getElementById(uniqueModuleName).nexes = nexes;
}

/**
 * Shut down the application instance.  This unhooks all the event listeners
 * and deletes the objects created in moduleDidLoad().
 */
ginsu.Application.prototype.terminate = function() {
  goog.events.removeAll();
  this.trackball_ = null;
  this.dragger_ = null;
  this.dragger_ = null;
}

/**
 * The name for the Ginsu module element.
 * @type {string}
 */
ginsu.Application.GINSU_MODULE_NAME = 'ginsu';

/**
 * The default name for the 3D content div element.
 * @type {string}
 */
ginsu.Application.DEFAULT_DIV_NAME = 'ginsu_instance';
