#include "format.h"
#include "test_framework.h"

void run_format_tests() {
    testing::suite("FormatNumber");

    EXPECT_EQ(FormatNumber(0.0), "0", "zero prints as '0'");
    EXPECT_EQ(FormatNumber(-0.0), "0", "negative zero prints as '0', not '-0'");
    EXPECT_EQ(FormatNumber(42.0), "42", "whole number prints without a decimal point");
    EXPECT_EQ(FormatNumber(-7.0), "-7", "negative whole number prints plainly");
    EXPECT_EQ(FormatNumber(0.1), "0.1", "0.1 prints cleanly despite binary rounding");
    EXPECT_EQ(FormatNumber(1.0 / 3.0), "0.333333333333",
              "1/3 gets full precision (12 significant digits), not to_string's fixed 6");
    EXPECT_EQ(FormatNumber(123456789012345.0), "123456789012345",
              "large exact integer within double's exact range prints in full");
}
