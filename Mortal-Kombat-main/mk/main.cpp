
// AS, CS454, Project MK (SNES version), code snippets.
// Version 2.
// December 2018, unit tested (partial), unrefactored.
// K:\Docs\Lectures\HY454\Content\Project\Project 2018\MK

#include <stdio.h>
#include <time.h>
#include <vector>
#include <assert.h>
#include <string>
#include <list>
#include <functional>
#include <map>
#include <set>
#include <algorithm>
#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <iostream>
#include "player.h"

#define DISPLAY_WIDTH	800
#define DISPLAY_HEIGHT	600

#define TICK 8

void collisions_checking(player*, player*);
void boundaries_check(player*);

int main(int argc, char** argv) {
	
	player p1 = player("scorpion");
	player p2 = player("subzero");



	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_BITMAP  *player1_bitmap = NULL, *each_sprite_p1 = NULL,*p1_health = NULL;
	ALLEGRO_BITMAP  *player2_bitmap = NULL, *each_sprite_p2 = NULL,*p2_health = NULL;
	ALLEGRO_BITMAP  *background = NULL,*finish_him=NULL,*mk_logo = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
	
	//initializations
	{
		if (!al_init_native_dialog_addon()) {
			std::cerr << "failed to initialize native dialog addon!\n";
			return -1;
		}
		if (!al_init()) {

			al_show_native_message_box(display, "Error", "Error", "Failed to initialize allegro!",
				NULL, ALLEGRO_MESSAGEBOX_ERROR);
			return 0;
		}
		if (!al_init_image_addon()) {
			al_show_native_message_box(display, "Error", "Error", "Failed to initialize al_init_image_addon!",
				NULL, ALLEGRO_MESSAGEBOX_ERROR);
			return 0;
		}
		if (!al_install_keyboard()) {
			fprintf(stderr, "failed to initialize the keyboard!\n");
			return -1;
		}
		timer = al_create_timer(1.0/TICK);
		if (!timer) {
			fprintf(stderr, "failed to create timer!\n");
			return -1;
		}
		display = al_create_display(DISPLAY_WIDTH, DISPLAY_HEIGHT);
		if (!display) {
			al_show_native_message_box(display, "Error", "Error", "Failed to initialize display!",
				NULL, ALLEGRO_MESSAGEBOX_ERROR);
			return 0;
		}

		background = al_load_bitmap("The_Balcony.png");
		if (!background) {
			al_show_native_message_box(display, "Error", "Error", "Failed to load background image!",
				NULL, ALLEGRO_MESSAGEBOX_ERROR);
			al_destroy_display(display);
			return 0;
		}
		//mk_logo = al_load_bitmap("mk2.jpeg");
		//if (!mk_logo) {
		//	al_show_native_message_box(display, "Error", "Error", "Failed to load mk image!",
		//		NULL, ALLEGRO_MESSAGEBOX_ERROR);
		//	al_destroy_display(display);
		//	return 0;
		//}
		//finish_him = al_load_bitmap("finishhim.jpeg");
		//if (!finish_him) {
		//	al_show_native_message_box(display, "Error", "Error", "Failed to load finish him image!",
		//		NULL, ALLEGRO_MESSAGEBOX_ERROR);
		//	al_destroy_display(display);
		//	return 0;
		//}
		//

		event_queue = al_create_event_queue();
		if (!event_queue) {
			fprintf(stderr, "failed to create event_queue!\n");
			al_destroy_bitmap(background);
			al_destroy_display(display);
			al_destroy_timer(timer);
			return -1;
		}
		al_register_event_source(event_queue, al_get_display_event_source(display));
		al_register_event_source(event_queue, al_get_timer_event_source(timer));
		al_register_event_source(event_queue, al_get_keyboard_event_source());


		player1_bitmap = al_load_bitmap(p1.getSprite("default")->filename);
		if (!player1_bitmap) {
			al_show_native_message_box(display, "Error", "Error", "Failed to load p1 image!",
				NULL, ALLEGRO_MESSAGEBOX_ERROR);
			al_destroy_display(display);
			return 0;
		}

		player2_bitmap = al_load_bitmap(p2.getSprite("default")->filename);
		if (!player2_bitmap) {
			al_show_native_message_box(display, "Error", "Error", "Failed to load p2 image!",
				NULL, ALLEGRO_MESSAGEBOX_ERROR);
			al_destroy_display(display);
			return 0;
		}
	} 
	std::list<std::string> keys_pressed_p1, keys_pressed_p2;

	al_start_timer(timer);
	while (1) {
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);
		if (ev.type == ALLEGRO_EVENT_TIMER) {

			if (!keys_pressed_p1.empty() && (p1.valid_Action(keys_pressed_p1)) && p1.isSpriteInterruptible() && p1.get_health_percentage()) {

				p1.nextAction(keys_pressed_p1);
				player1_bitmap = al_load_bitmap(p1.get_curr_sprite()->filename);
				keys_pressed_p1.clear();
			}

			if (!keys_pressed_p2.empty() && (p2.valid_Action(keys_pressed_p2)) && p2.isSpriteInterruptible() && p2.get_health_percentage()) {
				p2.nextAction(keys_pressed_p2);
				player2_bitmap = al_load_bitmap(p2.get_curr_sprite()->filename);
				keys_pressed_p2.clear();
			}

			al_draw_scaled_bitmap(background,
				0, 0, al_get_bitmap_width(background), al_get_bitmap_height(background),
				0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT,
				0);


			p1_health = al_load_bitmap(p1.get_health_percentage_to_string().c_str());
			if (!p1_health) {
				al_show_native_message_box(display, "Error", "Error", "Failed to load image!",
					NULL, ALLEGRO_MESSAGEBOX_ERROR);
				al_destroy_display(display);
				return 0;
			}
			p2_health = al_load_bitmap(p2.get_health_percentage_to_string().c_str());
			if (!p2_health) {
				al_show_native_message_box(display, "Error", "Error", "Failed to load image!",
					NULL, ALLEGRO_MESSAGEBOX_ERROR);
				al_destroy_display(display);
				return 0;
			}

			al_draw_scaled_bitmap(p1_health,
				0, 0, al_get_bitmap_width(p1_health), al_get_bitmap_height(p1_health),
				150, //where on x-axis
				50, //where on y-axis
				al_get_bitmap_width(p1_health) ,//width to draw
				al_get_bitmap_height(p1_health) ,//height to draw
				0//direction
			);

			al_draw_scaled_bitmap(p2_health,
				0, 0, al_get_bitmap_width(p2_health), al_get_bitmap_height(p2_health),
				500, //where on x-axis
				50, //where on y-axis
				al_get_bitmap_width(p2_health),//width to draw
				al_get_bitmap_height(p2_health) ,//height to draw
				1//direction
			);

			collisions_checking(&p1, &p2);

			player1_bitmap = al_load_bitmap(p1.get_curr_sprite()->filename);
			if (!player1_bitmap) {
				al_show_native_message_box(display, "Error", "Error", "Failed to load p1 image!",
					NULL, ALLEGRO_MESSAGEBOX_ERROR);
				al_destroy_display(display);
				return 0;
			}
			player2_bitmap = al_load_bitmap(p2.get_curr_sprite()->filename);
			if (!player2_bitmap) {
				al_show_native_message_box(display, "Error", "Error", "Failed to load p1 image!",
					NULL, ALLEGRO_MESSAGEBOX_ERROR);
				al_destroy_display(display);
				return 0;
			}

			each_sprite_p1 = al_create_sub_bitmap(player1_bitmap,
				p1.get_drawing_sprite_start(),
				(al_get_bitmap_height(player1_bitmap)),
				p1.get_drawing_sprite_width(),
				-(p1.get_drawing_sprite_height()));

			if (!each_sprite_p1) {
				al_show_native_message_box(display, "Error", "Error", "Failed to load image!",
					NULL, ALLEGRO_MESSAGEBOX_ERROR);
				al_destroy_display(display);
				return 0;
			}

			each_sprite_p2 = al_create_sub_bitmap(player2_bitmap,
				p2.get_drawing_sprite_start() + p2.get_drawing_sprite_width(),//where to start x-axis
				(al_get_bitmap_height(player2_bitmap)),//where to start y-axis
				-p2.get_drawing_sprite_width(),//width to cut
				-(p2.get_drawing_sprite_height()));//height to cut

			if (!each_sprite_p2) {
				al_show_native_message_box(display, "Error", "Error", "Failed to load image!",
					NULL, ALLEGRO_MESSAGEBOX_ERROR);
				al_destroy_display(display);
				return 0;
			}

			al_draw_scaled_bitmap(each_sprite_p1,
				0, 0, al_get_bitmap_width(each_sprite_p1), al_get_bitmap_height(each_sprite_p1),
				p1.get_pos_x(), //where on x-axis
				p1.get_pos_y(), //where on y-axis
				al_get_bitmap_width(each_sprite_p1) *2,//width to draw
				al_get_bitmap_height(each_sprite_p1)*2,//height to draw
				0//direction
			);

			al_draw_scaled_bitmap(each_sprite_p2,
				0, 0, al_get_bitmap_width(each_sprite_p2), al_get_bitmap_height(each_sprite_p2),
				p2.get_pos_x(), // x-axis
				p2.get_pos_y(), // y-axis
				al_get_bitmap_width(each_sprite_p2) * 2,//width to draw
				al_get_bitmap_height(each_sprite_p2) * 2,//height to draw
				1//direction
			);

			al_flip_display();


			p1.set_counters();
			p2.set_counters();
		}
		else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			break;
		}
		else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			//keys pressed
			switch (ev.keyboard.keycode) {
				case ALLEGRO_KEY_W:
					keys_pressed_p1.push_back("w");
					p1.set_pressed();
					break;
				case ALLEGRO_KEY_S:
					keys_pressed_p1.push_back("s");
					p1.set_pressed();
					break;

				case ALLEGRO_KEY_A:
					keys_pressed_p1.push_back("a");
					p1.set_pressed();
					break;

				case ALLEGRO_KEY_D:
					keys_pressed_p1.push_back("d");
					p1.set_pressed();
					break;

				case ALLEGRO_KEY_J:
					keys_pressed_p1.push_back("j");
					p1.set_pressed();
					break;

				case ALLEGRO_KEY_K:
					keys_pressed_p1.push_back("k");
					p1.set_pressed();
					break;

				case ALLEGRO_KEY_L:
					keys_pressed_p1.push_back("l");
					p1.set_pressed();
					break;

				case ALLEGRO_KEY_UP:
					keys_pressed_p2.push_back("w");
					p2.set_pressed();
					break;

				case ALLEGRO_KEY_DOWN:
					keys_pressed_p2.push_back("s");
					p2.set_pressed();
					break;

				case ALLEGRO_KEY_LEFT:
					keys_pressed_p2.push_back("d");
					p2.set_pressed();
					break;

				case ALLEGRO_KEY_RIGHT:
					keys_pressed_p2.push_back("a");
					p2.set_pressed();
					break;

				case ALLEGRO_KEY_PAD_1:
					keys_pressed_p2.push_back("j");
					p2.set_pressed();
					break;

				case ALLEGRO_KEY_PAD_2:
					keys_pressed_p2.push_back("k");
					p2.set_pressed();
					break;

				case ALLEGRO_KEY_PAD_3:
					keys_pressed_p2.push_back("l");
					p2.set_pressed();
					break;
				}
		}
		else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
			//keys unpressed
			switch (ev.keyboard.keycode) {
				case ALLEGRO_KEY_W:
					keys_pressed_p1.remove("w");
					p1.reset_pressed();
					break;
				case ALLEGRO_KEY_S:
					keys_pressed_p1.remove("s");
					p1.reset_pressed();
					break;

				case ALLEGRO_KEY_A:
					keys_pressed_p1.remove("a");
					p1.reset_pressed();
					break;

				case ALLEGRO_KEY_D:
					keys_pressed_p1.remove("d");
					p1.reset_pressed();
					break;

				case ALLEGRO_KEY_J:
					keys_pressed_p1.remove("j");
					p1.reset_pressed();
					break;

				case ALLEGRO_KEY_K:
					keys_pressed_p1.remove("k");
					p1.reset_pressed();
					break;

				case ALLEGRO_KEY_L:
					p1.reset_pressed();
					keys_pressed_p1.remove("l");
					break;

				case ALLEGRO_KEY_UP:
					keys_pressed_p2.remove("w");
					p2.reset_pressed();
					break;

				case ALLEGRO_KEY_DOWN:
					keys_pressed_p2.remove("s");
					p2.reset_pressed();
					break;

				case ALLEGRO_KEY_LEFT:
					keys_pressed_p2.remove("d");
					p2.reset_pressed();
					break;

				case ALLEGRO_KEY_RIGHT:
					keys_pressed_p2.remove("a");
					p2.reset_pressed();
					break;
				case ALLEGRO_KEY_PAD_1:
					keys_pressed_p2.remove("j");
					p2.reset_pressed();
					break;

				case ALLEGRO_KEY_PAD_2:
					keys_pressed_p2.remove("k");
					p2.reset_pressed();
					break;

				case ALLEGRO_KEY_PAD_3:
					keys_pressed_p2.remove("l");
					p2.reset_pressed();
					break;
			}
		}

	}

	al_destroy_bitmap(player1_bitmap);
	al_destroy_bitmap(each_sprite_p1);
	al_destroy_bitmap(player2_bitmap);
	al_destroy_bitmap(each_sprite_p2);
	al_destroy_bitmap(background);
	al_destroy_display(display);

	//p1.~player();
	//p2.~player();
}


void boundaries_check(player *p) {
	int boundary_x = p->get_next_pos_x();

	if (boundary_x < 150) {
		boundary_x = 150; 
	}
	else if (boundary_x  > DISPLAY_WIDTH){
		boundary_x = DISPLAY_WIDTH;
	}


	p->set_pos_x(boundary_x);
}

void collisions_checking(player *p1, player *p2) {
	int p1_next_right_x = 0, p2_next_left_x = 0,diff;

	p1_next_right_x = p1->get_next_pos_x() + p1->get_drawing_sprite_width() * 2;
	p2_next_left_x  = p2->get_next_pos_x() - p2->get_drawing_sprite_width() * 2;
	diff = p1_next_right_x - p2_next_left_x;

	if (diff > 0) {
		if (p1->is_Moving() && p2->is_Moving()) {
			p1->set_pos_x(p1->get_pos_x() - (diff/2) + 1);
			p2->set_pos_x(p2->get_pos_x() + (diff/2) + 1);
		}
		else if (p1->is_Moving()) p1->set_pos_x(p1->get_pos_x() - (diff + 1));
		else if (p2->is_Moving()) p2->set_pos_x(p2->get_pos_x() + (diff + 1));

		player::player_state_collision_check(p1,p2);
		if(!(!strcmp(p1->get_curr_action().c_str(),"win") || !strcmp(p2->get_curr_action().c_str(), "win")))
			player::winner(p1, p2);


	}
	else {
		boundaries_check(p1);
		boundaries_check(p2);
	}


}