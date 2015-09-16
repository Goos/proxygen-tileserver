#include <tileserver/handlers/TileRequestHandler.h>
#include <tileserver/datasources/BasicGeometryDatasource.h>

#include <mapnik/datasource_cache.hpp>
#include <mapnik/layer.hpp>
#include <mapnik/image.hpp>
#include <mapnik/image_util.hpp>
#include <mapnik/feature_type_style.hpp>
#include <mapnik/rule.hpp>
#include <mapnik/symbolizer_utils.hpp>

#include <boost/algorithm/string/join.hpp>
#include <proxygen/httpserver/ResponseBuilder.h>
#include <folly/io/async/EventBaseManager.h>

#include <thread>
#include <chrono>
#include <random>

namespace tileserver {

using namespace proxygen;

TileRequestHandler::TileRequestHandler(
      mapnik::Map* map,
      folly::wangle::CPUThreadPoolExecutor* renderingPool,
      folly::wangle::IOThreadPoolExecutor* ioPool):
  map_(map),
  renderingPool_(renderingPool),
  ioPool_(ioPool)
{
}

TileRequestHandler::~TileRequestHandler() {
  delete map_;
}

void TileRequestHandler::onRequest(std::unique_ptr<HTTPMessage> headers) noexcept {
  auto eventBase = folly::EventBaseManager::get()->getExistingEventBase();

  renderingPool_->add([this, eventBase]() {
    mapnik::parameters params;
    params["type"] = "BasicGeometry";
    auto datasource = std::make_shared<BasicGeometryDatasource>(params);
    auto bbox = map_->get_current_extent();
    auto center = bbox.center();
    
    /* Temporarily filling the datasource with some arbitrary test-data 
       in order to test the datasource. */
    std::vector<mapnik::geometry::geometry<double>> geoms;
    std::default_random_engine re;
    std::uniform_real_distribution<double> unifx(bbox.minx(), bbox.maxx());
    std::uniform_real_distribution<double> unify(bbox.miny(), bbox.maxy());
    for (int i = 0; i < 1000; i++) {
      geoms.push_back(mapnik::geometry::point<double>(unifx(re), unify(re)));
    }
    datasource->setGeometries(geoms);
    
    mapnik::feature_type_style style;
    
    mapnik::rule rule;
    mapnik::dot_symbolizer dotSym;
    put(dotSym, mapnik::keys::fill, mapnik::color(0,0,0));
    put(dotSym, mapnik::keys::width, 12.0);
    put(dotSym, mapnik::keys::height, 12.0);
    put(dotSym, mapnik::keys::opacity, 0.15);
    rule.append(std::move(dotSym));
    style.add_rule(std::move(rule));
    map_->insert_style("dotstyle", std::move(style));
    
    mapnik::layer layer("dotlayer", map_->srs());
    layer.set_datasource(datasource);
    layer.add_style("dotstyle");
    map_->add_layer(layer);
    
    map_->set_background(mapnik::color(255,255,255,255));
    
    mapnik::image_rgba8 image(map_->width(), map_->height());
    mapnik::agg_renderer<mapnik::image_rgba8> renderer(*map_, image);
    renderer.apply();
    auto imageStr = mapnik::save_to_string(image, "png");
    
    eventBase->runInEventBaseThread([this, imageStr]() {
      ResponseBuilder(this->downstream_)
        .status(200, "OK")
        .header(HTTP_HEADER_CONTENT_TYPE, "image/png")
        .body(std::move(imageStr))
        .sendWithEOM();
      });
  });
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
