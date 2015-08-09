#pragma once
#include <proxygen/httpserver/RequestHandler.h>

/* The agg_renderer import must be before the folly imports
  because they both use boost::mpl::vector, and mapnik does
  a bunch of defines changing how the mpl vector behaves. */
#include <mapnik/agg_renderer.hpp>
#include <mapnik/map.hpp>

#include <folly/wangle/concurrent/CPUThreadPoolExecutor.h>
#include <folly/wangle/concurrent/IOThreadPoolExecutor.h>

namespace tileserver {

class TileRequestHandler : public proxygen::RequestHandler {
  public:
    TileRequestHandler(
      mapnik::Map* map,
      folly::wangle::CPUThreadPoolExecutor* renderingPool,
      folly::wangle::IOThreadPoolExecutor* ioPool);
  
    ~TileRequestHandler();

    void onRequest(std::unique_ptr<proxygen::HTTPMessage> headers) noexcept override;

    void onBody(std::unique_ptr<folly::IOBuf> body) noexcept override;

    void onEOM() noexcept override;

    void onUpgrade(proxygen::UpgradeProtocol proto) noexcept override;

    void requestComplete() noexcept override;

    void onError(proxygen::ProxygenError err) noexcept override;

  protected:
    mapnik::Map* map_;
    folly::wangle::CPUThreadPoolExecutor* renderingPool_;
    folly::wangle::IOThreadPoolExecutor* ioPool_;
};

}
