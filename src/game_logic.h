#pragma once

#include <ctime>
#include <vector>
#include <random>
#include <thread>
#include <ctime>
#include <chrono>
#include <queue>
#include <mutex>

#include "piece.h"


enum GameState {
    BEFORE = 0, 
    GAMING, 
    GAME_OVER
};

class Game {
public:
    
    GameState game_state; 
    Matrix<piece_type> background_matrix;
    Matrix<piece_type> preview_matrix;
    unsigned int diff_level;
    unsigned int total_completed_lines;
    unsigned int total_score;
    Piece Piece_falling;
    Piece Piece_suivante;

    std::queue<Direction> operations;
    std::uniform_int_distribution<> random_piece_type;
    std::mt19937 random_int;
    std::chrono::system_clock::time_point last_update_time;
    std::thread gaming_thread;

    Game(): 
    game_state(BEFORE), background_matrix(20, 10), random_piece_type(1, 7), diff_level(0), total_score(0), total_completed_lines(0), Piece_falling(NONE), Piece_suivante(NONE), preview_matrix(4, 4) {
        background_matrix.change_all_value(NONE); 
    }

    ~Game() {}

    void record_operation(Direction operation) {
        operations.push(operation);
    }

    Direction next_operation() {
        Direction operation = operations.front();
        operations.pop();
        return operation;
    }
    
    // at position (0,4)
    Piece Piece_generation() {
        Piece t(static_cast<piece_type>(random_piece_type(random_int)), 0,4, UP);
        return t;
    }

    void preview_matrix_update() {
        auto piece_matrix = Piece_suivante.get_piece_matrix();
        preview_matrix.change_all_value(NONE);
        for (int i = 0; i < piece_matrix.rows_value(); i++) {
            for (int j = 0; j < piece_matrix.cols_value(); j++) {
                if (piece_matrix.position_value(i, j)) {
                    preview_matrix.change_value(i, j, Piece_suivante.type);
                }
            }
        }
    }

    double Piece_speed() const {
        switch (diff_level) {
            case 0:
                return 0.8;
            case 1:
                return 0.7167;
            case 2:
                return 0.6333;
            case 3:
                return 0.55;
            case 4:
                return 0.4667;
            case 5:
                return 0.333;
            case 6:
                return 0.3;
            case 7:
                return 0.21666;
            case 8:
                return 0.133;
            case 9:
                return 0.1;
            case 10 ... 12:
                return 0.083;
            case 13 ... 15:
                return 0.0667;
            case 16 ... 18:
                return 0.05;
            case 19 ... 28:
                return 0.0333;
            default:
                return 0.0166;
        }
    }
    
    unsigned getScore(int completed_lines) const {
        switch (completed_lines) {
            case 1:
                return 40*(diff_level + 1);
            case 2:
                return 100*(diff_level + 1);
            case 3:
                return 300*(diff_level + 1);
            case 4:
                return 1200*(diff_level + 1);
            default:
                return 0;
        }
    }

    void settlement() {
        auto number_of_complete_lines = count_complete_lines();
        total_completed_lines += number_of_complete_lines;
        total_score += getScore(number_of_complete_lines);
    	diff_level = int(total_completed_lines/10);
    }

    void Piece_suivante_test() {
        Piece_falling = Piece_suivante;
        if (Piece_falling.movement_test(background_matrix)){
            Piece_suivante = Piece_generation();
            preview_matrix_update();
            Piece_show(Piece_falling);
        }
        else{
            game_state = GAME_OVER;
        }
    }

    void Piece_show(const Piece& t) {
        auto piece_matrix = t.get_piece_matrix();
        for (int i = 0; i < piece_matrix.rows_value(); i++){
            for (int j = 0; j < piece_matrix.cols_value(); j++){
                if (piece_matrix.position_value(i, j)){
                    background_matrix.change_value(t.x + i, t.y + j, t.type);
                }
            }
        }
    }

    void Piece_eliminate(const Piece& t) {
        auto piece_matrix = t.get_piece_matrix();
        for (int i = 0; i < piece_matrix.rows_value(); i++){
            for (int j = 0; j < piece_matrix.cols_value(); j++){
                if (piece_matrix.position_value(i, j)){
                    background_matrix.change_value(t.x + i, t.y + j, NONE);
                }
            }
        }
    }

    void eliminate_complete_line(int line_number) {
        for (int i = line_number; i > 0; i--) {
            for (int j = 0; j < background_matrix.cols_value(); j++) {
                background_matrix.change_value(i, j, background_matrix.position_value(i - 1,j));
            }
        }
        for (int j = 0; j < background_matrix.cols_value(); j++) {
            background_matrix.change_value(0, j, NONE);
        }
    }

    int count_complete_lines() {
        int num_complete_lines = 0;
        while (1) {
            int line = background_matrix.rows_value() - 1;
            while (line >= 0) {
                bool is_complete = true;
                for (int j = 0; j < background_matrix.cols_value(); j++) {
                    if (background_matrix.position_value(line, j) == NONE) {
                        is_complete = false;
                        break;
                    }
                }
                if (is_complete) {
                    num_complete_lines++;
                    eliminate_complete_line(line);
                } 
                else {
                    line--;
                }
            }
            if (line == -1) {
                break;
            }
        }
        return num_complete_lines;
    }

    void gaming() {
        while (game_state == GAMING) {
            
    		if (!operations.empty()) {

    			Direction operation = next_operation();
    			auto new_Piece_falling(Piece_falling);
    			switch (operation) {
    				case UP:
    					new_Piece_falling.direction = static_cast<Direction>((new_Piece_falling.direction + 1) % 4);
    					break;
    				case RIGHT:
    					new_Piece_falling.y++;
    					break;
    				case DOWN:
    					new_Piece_falling.x++;
    					break;
    				case LEFT:
    					new_Piece_falling.y--;
    					break;
                    default:
                        break;
    			}

    			Piece_eliminate(Piece_falling);
    			if (new_Piece_falling.movement_test(background_matrix)) {
    				Piece_falling = new_Piece_falling;
    			}
    			Piece_show(Piece_falling);
    		}

    		std::chrono::duration<double> duration = std::chrono::system_clock::now() - last_update_time;
    		if (duration.count() > Piece_speed()) {
    			last_update_time = std::chrono::system_clock::now();

    			auto new_Piece_falling(Piece_falling);
    			new_Piece_falling.x++;

    			Piece_eliminate(Piece_falling);
    			if (new_Piece_falling.movement_test(background_matrix)) {
    				Piece_show(new_Piece_falling);
                    Piece_falling = new_Piece_falling;
    			} 
                else {
    				Piece_show(Piece_falling);
    				settlement();
                    Piece_suivante_test();
    			}
    		}
    	}
    }

    void Game_Start(long seed) {
        game_state = GAMING;

        random_int = std::mt19937(seed);

        Piece_falling = Piece_generation();
        Piece_suivante = Piece_generation();
        preview_matrix_update();

        Piece_show(Piece_falling);
        last_update_time = std::chrono::system_clock::now();

        gaming_thread = std::thread(&Game::gaming, this);
        gaming_thread.detach();
    }
    
    void Game_Stop() {
        game_state = BEFORE;
        background_matrix.change_all_value(NONE);
        preview_matrix.change_all_value(NONE);
    	diff_level = 0;
    	total_score = 0;
    	total_completed_lines = 0;
        operations.empty();
    }

};