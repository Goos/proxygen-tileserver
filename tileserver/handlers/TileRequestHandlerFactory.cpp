#include <tileserver/handlers/TileRequestHandlerFactory.h>
#include <tileserver/handlers/TileRequest.h>
#include <proxygen/httpserver/ResponseBuilder.h>
#include <mapnik/box2d.hpp>
#include <thread>
#include <chrono>

namespace tileserver {

using namespace proxygen;
using namespace folly::wangle;

TileRequestHandlerFactory::TileRequestHandlerFactory():
  renderingPool_(sysconf(_SC_NPROCESSORS_ONLN) * 2),
  ioPool_(sysconf(_SC_NPROCESSORS_ONLN))
{
}

TileRequestHandlerFactory::~TileRequestHandlerFactory() {
  renderingPool_.stop();
  ioPool_.stop();
}

void TileRequestHandlerFactory::onServerStart() noexcept {
  mercator_.reset(new SphericalMercator(256));
}

void TileRequestHandlerFactory::onServerStop() noexcept {
  mercator_.reset();
}

RequestHandler* TileRequestHandlerFactory::onRequest(RequestHandler* handler, proxygen::HTTPMessage* message) noexcept {
  return handler;
}

RequestHandler* TileRequestHandlerFactory::onRoutedRequest(RequestHandler* defaultHandler, HTTPMessage* message, std::map<std::string, std::string> args) noexcept {
  auto xStr = args["x"].c_str();
  auto yStr = args["y"].c_str();
  auto zoomStr = args["z"].c_str();
  if (!xStr || !yStr || !zoomStr) {
    return defaultHandler;
  }
  
  double x = atof(xStr);
  double y = atof(yStr);
  double zoom = atof(zoomStr);
  
  auto bbox = mercator_.get()->bbox(x, y, zoom, Projection::EPSG3857);
  auto map = new mapnik::Map(256, 256, mercator_.get()->proj4());
  map->zoom_to_box(bbox);
  auto request = TileRequest(bbox, 256.0, 1.0, "png", args);
  
  return new TileRequestHandler(map, &renderingPool_, &ioPool_);
}

}