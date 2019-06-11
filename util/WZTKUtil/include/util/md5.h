// Copyright 2007 Google Inc. All Rights Reserved.
// Author: liuli@google.com (Liu Li)
#ifndef COMMON_MD5_H__
#define COMMON_MD5_H__

#include <stdint.h>
#include <string>
using std::string;

typedef uint32_t u32;
typedef uint8_t u8;

class CMD5CheckSum
{
public:
    struct MD5Context {
      u32 buf[4];
      u32 bits[2];
      u8 in[64];
    };
    string GetMD5OfString(const char * pp );
private:
    void MD5Init(struct MD5Context *ctx);
    void MD5Update(struct MD5Context *ctx, unsigned char const *buf, unsigned len);
    void MD5Final(unsigned char digest[16], struct MD5Context *ctx);
private:
};




#endif  // COMMON_MD5_H__
