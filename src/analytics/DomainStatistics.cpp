#include "../../include/analytics/DomainStatistics.h"
#include <algorithm>
#include <cctype> // For std::tolower

// The implementation is fully in the header, as it's a small class and doesn't require a separate .cpp file for definitions.
// The tolower conversion for domain normalization should be handled within the increment method in the header file.
