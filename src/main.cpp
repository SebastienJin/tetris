#include <gtkmm/application.h>

#include "scene/GameWindow.h"


int main (int argc, char *argv[])
{
    auto app = Gtk::Application::create("org.tetris",Gio::APPLICATION_NON_UNIQUE );

    Game g;
    GameWindow w(g);

    return app->run(w);
}
