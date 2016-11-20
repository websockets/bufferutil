/*!
 * bufferutil: WebSocket buffer utils
 * Copyright(c) 2015 Einar Otto Stangvik <einaros@gmail.com>
 * MIT Licensed
 */

#include <v8.h>
#include <node.h>
#include <node_version.h>
#include <node_buffer.h>
#include <node_object_wrap.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <stdio.h>
#include "nan.h"

using namespace v8;
using namespace node;

class BufferUtil : public ObjectWrap
{
public:

  static void Initialize(v8::Handle<v8::Object> target)
  {
    Nan::HandleScope scope;
    Local<FunctionTemplate> t = Nan::New<FunctionTemplate>(New);
    t->InstanceTemplate()->SetInternalFieldCount(1);
    Nan::SetMethod(t, "unmask", BufferUtil::Unmask);
    Nan::SetMethod(t, "mask", BufferUtil::Mask);
    Nan::SetMethod(t, "merge", BufferUtil::Merge);
    Nan::Set(target, Nan::New<String>("BufferUtil").ToLocalChecked(), t->GetFunction());
  }

protected:

  static NAN_METHOD(New)
  {
    Nan::HandleScope scope;
    BufferUtil* bufferUtil = new BufferUtil();
    bufferUtil->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
  }

  static NAN_METHOD(Merge)
  {
    Nan::HandleScope scope;
    Local<Object> bufferObj = info[0]->ToObject();
    char* buffer = Buffer::Data(bufferObj);
    Local<Array> array = Local<Array>::Cast(info[1]);
    unsigned int arrayLength = array->Length();
    size_t offset = 0;
    unsigned int i;
    for (i = 0; i < arrayLength; ++i) {
      Local<Object> src = array->Get(i)->ToObject();
      size_t length = Buffer::Length(src);
      memcpy(buffer + offset, Buffer::Data(src), length);
      offset += length;
    }
    info.GetReturnValue().Set(Nan::True());
  }

  static NAN_METHOD(Unmask)
  {
    Nan::HandleScope scope;
    Local<Object> buffer_obj = info[0]->ToObject();
    Local<Object> mask_obj = info[1]->ToObject();

    char* mask = Buffer::Data(mask_obj);
    char* from = Buffer::Data(buffer_obj);
    size_t length = Buffer::Length(buffer_obj);

    info.GetReturnValue().Set(Nan::True());

    /* Alignment preamble */
    size_t index;
    for (index = 0; index < length && (reinterpret_cast<size_t>(from) & 0x07); index++)
        *from++ ^= mask[index % 4];
    length -= index;
    if (!length)
        return;

    /* Realign mask and convert to 64 bit */
    char maskAlignedArray[8];
    for (size_t i = 0; i < 8; i++, index++)
        maskAlignedArray[i] = mask[index % 4];

    /* Apply 64 bit mask in 8 byte chunks */
    size_t loop = length / 8;
    uint64_t* pSource8;
    uint64_t* pMask8 = reinterpret_cast<uint64_t*>(maskAlignedArray);
    while (loop--) {
        pSource8 = reinterpret_cast<uint64_t*>(from);
        *pSource8 ^= *pMask8;
        from += 8;
    }

    /* Apply mask to remaining data */
    char* pmaskAlignedArray = maskAlignedArray;
    length &= 0x7;
    while (length--)
        *from++ ^= *pmaskAlignedArray++;
  }

  static NAN_METHOD(Mask)
  {
    Nan::HandleScope scope;
    Local<Object> buffer_obj = info[0]->ToObject();
    Local<Object> mask_obj = info[1]->ToObject();
    Local<Object> output_obj = info[2]->ToObject();
    size_t dataOffset = info[3]->Int32Value();
    size_t length = info[4]->Int32Value();

    char* mask = Buffer::Data(mask_obj);
    char* from = Buffer::Data(buffer_obj);
    char* to = Buffer::Data(output_obj) + dataOffset;

    info.GetReturnValue().Set(Nan::True());

    /* Alignment preamble */
    size_t index;
    for (index = 0; index < length && (reinterpret_cast<size_t>(from) & 0x07); index++)
        *to++ = *from++ ^ mask[index % 4];
    length -= index;
    if (!length)
        return;

    /* Realign mask and convert to 64 bit */
    char maskAlignedArray[8];
    for (size_t i = 0; i < 8; i++, index++)
        maskAlignedArray[i] = mask[index % 4];

    /* Apply 64 bit mask in 8 byte chunks */
    size_t loop = length / 8;
    uint64_t* pFrom8;
    uint64_t* pTo8;
    uint64_t* pMask8 = reinterpret_cast<uint64_t*>(maskAlignedArray);
    while (loop--) {
        pFrom8 = reinterpret_cast<uint64_t*>(from);
        pTo8 = reinterpret_cast<uint64_t*>(to);
        *pTo8 = *pFrom8 ^ *pMask8;
        from += 8;
        to += 8;
    }

    /* Apply mask to remaining data */
    char* pmaskAlignedArray = maskAlignedArray;
    length &= 0x7;
    while (length--)
        *to++ = *from++ ^ *pmaskAlignedArray++;
  }
};

#if !NODE_VERSION_AT_LEAST(0,10,0)
extern "C"
#endif
void init (Handle<Object> target)
{
  Nan::HandleScope scope;
  BufferUtil::Initialize(target);
}

NODE_MODULE(bufferutil, init)
