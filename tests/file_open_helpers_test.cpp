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
    REQUIRE(lexer_for_path("icon.svg") == "xml"); // SVG is XML under a different extension
    REQUIRE(lexer_for_path("script.py") == "python");
    REQUIRE(lexer_for_path("script.pyw") == "python");
    REQUIRE(lexer_for_path("SCRIPT.PY") == "python"); // case-insensitive
    REQUIRE(lexer_for_path("README") == "cpp");        // no extension -- falls back
    REQUIRE(lexer_for_path("notes.txt") == "cpp");     // unrecognized -- falls back
}

// Indentation detection (tabs vs. spaces, and the space width) lives in
// ScintillaEdit::detect_and_apply_indentation() now, not here -- see
// scintilla-svision3/tests/indentation_detection_smoke_test.cpp.
