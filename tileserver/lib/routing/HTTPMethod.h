#pragma once
#include <proxygen/lib/http/HTTPMessage.h>

namespace tileserver {

#define DEF_METHOD(method, val) static const int HTTPMethod##method = val

DEF_METHOD(GET, 2);
DEF_METHOD(POST, 2<<1);
DEF_METHOD(PUT, 2<<2);
DEF_METHOD(DELETE, 2<<3);
DEF_METHOD(PATCH, 2<<4);
DEF_METHOD(HEAD, 2<<5);
DEF_METHOD(OPTIONS, 2<<6);

#undef DEF_METHOD

int methodEnumToInt(boost::optional<proxygen::HTTPMethod> methodEnum);

}