#!/usr/bin/python
#
# Copyright 2010, The Ginsu Authors.  All Rights Reserved.
# Use of this source code is governed by a BSD-style license that can
# be found in the LICENSE file.
#

"""A tiny web server.

This is intended to be used for testing, and
only run from within the scons-out directory.
"""

import BaseHTTPServer
import logging
import os
import SimpleHTTPServer
import SocketServer
import sys
import urlparse

logging.getLogger().setLevel(logging.INFO)

# Using 'localhost' means that we only accept connections
# via the loop back interface.
SERVER_PORT = 5103
SERVER_HOST = ''

# Map special path roots to dirs that are not under the root of this server.
# These are filled in at run-time (see below) once SanityCheck() has succeeded.
SPECIAL_PATH_MAP = dict()


# We only run from the staging directory, so that not too much is exposed
# via this HTTP server.  Everything in the directory is served, so there
# should never be anything potentially sensitive in the serving directory,
# especially if the machine might be a multi-user machine and not all users
# are trusted.  We only serve via the loopback interface.

SAFE_DIR_COMPONENTS = ['staging']
SAFE_DIR_SUFFIX = apply(os.path.join, SAFE_DIR_COMPONENTS)


def SetUpSpecialPathMap():
  ginsu_root = os.getenv('GINSU_ROOT')
  SPECIAL_PATH_MAP['closure'] = os.path.join(ginsu_root, 'third_party')
  SPECIAL_PATH_MAP['jsunit'] = os.path.join(ginsu_root, 'third_party')


def SanityCheck():
  if os.getcwd().endswith(SAFE_DIR_SUFFIX):
    return
  logging.error('httpd.py should only be run from the %s', SAFE_DIR_SUFFIX)
  logging.error('directory for testing purposes.')
  logging.error('We are currently in %s', os.getcwd())
  sys.exit(1)
  if os.getenv('GINSU_ROOT') is None:
    logging.error('GINSU_ROOT is not defined')
    sys.exit(1)


# An HTTP server that will quit when |is_running| is set to False.
class QuittableHTTPServer(BaseHTTPServer.HTTPServer):
  def serve_forever(self):
    self.is_running = True
    while self.is_running:
      self.handle_request()

  def shutdown(self):
    self.is_running = False
    return 1


# "Safely" split a string at |sep| into a [key, value] pair.  If |sep| does not
# exist in |str|, then the entire |str| is the key and the value is set to an
# empty string.
def KeyValuePair(str, sep='='):
  if sep in str:
    return str.split(sep)
  else:
    return [str, '']


# A small handler that looks for '?quit=1' query in the path and shuts itself
# down if it finds that parameter.  Also does some clever directory re-mapping
# for special, well-known directories such as 'closure' and 'jsunit'.
class QuittableHTTPHandler(SimpleHTTPServer.SimpleHTTPRequestHandler):
  def do_GET(self):
    (_, _, url_path, query, _) = urlparse.urlsplit(self.path)
    url_params = dict([KeyValuePair(key_value)
                      for key_value in query.split('&')])
    if 'quit' in url_params and '1' in url_params['quit']:
      self.send_response(200, 'OK')
      self.send_header('Content-type', 'text/html')
      self.send_header('Content-length', '0')
      self.end_headers()
      self.server.shutdown()
      return

    """ If the URL starts with one of the magical path names, then serve
        from the mapped dir.
    """
    path_components = url_path.split(os.sep)
    if (os.path.isabs(url_path)):
      del path_components[0]

    try:
      map_path = SPECIAL_PATH_MAP[path_components[0]]
      mapped_file_path = os.path.join(map_path, os.sep.join(path_components))
      mapped_file = open(mapped_file_path)
      self.send_response(200, 'OK')
      self.send_header('Content-type', 'text/html')
      self.end_headers()
      self.log_message('Remap special file %s to %s' %
          (url_path, mapped_file_path))
      self.wfile.write(mapped_file.read())
      mapped_file.close()
      return

    except KeyError:
      SimpleHTTPServer.SimpleHTTPRequestHandler.do_GET(self)

    except IOError:
      self.send_error(404, 'File Not Found: %s' % self.path)

    else:
      print 'Unhandled error'


def Run(server_address,
        server_class=QuittableHTTPServer,
        handler_class=QuittableHTTPHandler):
  httpd = server_class(server_address, handler_class)
  logging.info("Starting local server on port %d", server_address[1])
  logging.info("To shut down send http://localhost:%d?quit=1",
               server_address[1])
  httpd.serve_forever()
  logging.info("Shutting down local server on port %d", server_address[1])


if __name__ == '__main__':
  SanityCheck()
  SetUpSpecialPathMap()
  if len(sys.argv) > 1:
    Run((SERVER_HOST, int(sys.argv[1])))
  else:
    Run((SERVER_HOST, SERVER_PORT))
  sys.exit(0)
