#pragma once
#include <tileserver/datasources/BasicGeometryDatasource.h>
#include <mapnik/feature.hpp>

namespace tileserver {

class BasicGeometryFeatureset: public mapnik::Featureset {
  public:
    BasicGeometryFeatureset(std::vector<mapnik::feature_ptr> features);
    ~BasicGeometryFeatureset();
  
    mapnik::feature_ptr next();
  
  private:
    std::vector<mapnik::feature_ptr> features_;
    std::vector<mapnik::feature_ptr>::const_iterator iterator_;
    std::vector<mapnik::feature_ptr>::const_iterator end_;
};

}