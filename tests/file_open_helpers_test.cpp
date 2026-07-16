// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2026 Diego Iastrubni <diegoiast@gmail.com>

#include "../src/file_open_helpers.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("lexer_for_path picks the right lexer by extension", "[file_open]") {
    REQUIRE(lexer_for_path("main.cpp") == "cpp");
    REQUIRE(lexer_for_path("widget.hpp") == "cpp");
    REQUIRE(lexer_for_path("Main.java") == "cpp");
    REQUIRE(lexer_for_path("app.js") == "cpp");
    REQUIRE(lexer_for_path("app.tsx") == "cpp");
    REQUIRE(lexer_for_path("data.json") == "json");
    REQUIRE(lexer_for_path("DATA.JSON") == "json"); // case-insensitive
    REQUIRE(lexer_for_path("page.xml") == "xml");
    REQUIRE(lexer_for_path("page.html") == "xml");
    REQUIRE(lexer_for_path("page.htm") == "xml");
    REQUIRE(lexer_for_path("README") == "cpp");    // no extension -- falls back
    REQUIRE(lexer_for_path("notes.txt") == "cpp"); // unrecognized -- falls back
}

TEST_CASE("detect_indentation sniffs the first indented line's style", "[file_open]") {
    auto const [tabs_use_tabs, tabs_width] = detect_indentation("int main() {\n\treturn 0;\n}\n");
    REQUIRE(tabs_use_tabs == true);
    REQUIRE(tabs_width == 8);

    auto const [four_use_tabs, four_width] = detect_indentation("int main() {\n    return 0;\n}\n");
    REQUIRE(four_use_tabs == false);
    REQUIRE(four_width == 4);

    auto const [two_use_tabs, two_width] = detect_indentation("{\n  \"a\": 1\n}\n");
    REQUIRE(two_use_tabs == false);
    REQUIRE(two_width == 2);

    // No indented line at all -- keep Scintilla's own tabs/8 default.
    auto const [flat_use_tabs, flat_width] = detect_indentation("no indentation here\nor here\n");
    REQUIRE(flat_use_tabs == true);
    REQUIRE(flat_width == 8);

    // Empty document -- same fallback.
    auto const [empty_use_tabs, empty_width] = detect_indentation("");
    REQUIRE(empty_use_tabs == true);
    REQUIRE(empty_width == 8);
}
