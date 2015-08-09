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
  std::vector<ItemType> pairs;
  std::size_t idx = 0;
  for (auto &geom : geometries) {
    FeaturePtr feature(mapnik::feature_factory::create(context, featureId++));
    feature->set_geometry_copy(geom);
    features_.push_back(feature);
    auto bbox = feature->envelope();
    extent_.expand_to_include(bbox);
    pairs.push_back(ItemType(bbox, idx));
    idx++;
  }
  
  featureIndex_ = std::unique_ptr<SpatialIndexType>(new SpatialIndexType(pairs));
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
  
    // Querying the spatial index for features that are within the
    // given bounding-box.
    featureIndex_->query(boost::geometry::index::intersects(bbox), std::back_inserter(matches));
    
    // The spatial index only stores the indices to the features rather
    // than the features themselves, so we're mapping the indices to
    // the features from the feature vector.
    std::transform(matches.begin(), matches.end(), std::back_inserter(features), [this](ItemType match) {
      auto idx = match.second;
      return features_[idx];
    });
    
    return std::make_shared<BasicGeometryFeatureset>(features);
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