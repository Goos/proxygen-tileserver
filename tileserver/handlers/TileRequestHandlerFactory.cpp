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
  renderingPool_(0),
  ioPool_(0)
{
}

TileRequestHandlerFactory::~TileRequestHandlerFactory() {
  renderingPool_.stop();
  ioPool_.stop();
}

void TileRequestHandlerFactory::onServerStart() noexcept {
  mercator_.reset(new SphericalMercator(256));
  map_.reset(new mapnik::Map(256, 256, mercator_.get()->proj4_));
  
  size_t cpuThreadCount(renderingPool_.numThreads() + 2);
  size_t ioThreadCount(ioPool_.numThreads() + 1);
  renderingPool_.setNumThreads(cpuThreadCount);
  ioPool_.setNumThreads(ioThreadCount);
}

void TileRequestHandlerFactory::onServerStop() noexcept {
  mercator_.reset();
  map_.reset();
  size_t cpuThreadCount(renderingPool_.numThreads());
  size_t ioThreadCount(ioPool_.numThreads());
  if (cpuThreadCount >= 2) { cpuThreadCount -= 2; }
  if (ioThreadCount >= 1) { cpuThreadCount -= 1; }
  renderingPool_.setNumThreads(cpuThreadCount);
  ioPool_.setNumThreads(ioThreadCount);
}


RequestHandler* TileRequestHandlerFactory::onRequest(RequestHandler* handler, proxygen::HTTPMessage* message) noexcept {
  auto bbox = mercator_.get()->bbox(0, 0, 0, Projection::EPSG3857);
  return new TileRequestHandler(bbox, &renderingPool_, &ioPool_);
}

RequestHandler* TileRequestHandlerFactory::onRoutedRequest(RequestHandler* defaultHandler, HTTPMessage* message, std::map<std::string, std::string> args) noexcept {
  auto xStr = args["x"].c_str();
  auto yStr = args["y"].c_str();
  auto zoomStr = args["z"].c_str();
  double x = atof(xStr);
  double y = atof(yStr);
  double zoom = atof(zoomStr);
  
  auto bbox = mercator_.get()->bbox(x, y, zoom, Projection::EPSG3857);
  auto request = TileRequest(bbox);
  
  return new TileRequestHandler(bbox, &renderingPool_, &ioPool_);
}

}
