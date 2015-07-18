#pragma once
#include <proxygen/httpserver/RequestHandlerFactory.h>

namespace tileserver {

class RoutableHandlerFactory : public proxygen::RequestHandlerFactory {
  public:
    virtual proxygen::RequestHandler* onRoutedRequest(proxygen::RequestHandler* defaultHandler, proxygen::HTTPMessage* message, std::map<std::string, std::string> args) noexcept = 0;
};

}
