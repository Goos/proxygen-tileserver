#pragma once
#include <mapnik/box2d.hpp>
#include <map>
#include <string>

namespace tileserver {

class TileRequest {
  public:
    TileRequest(mapnik::box2d<double> bbox, double tileSize = 256.0, double scaleFactor = 1.0, std::string imageFormat = "png", std::map<std::string, std::string> routingArguments = {});
  
  mapnik::box2d<double> bbox() const;
  double tileSize() const;
  double scaleFactor() const;
  std::string imageFormat() const;
  std::map<std::string, std::string> routingArguments() const;
  
  protected:
    mapnik::box2d<double> bbox_;
    double tileSize_;
    double scaleFactor_;
    std::string imageFormat_;
    std::map<std::string, std::string> routingArguments_;
};

}