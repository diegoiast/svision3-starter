// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2026 Diego Iastrubni <diegoiast@gmail.com>

#include "scintilla_svision3/scintilla_edit.hpp"
#include "toolkit/application.hpp"
#include "toolkit/layout.hpp"
#include "toolkit/window.hpp"
#include <cstdio>
#include <string>

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
    editor->set_text("// SCINTILLA-SVISION3-LIVE-TEST -- if you can see and edit this, it works.\n"
                     "int main() {\n"
                     "    return 0;\n"
                     "}\n");
    // stretch=1: makes the editor fill the remaining window height instead
    // of just its size_hint() (200px, ScintillaEdit's fallback default --
    // see its header). A caller that forgot this entirely used to get a
    // *zero*-size widget before that fallback was added: invisible,
    // unfocusable, no scrollbars, no input. Found by reading layout.cpp's
    // stretch handling after being told input/scrolling didn't work in the
    // real app.
    layout->add_widget(std::move(editor), 1);
    window->set_root(std::move(layout));

    if (!screenshot_path.empty()) {
        window->relayout();
        auto ok = window->save_to_png(screenshot_path);
        std::printf("Screenshot saved to '%s': %s\n", screenshot_path.c_str(), ok ? "success" : "failed");
        return ok ? 0 : 1;
    }

    return app.run();
}
