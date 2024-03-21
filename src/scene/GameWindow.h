#ifndef TETRIS_GameWindow_H
#define TETRIS_GameWindow_H

#include <gtkmm.h>

#include "MatrixToGraph.h"
#include "Scoreboard.h"
#include "../game_logic.h"
#include "../modemulti/server.h"


enum Page {
    HOME, 
    GAME, 
    AFTER_GAME, 
    MULTI, 
    AFTER_MUILTI_GAME
};

class GameWindow : public Gtk::Window {
public:

    explicit GameWindow(Game& g);

    Game& game;
    Page page_state;

    Gtk::ButtonBox home_Button;
    Gtk::Button start_Button, join_Button, create_Button;

    Gtk::Grid grid;
    MatrixToGraph gameMatrix, nextMatrix;
    Gtk::Label score;
    Gtk::Button quit_Button;

    Gtk::ButtonBox after_Button;
    Gtk::Button afterquit_Button;
    Gtk::Label endText;

    Gtk::ButtonBox multi_Button;
    Gtk::Label info;
    Gtk::Button startGame;
    bool isMulti = false;
    Server server;

    Gtk::Grid multiAfterGameGrid;
    Scoreboard scoreboard;
    Gtk::Label info_end;
    Gtk::Button quitMulti_Button;

    bool keyinput(GdkEventKey *event);
    void switchpage(Page p);
    std::string ask(const std::string& question);
    bool updategame();
};

void throw_error(const char *message, bool is_error = false);

#endif
