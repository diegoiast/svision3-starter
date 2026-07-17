// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2026 Diego Iastrubni <diegoiast@gmail.com>

#include "../src/file_open_helpers.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("lexer_for_filename picks the right lexer by extension", "[file_open]") {
    // C-family -- Java/JS/TS share "cpp" (see lexer_for_filename()'s doc comment)
    REQUIRE(lexer_for_filename("main.cpp") == "cpp");
    REQUIRE(lexer_for_filename("widget.hpp") == "cpp");
    REQUIRE(lexer_for_filename("Main.java") == "cpp");
    REQUIRE(lexer_for_filename("app.js") == "cpp");
    REQUIRE(lexer_for_filename("app.jsx") == "cpp");
    REQUIRE(lexer_for_filename("app.ts") == "cpp");
    REQUIRE(lexer_for_filename("app.tsx") == "cpp");

    // Lexers with their own name
    REQUIRE(lexer_for_filename("script.py") == "python");
    REQUIRE(lexer_for_filename("script.pyw") == "python");
    REQUIRE(lexer_for_filename("SCRIPT.PY") == "python"); // case-insensitive
    REQUIRE(lexer_for_filename("data.json") == "json");
    REQUIRE(lexer_for_filename("DATA.JSON") == "json");
    REQUIRE(lexer_for_filename("page.xml") == "xml");
    REQUIRE(lexer_for_filename("icon.svg") == "xml"); // SVG is XML under a different extension
    REQUIRE(lexer_for_filename("page.html") == "hypertext");
    REQUIRE(lexer_for_filename("page.htm") == "hypertext");
    REQUIRE(lexer_for_filename("index.php") == "phpscript");
    REQUIRE(lexer_for_filename("style.css") == "css");
    REQUIRE(lexer_for_filename("run.sh") == "bash");
    REQUIRE(lexer_for_filename("run.bat") == "batch");
    REQUIRE(lexer_for_filename("script.lua") == "lua");
    REQUIRE(lexer_for_filename("script.pl") == "perl");
    REQUIRE(lexer_for_filename("script.rb") == "ruby");
    REQUIRE(lexer_for_filename("lib.rs") == "rust");
    REQUIRE(lexer_for_filename("query.sql") == "sql");
    REQUIRE(lexer_for_filename("CMakeLists.cmake") == "cmake");
    REQUIRE(lexer_for_filename("config.yaml") == "yaml");
    REQUIRE(lexer_for_filename("config.yml") == "yaml");
    REQUIRE(lexer_for_filename("Cargo.toml") == "toml");
    REQUIRE(lexer_for_filename("README.md") == "markdown");
    REQUIRE(lexer_for_filename("change.diff") == "diff");
    REQUIRE(lexer_for_filename("settings.ini") == "props");
    REQUIRE(lexer_for_filename("Module.vb") == "vb");
    REQUIRE(lexer_for_filename("script.vbs") == "vbscript");
    REQUIRE(lexer_for_filename("unit.pas") == "pascal");
    REQUIRE(lexer_for_filename("core.v") == "verilog");
    REQUIRE(lexer_for_filename("core.vhdl") == "vhdl");
    REQUIRE(lexer_for_filename("main.d") == "d");
    REQUIRE(lexer_for_filename("main.dart") == "dart");
    REQUIRE(lexer_for_filename("module.erl") == "erlang");
    REQUIRE(lexer_for_filename("prog.f90") == "fortran");
    REQUIRE(lexer_for_filename("Main.hs") == "haskell");
    REQUIRE(lexer_for_filename("Main.lhs") == "literatehaskell");
    REQUIRE(lexer_for_filename("script.jl") == "julia");
    REQUIRE(lexer_for_filename("main.nim") == "nim");
    REQUIRE(lexer_for_filename("boot.asm") == "asm");
    REQUIRE(lexer_for_filename("settings.reg") == "registry");
    REQUIRE(lexer_for_filename("paper.tex") == "tex");
    REQUIRE(lexer_for_filename("unit.ada") == "ada");
    REQUIRE(lexer_for_filename("main.zig") == "zig");
    REQUIRE(lexer_for_filename("player.gd") == "gdscript");

    // Fallback
    REQUIRE(lexer_for_filename("README") == "markdown"); // exact-uppercase, no extension -- special-cased
    REQUIRE(lexer_for_filename("readme") == "null");      // not the exact uppercase form -- falls back
    REQUIRE(lexer_for_filename("notes.txt") == "null");   // unrecognized extension -- falls back
}

// Indentation detection (tabs vs. spaces, and the space width) lives in
// ScintillaEdit::detect_and_apply_indentation() now, not here -- see
// scintilla-svision3/tests/indentation_detection_smoke_test.cpp.
