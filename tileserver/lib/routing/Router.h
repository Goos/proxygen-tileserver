#pragma once
#include <tileserver/lib/routing/Route.h>
#include <tileserver/lib/routing/HTTPMethod.h>
#include <tileserver/lib/routing/RoutableHandlerFactory.h>
#include <r3.hpp>
#include <memory>

namespace tileserver {

class Router : public proxygen::RequestHandlerFactory {
  public:
    Router(std::vector<std::shared_ptr<Route>> routes);

    void onServerStart() noexcept override;

    void onServerStop() noexcept override;

    proxygen::RequestHandler* onRequest(proxygen::RequestHandler* handler, proxygen::HTTPMessage* message) noexcept override;

  private:
    r3::Tree routeTree_;
    std::vector<std::shared_ptr<Route>> routes_;
    std::unordered_set<std::shared_ptr<RoutableHandlerFactory>> handlerFactories_;
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
