July 22, 2010

Ginsu uses jsUnit (www.jsUnit.net) for testing.  Note that jsUnit crashes
Chrome, so we use FireFox to run the tests.  The tests are not automated yet,
you have to use jsUnit's testRunner.html to run the tests.

To run various Ginsu tests, you will need to run the simple HTTP server
included in the project.  It gets installed in $STAGING_DIR (for debug, it's
scons-out/dbg/staging).  All of the test pages for jsUnit tests are installed
in the same staging dir.  This will change once we get automated tests working.

Open FireFox, and enter localhost:5013/jsunit/testRunner.html in the address
bar.  In the test page box, enter the test you want, prefixed with
localhost:5013/; for example, to run the orbittool_test, enter
    localhost:5103/orbittool_test.html

You can run all the tests at once by running
    localhost:5103/ginsu_test_suite.html

