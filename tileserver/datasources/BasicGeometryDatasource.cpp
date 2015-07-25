#include "BasicGeometryDatasource.h"
#include <mapnik/feature_factory.hpp>
#include <mapnik/geometry_adapters.hpp>

namespace tileserver {

using mapnik::datasource;
using mapnik::parameters;

DATASOURCE_PLUGIN(BasicGeometryDatasource)

typedef mapnik::feature_ptr FeaturePtr;
typedef std::vector<mapnik::feature_ptr> FeatureVector;
typedef mapnik::geometry::geometry<double> Geometry;
typedef std::vector<Geometry> GeometryVector;

BasicGeometryDatasource::BasicGeometryDatasource(mapnik::parameters const& parameters):
  mapnik::datasource(parameters),
  type_(datasource::Vector),
  description_(BasicGeometryDatasource::name(), "utf-8"),
  extent_(-180, -90, 180, 90),
  features_(),
  featureIndex_(nullptr)
{
  boost::optional<GeometryVector> geometries = parameters.get<GeometryVector>("features");
  
  if (geometries) {
    setGeometries(*geometries);
  }
}

BasicGeometryDatasource::~BasicGeometryDatasource() {
}

void BasicGeometryDatasource::setGeometries(GeometryVector const& geometries) {
  int featureId = 0;
  auto context = std::make_shared<mapnik::context_type>();
  for (auto &geom : geometries) {
    FeaturePtr feature(mapnik::feature_factory::create(context, featureId++));
    feature->set_geometry_copy(geom);
    features_.push_back(feature);
    extent_.expand_to_include(feature->envelope());
    //rTree_->insert(shape);
  }
  
  //featureIndex_ = std::unique_ptr(new SpatialIndexType())
}

}