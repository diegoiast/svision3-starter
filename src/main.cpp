// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2026 Diego Iastrubni <diegoiast@gmail.com>

#include "svision3/application.hpp"
#include "svision3/label.hpp"
#include "svision3/layout.hpp"
#include "svision3/window.hpp"

using namespace svision3;

int main(int argc, char *argv[]) {
    Application app;

    auto window = app.create_window("SVision3 demo", {600, 400});
    auto layout = std::make_unique<VBoxLayout>();

    layout->add_widget(std::make_unique<Label>("Hello friends!"));
    window->set_root(std::move(layout));
    window->show();

    return app.run();
}
