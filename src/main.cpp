// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2026 Diego Iastrubni <diegoiast@gmail.com>

#include "toolkit/application.hpp"
#include "toolkit/label.hpp"
#include "toolkit/layout.hpp"
#include "toolkit/window.hpp"

using namespace toolkit;

int main(int argc, char *argv[]) {
    Application app;

    auto window = app.create_window("SVision3 demo", {600, 400});
    auto layout = std::make_unique<VBoxLayout>();

    layout->add_widget(std::make_unique<Label>("Hello friends!"));
    window->set_root(std::move(layout));

    return app.run();
}
