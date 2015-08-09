#pragma once
#include <tileserver/lib/routing/RoutableHandlerFactory.h>
#include <tileserver/lib/projections/SphericalMercator.h>
#include <tileserver/handlers/TileRequestHandler.h>
#include <folly/wangle/concurrent/CPUThreadPoolExecutor.h>
#include <folly/wangle/concurrent/IOThreadPoolExecutor.h>
#include <folly/ThreadLocal.h>
#include <mapnik/map.hpp>

namespace tileserver {

class TileRequestHandlerFactory : public RoutableHandlerFactory {
  public:
    TileRequestHandlerFactory();
  
    ~TileRequestHandlerFactory();

    void onServerStart() noexcept override;

    void onServerStop() noexcept override;

    proxygen::RequestHandler* onRequest(proxygen::RequestHandler* handler, proxygen::HTTPMessage* message) noexcept override;
    proxygen::RequestHandler* onRoutedRequest(proxygen::RequestHandler* defaultHandler, proxygen::HTTPMessage* message, std::map<std::string, std::string> args) noexcept override;
  
  protected:
    folly::ThreadLocalPtr<SphericalMercator> mercator_;
    folly::wangle::CPUThreadPoolExecutor renderingPool_;
    folly::wangle::IOThreadPoolExecutor ioPool_;
};

}
