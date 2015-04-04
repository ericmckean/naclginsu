# Prerequisites #

  1. An installed copy of [depot\_tools](http://dev.chromium.org/developers/how-tos/depottools) for gclient and gcl
  1. An installed copy of the [Native Client SDK](http://code.google.com/p/nativeclient-sdk/)
  1. Set the environment variable `NACL_TOOLCHAIN_ROOT` to point into the toolchain of your NaCl SDK installation, e.g.:<blockquote><pre><code><br>
export NACL_TOOLCHAIN_ROOT=/usr/local/my_ldap/native_client_sdk/toolchain/mac_x86<br>
</code></pre></blockquote>

# Building Ginsu #

  1. Create a place for the project:<blockquote><pre><code><br>
mkdir /usr/local/my_ldap/ginsu<br>
</code></pre></blockquote>
  1. Set the `GINSU_ROOT` environment variable to point into your ginsu client directory, e.g.:<blockquote><pre><code><br>
export GINSU_ROOT=/usr/local/my_ldap/ginsu/trunk<br>
</code></pre></blockquote>
  1. Check out the code:<blockquote><pre><code><br>
cd /usr/local/my_ldap/ginsu<br>
gclient config https://naclginsu.googlecode.com/svn/trunk<br>
gclient sync<br>
</code></pre></blockquote>
  1. Run the script that sets up all the naclports libraries:<blockquote><pre><code><br>
cd ginsu/trunk<br>
python scripts/setup_naclports_packages.py<br>
</code></pre></blockquote>
  1. Build ginsu using hammer:<blockquote><pre><code><br>
./hammer.sh<br>
</code></pre></blockquote>

# Running Ginsu #

**Note: Ginsu only builds and runs in 32-bits.**

  1. Run the python-based HTTP server:<blockquote><pre><code><br>
cd scons-out/dbg/staging<br>
python httpd.py &amp;<br>
</code></pre></blockquote>
  1. Run a supported version of Chrome with the `--enable-nacl` flag (see also [the Getting Started](http://code.google.com/p/nativeclient-sdk/wiki/HowTo_GetStarted) page in the NaCl SDK codesite).  We recommend that you start a new incognito window to make sure ginsu is not cached.
  1. Enter this in the address bar:<blockquote><pre><code><br>
http://localhost:5103/ginsu.html<br>
</code></pre></blockquote>