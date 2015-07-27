#include <tileserver/handlers/TileRequestHandler.h>
#include <tileserver/datasources/BasicGeometryDatasource.h>
  
#include <boost/algorithm/string/join.hpp>
#include <proxygen/httpserver/ResponseBuilder.h>
#include <mapnik/datasource_cache.hpp>

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
  mapnik::parameters params;
  params["type"] = "BasicGeometry";
  auto datasource = std::make_shared<BasicGeometryDatasource>(params);
  auto center = bbox_.center();
  std::cout << "[" << bbox_.minx() << ", " << bbox_.miny() << ", " << bbox_.maxx() << ", " << bbox_.maxy() << "]" << std::endl;
  
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
