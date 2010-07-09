vars = {
  "chromium_trunk": "http://src.chromium.org/svn/trunk/",
  "chromium_version": "45198",
  "ppapi_trunk": "http://ppapi.googlecode.com/svn/trunk/",
  "ppapi_version": "HEAD",
}

deps = {
  "trunk/third_party/scons":
    Var("chromium_trunk") + "src/third_party/scons@" + Var("chromium_version"),
  "trunk/third_party/swtoolkit":
    "http://swtoolkit.googlecode.com/svn/trunk@66",
  "trunk/third_party/ppapi/c":
    Var("ppapi_trunk") + "c@" + Var("ppapi_version"),
  "trunk/third_party/ppapi/cpp":
    Var("ppapi_trunk") + "cpp@" + Var("ppapi_version"),
}
