#include "format.h"
#include <cmath>
#include <cstdio>

std::string FormatNumber(double value, int significantDigits) {
    if (value == 0.0) return "0";  // biar -0.0 gak ketampil

    char buf[64];

    if (std::floor(value) == value && std::fabs(value) < 1e15) {
        std::snprintf(buf, sizeof(buf), "%.0f", value);
        return std::string(buf);
    }

    std::snprintf(buf, sizeof(buf), "%.*g", significantDigits, value);
    return std::string(buf);
}
