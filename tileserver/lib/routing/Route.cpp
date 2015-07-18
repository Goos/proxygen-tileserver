#include "Route.h"
#include <boost/regex.hpp>
#include <algorithm>

namespace tileserver {

Route::Route(std::string path, unsigned int method, std::shared_ptr<RoutableHandlerFactory> handlerFactory):
  path(path),
  method(method),
  handlerFactory(handlerFactory)
{
  boost::regex expression("\\{(.*?)[:\\}]");
  boost::sregex_iterator next(path.begin(), path.end(), expression);
  boost::sregex_iterator end;
  std::for_each(next, end, [this](const boost::match_results<std::string::const_iterator>& match) {
    variableKeys.push_back(match[1].str());
  });
}

}