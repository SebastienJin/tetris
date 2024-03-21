
#include <iostream>
#include <glibmm/main.h>
#include <ifaddrs.h>
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>

#include "GameWindow.h"


void getIPAddress() {
    struct ifaddrs *interfaces = nullptr;
    struct ifaddrs *addr = nullptr;
    void *tmpAddrPtr = nullptr;

    getifaddrs(&interfaces); 
    for (addr = interfaces; addr != nullptr; addr = addr->ifa_next) {
        if (addr->ifa_addr->sa_family == AF_INET) {
            tmpAddrPtr = &((struct sockaddr_in *)addr->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            std::cout << "Interface: " << addr->ifa_name << " IP Address: " << addressBuffer << std::endl;
        }
    }
    freeifaddrs(interfaces);
}

GameWindow::GameWindow(Game &g) : home_Button(Gtk::ORIENTATION_VERTICAL),
                                  after_Button(Gtk::ORIENTATION_VERTICAL),
                                  multi_Button(Gtk::ORIENTATION_VERTICAL),
                                  game(g),
                                  server(g),
                                  page_state(HOME),
                                  gameMatrix(game.background_matrix),
                                  nextMatrix(game.preview_matrix),
                                  scoreboard(server.clients, server.self)
{
    start_Button.set_size_request(300, 100);
    join_Button.set_size_request(300, 100);
    create_Button.set_size_request(300, 100);

    home_Button.set_margin_top(50);
    home_Button.set_margin_bottom(50);

    const Glib::ustring css_data = R"css(
        button {
            border-radius: 10px;
            padding: 5px;
            background: linear-gradient(to right, #ff9966, #ff5e62);
            color: #fff;
            font-size: 20px;
        }
        window {
            background: linear-gradient(to right, green, blue);
        }
        #scoreLabel, #info{
            font-family: Arial, sans-serif;
            font-size: 25px;
            color: white;
        }
    )css";

    start_Button.set_label("Start a game");
    auto css_provider = Gtk::CssProvider::create();
    css_provider->load_from_data(css_data);
    auto context = start_Button.get_style_context();

    context->add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    start_Button.signal_button_release_event().connect([&](GdkEventButton *)
                                                       {
        game.Game_Start(std::time(nullptr));

        switchpage(GAME);
        return true; });

    home_Button.add(start_Button);

    join_Button.set_label("Join a multi-player game");
    join_Button.signal_button_release_event().connect([&](GdkEventButton *)
                                                      {
        try {
            server.connectToServer(ask("Enter IP adress of game"),
                                   ask("Enter your name"));

			isMulti = true;

            switchpage(MULTI);
        } catch (const std::system_error& e) {
            std::cerr << "Error: " << e.what() << std::endl;
			throw_error(e.what(), true);
        }
        return true; });

    context = join_Button.get_style_context();
    context->add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    home_Button.add(join_Button);

    create_Button.set_label("Start a multi-player game");
    create_Button.signal_button_release_event().connect([&](GdkEventButton *)
                                                        {
        server.self.setName(ask("Enter your name"));
        try {
            getIPAddress();
			server.startServer();
            isMulti = true;
            multi_Button.add(startGame);
            multiAfterGameGrid.attach(quitMulti_Button, 0, 3);
			switchpage(MULTI);
		} catch (const std::system_error& e) {
			std::cerr << "Error: " << e.what() << std::endl;
			throw_error(e.what(), true);
		}

        return true; });

    context = create_Button.get_style_context();
    context->add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    home_Button.add(create_Button);

    auto screen = Gdk::Screen::get_default();
    Gtk::StyleContext::add_provider_for_screen(screen, css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);

    gameMatrix.set_margin_right(10);
    grid.attach(gameMatrix, 0, 0, 1, 3);
    grid.attach(nextMatrix, 1, 0);

    score.set_name("scoreLabel");
    score.set_text("Score : " + std::to_string(game.total_score) + "\nLevel : " + std::to_string(game.diff_level));

    context = score.get_style_context();
    css_provider->load_from_data(css_data);
    context->add_provider_for_screen(screen, css_provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    grid.attach(score, 1, 1);

    quit_Button.set_label("Exit");
    quit_Button.signal_button_release_event().connect([&](GdkEventButton *)
                                                      {
        game.Game_Stop();
        switchpage(HOME);
        return true; });

    context = quit_Button.get_style_context();
    context->add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    grid.attach(quit_Button, 1, 2);

    after_Button.add(endText);
    afterquit_Button.set_label("Back to home");

    afterquit_Button.signal_button_release_event().connect([&](GdkEventButton *)
                                                           {
        game.Game_Stop();
        switchpage(HOME);
        return true; });
    after_Button.add(afterquit_Button);

    startGame.set_label("Start a game");
    context = startGame.get_style_context();
    context->add_provider_for_screen(screen, css_provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    startGame.signal_button_release_event().connect([&](GdkEventButton *)
                                                    {
        long date = std::time(nullptr);

        server.startGame(date);

        game.Game_Start(date);
        return true; });

    info.set_name("info");
    context = info.get_style_context();
    context->add_provider_for_screen(screen, css_provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    multi_Button.add(info);

    info_end.set_name("info");
    info_end.set_markup("<span size='large'><b>Scoreboard</b></span>");
    context = info_end.get_style_context();
    context->add_provider_for_screen(screen, css_provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    info_end.set_margin_top(50);
    info_end.set_margin_bottom(50);

    multiAfterGameGrid.attach(info_end, 0, 1);

    multiAfterGameGrid.attach(scoreboard, 1, 2);

    quitMulti_Button.set_label("Quit");
    quitMulti_Button.signal_button_release_event().connect([&](GdkEventButton *)
                                                           {
        server.stopServer();

        multi_Button.remove(startGame);
        multiAfterGameGrid.remove(quitMulti_Button);
        return true; });

    set_default_size(500, 500);
    set_border_width(5);

    add(home_Button);

    Glib::signal_timeout().connect(sigc::mem_fun(*this, &GameWindow::updategame), 1000 / 30);
    this->signal_key_press_event().connect(sigc::mem_fun(*this, &GameWindow::keyinput), false);

    show_all();
}

void GameWindow::switchpage(Page p)
{
    remove();

    switch (p)
    {
    case HOME:
        add(home_Button);
        break;
        
    case GAME:
        if (isMulti)
        {
            multiAfterGameGrid.remove(scoreboard);
            grid.remove(quit_Button);
            scoreboard.set_size_request(250, 170);
            grid.attach(scoreboard, 1, 2);
        }

        add(grid);
        break;

    case AFTER_GAME:
        add(after_Button);
        break;

    case MULTI:
        add(multi_Button);
        break;

    case AFTER_MUILTI_GAME:
        if (isMulti)
        {
            grid.remove(scoreboard);
            grid.attach(quit_Button, 1, 2);

            scoreboard.set_size_request(400, 300);
            multiAfterGameGrid.attach(scoreboard, 0, 2);
        }

        add(multiAfterGameGrid);
        break;
    }

    page_state = p;

    show_all();
}

bool GameWindow::keyinput(GdkEventKey *event)
{
    if (game.game_state != GAMING)
        return false;

    switch (event->keyval)
    {
    case 65307:
        game.Game_Stop();
        switchpage(HOME);
        break;
    case 65364:
        game.record_operation(DOWN);
        break;
    case 65363:
        game.record_operation(RIGHT);
        break;
    case 65362:
        game.record_operation(UP);
        break;
    case 65361:
        game.record_operation(LEFT);
        break;
    }

    return true;
}

bool GameWindow::updategame()
{
    switch (game.game_state)
    {
    case BEFORE:
        if (page_state == MULTI)
        {
            if (!server.running)
            {
                switchpage(HOME);
                isMulti = false;
            }

            std::vector<Player *> players = server.clients;

            std::string play_info = (server.isServer) ? "You have created a multi-player game!\n\n\n\n" : "You have connected to a multi-player game!\n\n\n\n";

            if (!players.empty())
            {
                play_info += "Players connected:\n ";

                for (auto player : players)
                {
                    play_info += player->name + "\n";
                }

                play_info += server.self.name;
            }
            else
            {
                play_info += "No players connected yet...\n";
            }

            info.set_text(play_info);
        }
        break;
    case GAMING:
    
        if (page_state == MULTI)
        {
            switchpage(GAME);
        }
        score.set_text("Score : " + std::to_string(game.total_score) + "\nLevel : " + std::to_string(game.diff_level));
        queue_draw();
        break;

    case GAME_OVER:
        if (isMulti)
        {
            if (page_state != AFTER_MUILTI_GAME)
            {
                switchpage(AFTER_MUILTI_GAME);
            }
            else if (!server.running)
            {
                switchpage(HOME);
                isMulti = false;
                scoreboard.clear_list();
                game.Game_Stop();
            }
        }
        else
        {
            if (page_state != AFTER_GAME)
            {
                switchpage(AFTER_GAME);

                std::string scores =
                    "Your score: " + std::to_string(game.total_score);

                endText.set_margin_top(50);

                endText.set_markup("<span size='large'><b>Game over</b></span>\n\n\n"
                                   "<span size='large'>" +
                                   scores + "</span>");

                endText.set_alignment(Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER);

                endText.set_name("endLabel");

                const Glib::ustring css_end = R"css(
                    #endLabel {
                        font-family: Arial, sans-serif;
                        font-size: 25px;
                        color:white
                    }
                    )css";

                auto screen = Gdk::Screen::get_default();
                auto context_end = endText.get_style_context();
                auto cssProvider_end = Gtk::CssProvider::create();
                cssProvider_end->load_from_data(css_end);
                context_end->add_provider_for_screen(screen, cssProvider_end, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
            }
        }
        break;
    }
    return true;
}

std::string GameWindow::ask(const std::string &question)
{
    const Glib::ustring dialog_css = R"css(
    dialog {
        background-color: #ffffff; 
        border-radius: 12px;
        box-shadow: 0 2px 5px rgba(0, 0, 0, 0.2); 
        padding: 20px; 
    }

    dialog entry {
        border: 2px solid #007BFF; 
        padding: 5px 10px; 
        margin-top: 10px; 
        box-shadow: inset 0 1px 3px rgba(0, 0, 0, 0.1);
    }

    dialog button {
        color: black;
        background-color: #4CAF50; 
        font-size: 20px;
        border: 1px dashed yellow;
        padding: 12px 24px;
    }
        )css";

    Gtk::Dialog dialog(question, true);
    dialog.set_default_size(700, 150);

    Gtk::Entry entry;
    auto cssProvider = Gtk::CssProvider::create();
    cssProvider->load_from_data(dialog_css);
    auto screen = Gdk::Screen::get_default();
    auto context = dialog.get_style_context();
    context->add_provider_for_screen(screen, cssProvider, GTK_STYLE_PROVIDER_PRIORITY_USER);

    dialog.get_content_area()->pack_start(entry, Gtk::PACK_SHRINK);

    dialog.add_button("_Enter", Gtk::RESPONSE_OK);

    dialog.show_all_children();
    dialog.run();

    return entry.get_text();
}

void throw_error(const char *message, bool is_error)
{
    auto throw_error = Gtk::Application::create();
    Gtk::MessageType msg_type = is_error ? Gtk::MESSAGE_ERROR : Gtk::MESSAGE_INFO;
    Gtk::MessageDialog dialog(message, false, msg_type, Gtk::BUTTONS_CLOSE, true);
    if (is_error == true)
    {
        dialog.set_title("Error");
    }
    dialog.run();
}