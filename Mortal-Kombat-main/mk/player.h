#pragma once
#include <string>
#include <list>
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include "rapidjson/reader.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#define DEFAULT_HEIGHT  500

struct sprite {
	int movement_x;
	int movement_y;
	char* filename;
	unsigned iterations;
	unsigned state;
	unsigned interruptible;
	int startover;
};

class player {

private:
	enum STATES;
	static int order;
	using key_combination = std::list<std::string>;
	using action = std::pair<key_combination, std::string>;
	
	STATES player_state;
	std::string name,curr_action;
	int pos_x,pos_y, which,curr_frame, step;
	int health_percentage;
	bool isMoving,gaveHit;
	bool pressed;
	struct sprite *curr_sprite;
public:
	static bool game_finished;
	rapidjson::Document sprites, actions;

	player(std::string);
	//~player();
	void set_pressed() { this->pressed = true; }
	void reset_pressed() { this->pressed = false; }
	bool get_pressed() { return this->pressed; }
	void substract_health_percentage() { if(--this->health_percentage < 0)  this->health_percentage = 0; }
	int get_health_percentage() { return this->health_percentage; }

	std::string get_health_percentage_to_string() { 
		std::string s("sprites/all/");
		s += std::to_string(this->health_percentage);
		s += ".png";
		return s;
	}

	bool is_Moving();
	void set_counters();

	int get_curr_frame() { return this->curr_frame; }
	void reset_curr_frame() { this->curr_frame = 0; }

	int get_drawing_sprite_width();
	int get_drawing_sprite_height();
	int get_drawing_sprite_start();

	void set_name(std::string n) {name = n;}
	std::string get_name() { return name;}

	void set_curr_action(std::string);
	std::string get_curr_action();

	void set_curr_sprite(struct sprite *s) {curr_sprite = s;}
	struct sprite* get_curr_sprite() {assert(curr_sprite); return curr_sprite;}

	void  set_pos_x(int p) { pos_x = p; }
	int get_pos_x() { return pos_x; }
	int get_next_pos_x();

	void  set_pos_y(int p) { pos_y = p; }
	int get_pos_y() { return pos_y; }
	int get_next_pos_y();

	int get_movement_x() { return this->curr_sprite->movement_x; }
	int get_movement_y() { return this->curr_sprite->movement_y; }

	struct sprite* getSprite(std::string movement);

	void player::setState(unsigned);
	STATES player::getState();

	void nextAction(player::key_combination );
	void nextAction(std::string);
	std::string getAction(key_combination action);
	bool valid_Action(key_combination action);

	void init_sprites(player*);

	void init_actions(player*);
	
	void static player_state_collision_check(player *, player *);
	void static winner(player *, player *);
	bool validDefense(const char*);
	void player::take_a_hit(std::string action);
	bool isSpriteInterruptible();

};