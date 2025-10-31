'use strict';

const { deepStrictEqual } = require('assert');
const { randomBytes } = require('crypto');
const { join } = require('path');

const native = require('node-gyp-build')(join(__dirname, '..'));
const fallback = require('../fallback');

function use(bufferUtil) {
  return function () {
    it('masks a buffer (1/2)', function () {
      const buf = Buffer.from([0x6c, 0x3c, 0x58, 0xd9, 0x3e, 0x21, 0x09, 0x9f]);
      const mask = Buffer.from([0x48, 0x2a, 0xce, 0x24]);

      bufferUtil.mask(buf, mask, buf, 0, buf.length);

      deepStrictEqual(
        buf,
        Buffer.from([0x24, 0x16, 0x96, 0xfd, 0x76, 0x0b, 0xc7, 0xbb])
      );
    });

    it('masks a buffer (2/2)', function () {
      const src = Buffer.from([0x6c, 0x3c, 0x58, 0xd9, 0x3e, 0x21, 0x09, 0x9f]);
      const mask = Buffer.from([0x48, 0x2a, 0xce, 0x24]);
      const dest = Buffer.alloc(src.length + 2);

      bufferUtil.mask(src, mask, dest, 2, src.length);

      deepStrictEqual(
        dest,
        Buffer.from([0x00, 0x00, 0x24, 0x16, 0x96, 0xfd, 0x76, 0x0b, 0xc7, 0xbb])
      );
    });

    it('unmasks a buffer', function () {
      const buf = Buffer.from([0x24, 0x16, 0x96, 0xfd, 0x76, 0x0b, 0xc7, 0xbb]);
      const mask = Buffer.from([0x48, 0x2a, 0xce, 0x24]);

      bufferUtil.unmask(buf, mask);

      deepStrictEqual(
        buf,
        Buffer.from([0x6c, 0x3c, 0x58, 0xd9, 0x3e, 0x21, 0x09, 0x9f])
      );
    });
  };
}

describe('bindings', use(native));
describe('fallback', use(fallback));

describe('bindings match fallback', () => {
  const sizes = [1, 127, 128, 200, 1024, 10 * 1024 - 1, 10 * 1024]
  const offsets = [0, 1, 10, 16, 128]

  it('masks', function () {
    for (const size of sizes) {
      for (const offset of offsets) {
        const src = randomBytes(size);
        const mask = randomBytes(4);
        const dest = randomBytes(size + offset);
        const destFallback = Buffer.from(dest);

        native.mask(src, mask, dest, offset, size);
        fallback.mask(src, mask, destFallback, offset, size);

        deepStrictEqual(dest, destFallback);
      }
    }
  });

  it('unmasks', function () {
    for (const size of sizes) {
      const buf1 = randomBytes(size);
      const buf2 = Buffer.from(buf1);
      const mask = randomBytes(4);

      native.unmask(buf1, mask);
      fallback.unmask(buf2, mask);

      deepStrictEqual(buf1, buf2);
    }
  });
})
