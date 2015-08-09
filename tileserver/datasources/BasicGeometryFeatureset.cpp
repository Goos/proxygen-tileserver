#include "BasicGeometryFeatureset.h"

namespace tileserver {

BasicGeometryFeatureset::BasicGeometryFeatureset(std::vector<mapnik::feature_ptr> features):
  features_(features),
  iterator_(features_.begin()),
  end_(features_.end())
{
}

BasicGeometryFeatureset::~BasicGeometryFeatureset() {
}

mapnik::feature_ptr BasicGeometryFeatureset::next() {
  if (iterator_ != end_) {
    return *iterator_++;
  }
  
  return mapnik::feature_ptr();
}

}

