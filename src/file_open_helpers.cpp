// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2026 Diego Iastrubni <diegoiast@gmail.com>

#include "file_open_helpers.hpp"
#include <algorithm>
#include <cctype>
#include <filesystem>

std::string lexer_for_filename(std::string_view path) {
    struct Mapping {
        std::string_view ext;
        std::string_view lexer;
    };
    // clang-format off
    static constexpr Mapping mappings[] = {
        // C-family (shares "cpp" with Java/JS/TS -- see doc comment above)
        {"c", "cpp"}, {"cpp", "cpp"}, {"cc", "cpp"}, {"cxx", "cpp"}, {"c++", "cpp"},
        {"h", "cpp"}, {"hpp", "cpp"}, {"hxx", "cpp"}, {"h++", "cpp"}, {"inl", "cpp"},
        {"ino", "cpp"}, {"java", "cpp"},
        {"js", "cpp"}, {"jsx", "cpp"}, {"mjs", "cpp"}, {"cjs", "cpp"},
        {"ts", "cpp"}, {"tsx", "cpp"},
        // Own lexers
        {"py", "python"}, {"pyw", "python"}, {"pyi", "python"},
        {"json", "json"}, {"jsonc", "json"},
        {"xml", "xml"}, {"xsd", "xml"}, {"xsl", "xml"}, {"xslt", "xml"},
        {"svg", "xml"}, {"rss", "xml"}, {"atom", "xml"}, {"xaml", "xml"}, {"plist", "xml"},
        {"html", "hypertext"}, {"htm", "hypertext"}, {"xhtml", "hypertext"}, {"shtml", "hypertext"},
        {"php", "phpscript"}, {"php3", "phpscript"}, {"php4", "phpscript"},
        {"php5", "phpscript"}, {"phtml", "phpscript"},
        {"css", "css"},
        {"sh", "bash"}, {"bash", "bash"}, {"zsh", "bash"}, {"ksh", "bash"},
        {"bat", "batch"}, {"cmd", "batch"},
        {"lua", "lua"},
        {"pl", "perl"}, {"pm", "perl"}, {"perl", "perl"},
        {"rb", "ruby"}, {"rbw", "ruby"}, {"gemspec", "ruby"},
        {"rs", "rust"},
        {"sql", "sql"},
        {"cmake", "cmake"},
        {"yaml", "yaml"}, {"yml", "yaml"},
        {"toml", "toml"},
        {"md", "markdown"}, {"markdown", "markdown"},
        {"diff", "diff"}, {"patch", "diff"},
        {"ini", "props"}, {"cfg", "props"}, {"properties", "props"},
        {"vb", "vb"}, {"vbs", "vbscript"},
        {"pas", "pascal"}, {"pp", "pascal"},
        {"v", "verilog"}, {"vh", "verilog"},
        {"vhd", "vhdl"}, {"vhdl", "vhdl"},
        {"d", "d"},
        {"dart", "dart"},
        {"erl", "erlang"}, {"hrl", "erlang"},
        {"f90", "fortran"}, {"f95", "fortran"}, {"f03", "fortran"}, {"f08", "fortran"},
        {"hs", "haskell"},
        {"lhs", "literatehaskell"},
        {"jl", "julia"},
        {"nim", "nim"},
        {"asm", "asm"}, {"s", "asm"},
        {"reg", "registry"},
        {"tex", "tex"},
        {"ada", "ada"}, {"adb", "ada"}, {"ads", "ada"},
        {"zig", "zig"},
        {"gd", "gdscript"},
    };
    // clang-format on

    auto const fs_path = std::filesystem::path(path);

    // The conventional exact-uppercase "README" (no extension) is treated
    // as markdown directly, rather than falling all the way through to the
    // generic "null" default just because it has no extension.
    if (fs_path.filename() == "README") {
        return "markdown";
    }

    auto ext = fs_path.extension().string(); // ".ext", or empty if there's no extension
    if (ext.empty()) {
        return "null"; // no extension and not a special-cased name -- plain text, no highlighting
    }
    ext.erase(ext.begin()); // drop the leading '.'
    std::transform(ext.begin(), ext.end(), ext.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    for (auto const &mapping : mappings) {
        if (mapping.ext == ext) {
            return std::string(mapping.lexer);
        }
    }
    return "null"; // unrecognized extension -- plain text, no highlighting
}
