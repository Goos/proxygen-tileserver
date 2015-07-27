#pragma once
#include <boost/tuple/tuple.hpp>
#include <mapnik/box2d.hpp>
#include <vector>

namespace tileserver {

enum class Projection {
  WGS84,
  EPSG3857
};

class SphericalMercator {
  public:
  
    SphericalMercator(double tileSize);
  
    mapnik::box2d<double> bbox(const double& x, const double& y, const double& zoom, const Projection& proj = Projection::WGS84);
  
    mapnik::coord2d coordinate(const mapnik::coord2d& screenPoint, const double& zoom);
    
    mapnik::coord2d screenPoint(const mapnik::coord2d& coordinate, const double& zoom);
  
    mapnik::box2d<double> convert(const mapnik::box2d<double>& bbox, const Projection& projection);
  
    mapnik::coord2d forward(const mapnik::coord2d& coordinateWGS);
    mapnik::coord2d inverse(const mapnik::coord2d& coordinate3857);

    double tileSize_;
    std::string proj4_;
    
  private:
    std::vector<double> Bc_;
    std::vector<double> Cc_;
    std::vector<double> zc_;
    std::vector<double> Ac_;

    static const double EPSLN;
    static const double D2R;
    static const double R2D;
    static const double A;
    static const double MAXEXTENT;
};

}
