#include "player.h"
enum STATES
{
	DEFAULT=0,
	ATTACK,
	DEFENSE
};
int player::order = 0;
bool player::game_finished = 0;

player::player(std::string name) {
	this->set_name(name);
	init_sprites(this);
	init_actions(this);
	this->step = 0;
	this->health_percentage = 10;
	(player::order) ? this->pos_x = 550 : this->pos_x = 250;
	this->which = player::order++;

	this->nextAction("default");
	this->pressed = false;
}

//player::~player() {}
bool player::validDefense(const char* attack) {
	if (this->getState() != DEFENSE) return false;

	if (!strcmp(this->curr_action.c_str(), "block")) {
		if (!strcmp(attack, "kick")				||
			!strcmp(attack, "highkick")			||
			!strcmp(attack, "jumpkickleft")		||
			!strcmp(attack, "jumpkickright")	||
			!strcmp(attack, "roundhouse")		||
			!strcmp(attack, "punch")			||
			!strcmp(attack, "punchleft")		||
			!strcmp(attack, "punchright")		||
			!strcmp(attack, "highpunchright")	||
			!strcmp(attack, "jumppunch")		||
			!strcmp(attack, "highpunchleft"))	return true;
	}
	else if (!strcmp(this->get_curr_action().c_str(), "downblock")) {
		if (strcmp(attack, "freezeball")!=0) return true;
	}

	return false;
}

void  player::winner(player *p1, player *p2) {
	if (!strcmp(p1->get_curr_action().c_str(), "died") || !strcmp(p1->get_curr_action().c_str(), "frozenstatue") || !strcmp(p1->get_curr_action().c_str(), "burnt")) {
		p2->nextAction("win");
		
	}
	else if (!strcmp(p2->get_curr_action().c_str(), "died") || !strcmp(p2->get_curr_action().c_str(), "frozenstatue") || !strcmp(p2->get_curr_action().c_str(), "burnt")) {
		p1->nextAction("win");
	}
}

void  player::player_state_collision_check(player *p1, player *p2) {
	if (p1->getState() == ATTACK && p2->getState() == ATTACK) {
		p1->gaveHit = 1;
		p2->gaveHit = 1;
		std::string t = p1->get_curr_action();
		p1->take_a_hit(p2->get_curr_action());
		p2->take_a_hit(t);
	}
	else
	if (p1->getState() == ATTACK && !p2->validDefense(p1->get_curr_action().c_str()) && !p1->gaveHit ) {
		p1->gaveHit = 1;
		p2->take_a_hit(p1->get_curr_action());
	}

	else 
	if (p2->getState() == ATTACK && !p1->validDefense(p2->get_curr_action().c_str()) && !p2->gaveHit ) {
		p2->gaveHit = 1;
		p1->take_a_hit(p2->get_curr_action());
	}

}

void player::take_a_hit(std::string action) {
		std::cout << "Player " << this->which + 1 << " took a hit ("<<action<<")" << std::endl;

		if (this->get_health_percentage() == 0) {
			if(!strcmp(action.c_str(),"freezeball")) this->nextAction("tookfreezeball2");
			else if (!strcmp(this->curr_action.c_str(), "tookfreezeball2")) this->nextAction("frozenstatue");
			else if (!strcmp(action.c_str(), "fire")) this->nextAction("burnt");
			else this->nextAction("died");
			return;
		}

		this->substract_health_percentage();
		if (this->get_health_percentage() == 0) this->nextAction("tookfinishhim");
		else this->nextAction("took" + action);
}

void player::set_counters() {
	if (this->curr_action == "default") {
		if (this->curr_frame == 0) this->step = 1;
		else if (this->curr_frame ==this->curr_sprite->iterations - 1) this->step = -1;
		this->curr_frame += step;
	}
	else {
		this->curr_frame++;
		
		if (this->get_movement_y()){//y-axis pos movement
			if (this->curr_frame < ((this->curr_sprite->iterations + 1) / 2) ) {
				this->set_pos_y(this->get_pos_y() - this->get_movement_y());
			}
			else {
				this->set_pos_y(this->get_pos_y() + this->get_movement_y());
			}
		}

		if (this->curr_sprite->interruptible && !this->get_pressed())
			this->nextAction("default"); 

		if (this->curr_frame == this->curr_sprite->iterations) {
			if (this->curr_action == "tooksweepkick") {
				this->nextAction("jumpfromtackle");
			}
			else {
				if (this->curr_sprite->startover == -1) this->nextAction("default");

				else {
					if (this->get_pressed() || this->get_health_percentage() == 0 || !strcmp(this->curr_action.c_str() , "win") )
						this->curr_frame = this->curr_sprite->startover;
					else
						this->nextAction("default");
				}
			}

		}
	}
}

bool player::isSpriteInterruptible() {
	return this->curr_sprite->interruptible;
}

bool player::is_Moving() {
	return this->isMoving;
}

void player::set_curr_action(std::string c) {
	this->curr_action = c;
}

std::string player::get_curr_action() {
	return this->curr_action;
}

int player::get_drawing_sprite_start() {
	return this->sprites[this->curr_action.c_str()]["frames_start"][this->get_curr_frame()].GetInt();
}

int player::get_drawing_sprite_width() {
	return this->sprites[this->curr_action.c_str()]["frames_width"][this->get_curr_frame()].GetInt();
}

int player::get_drawing_sprite_height() {
	return this->sprites[this->curr_action.c_str()]["frames_height"][this->get_curr_frame()].GetInt();
}

void player::nextAction(player::key_combination keys_pressed) {
	this->nextAction(this->getAction(keys_pressed));
}

void player::nextAction(std::string action) {
	this->set_curr_action(action);
	this->set_curr_sprite(this->getSprite(this->get_curr_action()));
	this->setState(this->get_curr_sprite()->state);
	this->reset_curr_frame();
	this->isMoving = (bool)this->get_movement_x();
	this->set_pos_y(DEFAULT_HEIGHT - this->get_movement_y());
	this->gaveHit = 0;
}

bool player::valid_Action(key_combination action) {
	std::string all;
	for (auto i : action) { all += i; }
	if (actions.HasMember(all.c_str())) {
		return true;
	}
	return false;
}

std::string player::getAction(key_combination action) {
	std::string all;
	for (auto i : action) { all += i; }
	if (actions.HasMember(all.c_str())) {
		return actions[all.c_str()].GetString();
	}
}

struct sprite* player::getSprite(std::string movement) {
	struct sprite *x = (struct sprite*)malloc(sizeof(struct sprite));
	if (sprites.HasMember(movement.c_str())) {
		x->filename = (char*)sprites[movement.c_str()]["name"].GetString();
		x->iterations	=	sprites[movement.c_str()]["no_of_iterations"].GetInt();
		x->movement_x	=	sprites[movement.c_str()]["movement_x"].GetInt();
		x->movement_y	=	sprites[movement.c_str()]["movement_y"].GetInt();
		x->state		=	sprites[movement.c_str()]["state"].GetInt();
		x->interruptible=	sprites[movement.c_str()]["interruptible"].GetInt();
		x->startover	= sprites[movement.c_str()]["startover"].GetInt();
		return x;
	}

	return NULL;
}

int player::get_next_pos_x() {
	return this->get_pos_x() + this->get_movement_x()*((which) ? -1 : 1);
}

int player::get_next_pos_y() {
	return this->get_pos_y() + this->get_movement_y();
}

void player::setState(unsigned next_state) {
	this->player_state = (STATES)next_state;
}

player::STATES player::getState() {
	return player_state;
}

void player::init_sprites(player *p) {
	std::ifstream myjs(p->get_name() + ".json");
	std::string str((std::istreambuf_iterator<char>(myjs)),
		std::istreambuf_iterator<char>());
	p->sprites.Parse(str.c_str());
}

void player::init_actions(player *p) {
	std::ifstream myjs(p->get_name() + "_keybindings.json");
	std::string str((std::istreambuf_iterator<char>(myjs)),
		std::istreambuf_iterator<char>());
	p->actions.Parse(str.c_str());
}
