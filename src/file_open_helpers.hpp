// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2026 Diego Iastrubni <diegoiast@gmail.com>

#pragma once

#include <string>
#include <string_view>

// Picks ScintillaEdit::set_lexer()'s lexer name from a file's extension.
// The names on the right are Lexilla's own lexer names (the third argument
// to each lexers/*.cxx file's `LexerModule lmXxx(...)`, e.g. LexPython.cxx's
// `lmPython(SCLEX_PYTHON, ..., "python", ...)`) -- every one of them is
// compiled into lexilla (see lexilla/CMakeLists.txt's explicit source
// list), so any name here is guaranteed loadable via CreateLexer(), not a
// guess. Not every one of Lexilla's ~140 lexers has an entry -- many have no
// common/unambiguous extension of their own (or the extension collides with
// something far more common, e.g. classic VB6's .bas vs. FreeBasic/
// PureBasic/BlitzBasic, all skipped here) -- this covers the languages
// likely to actually show up when opening a file. An unrecognized
// extension (or none at all) maps to Lexilla's own "null" lexer (plain
// text, no highlighting at all) rather than guessing "cpp" -- a
// non-C/C++ file styled as if it were one would misrender confusingly,
// where no highlighting at all is an honest "this isn't a language this
// editor recognizes."
//
// Java/JS/TS all route through Lexilla's "cpp" lexer family (SCLEX_CPP)
// just like C/C++ itself -- same brace-based folding, same auto-indent
// behavior, no per-language mapping needed beyond routing them here (see
// ScintillaEdit::set_auto_indent()'s own doc comment). Every other lexer
// below is its own independent lexer, but (per set_auto_indent()'s design)
// only needs the generic "fold" document property to compute fold levels
// the exact same way -- confirmed for python (LexPython.cxx sets
// SC_FOLDLEVELHEADERFLAG the same way LexCPP/LexHTML do); xml/hypertext
// (html) are the only ones seen so far that additionally gate folding
// behind their own "fold.html" property too (see
// ScintillaEdit::apply_fold_properties()'s doc comment).
std::string lexer_for_filename(std::string_view path);
