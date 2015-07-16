#include <tileserver/handlers/TileRequestHandlerFactory.h>
#include <proxygen/httpserver/ResponseBuilder.h>
#include <mapnik/box2d.hpp>

namespace tileserver {

using namespace proxygen;

TileRequestHandlerFactory::TileRequestHandlerFactory() {
}

void TileRequestHandlerFactory::onServerStart() noexcept {
}

void TileRequestHandlerFactory::onServerStop() noexcept {
}


RequestHandler* TileRequestHandlerFactory::onRequest(RequestHandler* handler, proxygen::HTTPMessage* message) noexcept {
  std::map<std::string, std::string> args;
  return new TileRequestHandler(args);
}

RequestHandler* TileRequestHandlerFactory::onRoutedRequest(RequestHandler* defaultHandler, HTTPMessage* message, std::map<std::string, std::string> args) noexcept {
  return new TileRequestHandler(args);
}

}
