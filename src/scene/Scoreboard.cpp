#include "Scoreboard.h"


Scoreboard::Scoreboard(std::vector<Player *> &players, Player &self): players(players), self(self)
{
    set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    add(treeView);
    listStore = Gtk::ListStore::create(m_Columns);
    treeView.set_model(listStore);

    treeView.append_column("Name", m_Columns.name);
    treeView.append_column("Status", m_Columns.status);
    treeView.append_column("Score", m_Columns.score);
    show_all_children();
}

bool Scoreboard::update(const Player &p)
{
    if (!p.name.empty())
    {

        for (auto &iter : listStore->children())
        {
            auto name = iter[m_Columns.name];
            auto status = (p.alive && p.connected) ? "Playing" : "Over";

            if (name == p.name)
            {
                if (iter[m_Columns.score] != p.score || iter[m_Columns.status] != status)
                {
                    iter[m_Columns.score] = p.score;
                    iter[m_Columns.status] = status;
                    return true;
                }
                return false;
            }
        }

        auto row = *(listStore->append());
        row[m_Columns.name] = p.name;
        row[m_Columns.score] = p.score;
        row[m_Columns.status] = (p.alive && p.connected) ? "Playing" : "Over";

        return true;
    }
    return false;
}

void Scoreboard::clear_list()
{
    listStore->clear();
}

bool Scoreboard::on_draw(const ::Cairo::RefPtr<::Cairo::Context> &cr)
{
    bool mofify = update(self);
    
    for (const auto &player : players) 
    {
        mofify |= update(*player);
    }

    if (mofify)
    {
        listStore->set_sort_column(m_Columns.score, Gtk::SORT_DESCENDING);
    }

    return Gtk::ScrolledWindow::on_draw(cr);
}
