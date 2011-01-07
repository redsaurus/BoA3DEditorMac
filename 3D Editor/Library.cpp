//#include <Carbon/Carbon.h>	// included in pre-compiled header
//#include <Memory.h>
//#include <Quickdraw.h>
//#include <QuickdrawText.h>
//#include <Fonts.h>
//#include <Events.h>
//#include <Menus.h>
//#include <Windows.h>
//#include <Dialogs.h>
//#include <Resources.h>
//#include <OSUtils.h>
//#include <TextUtils.h>
//#include <QDOffscreen.h>

//#include "stdio.h"
//#include "string.h"
#include "global.h"

extern short max_zone_dim[3];// = {64,48,32};

void graphic_id_type::clear_graphic_id_type()
{
	which_sheet = -1;
	which_icon = 0;
	graphic_adjust = 0;
}

Boolean graphic_id_type::not_legit()
{
	if (which_sheet < 0)
		return TRUE;
	if (which_icon < 0)
		return TRUE;
	return FALSE;
}

floor_type_type::floor_type_type()
{
	clear_floor_values();
}

void floor_type_type::clear_floor_values()
{
	sprintf(floor_name,"Unused");
	
	pic.clear_graphic_id_type();
	ed_pic.clear_graphic_id_type();
	blocked = 0;
 	step_sound = -1;
 	light_radius = 0;
 	floor_height = 0;
	
	// special properties of floor
	special = 0;
	effect_adjust = 0;
	
	is_water = FALSE; // water?
	is_floor = FALSE; // a human built (stone or wood) floor
	is_ground = FALSE; // regular grass, save floor?
	is_rough = FALSE; // rought errain? (i.e. desert or rough cave)
	fly_over = FALSE; // can this terrain be flied over?
	
	shortcut_key = 255;
	for (short i = 0; i < 6; i++)
		extra[i] = 0;
	num_anim_steps = 1;
	outdoor_combat_town_used = 1000;
}

terrain_type_type::terrain_type_type()
{
	clear_terrain_values();
}

void terrain_type_type::clear_terrain_values()
{
	sprintf(ter_name,"Unused");
	
	pic.clear_graphic_id_type();
	cut_away_pic.clear_graphic_id_type();
	ed_pic.clear_graphic_id_type();
	
	icon_offset_x = icon_offset_y = 0;
	second_icon = -1;
	second_icon_offset_x = second_icon_offset_y = 0;
	cut_away_second_icon = -1;
	cut_away_offset_x = cut_away_offset_y = 0;
	anim_steps = 1;
	
	for (short i = 0; i < 4; i++) {
		move_block[i] = 0; // 0 - no block, 1 - block movement, 0 - n, 1 - w, 2 - s, 3 - e
		see_block[i] = 0; // 0 - no block, 1 - sight movement, 0 - n, 1 - w, 2 - s, 3 - e
		blocks_view[i] = 0; // 0 - draw after pc in spot, 1 - draw before, 0 - n, 1 - w, 2 - s, 3 - e
	}
	height_adj = 0;
	suppress_floor = 0; // 0 - no, if 1, floor type under terrain type is ignored
	light_radius = 0; // radius of light coming from this spot
	step_sound = -1; // -1 - no sound, takes precedence over floor sound
	
	shortcut_key = 255;
	
	// various special properties
	crumble_type = 0; 
	beam_hit_type = 0; 
	terrain_to_crumble_to = 0;
	hidden_town_terrain = -1;
	swap_terrain = 0; // ter to swap to if a swap terrain special call is used
	is_bridge = FALSE; // allows people to cross this space even if floor is a blocking type
	is_road = FALSE;
	can_look_at = FALSE; // a terrain which is always selected to be inspected when player looks
	draw_on_automap = FALSE;
	default_script[0] = 0;
	
	// special properties of terrain
	special = 0; 
	effect_adjust = 0; 
}

out_wandering_type::out_wandering_type()
{
	clear_out_wandering_type();
}

void out_wandering_type::clear_out_wandering_type()
{
	for (short i = 0; i < 4; i++) {
		hostile[i] = -1;
		hostile_amount[i] = 0;
	}
	for (short i = 0; i < 3; i++) {
		friendly[i] = -1;
		friendly_amount[i] = 0;
	}
	cant_flee = 0;
	end_spec1 = end_spec2 = -1;
	killed_flag1 = killed_flag2 = -1;
	
	forced = 0; 
	check_every_turn = 0; 
	move_type = 0;
	start_loc.x = 0; start_loc.y = 0; 
	start_state_when_encountered = -1;
	start_state_when_defeated = -1;
	start_state_when_fled = -1;
	random_move_chance = 0;
}

void out_wandering_type::port()
{
	short i;
	
	for (i = 0; i < 4; i++) {
		flip_short(&hostile[i]);
		flip_short(&hostile_amount[i]);
	}
	for (i = 0; i < 3; i++) {
		flip_short(&friendly[i]);
		flip_short(&friendly_amount[i]);
	}
	flip_short(&cant_flee);
	flip_short(&end_spec1);
	flip_short(&end_spec2);
	flip_short(&killed_flag1);
	flip_short(&killed_flag2);
	flip_short(&forced);
	flip_short(&check_every_turn);
	flip_short(&move_type);
	flip_short(&start_state_when_encountered);
	flip_short(&start_state_when_defeated);
	flip_short(&start_state_when_fled);
	flip_short(&random_move_chance);
}

outdoor_record_type::outdoor_record_type()
{
	clear_outdoor_record_type();
}

void outdoor_record_type::clear_outdoor_record_type()
{
	sprintf(name,"New Section");
	for (short i = 0; i < 48; i++){ 
		for (short j = 0; j < 48; j++) {
			floor[i][j] = 0;
			terrain[i][j] = 0;
			height[i][j] = 9;
		}
	}
	for (short i = 0; i < 8; i++) { 
		SetRect(&exit_rects[i],-1,-1,-1,-1);
		exit_dests[i] = -1;
	}	
	for (short i = 0; i < NUM_OUT_PLACED_SPECIALS; i++) { 
		SetRect(&special_rects[i],-1,-1,-1,-1);
		spec_id[i] = kNO_OUT_SPECIALS;
	}	 
	for (short i = 0; i < 8; i++) { 
		sign_locs[i].x = 255;
		sign_text[i][0] = 0;
	}	 
	for (short i = 0; i < 4; i++) { 
		wandering[i].clear_out_wandering_type();
		special_enc[i].clear_out_wandering_type();
	}
	for (short i = 0; i < 8; i++) { 
		preset[i].clear_out_wandering_type();
	}
	
	wandering_locs[0].x = 12;
	wandering_locs[0].y = 12;
	wandering_locs[1].x = 36;
	wandering_locs[1].y = 12;
	wandering_locs[2].x = 12;
	wandering_locs[2].y = 36;
	wandering_locs[3].x = 36;
	wandering_locs[3].y = 36;
	
	for (short i = 0; i < 8; i++) { 
		info_rect[i].right = 0;
		info_rect_text[i][0] = 0;
	}	
	
	section_script[0] = 0;
	is_on_surface = TRUE;
	
	for (short i = 0; i < 10; i++)
		extra[i] = 0;
}
void outdoor_record_type::port()
{
	short i,j;
	
	for (i = 0; i < OUTDOOR_SIZE; i++)
		for (j = 0; j < OUTDOOR_SIZE; j++)
			flip_short(&terrain[i][j]);
	for (i = 0; i < NUM_OUT_PLACED_SPECIALS; i++) 
		flip_rect(&special_rects[i]);
	for (i = 0; i < NUM_OUT_PLACED_SPECIALS; i++) 
		flip_short(&spec_id[i]);
	for (i = 0; i < NUM_OUT_TOWN_ENTRANCES; i++) 
		flip_rect(&exit_rects[i]);
	for (i = 0; i < NUM_OUT_TOWN_ENTRANCES; i++) 
		flip_short(&exit_dests[i]);
	for (i = 0; i < 4; i++) {
		wandering[i].port();
		special_enc[i].port();
	}
	for (i = 0; i < 8; i++)
		preset[i].port();
	for (i = 0; i < NUM_OUT_DESCRIPTION_AREAS; i++)
		flip_rect(&info_rect[i]);
	flip_short(&is_on_surface);
	for (i = 0; i < 10; i++) 
		flip_short(&extra[i]);
}

creature_start_type::creature_start_type()
{
	clear_creature_start_type();
}

void creature_start_type::clear_creature_start_type()
{
	number = -1;
	facing = 0;
	start_attitude = 3;
	start_loc.x = start_loc.y = 0;
	personality = 0;
	extra_item = -1;
	extra_item_chance_1 = 100;
	extra_item_2 = -1;
	extra_item_chance_2 = 100;
	hidden_class = 0;
	character_id = -1;
	// a number from 0-20000, a unique id for this character
	
	// timing variable
	time_flag = 0;
	
	creature_time = 0;
	attached_event = 0;
	
	// scripting
	for (short i = 0; i < SCRIPT_NAME_LEN; i++) 
		char_script[i] = 0; 
	
	for (short i = 0; i < 10; i++)
		memory_cells[i] = 0;
	
	for (short i = 0; i < 4; i++)
		extra[i] = 0;
	act_at_distance = 0;
	unique_char = 0;
}
void creature_start_type::port()
{
	flip_short(&number);
	flip_short(&facing);
	flip_short(&start_attitude);
	flip_short(&personality);
	flip_short(&extra_item);
	flip_short(&extra_item_2);
	flip_short(&extra_item_chance_1);
	flip_short(&extra_item_chance_2);
	flip_short(&hidden_class);
	flip_short(&character_id);
	flip_short(&time_flag);
	flip_short(&creature_time);
	flip_short(&attached_event);
	flip_short(&unique_char);
	flip_short(&act_at_distance);
	for (short i = 0; i < 10; i++)
		flip_short(&memory_cells[i]);
	
	for (short i = 0; i < 4; i++)
		flip_short(&extra[i]);
}

Boolean creature_start_type::exists()
{
	if (number < 0)
		return FALSE;
	return TRUE;
}

item_record_type::item_record_type()
{
	clear_item_record_type();
}

void item_record_type::clear_item_record_type()
{
	variety = 0;
	damage = 0; // weapons do (1 - damage/2) + (1 - damage/2)
	bonus = 0; // amt. added to weapon damage, and to hit increased by %5 * bonus
	weap_type = 4; // number of skill used for weapon, such as 4 for melee
	protection = 0; // prevents (1 - protection/2) + (1 - protection/2) melee damage
	charges = 0;
	encumbrance = 1;
	item_floor_graphic.clear_graphic_id_type();
	item_floor_graphic.which_sheet = 1000;
	item_floor_graphic.which_icon = 0;
	item_floor_graphic.graphic_adjust = 0;
	inven_icon = 0;
	missile_anim_type = 0;
	
	// item abilities
	for (short i = 0; i < 4; i++) {
		ability_in_slot[i] = -1; 
		ability_strength[i] = 0; 
	}
	
	special_class = 0; // 0 - no class
	value = 0;
	weight = 0;
	value_class = -1;
	
	// property flags
	ident = 0; // if 1, always ident
	magic = 0; // if 1, magic item
	cursed = 0; // if 1, cursed
	once_per_day = 0; // if 1, always ident
	junk_item = 0; // if 1, can delete this item to make room in town
	for (short i = 0; i < 4; i++)
		extra[i] = 0;
	sprintf(full_name,"Unused");
	sprintf(name,"Unused");	
}

item_type::item_type()
{
	clear_item_type();
}

void item_type::clear_item_type()
{
	which_item = -1; 
	
	item_loc.x = 0; item_loc.y = 0;
	charges = 0;
	properties = 0; 
	item_shift.x = 0; item_shift.y = 0;
}

Boolean item_type::exists()
{
	if (which_item < 0)
		return FALSE;
	return TRUE;
}

void item_type::port()
{
	flip_short(&which_item);
}

unsigned char item_type::identified_bit(0x1);
unsigned char item_type::property_bit(0x2);
unsigned char item_type::contained_bit(0x4);
unsigned char item_type::cursed_bit(0x8);
unsigned char item_type::usedToday_bit(0x10);

preset_field_type::preset_field_type()
{
	clear_preset_field_type();
}

void preset_field_type::clear_preset_field_type()
{
	field_loc.x = 0; field_loc.y = 0;
	field_type = -1;
}
void preset_field_type::port()
{
	flip_short(&field_type);
}

in_town_on_ter_script_type::in_town_on_ter_script_type()
{
	clear_in_town_on_ter_script_type();
}

void in_town_on_ter_script_type::clear_in_town_on_ter_script_type()
{
	exists = 0;
	loc.x = 0; loc.y = 0;
	for (short i = 0; i < SCRIPT_NAME_LEN; i++) 
		script_name[i] = 0; 
	for (short i = 0; i < 10; i++)
		memory_cells[i] = 0;
}

void in_town_on_ter_script_type::port()
{
	flip_short(&exists);
	for (short i = 0; i < 10; i++)
		flip_short(&memory_cells[i]);
}

town_record_type::town_record_type()
{
	clear_town_record_type();
}

void town_record_type::clear_town_record_type()
{
	sprintf(town_name,"New Town");
	for (short i = 0; i < 4; i++){
		for (short j = 0; j < 6; j++){
			respawn_monsts[i][j] = -1;
		}
	}
	respawn_locs[0].x = 12;
	respawn_locs[0].y = 12;
	respawn_locs[1].x = 36;
	respawn_locs[1].y = 12;
	respawn_locs[2].x = 12;
	respawn_locs[2].y = 36;
	respawn_locs[3].x = 36;
	respawn_locs[3].y = 36;
	respawn_locs[4] = respawn_locs[0];
	respawn_locs[5] = respawn_locs[3];
	
	for (short i = 0; i < 60; i++) {
		SetRect(&special_rects[i],-1,-1,-1,-1);
		spec_id[i] = kNO_TOWN_SPECIALS;
	}
	lighting = 0;
	
	// signs
	for (short i = 0; i < 15; i++) {
		sign_locs[i].x = sign_locs[i].y = 255;
		sign_text[i][0] = 0;
	}
	
	
	for (short i = 0; i < 4; i++) {
		//start_locs[i].x = start_locs[i].y = 0;
		exit_locs[i].x = exit_locs[i].y = 255;
	}
	set_start_locs(1);
	
	//SetRect(&in_town_rect,4,4,28,28);
	
	for (short i = 0; i < 144; i++) {
		preset_items[i].clear_item_type();
	}	
	for (short i = 0; i < 60; i++) {
		preset_fields[i].clear_preset_field_type();
	}
	
	wall_1_sheet = wall_2_sheet = 600;
	cliff_sheet = 650;
	wall_1_height = wall_2_height = 1;
	beam_type = 0;
	environmental_sound = -1;
	is_on_surface = TRUE;
	town_kill_day = -1;
	town_linked_event = -1;
	external_floor_type = -1;
	
	// scripts and specials
	for (short i = 0; i < SCRIPT_NAME_LEN; i++) 
		town_script[i] = 0; 
	for (short i = 0; i < NUM_TER_SCRIPTS; i++) 
		ter_scripts[i].clear_in_town_on_ter_script_type();
	
	for (short i = 0; i < 16; i++) {
		SetRect(&room_rect[i],-1,-1,-1,-1);
		info_rect_text[i][0] = 0;
	}
	for (short i = 0; i < 80; i++) {
		creatures[i].clear_creature_start_type();
	}
	for (short i = 0; i < 20; i++) 
		extra[i] = 0;
	for (short i = 0; i < NUM_WAYPOINTS; i++) {
		waypoints[i].x = kINVAL_TOWN_LOC_X;
		waypoints[i].y = kINVAL_TOWN_LOC_Y;
	}
	
	monster_respawn_chance = 0;
	
	for (short i = 0; i < 4; i++) 
		exit_specs[i] = -1;
	spec_on_entry = spec_on_entry_if_dead = -1;
}

void town_record_type::set_start_locs(short town_size)
{	
	short s = max_zone_dim[town_size];
	in_town_rect.top = 3;
	in_town_rect.bottom = s - 4;
	in_town_rect.left = 3;
	in_town_rect.right = s - 4;
	start_locs[0].x = s / 2;
	start_locs[0].y = 4;
	start_locs[1].x = 4;
	start_locs[1].y = s / 2;
	start_locs[2].x = s / 2;
	start_locs[2].y = s - 5;
	start_locs[3].x = s - 5;
	start_locs[3].y = s / 2;
	
	respawn_locs[0].x = s / 4;
	respawn_locs[0].y = s / 4;
	respawn_locs[1].x = (s * 3) / 4;
	respawn_locs[1].y = s / 4;
	respawn_locs[2].x = s / 4;
	respawn_locs[2].y = (s * 3) / 4;
	respawn_locs[3].x = (s * 3) / 4;
	respawn_locs[3].y = (s * 3) / 4;
	respawn_locs[4] = respawn_locs[0];
	respawn_locs[5] = respawn_locs[3];
	
}

void town_record_type::port()
{	
	short i,j;
	
	for (i = 0; i < 4; i++)
		for (j = 0; j < 6; j++)
			flip_short(&respawn_monsts[i][j]);
	for (i = 0; i < NUM_TOWN_PLACED_SPECIALS; i++)
		flip_rect(&special_rects[i]);
	flip_short(&lighting);
	flip_rect(&in_town_rect);
	for (i = 0; i < NUM_TOWN_PLACED_ITEMS; i++)
		preset_items[i].port();
	for (i = 0; i < NUM_TOWN_PLACED_FIELDS; i++)
		preset_fields[i].port();
	
	flip_short(&wall_1_sheet);
	flip_short(&wall_1_height);
	flip_short(&wall_2_height);
	flip_short(&wall_2_sheet);
	flip_short(&cliff_sheet);
	flip_short(&beam_type);
	flip_short(&environmental_sound);
	flip_short(&is_on_surface);
	flip_short(&town_kill_day);
	flip_short(&town_linked_event);
	flip_short(&external_floor_type);
	flip_short(&monster_respawn_chance);
	
	for (i =0 ; i < NUM_TER_SCRIPTS; i++) 
		ter_scripts[i].port();
	for (i = 0; i < NUM_TOWN_DESCRIPTION_AREAS; i++)
		flip_rect(&room_rect[i]);
	for (i = 0; i < NUM_TOWN_PLACED_CREATURES; i++) 
		creatures[i].port();
	
	for (i =0 ; i < 20; i++) 
		flip_short(&extra[i]);
	for (i =0 ; i < 4; i++) 
		flip_short(&exit_specs[i]);
	
	flip_short(&spec_on_entry);
	flip_short(&spec_on_entry_if_dead);
}

scen_item_data_type::scen_item_data_type()
{
	clear_scen_item_data_type();
}

void scen_item_data_type::clear_scen_item_data_type()
{
	for (short i = 0; i < NUM_SCEN_ITEMS; i++) 
		scen_items[i].clear_item_record_type();
	for (short i = 0; i < 256; i++) 
		scen_floors[i].clear_floor_values();
	for (short i = 0; i < 512; i++) 
		scen_terrains[i].clear_terrain_values();
	for (short i = 0; i < 256; i++) 
		scen_creatures[i].clear_creature_type();	
}

big_tr_type::big_tr_type()
{
	clear_big_tr_type();
}

void big_tr_type::clear_big_tr_type()
{
	for (short i = 0; i < 64; i++){
		for (short j = 0; j < 64; j++) {
			terrain[i][j] = 0;
			floor[i][j] = 0;
			height[i][j] = 5;
			lighting[i][j] = 0;
		}
	}
}

void big_tr_type::port()
{
	for (short i = 0; i < 64; i++){
		for (short j = 0; j < 64; j++){
			flip_short(&terrain[i][j]);
		}
	}
}

ave_tr_type::ave_tr_type()
{
	clear_ave_tr_type();
}

void ave_tr_type::clear_ave_tr_type()
{
	for (short i = 0; i < 48; i++){
		for (short j = 0; j < 48; j++) {
			terrain[i][j] = 0;
			floor[i][j] = 0;
			height[i][j] = 5;
			lighting[i][j] = 0;
		}
	}
}
void ave_tr_type::port()
{
	for (short i = 0; i < 48; i++){
		for (short j = 0; j < 48; j++){ 
			flip_short(&terrain[i][j]);
		}
	}
}

tiny_tr_type::tiny_tr_type()
{
	clear_tiny_tr_type();
}

void tiny_tr_type::clear_tiny_tr_type()
{
	for (short i = 0; i < 32; i++){
		for (short j = 0; j < 32; j++) {
			terrain[i][j] = 0;
			floor[i][j] = 0;
			height[i][j] = 5;
			lighting[i][j] = 0;
		}
	}
}

void tiny_tr_type::port()
{
	for (short i = 0; i < 32; i++){
		for (short j = 0; j < 32; j++){ 
			flip_short(&terrain[i][j]);
		}
	}
}

scenario_data_type::scenario_data_type()
{
	clear_scenario_data_type();
}

void scenario_data_type::clear_scenario_data_type()
{
	//In theory one could use the native byte ordering. 
	//For now, however, the convention is that all scenarios 
	//should be stored in big-endian format
	
	//if(endianness.isLittle){
	//	flag1 = 199;
	//	flag2 = 61;
	//	flag3 = 2;
	//	flag4 = 53;
	//}
	//else{
	//	big-endian
		flag1 = 97;
		flag2 = 215;
		flag3 = 7;
		flag4 = 33;
	//}
	
	ver[0] = 1; 
	ver[1] = 0; 
	ver[2] = 0; 
	min_run_ver = 1;
	prog_make_ver[0] = 1; 
	prog_make_ver[1] = 0; 
	prog_make_ver[2] = 0; 
	num_towns = 1;
	out_width = out_height = 1;
	min_level = max_level = 1;
	rating = 0;
	
	sprintf(scen_name,"dummy_name");
	scen_label_pic = 0;
	//	sprintf(scen_desc,"");
	*(scen_desc) = '\0';
	credits_text[0][0] = 0;
	credits_text[1][0] = 0;
	comments_text[0][0] = 0;
	comments_text[1][0] = 0;
	intro_pic_resources[0] = -1;
	intro_pic_resources[1] = -1;
	intro_pic_resources[2] = -1;
	
	for (short i = 0; i < 3; i++){
		for (short j = 0; j < 6; j++){ 
			intro_text[i][j][0] = 0;
		}
	}
	
	for (short i = 0; i < 200; i++) {
		town_size[i] = 1;
		town_starts_hidden[i] = 0;
	}
	
	start_in_what_town = 0;
	what_start_loc_in_town.x = 24;
	what_start_loc_in_town.y = 24;
	what_outdoor_section_start_in.x = 0;
	what_outdoor_section_start_in.y = 0;
	start_where_in_outdoor_section.x = 24;
	start_where_in_outdoor_section.y = 24;
	
	for (short i = 0; i < 10; i++) {
		town_to_add_to[i] = -1;
		flag_to_add_to_town[i][0] = 0;
		flag_to_add_to_town[i][1] = 0;
	}
	
	
	for (short i = 0; i < 30; i++) {
		scen_boats[i].clear_boat_record_type();
		scen_horses[i].clear_horse_record_type();
	}	
	
	for (short i = 0; i < 10; i++) {
		storage_shortcuts[i].ter_type = -1;
		for (short j = 0; j < 10; j++) {
			storage_shortcuts[i].item_num[j] = -1;
			storage_shortcuts[i].item_odds[j] = 0;
		}
		storage_shortcuts[i].property = 0;
	}
	
	last_out_edited.x = 0;
	last_out_edited.y = 0;
	last_town_edited = 0;
}

// checks the validity of the scenario data
// returns -1 if invalid scenario
// returns 0 if mac scenairo
// returns 1 if win scenario
short scenario_data_type::scenario_platform()
{
	if ((flag1 == 97) &&
		(flag2 == 215) &&
		(flag3 == 7) &&
		(flag4 == 33))
		return 0;
	if ((flag1 == 199) &&
		(flag2 == 61) &&
		(flag3 == 2) &&
		(flag4 == 53))
		return 1;
	return -1;
}

void scenario_data_type::port()
{	
	short i,j;
	
	flip_short(&out_width);
	flip_short(&out_height);
	flip_short(&min_level);
	flip_short(&max_level);
	flip_short(&rating);
	flip_short(&scen_label_pic);
	flip_short(&intro_pic_resources[0]);
	flip_short(&intro_pic_resources[1]);
	flip_short(&intro_pic_resources[2]);
	flip_short(&start_in_what_town);
	
	for (i = 0; i < 10; i++)  {
		flip_short(&town_to_add_to[i]);
		flip_short(&flag_to_add_to_town[i][0]);
		flip_short(&flag_to_add_to_town[i][1]);
		
		flip_short(&storage_shortcuts[i].ter_type);
		flip_short(&storage_shortcuts[i].property);
		for (j = 0; j < 10; j++)  {
			flip_short(&storage_shortcuts[i].item_num[j]);
			flip_short(&storage_shortcuts[i].item_odds[j]);
		}
	}
	
	for (i = 0; i < 30; i++)  {
		scen_boats[i].port();
		scen_horses[i].port();
	}
	
	flip_short(&last_town_edited);	
}

creature_type::creature_type()
{
	clear_creature_type();
}

void creature_type::clear_creature_type()
{
	sprintf(name,"Unused");
	level = 1; // level of creature
	for (short i = 0; i < 6; i++) {
		which_to_add_to[i] = -1; // which 6 skills to give bonuses to
		how_much[i] = 0; // how much to add to each skill
	}
	for (short i = 0; i < 8; i++) {
		items[i] = -1; // numbers of items it has
		item_chances[i] = 100; // chance, from 0 to 100, of the item being present
	}
	hp_bonus = 0;
	sp_bonus = 0;
	spec_abil = 0; 
	default_attitude = 2;
	species = 0;
	for (short i = 0; i < 6; i++) 
		immunities[i] = 0; 
	natural_armor = 0; // like armor, starts at 0
	
	// graphic
	char_graphic.clear_graphic_id_type();
	char_graphic.which_sheet = 1624;
	char_upper_graphic_sheet = -1;
	small_or_large_template = 0;
	
	a[0] = 0;
	a[1] = 0;
	a[2] = 0;
	a1_type = a23_type = 0;
	ap_bonus = 0;
	
	for (short i = 0; i < SCRIPT_NAME_LEN; i++) 
		default_script[i] = 0; 
	default_strategy = 0;
	default_aggression = default_courage = 100;
	
	summon_class = -1;
	
	for (short i = 0; i < 10; i++) 
		extra[i] = 0;
}

boat_record_type::boat_record_type()
{
	clear_boat_record_type();
}

void boat_record_type::clear_boat_record_type()
{
	boat_loc.x = 0; boat_loc.y = 0;
	boat_loc_in_sec.x = 0; boat_loc_in_sec.y = 0;
	boat_sector.x = 0; boat_sector.y = 0;
	
	which_town = 0;
	exists = FALSE;
	property = FALSE;
}

void boat_record_type::port(){
	flip_short(&which_town);
}

horse_record_type::horse_record_type()
{
	clear_horse_record_type();
}

void horse_record_type::clear_horse_record_type()
{
	horse_loc.x = 0; horse_loc.y = 0;
	horse_loc_in_sec.x = 0; horse_loc_in_sec.y = 0;
	horse_sector.x = 0; horse_sector.y = 0;
	
	which_town = 0;
	exists = FALSE;
	property = FALSE;
}

void horse_record_type::port(){
	flip_short(&which_town);
}

void flip_short(short *s)
{
	*(unsigned short*)s = ((*(unsigned short*)s >> 8) & 0xFF) | ((*(unsigned short*)s & 0xFF) << 8);
}

void alter_rect(Rect *r)
{
	short a;
	
	a = r->top;
	r->top = r->left;
	r->left = a;
	a = r->bottom;
	r->bottom = r->right;
	r->right = a;
}

void flip_rect(Rect *s)
{
	flip_short(&(s->top));
	flip_short(&(s->bottom));
	flip_short(&(s->left));
	flip_short(&(s->right));
	//alter_rect(s); //It's unclear why this line was ever here, and it was causing horrible errors
}