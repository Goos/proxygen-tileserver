#include <tileserver/lib/projections/SphericalMercator.h>

namespace tileserver {

double SphericalMercator::ESPLN = 1.0e-10;
double SphericalMercator::D2R = M_PI / 180.0;
double SphericalMercator::R2D = 180.0 / M_PI;
double SphericalMercator::A = 6378137;
double SphericalMercator::MAXEXTENT = 20037508.34;

}
