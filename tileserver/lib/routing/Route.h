#pragma once
#include <tileserver/lib/routing/RoutableHandlerFactory.h>
#include <tileserver/lib/routing/HTTPMethod.h>
#include <string>
#include <vector>
#include <memory>

namespace tileserver {

struct Route {
  Route(std::string path, unsigned int method, std::shared_ptr<RoutableHandlerFactory> handlerFactory);

  std::string path;
  unsigned int method;
  std::shared_ptr<RoutableHandlerFactory> handlerFactory;
  std::vector<std::string> variableKeys;
};

}