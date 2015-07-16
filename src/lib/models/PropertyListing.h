#pragma once
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>

namespace tileserver {

class PropertyListing final {
  boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian> coordinate;
  std::string type;
};

}
