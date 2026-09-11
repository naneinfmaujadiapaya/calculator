#include "history.h"
#include "test_framework.h"

void run_history_tests() {
    testing::suite("History");

    {
        History h;
        EXPECT_TRUE(h.entries().empty(), "starts empty");

        h.add("2+2", 4.0);
        EXPECT_EQ(h.entries().size(), 1u, "one entry after one add");
        EXPECT_EQ(h.entries()[0].expression, "2+2", "stores the expression text");
        EXPECT_NEAR(h.entries()[0].result, 4.0, 1e-9, "stores the correct result");

        h.add("3*3", 9.0);
        EXPECT_EQ(h.entries().size(), 2u, "two entries after a second add");
        EXPECT_EQ(h.entries().back().expression, "3*3", "most recent entry is last");

        h.clear();
        EXPECT_TRUE(h.entries().empty(), "clear() empties the history");
    }

    {
        History h;
        for (int i = 0; i < 60; i++) {
            h.add(std::to_string(i), static_cast<double>(i));
        }
        EXPECT_EQ(h.entries().size(), 50u, "caps at 50 entries after 60 adds");
        EXPECT_EQ(h.entries().front().expression, "10",
                  "oldest surviving entry is the 11th add (0-indexed: '10')");
        EXPECT_EQ(h.entries().back().expression, "59", "newest entry ('59') is last");
    }
}
