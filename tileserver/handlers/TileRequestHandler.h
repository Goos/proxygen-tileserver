#pragma once
#include <proxygen/httpserver/RequestHandler.h>
#include <folly/wangle/concurrent/CPUThreadPoolExecutor.h>
#include <folly/wangle/concurrent/IOThreadPoolExecutor.h>
#include <mapnik/box2d.hpp>

namespace tileserver {

class TileRequestHandler : public proxygen::RequestHandler {
  public:
    TileRequestHandler(
      mapnik::box2d<double> bbox,
      folly::wangle::CPUThreadPoolExecutor* renderingPool,
      folly::wangle::IOThreadPoolExecutor* ioPool);

    void onRequest(std::unique_ptr<proxygen::HTTPMessage> headers) noexcept override;

    void onBody(std::unique_ptr<folly::IOBuf> body) noexcept override;

    void onEOM() noexcept override;

    void onUpgrade(proxygen::UpgradeProtocol proto) noexcept override;

    void requestComplete() noexcept override;

    void onError(proxygen::ProxygenError err) noexcept override;

  protected:
    mapnik::box2d<double> bbox_;
    folly::wangle::CPUThreadPoolExecutor* renderingPool_;
    folly::wangle::IOThreadPoolExecutor* ioPool_;
};

}
