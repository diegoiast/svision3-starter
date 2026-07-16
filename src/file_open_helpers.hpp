// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2026 Diego Iastrubni <diegoiast@gmail.com>

#pragma once

#include <cctype>
#include <string>
#include <string_view>
#include <utility>

// Picks ScintillaEdit::set_lexer()'s lexer name from a file's extension.
// Java/JS/TS all route through Lexilla's "cpp" lexer family (SCLEX_CPP)
// just like C/C++ itself -- same brace-based folding, same auto-indent
// behavior, no per-language mapping needed beyond routing them here (see
// ScintillaEdit::set_auto_indent()'s own doc comment). Python is its own
// lexer (LexPython.cxx, indentation/colon-based blocks, not braces) but
// still only needs the generic "fold" document property (no "fold.html"-
// style extra gate), so it's covered by the same auto-indent/folding
// mechanism with zero extra code either -- confirmed by
// LexPython.cxx setting SC_FOLDLEVELHEADERFLAG the same way LexCPP/
// LexHTML do. SVG is plain XML under a different extension, so it maps to
// the "xml" lexer too.
inline std::string lexer_for_path(std::string_view path) {
    auto const dot = path.find_last_of('.');
    if (dot == std::string_view::npos) {
        return "cpp";
    }
    auto ext = std::string(path.substr(dot + 1));
    for (auto &c : ext) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    if (ext == "json") {
        return "json";
    }
    if (ext == "xml" || ext == "html" || ext == "htm" || ext == "svg") {
        return "xml";
    }
    if (ext == "py" || ext == "pyw") {
        return "python";
    }
    return "cpp"; // c/cpp/cc/cxx/h/hpp/hxx/java/js/jsx/ts/tsx, and anything unrecognized
}

// Sniffs the file's own existing indentation convention from its first
// indented line, rather than assuming Scintilla's own tab-width-8-and-tabs
// default -- the same "match the document's existing style" spirit as
// ScintillaEdit::set_auto_indent()'s enclosing_indent() step-inference, just
// applied once up front instead of line by line. Returns {use_tabs, width}.
inline std::pair<bool, int> detect_indentation(std::string_view text) {
    size_t pos = 0;
    while (pos < text.size()) {
        auto const eol = text.find('\n', pos);
        auto const line = text.substr(pos, eol == std::string_view::npos ? std::string_view::npos : eol - pos);
        if (!line.empty() && line[0] == '\t') {
            return {true, 8}; // tabs -- keep Scintilla's own default width
        }
        if (!line.empty() && line[0] == ' ') {
            size_t width = 0;
            while (width < line.size() && line[width] == ' ') {
                ++width;
            }
            return {false, static_cast<int>(width)};
        }
        if (eol == std::string_view::npos) {
            break;
        }
        pos = eol + 1;
    }
    return {true, 8}; // no indented line found -- keep Scintilla's own defaults
}
