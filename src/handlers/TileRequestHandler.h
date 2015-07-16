#pragma once
#include <proxygen/httpserver/RequestHandler.h>

namespace tileserver {

class TileRequestHandler : public proxygen::RequestHandler {
  public:
    TileRequestHandler(std::map<std::string, std::string> args);

    void onRequest(std::unique_ptr<proxygen::HTTPMessage> headers) noexcept override;

    void onBody(std::unique_ptr<folly::IOBuf> body) noexcept override;

    void onEOM() noexcept override;

    void onUpgrade(proxygen::UpgradeProtocol proto) noexcept override;

    void requestComplete() noexcept override;

    void onError(proxygen::ProxygenError err) noexcept override;

  protected:
    std::map<std::string, std::string> args_;
};

}
