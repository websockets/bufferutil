'use strict';

try {
  module.exports = require('./build/Release/bufferutil');
} catch (e) {
  module.exports = require('./fallback');
}
