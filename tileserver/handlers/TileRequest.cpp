#include "TileRequest.h"

namespace tileserver {

TileRequest::TileRequest(mapnik::box2d<double> bbox, double tileSize, double scaleFactor):
  bbox_(bbox), tileSize_(tileSize), scaleFactor_(scaleFactor)
{
}

}