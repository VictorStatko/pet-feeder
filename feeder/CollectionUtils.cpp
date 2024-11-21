#include "CollectionUtils.h"

String CollectionUtils::joinVector(const std::vector<String>& data) {
  String result = "";
  for (size_t i = 0; i < data.size(); ++i) {
    result += data[i];
    if (i != data.size() - 1) {
      result += ",";  // Add comma only between elements
    }
  }
  return result;
}