#include <tileserver/lib/routing/Router.h>
#include <proxygen/httpserver/ResponseBuilder.h>
#include <boost/regex.hpp>

namespace tileserver {

using namespace proxygen;

int methodEnumToInt(boost::optional<proxygen::HTTPMethod> methodEnum) {
  if (!methodEnum) { return 0; }
  switch(*methodEnum) {
  case HTTPMethod::GET: return HTTPMethodGET;
  case HTTPMethod::POST: return HTTPMethodPOST;
  case HTTPMethod::PUT: return HTTPMethodPUT;
  case HTTPMethod::DELETE: return HTTPMethodDELETE;
  case HTTPMethod::HEAD: return HTTPMethodHEAD;
  case HTTPMethod::OPTIONS: return HTTPMethodOPTIONS;
  default: return 0;
  }
}

Route::Route(std::string path, unsigned int method, RoutableHandlerFactory* handlerFactory):
  path(path),
  method(method),
  handlerFactory(handlerFactory)
{
  boost::regex expression("\\{(.*?)\\}");
  boost::sregex_iterator next(path.begin(), path.end(), expression);
  boost::sregex_iterator end;
  std::for_each(next, end, [this](const boost::match_results<std::string::const_iterator>& match) {
    variableKeys.push_back(match[1].str());
  });
}

Route::~Route() {
  //delete handlerFactory;
}

Router::Router(std::vector<Route*> routes):
  routeTree_(routes.size()),
  routes_(routes)
{ 
  for (auto iterator = routes.begin(); iterator != routes.end(); iterator++) {
    auto route = *iterator;   
    auto path = route->path.c_str();
    void *data = route;
    routeTree_.insert_route(route->method, path, data);
  }

  char *errStr;
  int err = routeTree_.compile(&errStr);
  if (err != 0) {
    std::cout << "Error compiling route tree: " << errStr << std::endl;
    delete errStr;
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
  auto cPath = path.substr(1, path.size() - 1).c_str();
  r3::MatchEntry entry(cPath);
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
