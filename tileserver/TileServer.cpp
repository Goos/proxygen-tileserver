#include <tileserver/lib/routing/Router.h>
#include <tileserver/handlers/TileRequestHandlerFactory.h>
#include <gflags/gflags.h>
#include <folly/Portability.h>
#include <folly/io/async/EventBaseManager.h>
#include <proxygen/httpserver/HTTPServer.h>
#include <vector>
#include <chrono>

using namespace tileserver;
using namespace proxygen;

using folly::EventBase;
using folly::EventBaseManager;
using folly::SocketAddress;

using Protocol = HTTPServer::Protocol;

DEFINE_int32(http_port, 11000, "Port used to listen with HTTP protocol.");
DEFINE_int32(spdy_port, 11001, "Port used to listen with SPDY protocol.");
DEFINE_string(ip, "localhost", "IP/hostname to bind to.");
DEFINE_int32(threads, 0, "Number of threads to listen on. Numbers <= 0 will use the number of cores in the current machine.");

int main(int argc, char* argv[]) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);
  google::InstallFailureSignalHandler();

  std::vector<HTTPServer::IPConfig> IPs = {
    {SocketAddress(FLAGS_ip, FLAGS_http_port, true), Protocol::HTTP},
    {SocketAddress(FLAGS_ip, FLAGS_spdy_port, true), Protocol::SPDY},
  };

  if (FLAGS_threads <= 0) {
    FLAGS_threads = sysconf(_SC_NPROCESSORS_ONLN);
    CHECK(FLAGS_threads > 0);
  }

  auto tileHandlerFactory = std::make_shared<TileRequestHandlerFactory>();
  std::vector<std::shared_ptr<Route>> routes = {
    std::make_shared<Route>("results/{x:\\d+}/{y:\\d+}/{z:\\d+}@{scale}x.{filetype}", HTTPMethodGET, tileHandlerFactory),
    std::make_shared<Route>("results/{x:\\d+}/{y:\\d+}/{z:\\d+}.{filetype}", HTTPMethodGET, tileHandlerFactory),
    std::make_shared<Route>("locations/{x}/{y}/{z}", HTTPMethodGET, tileHandlerFactory)
  };

  HTTPServerOptions options;
  options.threads = static_cast<size_t>(FLAGS_threads);
  options.idleTimeout = std::chrono::milliseconds(60000);
  options.shutdownOn = {SIGINT, SIGTERM};
  options.enableContentCompression = false;
  options.handlerFactories = RequestHandlerChain().addThen<Router>(routes).build();

  HTTPServer server(std::move(options));
  server.bind(IPs);

  std::thread t([&] () {
    server.start();
  });

  t.join();
  return 0;
}
