#pragma once
#include <mapnik/box2d.hpp>

namespace tileserver {

class TileRequest {
  public:
    TileRequest(mapnik::box2d<double> bbox, double tileSize = 256, double scaleFactor = 1.0);
  
  protected:
    mapnik::box2d<double> bbox_;
    double tileSize_;
    double scaleFactor_;
};

}