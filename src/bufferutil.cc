/*!
 * bufferutil: WebSocket buffer utils
 * Copyright(c) 2015 Einar Otto Stangvik <einaros@gmail.com>
 * MIT Licensed
 */

#include <napi.h>

void mask(const Napi::CallbackInfo& info) {
  char* from = info[0].As<Napi::Buffer<char>>().Data();
  char* mask = info[1].As<Napi::Buffer<char>>().Data();
  char* to = info[2].As<Napi::Buffer<char>>().Data()
           + info[3].As<Napi::Number>().Int32Value();
  size_t length = info[4].As<Napi::Number>().Int32Value();
  size_t index = 0;

  //
  // Alignment preamble.
  //
  while (index < length && (reinterpret_cast<size_t>(from) & 0x07)) {
    *to++ = *from++ ^ mask[index % 4];
    index++;
  }
  length -= index;
  if (!length) return;

  //
  // Realign mask and convert to 64 bit.
  //
  char maskAlignedArray[8];

  for (size_t i = 0; i < 8; i++, index++) {
    maskAlignedArray[i] = mask[index % 4];
  }

  //
  // Apply 64 bit mask in 8 byte chunks.
  //
  size_t loop = length / 8;
  uint64_t* pMask8 = reinterpret_cast<uint64_t*>(maskAlignedArray);

  while (loop--) {
    uint64_t* pFrom8 = reinterpret_cast<uint64_t*>(from);
    uint64_t* pTo8 = reinterpret_cast<uint64_t*>(to);
    *pTo8 = *pFrom8 ^ *pMask8;
    from += 8;
    to += 8;
  }

  //
  // Apply mask to remaining data.
  //
  char* pmaskAlignedArray = maskAlignedArray;

  length &= 0x7;
  while (length--) {
    *to++ = *from++ ^ *pmaskAlignedArray++;
  }
}

void unmask(const Napi::CallbackInfo& info) {
  char* from = info[0].As<Napi::Buffer<char>>().Data();
  size_t length = info[0].As<Napi::Buffer<char>>().Length();
  char* mask = info[1].As<Napi::Buffer<char>>().Data();
  size_t index = 0;

  //
  // Alignment preamble.
  //
  while (index < length && (reinterpret_cast<size_t>(from) & 0x07)) {
    *from++ ^= mask[index % 4];
    index++;
  }
  length -= index;
  if (!length) return;

  //
  // Realign mask and convert to 64 bit.
  //
  char maskAlignedArray[8];

  for (size_t i = 0; i < 8; i++, index++) {
    maskAlignedArray[i] = mask[index % 4];
  }

  //
  // Apply 64 bit mask in 8 byte chunks.
  //
  size_t loop = length / 8;
  uint64_t* pMask8 = reinterpret_cast<uint64_t*>(maskAlignedArray);

  while (loop--) {
    uint64_t* pSource8 = reinterpret_cast<uint64_t*>(from);
    *pSource8 ^= *pMask8;
    from += 8;
  }

  //
  // Apply mask to remaining data.
  //
  char* pmaskAlignedArray = maskAlignedArray;

  length &= 0x7;
  while (length--) {
    *from++ ^= *pmaskAlignedArray++;
  }
}

Napi::Object init(Napi::Env env, Napi::Object exports) {
  exports.Set("mask", Napi::Function::New(env, mask));
  exports.Set("unmask", Napi::Function::New(env, unmask));
  return exports;
}

NODE_API_MODULE(bufferutil, init)
