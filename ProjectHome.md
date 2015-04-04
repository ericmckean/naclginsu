Ginsu is a light-weight 3D modeler that runs in a browser.  It uses [Native Client technology](http://code.google.com/p/nativeclient-sdk) to run the geometry computer on the local CPU, and use the local GPU for rendering.

[How to build and run Ginsu](HowTo_BuildAndRun.md)

Ginsu is also a sandbox for building [c\_salt](http://sites.google.com/a/chromium.org/dev/nativeclient/sdk-design-documents/c_salt-design).

To get started with Ginsu, see: [Set Up an Eclipse project](HowTo_SetUpEclipse.md)

Ginsu uses jsUnit to run tests for the browser code.  See: [How To Run jsUnit Tests](HowTo_RunJsUnitTests.md)

**Note:** there is a problem with patch 2.6 on Linux systems which produces errors while installing naclports packages.  You will need to upgrade to patch 2.6.1 ([visit this ftp site](ftp://ftp.gnu.org/gnu/patch)).