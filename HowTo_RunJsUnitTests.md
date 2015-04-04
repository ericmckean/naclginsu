# Introduction #
July 22, 2010

Ginsu uses jsUnit (www.jsUnit.net) for testing.  Note that jsUnit crashes
Chrome, so we use FireFox to run the tests.  The tests are not automated yet,
you have to use jsUnit's testRunner.html to run the tests.

# Manually Run jsUnit Tests #

To run various Ginsu tests, you will need to run the simple HTTP server
included in the project.  It gets installed in `$STAGING_DIR` (for debug, it's
`scons-out/dbg/staging`).  All of the test pages for jsUnit tests are installed
in the same staging dir.  This will change once we get automated tests working.

Open your browser, and enter
<blockquote><pre><code>localhost:5013/jsunit/testRunner.html</code></pre></blockquote> in the address
bar.  In the test page box, enter the test you want, prefixed with
`localhost:5013/`; for example, to run the orbittool\_test, enter
<blockquote><pre><code>localhost:5103/orbittool_test.html</code></pre></blockquote>

You can run all the tests at once by running
<blockquote><pre><code>localhost:5103/ginsu_test_suite.html</code></pre></blockquote>

# Known Issues #
jsUnit crashes Chrome if there is a test exception (which means a test failed).