#include "TileRequest.h"

namespace tileserver {

TileRequest::TileRequest(mapnik::box2d<double> bbox, double tileSize, double scaleFactor, std::string imageFormat, std::map<std::string, std::string> routingArguments):
  bbox_(bbox),
  tileSize_(tileSize),
  scaleFactor_(scaleFactor),
  imageFormat_(imageFormat),
  routingArguments_(routingArguments)
{
}

mapnik::box2d<double> TileRequest::bbox() const {
  return bbox_;
}

double TileRequest::tileSize() const {
  return tileSize_;
}

double TileRequest::scaleFactor() const {
  return scaleFactor_;
}

std::string TileRequest::imageFormat() const {
  return imageFormat_;
}

std::map<std::string, std::string> TileRequest::routingArguments() const {
  return routingArguments_;
}

}