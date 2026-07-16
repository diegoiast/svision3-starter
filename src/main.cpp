// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2026 Diego Iastrubni <diegoiast@gmail.com>

#include "scintilla_svision3/scintilla_edit.hpp"
#include "toolkit/application.hpp"
#include "toolkit/button.hpp"
#include "toolkit/command.hpp"
#include "toolkit/layout.hpp"
#include "toolkit/line_input.hpp"
#include "toolkit/window.hpp"
#include <spdlog/spdlog.h>
#include <cctype>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

using namespace toolkit;

int main(int argc, char *argv[]) {
    std::string screenshot_path;
    for (int i = 1; i < argc; i++) {
        auto arg = std::string(argv[i]);
        if (arg.starts_with("--screenshot=")) {
            screenshot_path = arg.substr(13);
        }
    }

    Application app;

    auto window = app.create_window("SVision3 demo", {600, 400});
    auto layout = std::make_unique<VBoxLayout>();

    auto editor = std::make_unique<ScintillaEdit>();
    editor->set_lexer("cpp");
    editor->set_line_numbers(true);
    editor->set_code_folding(true);
    // Click margin 1 (just left of the fold margin) to toggle a bookmark on
    // that line -- no F-key needed, unlike autocomplete/calltips: a margin
    // click is already the natural trigger (same as folding's margin 2),
    // there's no "typed the right text" precondition to fake here.
    editor->set_bookmarks(true);
    // No frame -- ScintillaEdit's constructor turns one on by default
    // (rounded, per the active theme's corner_radius, since Widget's
    // default frame drawing has no per-widget corner override), but a
    // full-bleed editor without one reads better in this demo's layout.
    editor->set_frame(false);
    // additionalSelectionTyping defaults to off in Scintilla itself (see
    // Editor::FilterSelections()) -- without this, Alt+Shift+Up/Down still
    // creates a real multi-caret rectangular selection, but the moment you
    // type, Scintilla silently drops every range but the main one before
    // inserting. Not a bug in the port; matches stock Notepad++/SciTE
    // unless the host app opts in like this.
    editor->set_multiple_selection(true);
    editor->set_text("// SCINTILLA-SVISION3-LIVE-TEST -- if you can see and edit this, it works.\n"
                     "int main() {\n"
                     "    return 0;\n"
                     "}\n");

    // Demonstrates ScintillaEdit::set_context_menu_extra_items(): edits
    // Scintilla's own built-in Undo/Redo/Cut/Copy/Paste/Delete/Select-All
    // menu in place instead of replacing it (set_context_menu_handler()
    // does full replacement, if that's ever needed instead). "Format
    // Document" is shown disabled (grayed out, does nothing if clicked)
    // since this demo doesn't actually implement it -- proving disabled
    // items work the same way here as in the built-in menu (see
    // context_menu_smoke_test.cpp).
    editor->set_context_menu_extra_items([](std::vector<MenuItem> &items, Point) {
        items.push_back(MenuItem::sep());
        items.push_back(MenuItem::action("Say Hello", [] { spdlog::info("Hello from the demo context menu!"); }));
        items.push_back(MenuItem::action("Format Document", [] {}, /*enabled=*/false));
    });

    // Demonstrates ScintillaEdit::set_on_char_added() driving
    // show_autocomplete() for real, the way an app actually would:
    // SCN_CHARADDED fires after every real typed character (not
    // programmatic set_text() -- see set_on_char_added()'s doc comment),
    // and the app decides here whether/what to suggest. This demo's
    // "language" is just five fixed fruit names filtered by whatever
    // word-in-progress has been typed so far; a real app would consult a
    // symbol table or language server instead, but the *trigger* mechanism
    // -- react to each inserted character, decide, call show_autocomplete()
    // -- is the same either way. current_word (and the callback closure
    // capturing it) lives as long as `window` itself does, both being tied
    // to `app`'s lifetime here in main(), so capturing it by reference is
    // safe for the whole app.run() below.
    // Demonstrates ScintillaEdit::show_calltip(): same "react to
    // SCN_CHARADDED, decide, show" trigger as autocomplete above, but for
    // signature help instead -- typing '(' right after one of this demo's
    // five fruit names shows its (made up) signature, mimicking a real
    // app's "just opened a function call" moment. A real app would look the
    // function up in a symbol table instead of this fixed list.
    static constexpr std::pair<std::string_view, std::string_view> signatures[] = {
        {"apple", "void apple(int freshness)"},
        {"banana", "void banana(bool ripe)"},
        {"cherry", "void cherry(int count, bool pitted)"},
    };

    auto *edit_ptr = editor.get();
    auto current_word = std::string{};
    editor->set_on_char_added([edit_ptr, &current_word](int ch) {
        if (ch == '(') {
            for (auto const &[name, signature] : signatures) {
                if (current_word == name) {
                    edit_ptr->show_calltip(edit_ptr->current_position(), std::string(signature));
                    break;
                }
            }
            current_word.clear();
            return;
        }
        if (std::isalnum(ch) || ch == '_') {
            current_word.push_back(static_cast<char>(ch));
        } else {
            current_word.clear();
            return;
        }
        if (current_word.size() < 2) {
            return;
        }
        static constexpr std::string_view candidates[] = {"apple", "banana", "cherry", "date", "elderberry"};
        auto matches = std::vector<std::string>{};
        for (auto candidate : candidates) {
            if (candidate.size() >= current_word.size() &&
                candidate.compare(0, current_word.size(), current_word) == 0) {
                matches.emplace_back(candidate);
            }
        }
        if (!matches.empty()) {
            edit_ptr->show_autocomplete(static_cast<int>(current_word.size()), matches);
        }
    });

    // F9: manual "show it regardless of what's typed" override alongside
    // the real typing-driven trigger above -- Widget::add_command(), not
    // toolkit::Window::on_key. A widget-scoped Command is only ever checked
    // while this widget (or an ancestor) is the focused one -- Window::
    // handle_key()'s "focused widget and its parents" loop calls
    // handle_key_impl(), which matches commands_ before handle_key() itself
    // -- unlike Window::on_key, which fires for every key regardless of
    // focus (that's what needed the manual is_focused() check this replaces).
    // F2/Shift+F2 used to be autocomplete's manual trigger, but now drive
    // bookmark navigation instead (see below) -- F9 took over here.
    auto autocomplete_cmd =
        Command::create("Show Autocomplete", [edit_ptr] {
            edit_ptr->show_autocomplete(0, {"apple", "banana", "cherry", "date", "elderberry"});
        });
    autocomplete_cmd->set_shortcut("F9");
    editor->add_command(autocomplete_cmd);

    // F2/Shift+F2: jump to the next/previous bookmarked line (wrapping
    // around at either end) -- click the margin between the line numbers
    // and the fold +/- icons to actually set one.
    auto next_bookmark_cmd = Command::create("Next Bookmark", [edit_ptr] { edit_ptr->goto_next_bookmark(); });
    next_bookmark_cmd->set_shortcut("F2");
    editor->add_command(next_bookmark_cmd);

    auto prev_bookmark_cmd =
        Command::create("Previous Bookmark", [edit_ptr] { edit_ptr->goto_previous_bookmark(); });
    prev_bookmark_cmd->set_shortcut("Shift+F2");
    editor->add_command(prev_bookmark_cmd);

    // F3: same idea as F9's autocomplete override, but for the calltip --
    // shows apple's signature regardless of what's actually typed, for a
    // quick manual check.
    auto calltip_cmd = Command::create("Show Calltip", [edit_ptr] {
        edit_ptr->show_calltip(edit_ptr->current_position(), "void apple(int freshness)");
    });
    calltip_cmd->set_shortcut("F3");
    editor->add_command(calltip_cmd);

    // A button and a line input, purely to have other focusable widgets in
    // the window to test focus transitions against: click into the line
    // input or the button, then back into the editor, to confirm the
    // editor's caret/keyboard input correctly follow real
    // toolkit::Window::set_focused_widget() changes -- not just the F2/
    // autocomplete-popup-specific path above.
    auto toolbar = std::make_unique<HBoxLayout>();
    auto button = std::make_unique<Button>("Click me");
    button->on_click = [] { spdlog::info("Button clicked"); };
    auto line_input = std::make_unique<LineInput>("Type here to test focus");
    toolbar->add_widget(std::move(button));
    toolbar->add_widget(std::move(line_input), 1);
    layout->add_widget(std::move(toolbar));

    // stretch=1: makes the editor fill the remaining window height instead
    // of just its size_hint() (200px, ScintillaEdit's fallback default --
    // see its header). A caller that forgot this entirely used to get a
    // *zero*-size widget before that fallback was added: invisible,
    // unfocusable, no scrollbars, no input. Found by reading layout.cpp's
    // stretch handling after being told input/scrolling didn't work in the
    // real app.
    layout->add_widget(std::move(editor), 1);
    window->set_root(std::move(layout));
    // Nothing focuses the editor on its own after set_root() -- a real app
    // would normally get real focus from the user's first click, but typing
    // fast enough right after launch could beat that. Without this,
    // toolkit::Window::open_popup() (opened by show_autocomplete()) saves
    // focused_widget_==nullptr as what to restore once the popup closes,
    // which then blurs the editor a second time, unprotected, instead of
    // restoring it -- no caret, no keyboard input, until something else
    // (like a real click) focuses it again.
    window->set_focused_widget(edit_ptr);

    if (!screenshot_path.empty()) {
        window->relayout();
        auto ok = window->save_to_png(screenshot_path);
        spdlog::info("Screenshot saved to '{}': {}", screenshot_path, ok ? "success" : "failed");
        return ok ? 0 : 1;
    }

    return app.run();
}
