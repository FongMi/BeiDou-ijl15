#pragma once
#include <string>

struct KeyValuePair {
    int key;
    std::string value;
};

extern KeyValuePair keyValuePairs[];
extern const int keyValuePairsCount;
extern const char* const ALLIANCE;