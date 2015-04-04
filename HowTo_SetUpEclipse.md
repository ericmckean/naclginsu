

# How To Set Up Eclipse #


These instructions show you how to set up
[Eclipse](http://www.eclipse.org/) to build Ginsu.

**Note:** Ginsu does not build a 64-bit version.  When running Ginsu, you must use a 32-bit browser configuration.

After following the steps below, you should have an Eclipse project that will build Ginsu and install the Ginsu Web app in `scons-out/dbg/staging/`.

## Set Up Eclipse ##

### Prerequisites ###
<ul>
<blockquote><li>A recent version of Eclipse.  At time of writing (July 12, 2010), the latest version of Eclipse is Helios (3.6).  All of these instructions were developed on Galileo (version 3.5).  To get a copy of Eclipse, visit <a href='http://www.eclipse.org'>www.eclipse.org</a>.</li>
<li>The Native Client SDK (<a href='http://code.google.com/p/nativeclient-sdk/'>Get the SDK here</a>). Install the SDK on your local disk, e.g. <code>/usr/local/me/native_client_sdk</code>.</li>
<li>The shell environment variable <code>NACL_SDK_BASE</code> set to the absolute path of the toolchain within the NaClSDK.  For example:<br>
<pre><code>export NACL_SDK_BASE=/usr/local/me/native_client_sdk/toolchain/linux_x86<br>
export NACL_TOOLCHAIN_ROOT=$NACL_SDK_BASE<br>
</code></pre>
</li>
<li>Local copy of the Ginsu repository.</li>
<li>The shell environment variable <code>GINSU_ROOT</code> set to the absolute path of your Ginsu repository (this is needed for jsUnit tests).  For example:<br>
<pre><code>export GINSU_ROOT=/usr/local/me/ginsu/trunk<br>
</code></pre>
</li>
<li><a href='http://code.google.com/p/naclports/'>NaClPorts</a> packages installed by running <code>scripts/setup_naclports_packages.py</code> in Ginsu.<br>
<blockquote><blockquote><b>Note:</b> You might see messages such as <code>Get file gtest-1.5.0/src/gtest-death-test.cc from Perforce with lock? [y]</code>.  Answer <b>n</b> to each of these.</blockquote>
</blockquote></li>
</ul></blockquote>

### Create a new project ###
See also the Eclipse documentation here: http://help.eclipse.org/galileo/index.jsp.  Look for 'Creating new projects from existing source roots'.
<ol>
<blockquote><li><b>Start a new project</b>.  You will make an empty Makefile project located at the trunk of your local Ginsu working copy.  Start Eclipse, and select File -> New -> C++ Project<br>
<blockquote><ul>
<blockquote><li><b>Uncheck</b> 'Use default location', and enter the location of your Ginsu project source root in the 'Location:' text field.  Be sure to include <code>trunk</code> in the path, e.g. <code>/usr/local/me/ginsu/trunk</code>.  Eclipse will automatically import the Ginsu tree for you.</li>
<li>Under Project Type, select Empty Project</li>
<li>Click Next</li>
<li>On the 'Select Configurations' panel, click 'Advanced...' to configure the C++ Builder</li>
</blockquote></ul>
</blockquote></li>
<li><b>Configure the C++ Builder</b>.<br>
<blockquote><ul>
<blockquote><li>Make sure the 'Default' configuration is active.  For example, set Configuration to 'All Configurations'</li>
<li>Under 'C/C++ Build', select the 'Builder Settings' tab.  <b>Uncheck</b> 'Use default build command', then enter <code>${ProjDirPath}/hammer.sh</code> as the build command.</li>
<li>Also, under 'Makefile generation' <b>uncheck</b> 'Generate Makefiles automatically', and then under 'Build directory' remove the Debug portion (so that it looks similar to this:  ${workspace_loc:/ginsu}).</li>
<li>Click 'Apply'.</li>
<li>Click the 'Behaviour' tab.</li>
<li>In the 'Build (incremental build)' text field, enter <code>--mode=dbg</code> (<b>Note:</b> <code>--mode=opt</code> will compile optimized code.  You can add this as a configuration later.)</li>
<li>In the 'Clean' text field, enter <code>-c</code></li>
<li>Click 'Apply'</li>
</blockquote></ul>
</blockquote></li>
<li><b>Add NACL_SDK_BASE and GINSU_ROOT</b>
<blockquote><ul>
<blockquote><li>Under 'C/C++ Build' ->'Environment', add NACL_SDK_BASE whose value is the full path to the toolchain within your Native Client SDK installation, e.g. <code>/usr/local/me/native_client_sdk/toolchain/linux_x86</code></li>
<li>Add GINSU_ROOT whose value is the full path to your Ginsu repo trunk (this is needed for jsUnit testing), e.g. <code>/usr/local/me/ginsu/trunk</code></li>
<li>Click 'OK' - you're done!</li>
</blockquote></ul>
</blockquote></li>
</ol></blockquote>


## How to Run Ginsu ##

<blockquote><b>Note:</b> these instructions apply to Linux only.  See<br>
<a href='http://nativeclient-sdk.googlecode.com'>The Native Client SDK</a> for info on other platforms)</blockquote>

<ol>
<blockquote><li>Run a web server that can serve the contents of <code>scons-out/</code>.  A simple python-based server is installed into <code>scons-out/httpd.py</code>, you can run this with:<br>
<blockquote><blockquote><pre><code>python httpd.py 5103&amp;</code></pre></blockquote>
</blockquote></li>
<li>Run a <b>32-bit</b> NaCl-enabled version of the Chrome Browser. On Linux, you can use this command line:<br>
<blockquote><blockquote><pre><code>/opt/google/chrome/chrome --enable-nacl&amp;</code></pre></blockquote>
</blockquote></li>
<li>In the browser, go to <code>ginsu.html</code>.  If you used the python-based web server from step 1, type this into the Google Chrome address bar:<br>
<blockquote><blockquote><pre><code>http://localhost:5103/dbg/staging/ginsu.html</code></pre></blockquote>
<blockquote><b>Note:</b> For optimized builds, replace <code>dbg</code> with <code>opt</code>, e.g. <code>http://localhost:5103/opt/staging/ginsu.html</code></blockquote>
</blockquote></li>
</ol></blockquote>

_TODO:_ Add info on how to attach nacl-gdb to a browser process.