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
                     "    int total = 0;\n"
                     "    for (int i = 0; i < 10; i++) {\n"
                     "        total += i;\n"
                     "    }\n"
                     "    if (total > 0) {\n"
                     "        print_result(total);\n"
                     "    } else {\n"
                     "        print_result(0);\n"
                     "    }\n"
                     "    return 0;\n"
                     "}\n"
                     "\n"
                     "void print_result(int value) {\n"
                     "    // TODO: replace with real formatting\n"
                     "    printf(\"result: %d\\n\", value);\n"
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
    // word-in-progress precedes the caret; a real app would consult a
    // symbol table or language server instead, but the *trigger* mechanism
    // -- react to each inserted character, decide, call show_autocomplete()
    // -- is the same either way.
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
    // The word immediately before `end` (a document position), scanning
    // backward over alnum/'_' bytes -- re-derived from the real document on
    // every call below instead of accumulated char-by-char across calls.
    // An earlier version tracked a `current_word` string that only ever
    // grew on SCN_CHARADDED and cleared on a non-word character, which
    // silently drifted out of sync with the document the moment Backspace/
    // Delete/cancel-and-retype entered the picture (SCN_CHARADDED never
    // fires for deletions) -- found live: type "cher" (completion shows,
    // correct), Escape, Backspace, retype "r": the tracked word became
    // "cherr", one character ahead of the real "cher", so
    // show_autocomplete()'s len_entered no longer matched the actual
    // document and nothing appeared. Deriving fresh from the document every
    // time is self-correcting regardless of what happened in between.
    auto word_before = [](std::string const &content, size_t end) {
        auto start = end;
        while (start > 0 &&
               (std::isalnum(static_cast<unsigned char>(content[start - 1])) || content[start - 1] == '_')) {
            --start;
        }
        return content.substr(start, end - start);
    };
    editor->set_on_char_added([edit_ptr, word_before](int ch) {
        auto const pos = edit_ptr->current_position();
        auto const content = edit_ptr->text();

        if (ch == '(') {
            auto const word = word_before(content, static_cast<size_t>(pos) - 1); // before the '(' itself
            for (auto const &[name, signature] : signatures) {
                if (word == name) {
                    edit_ptr->show_calltip(pos, std::string(signature));
                    break;
                }
            }
            return;
        }
        if (!(std::isalnum(ch) || ch == '_')) {
            return;
        }

        auto const word = word_before(content, static_cast<size_t>(pos));
        if (word.size() < 2) {
            return;
        }
        static constexpr std::string_view candidates[] = {"apple", "banana", "cherry", "date", "elderberry"};
        auto matches = std::vector<std::string>{};
        for (auto candidate : candidates) {
            if (candidate.size() >= word.size() && candidate.compare(0, word.size(), word) == 0) {
                matches.emplace_back(candidate);
            }
        }
        if (!matches.empty()) {
            edit_ptr->show_autocomplete(static_cast<int>(word.size()), matches);
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

    // A button and a line input, giving the demo other focusable widgets to
    // test focus transitions against (click into the line input or the
    // button, then back into the editor, to confirm the editor's caret/
    // keyboard input correctly follow real
    // toolkit::Window::set_focused_widget() changes) -- the button also
    // demonstrates indicate_range()/clear_indicator_range(): toggles a
    // squiggly-underline indicator on "main" (the word on the 2nd line,
    // "int main() {") each click. Finding that word is demo-owned logic --
    // the library only knows how to mark/clear a given byte range, not
    // which word to pick, same "app decides content" split as autocomplete/
    // calltips.
    constexpr int word_indicator = 8; // IndicatorNumbers::Container -- 0-7 are reserved for lexers.
    edit_ptr->set_indicator_style(word_indicator, Color::rgb(0.8f, 0.0f, 0.0f));
    auto word_marked = std::make_shared<bool>(false);

    auto toolbar = std::make_unique<HBoxLayout>();
    auto button = std::make_unique<Button>("Mark word");
    button->on_click = [edit_ptr, window, word_marked] {
        auto const content = edit_ptr->text();
        auto const line1_start = content.find('\n') + 1;
        auto const word_pos = content.find("main", line1_start);
        if (word_pos == std::string::npos) {
            return;
        }
        auto const start = static_cast<int>(word_pos);
        constexpr int length = 4; // "main"
        if (*word_marked) {
            edit_ptr->clear_indicator_range(word_indicator, start, length);
        } else {
            edit_ptr->indicate_range(word_indicator, start, length);
        }
        *word_marked = !*word_marked;
        spdlog::info("Word indicator {}", *word_marked ? "set on 'main'" : "cleared");
        // Give focus back to the editor -- clicking the button itself moved
        // it there (toolkit::Window::set_focused_widget()), and without
        // this the caret/keyboard input would stay stuck on the button
        // instead of returning to the document.
        window->set_focused_widget(edit_ptr);
    };

    // Toggles a lint-style warning annotation below "print_result(0);" --
    // demonstrates set_annotation()/clear_annotation(): a per-*line*
    // decoration (rendered as extra virtual lines right below the line,
    // boxed by default) rather than indicate_range()'s per-byte-*range* one.
    constexpr int magic_number_line = 9; // "        print_result(0);" in the sample text above.
    auto annotation_shown = std::make_shared<bool>(false);
    auto toggle_annotation_button = std::make_unique<Button>("Toggle Annotation");
    toggle_annotation_button->on_click = [edit_ptr, window, annotation_shown] {
        if (*annotation_shown) {
            edit_ptr->clear_annotation(magic_number_line);
        } else {
            edit_ptr->set_annotation(magic_number_line, "warning: magic number 0, consider a named constant",
                                     Color::rgb(0.8f, 0.5f, 0.0f));
        }
        *annotation_shown = !*annotation_shown;
        spdlog::info("Annotation {}", *annotation_shown ? "shown" : "cleared");
        window->set_focused_widget(edit_ptr);
    };

    // Toggles word wrap -- the sample text's first line (the banner
    // comment) is long enough to visibly wrap once the window is narrower
    // than it, demonstrating both the reflow itself and that the
    // horizontal scrollbar correctly disappears once wrapping is on (see
    // set_word_wrap()'s doc comment for the ModifyScrollBars() fix that
    // made that part actually true).
    auto toggle_wrap_button = std::make_unique<Button>("Toggle Word Wrap");
    toggle_wrap_button->on_click = [edit_ptr, window] {
        edit_ptr->set_word_wrap(!edit_ptr->has_word_wrap());
        spdlog::info("Word wrap {}", edit_ptr->has_word_wrap() ? "on" : "off");
        window->set_focused_widget(edit_ptr);
    };

    // Toggles a bookmark on the caret's current line -- a button-driven
    // alternative to clicking the bookmark margin, which is a thin (16px),
    // unlabeled strip that's easy to miss ("F2 does nothing" almost always
    // means no bookmark exists yet, not that F2 itself is broken --
    // tests/bookmark_smoke_test.cpp's Part 5 exercises the exact F2/
    // Shift+F2 Command wiring end-to-end and confirms it reaches the editor
    // correctly whether or not the editor currently has focus).
    auto toggle_bookmark_button = std::make_unique<Button>("Toggle Bookmark");
    toggle_bookmark_button->on_click = [edit_ptr, window] {
        edit_ptr->toggle_bookmark(edit_ptr->current_line());
        window->set_focused_widget(edit_ptr);
    };

    // Same action as the F2 shortcut above, exposed as a button too.
    auto next_bookmark_button = std::make_unique<Button>("Next Bookmark");
    next_bookmark_button->on_click = [edit_ptr, window] {
        edit_ptr->goto_next_bookmark();
        window->set_focused_widget(edit_ptr);
    };

    auto line_input = std::make_unique<LineInput>("Type here to test focus");
    toolbar->add_widget(std::move(button));
    toolbar->add_widget(std::move(toggle_annotation_button));
    toolbar->add_widget(std::move(toggle_wrap_button));
    toolbar->add_widget(std::move(toggle_bookmark_button));
    toolbar->add_widget(std::move(next_bookmark_button));
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
