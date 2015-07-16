#pragma once
#include <tileserver/lib/routing/RoutableHandlerFactory.h>
#include <tileserver/handlers/TileRequestHandler.h>

namespace tileserver {

class TileRequestHandlerFactory : public RoutableHandlerFactory {
  public:
    TileRequestHandlerFactory();

    void onServerStart() noexcept override;

    void onServerStop() noexcept override;

    proxygen::RequestHandler* onRequest(proxygen::RequestHandler* handler, proxygen::HTTPMessage* message) noexcept override;
    proxygen::RequestHandler* onRoutedRequest(proxygen::RequestHandler* defaultHandler, proxygen::HTTPMessage* message, std::map<std::string, std::string> args) noexcept;
};

}
