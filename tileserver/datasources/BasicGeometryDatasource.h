#pragma once
#include <mapnik/datasource.hpp>
#include <mapnik/feature.hpp>
#include <mapnik/geometry.hpp>
#include <boost/geometry/index/rtree.hpp>
#include <boost/geometry.hpp>
#include <boost/optional.hpp>

namespace tileserver {

template <std::size_t Max, std::size_t Min>
struct GeometryIndexLinear : boost::geometry::index::linear<Max, Min> {};

}

namespace boost { namespace geometry { namespace index { namespace detail { namespace rtree {

template <std::size_t Max, std::size_t Min>
struct options_type<tileserver::GeometryIndexLinear<Max, Min>>
{
  typedef options<
    tileserver::GeometryIndexLinear<Max, Min>,
    insert_default_tag,
    choose_by_content_diff_tag,
    split_default_tag,
    linear_tag,
    node_variant_static_tag
  > type;
};

}}}}}

namespace tileserver {

class BasicGeometryDatasource: public mapnik::datasource {
  public:
    typedef mapnik::box2d<double> BoxType;
    typedef std::pair<BoxType, std::size_t> ItemType;
    typedef boost::geometry::index::rtree<ItemType, GeometryIndexLinear<16, 4>> SpatialIndexType;

    BasicGeometryDatasource(mapnik::parameters const& parameters);
    ~BasicGeometryDatasource();
  
    void setGeometries(std::vector<mapnik::geometry::geometry<double>> const& geometries);

    mapnik::datasource::datasource_t type() const;
    static const char* name();
    boost::optional<mapnik::datasource_geometry_t> get_geometry_type() const;
    mapnik::layer_descriptor get_descriptor() const;
  
    mapnik::box2d<double> envelope() const;
    mapnik::featureset_ptr features(mapnik::query const& q) const;
    mapnik::featureset_ptr features_at_point(mapnik::coord2d const& pt, double tol = 0) const;
  
  protected:
    static const std::string name_;
    mapnik::datasource::datasource_t type_;
    mapnik::layer_descriptor description_;
    mapnik::box2d<double> extent_;
    std::vector<mapnik::feature_ptr> features_;
    std::unique_ptr<SpatialIndexType> featureIndex_;
  
  private:
};

}
