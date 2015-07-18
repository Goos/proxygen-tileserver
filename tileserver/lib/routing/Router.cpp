#include <tileserver/lib/routing/Router.h>
#include <proxygen/httpserver/ResponseBuilder.h>

namespace tileserver {

using namespace proxygen;

Router::Router(std::vector<std::shared_ptr<Route>> routes):
  routeTree_(routes.size()),
  routes_(routes)
{ 
  for (auto iterator = routes.begin(); iterator != routes.end(); iterator++) {
    auto route = *iterator;   
    auto path = route->path.c_str();
    void* data = route.get();
    routeTree_.insert_route(route->method, path, data);
  }

  char *errStr;
  int err = routeTree_.compile(&errStr);
  if (err != 0) {
    std::cout << "Error compiling route tree: " << errStr << std::endl;
    routeTree_.dump(0);
    delete errStr;
    abort();
  }
}

void Router::onServerStart() noexcept {
  std::cout << "Starting router." << std::endl;
  for (auto iterator = routes_.begin(); iterator != routes_.end(); iterator++) {
    auto route = *iterator;
    route->handlerFactory->onServerStart();
  }
}

void Router::onServerStop() noexcept {
  std::cout << "Stopping router." << std::endl;
  for (auto iterator = routes_.begin(); iterator != routes_.end(); iterator++) {
    auto route = *iterator;
    route->handlerFactory->onServerStop();
  }
}

RequestHandler* Router::onRequest(RequestHandler* handler, HTTPMessage* message) noexcept {
  // Getting the path and clipping the leading slash.
  auto path = message->getURL();
  auto clippedPath = path.substr(1, path.size() - 1);
  r3::MatchEntry entry(clippedPath.c_str());
  auto method = message->getMethod();
  if (method) {
    auto rawMethod = methodEnumToInt(method);
    entry.set_request_method(rawMethod);
  }
  r3::Route matchingRoute = routeTree_.match_route(entry);

  if (matchingRoute) {
    auto route = static_cast<Route*>(matchingRoute.data());

    std::map<std::string, std::string> args;

    auto varKeys = route->variableKeys;
    auto cArgs = entry.get()->vars;
    for (int i = 0; i != cArgs->len; ++i) {
      auto key = varKeys[i];
      auto arg = std::string(cArgs->tokens[i]);
      args[key] = arg;
    }
    return route->handlerFactory->onRoutedRequest(handler, message, args);
  } else {
    return new MissingRouteHandler(message);
  }
}

MissingRouteHandler::MissingRouteHandler(HTTPMessage* message) {
}

void MissingRouteHandler::onRequest(std::unique_ptr<HTTPMessage> headers) noexcept {
  ResponseBuilder(downstream_)
    .status(404, "Not found")
    .sendWithEOM();
}

void MissingRouteHandler::onBody(std::unique_ptr<folly::IOBuf> body) noexcept {
}

void MissingRouteHandler::onEOM() noexcept {
}

void MissingRouteHandler::onUpgrade(UpgradeProtocol protocol) noexcept {
}

void MissingRouteHandler::requestComplete() noexcept {
  delete this;
}

void MissingRouteHandler::onError(ProxygenError err) noexcept {
  delete this;
}

}
