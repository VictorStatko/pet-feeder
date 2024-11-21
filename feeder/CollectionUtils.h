#ifndef COLLECTION_UTILS_H
#define COLLECTION_UTILS_H

#include <Arduino.h>

class CollectionUtils {
public:
  static String joinVector(const std::vector<String>& data);
};

#endif
