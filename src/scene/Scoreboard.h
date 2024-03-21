#include <vector>
#include <gtkmm.h>

#include "../modemulti/player.h"


class List : public Gtk::TreeModelSort::ColumnRecord
{
public:
    Gtk::TreeModelColumn<Glib::ustring> name;
    Gtk::TreeModelColumn<Glib::ustring> status;
    Gtk::TreeModelColumn<int> score;
    List()
    {
        add(name);
        add(status);
        add(score);
    }
};

class Scoreboard : public Gtk::ScrolledWindow
{
public:
    Scoreboard(std::vector<Player *> &players, Player &self);
    List m_Columns;
    void clear_list();

protected:
    Glib::RefPtr<Gtk::ListStore> listStore;
    Gtk::TreeView treeView;
    std::vector<Player *> &players;
    Player &self;

    bool update(const Player &p);
    bool on_draw(const Cairo::RefPtr<::Cairo::Context> &cr) override;
};

