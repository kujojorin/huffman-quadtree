#include "MainWindow.hpp"
#include <gtkmm.h> // Para Gtk::Application

int main(int argc, char* argv[]) {
    auto app = Gtk::Application::create("org.exemplo.compressor.gtkmm4");
    return app->make_window_and_run<MainWindow>(argc, argv);
}
