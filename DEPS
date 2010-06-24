vars = {
  "chromium_trunk": "http://src.chromium.org/svn/trunk/",
  "chromium_version": "45198",
  "compiler_version": "latest",
  "nativeclient-sdk_version": "324",
}

deps = {
  "trunk/third_party/boost/trunk":
    "http://svn.boost.org/svn/boost/trunk@60157",
  "trunk/third_party/chromium/src/base":
    Var("chromium_trunk") + "src/base@" + Var("chromium_version"),
  "trunk/third_party/chromium/src/build":
    Var("chromium_trunk") + "src/build@" + Var("chromium_version"),
  "trunk/third_party/chromium/src/gpu":
    Var("chromium_trunk") + "src/gpu@" + Var("chromium_version"),
  "trunk/third_party/npapi":
    Var("chromium_trunk") + "src/third_party/npapi@" + Var("chromium_version"),
  "trunk/third_party/openscenegraph/trunk":
    "http://www.openscenegraph.org/svn/osg/OpenSceneGraph/trunk",
  "trunk/third_party/scons":
    Var("chromium_trunk") + "src/third_party/scons@" + Var("chromium_version"),
  "trunk/third_party/swtoolkit":
    "http://swtoolkit.googlecode.com/svn/trunk@66",
  "trunk/third_party/nativeclient-sdk/build_tools":
    "http://nativeclient-sdk.googlecode.com/svn/trunk/src/build_tools@" +
    Var("nativeclient-sdk_version"),
  "trunk/third_party/nativeclient-sdk/packages":
    "http://nativeclient-sdk.googlecode.com/svn/trunk/src/packages@" +
    Var("nativeclient-sdk_version"),
}

deps_os = {
  "mac": {
  "trunk/third_party/chromium/src/gpu/OpenGLES/ES2":
    Var("chromium_trunk") + "src/gpu/GLES2@" + Var("chromium_version"),
  },
}

hooks = [
   {
     "pattern": ".",
     "action": ["python", "trunk/third_party/nativeclient-sdk/build_tools/download_compilers.py","-v", Var("compiler_version")],
   }
]
