#pragma once
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/sequenced_index.hpp>

namespace tileserver {

//template <typename T>
//class LRUCache {
//  using LRUList = boost::multi_index_container<
//    T,
//    boost::indexed_by<
//      boost::sequence<>,
//      boost::hashed_unique<boost::identity<T>>
//    >
//  >;
//  
//  
//}

}