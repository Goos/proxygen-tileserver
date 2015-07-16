#pragma once
#include <r3.hpp>
#include <tileserver/lib/routing/RoutableHandlerFactory.h>

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

struct Route {
  Route(std::string path, unsigned int method, RoutableHandlerFactory* handlerFactory);
  ~Route();

  std::string path;
  unsigned int method;
  RoutableHandlerFactory *handlerFactory;
  std::vector<std::string> variableKeys;
};

class Router : public proxygen::RequestHandlerFactory {
  public:
    Router(std::vector<Route*> routes);

    void onServerStart() noexcept override;

    void onServerStop() noexcept override;

    proxygen::RequestHandler* onRequest(proxygen::RequestHandler* handler, proxygen::HTTPMessage* message) noexcept override;

  private:
    r3::Tree routeTree_;
    std::vector<Route*> routes_;
};

class MissingRouteHandler : public proxygen::RequestHandler {
  public:
    MissingRouteHandler(proxygen::HTTPMessage* message);

    void onRequest(std::unique_ptr<proxygen::HTTPMessage> headers) noexcept override;

    void onBody(std::unique_ptr<folly::IOBuf> body) noexcept override;

    void onEOM() noexcept override;

    void onUpgrade(proxygen::UpgradeProtocol proto) noexcept override;

    void requestComplete() noexcept override;

    void onError(proxygen::ProxygenError err) noexcept override;
};

}
