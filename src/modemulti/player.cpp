#include <utility>

#include "player.h"


Player::Player(int socket): client_socket(socket), level(0), score(0), completed_lines(0), alive(true), connected(true) {}

void Player::codemessage(std::vector<char> &data) {
    data.resize(SIZE_OF_MESSAGE_SIZE + 1 + (name.size() + 1) +
                + sizeof(level) + sizeof(score) + sizeof(completed_lines) + sizeof(alive));

    auto* p = reinterpret_cast<player *>(data.data() + SIZE_OF_MESSAGE_SIZE + 1);

    p->level = level;
    p->score = score;
    p->completed_lines = completed_lines;
    p->alive = alive;
    for (int i = 0; i < name.size(); i++)
        p->name[i] = name[i];
    p->name[name.size()] = '\0';
}

void Player::decodemessage(std::vector<char> data) {
    auto* p = reinterpret_cast<player *>(data.data());

    level = p->level;
    score = p->score;
    completed_lines = p->completed_lines;
    alive = p->alive;
    name = p->name;
}

messageType Player::getType() {
    return PLAYER_DATA;
}

void Player::setName(std::string n) {
    name = std::move(n);
}

void Player::update(Game &g) {
    level = g.diff_level;
    score = g.total_score;
    completed_lines = g.total_completed_lines;
    alive = g.game_state != GAME_OVER;

}

void Player::update(Player *p) {
    level = p->level;
    score = p->score;
    completed_lines = p->completed_lines;
    alive = p->alive;
}