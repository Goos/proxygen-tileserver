#include <tileserver/handlers/TileRequestHandler.h>
#include <boost/algorithm/string/join.hpp>
#include <proxygen/httpserver/ResponseBuilder.h>

namespace tileserver {

using namespace proxygen;

TileRequestHandler::TileRequestHandler(
      mapnik::box2d<double> bbox,
      folly::wangle::CPUThreadPoolExecutor* renderingPool,
      folly::wangle::IOThreadPoolExecutor* ioPool):
  bbox_(bbox),
  renderingPool_(renderingPool),
  ioPool_(ioPool)
{
}

void TileRequestHandler::onRequest(std::unique_ptr<HTTPMessage> headers) noexcept {
  ResponseBuilder(downstream_)
    .status(200, "OK")
    .sendWithEOM();
}

void TileRequestHandler::onBody(std::unique_ptr<folly::IOBuf> body) noexcept {
}

void TileRequestHandler::onEOM() noexcept {
}

void TileRequestHandler::onUpgrade(UpgradeProtocol protocol) noexcept {
}

void TileRequestHandler::requestComplete() noexcept {
  delete this;
}

void TileRequestHandler::onError(ProxygenError err) noexcept {
  delete this;
}

}
