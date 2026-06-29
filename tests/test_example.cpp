// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2026 Diego Iastrubni <diegoiast@gmail.com>

#include <catch2/catch_test_macros.hpp>
#include <string>
#include <vector>

// Replace these with tests for your own application logic.

TEST_CASE("arithmetic", "[math]") {
    REQUIRE(1 + 1 == 2);
    REQUIRE(10 - 3 == 7);
    REQUIRE(6 * 7 == 42);
}

TEST_CASE("string operations", "[string]") {
    std::string s = "SVision3";
    REQUIRE(s.size() == 8);
    REQUIRE(s.substr(0, 7) == "SVision");
    REQUIRE(s + "Demo" == "SVision3Demo");
}

TEST_CASE("vector operations", "[container]") {
    std::vector<int> v = {1, 2, 3, 4, 5};
    REQUIRE(v.size() == 5);
    REQUIRE(v.front() == 1);
    REQUIRE(v.back() == 5);

    v.push_back(6);
    REQUIRE(v.size() == 6);
}
