#include <tileserver/handlers/TileRequestHandler.h>
#include <boost/algorithm/string/join.hpp>
#include <proxygen/httpserver/ResponseBuilder.h>

namespace tileserver {
using namespace proxygen;

TileRequestHandler::TileRequestHandler(std::map<std::string, std::string> args):
  args_(args)  
{
}

void TileRequestHandler::onRequest(std::unique_ptr<HTTPMessage> headers) noexcept {
  for (const auto& kv : args_) {
    std::cout << "{" << kv.first << ": " << kv.second << "}" << std::endl;
  }
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
