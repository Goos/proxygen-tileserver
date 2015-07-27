#include <tileserver/datasources/BasicGeometryDatasource.h>
#include <tileserver/datasources/BasicGeometryFeatureset.h>
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
    
mapnik::datasource::datasource_t BasicGeometryDatasource::type() const {
  return type_;
}

const char* BasicGeometryDatasource::name() {
  return "BasicGeometry";
}

boost::optional<mapnik::datasource_geometry_t> BasicGeometryDatasource::get_geometry_type() const {
  return mapnik::datasource_geometry_t::Collection;
}

mapnik::layer_descriptor BasicGeometryDatasource::get_descriptor() const {
  return description_;
}

BasicGeometryDatasource::BoxType BasicGeometryDatasource::envelope() const {
  return extent_;
}

mapnik::featureset_ptr BasicGeometryDatasource::features(mapnik::query const& q) const {
  BoxType const& bbox = q.get_bbox();
  if (featureIndex_ && extent_.intersects(bbox)) {
    std::deque<ItemType> matches;
    std::vector<mapnik::feature_ptr> features;
  
    featureIndex_->query(boost::geometry::index::intersects(bbox), std::back_inserter(matches));
    std::transform(matches.begin(), matches.end(), features.begin(), [this](ItemType match) {
      auto idx = match.second;
      auto feature = features_[idx];
      return feature;
    });
    
    return std::make_shared<BasicGeometryFeatureset>(std::move(features));
  } else {
    return mapnik::featureset_ptr();
  }
}

mapnik::featureset_ptr BasicGeometryDatasource::features_at_point(const mapnik::coord2d &pt, double tol) const {
  BoxType bbox(pt, pt);
  bbox.pad(tol);
  mapnik::query query(bbox);
  return features(query);
}

}