'use strict';

try {
  if (process.env.WS_NO_BUFFER_UTIL) {
    throw new Error('WS_NO_BUFFER_UTIL is set');
  }
  
  module.exports = require('node-gyp-build')(__dirname);
} catch (e) {
  module.exports = require('./fallback');
}
