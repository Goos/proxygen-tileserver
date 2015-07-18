#include "HTTPMethod.h"

namespace tileserver {

using namespace proxygen;

int methodEnumToInt(boost::optional<proxygen::HTTPMethod> methodEnum) {
  if (!methodEnum) { return 0; }
  switch(*methodEnum) {
  case HTTPMethod::GET: return HTTPMethodGET;
  case HTTPMethod::POST: return HTTPMethodPOST;
  case HTTPMethod::PUT: return HTTPMethodPUT;
  case HTTPMethod::DELETE: return HTTPMethodDELETE;
  case HTTPMethod::HEAD: return HTTPMethodHEAD;
  case HTTPMethod::OPTIONS: return HTTPMethodOPTIONS;
  default: return 0;
  }
}

}