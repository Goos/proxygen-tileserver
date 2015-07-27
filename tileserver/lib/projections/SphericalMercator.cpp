#include <tileserver/lib/projections/SphericalMercator.h>
#include <algorithm>
#include <math.h>
#include <iostream>
#include <string>

namespace tileserver {

using namespace mapnik;

const double SphericalMercator::EPSLN = 1.0e-10;
const double SphericalMercator::D2R = M_PI / 180.0;
const double SphericalMercator::R2D = 180.0 / M_PI;
const double SphericalMercator::A = 6378137;
const double SphericalMercator::MAXEXTENT = 20037508.34;

SphericalMercator::SphericalMercator(double tileSize):
  tileSize_(tileSize)
{
  for (int i = 0; i < 30; ++i) {
    Bc_.push_back(tileSize / 360.0);
    Cc_.push_back(tileSize / (2.0 * M_PI));
    zc_.push_back(tileSize / 2.0);
    Ac_.push_back(tileSize);
    tileSize *= 2.0;
  }
  
  proj4_ = "+proj=merc +a=6378137 +b=6378137 +lat_ts=0.0 +lon_0=0.0 +x_0=0.0 +y_0=0 +k=1.0 +units=m +nadgrids=@null +wktext +no_defs +over";
}

box2d<double> SphericalMercator::bbox(const double& x, const double& y, const double& zoom, const Projection& proj) {
  coord2d lowerLeft = coord2d(x * tileSize_, (y + 1) * tileSize_);
  coord2d upperRight = coord2d((x + 1) * tileSize_, y * tileSize_);
  box2d<double> box = box2d<double>(coordinate(lowerLeft, zoom), coordinate(upperRight, zoom));
  
  if (proj == Projection::EPSG3857) {
    return convert(box, proj);
  } else {
    return box;
  }
}

coord2d SphericalMercator::coordinate(const coord2d& screenPoint, const double& zoom) {
  double g = (screenPoint.y - zc_[zoom]) / (-Cc_[zoom]);
  double lon = (screenPoint.x - zc_[zoom]) / Bc_[zoom];
  double lat = R2D * (2 * atan(exp(g)) - 0.5 * M_PI);
  return coord2d(lon, lat);
}

coord2d SphericalMercator::screenPoint(const coord2d& coordinate, const double& zoom) {
  double d = zc_[zoom];
  double f = std::min(std::max(sin(D2R * coordinate.y), -0.9999), 0.9999);
  double x = round(d + coordinate.x * Bc_[zoom]);
  double y = round(d + 0.5 * log((1 + f) / (1 - f)) * (-Cc_[zoom]));
  if (x > Ac_[zoom]) {
    x = Ac_[zoom];
  }
  if (y > Ac_[zoom]) {
    y = Ac_[zoom];
  }
  return coord2d(x, y);
}

box2d<double> SphericalMercator::convert(const box2d<double>& bbox, const Projection& projection) {
  coord2d lowerLeft = coord2d(bbox.minx(), bbox.miny());
  coord2d upperRight = coord2d(bbox.maxx(), bbox.maxy());
  if (projection == Projection::EPSG3857) {
    return box2d<double>(forward(lowerLeft), forward(upperRight));
  } else {
    return box2d<double>(inverse(lowerLeft), forward(upperRight));
  }
}

coord2d SphericalMercator::forward(const coord2d& coordinateWGS) {
  coord2d coordinate3857 = coord2d(
    A * coordinateWGS.x * D2R,
    A * log(tan((M_PI * 0.25) + (0.5 * coordinateWGS.y * D2R)))
  );
  
  if (coordinate3857.x > MAXEXTENT) { coordinate3857.x = MAXEXTENT; }
  if (coordinate3857.x < -MAXEXTENT) { coordinate3857.x = -MAXEXTENT; }
  if (coordinate3857.y > MAXEXTENT) { coordinate3857.y = MAXEXTENT; }
  if (coordinate3857.y < -MAXEXTENT) { coordinate3857.y = -MAXEXTENT; }
  
  return coordinate3857;
}

coord2d SphericalMercator::inverse(const coord2d& coordinate3857) {
  return coord2d(
    (coordinate3857.x * R2D / A),
    ((M_PI * 0.5) - 2.0 * atan(exp(-coordinate3857.y / A))) * R2D
  );
}

}