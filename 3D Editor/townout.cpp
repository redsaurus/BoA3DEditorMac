//#include <Carbon/Carbon.h>	// included in pre-compiled header
//#include <QDOffscreen.h>
//#include <Quickdraw.h>
//#include <Dialogs.h>
//#include <TextUtils.h>
//#include <OSUtils.h>
//#include <stdio.h>
//#include <string.h>
#include "global.h"
#include "Undo.h"

extern ModalFilterUPP main_dialog_UPP;
extern short cen_x, cen_y;
extern Boolean dialog_not_toast;

extern scenario_data_type scenario;
extern zone_names_data_type zone_names;
extern town_record_type town;
extern big_tr_type t_d;
extern outdoor_record_type current_terrain;
extern scen_item_data_type scen_data;
extern short cur_town;
extern location cur_out;
extern short current_drawing_mode;
extern short town_type ;
extern Boolean editing_town;
extern short overall_mode;
extern char *attitude_types[4];
extern  Boolean file_is_loaded;

extern SelectionType::SelectionType_e selected_object_type;
extern unsigned short selected_object_number;

static creature_start_type store_placed_monst;
static short store_which_placed_monst;
static short store_which_placed_item;

short dialog_answer;

static short extra_dialog_answer[4];
static short str_do_delete[16];
static short tn_a, tn_b, tn_c;
static short store_which_out_wand;
static short store_out_wand_mode;
static out_wandering_type store_out_wand;

static location store_cur_loc;

static short store_which_dlog; // used for get_num
static short store_which_str;
static short store_dlog_min;
static short store_dlog_max;
static short store_which_part;
static short store_max_x,store_max_y;

static item_storage_shortcut_type store_storage;
static short cur_shortcut;

void put_placed_monst_in_dlog()
{
	Str255 str;
	short i;

	cd_set_item_text(837,38,scen_data.scen_creatures[store_placed_monst.number].name);
 	cd_set_led_range(837,27,30,store_placed_monst.start_attitude - 2);
	if(strlen((char *)store_placed_monst.char_script) <= 0){
		if(strlen(scen_data.scen_creatures[store_placed_monst.number].default_script)<=0)
			CDST(837,2,"basicnpc");
		else
			CDST(837,2,scen_data.scen_creatures[store_placed_monst.number].default_script);
	}
	else
		CDST(837,2,store_placed_monst.char_script);
  	CDSN(837,4,store_placed_monst.extra_item);
 	CDSN(837,6,store_placed_monst.extra_item_2);
  	CDSN(837,3,store_placed_monst.extra_item_chance_1);
 	CDSN(837,5,store_placed_monst.extra_item_chance_2);

 	CDSN(837,7,store_placed_monst.personality);
 	CDSN(837,8,store_placed_monst.character_id);
 	CDSN(837,9,store_placed_monst.hidden_class);
 	CDSN(837,10,store_placed_monst.act_at_distance);
 	CDSN(837,11,store_placed_monst.unique_char);

 	CDSN(837,13,store_placed_monst.creature_time);
 	CDSN(837,14,store_placed_monst.attached_event);

 	for (i = 0; i < 10; i++)
	 	CDSN(837,15 + i,store_placed_monst.memory_cells[i]);

 	get_str(str,4,store_placed_monst.time_flag + 1);
 	cd_set_item_text(837,39,(char *) str);
}
Boolean get_placed_monst_in_dlog()
{
	char str[256];
	short i;

	// check for errors
	CDGT(837,2,str);
	if (string_not_clean(str,SCRIPT_NAME_LEN,1,"This creature's script",837))
		return FALSE;

 	CDGT(837,2,store_placed_monst.char_script);
 	store_placed_monst.start_attitude = cd_get_led_range(837,27,30) + 2;

  	store_placed_monst.extra_item = CDGN(837,4);
 	store_placed_monst.extra_item_2 = CDGN(837,6);
  	store_placed_monst.extra_item_chance_1 = CDGN(837,3);
 	store_placed_monst.extra_item_chance_2 = CDGN(837,5);

 	store_placed_monst.personality = CDGN(837,7);
 	store_placed_monst.character_id = CDGN(837,8);
 	store_placed_monst.hidden_class = CDGN(837,9);
 	store_placed_monst.act_at_distance = CDGN(837,10);
 	store_placed_monst.unique_char = CDGN(837,11);

 	store_placed_monst.creature_time = CDGN(837,13);
 	store_placed_monst.attached_event = CDGN(837,14);

 	for (i = 0; i < 10; i++)
	 	store_placed_monst.memory_cells[i] = CDGN(837,15 + i);
	 	

	town.creatures[store_which_placed_monst] = store_placed_monst;
	return TRUE;
}

void edit_placed_monst_event_filter (short item_hit)
{
	short i;
	
	switch (item_hit) {
		case 31:
			if (get_placed_monst_in_dlog() == FALSE)
				break;
			dialog_not_toast = FALSE;
			break;
		case 32:
			dialog_not_toast = FALSE;
			break;
		case 33: // choose m type
			if (get_placed_monst_in_dlog() == FALSE)
				break;
			i = choose_text_res(-1,1,255,store_placed_monst.number,837,"Choose Which Creature Type:");
			if (i >= 0) {
				store_placed_monst.number = i;
				put_placed_monst_in_dlog();
				}			
			break;
		case 36: // choose time type
			if (get_placed_monst_in_dlog() == FALSE)
				break;
			i = choose_text_res(4,1,11,store_placed_monst.time_flag + 1,837,"Choose Time Type:");
			if (i >= 0) {
				store_placed_monst.time_flag = i - 1;
				put_placed_monst_in_dlog();
				}			
			break;
		case 34:
			i = choose_text_res(-2,0,NUM_SCEN_ITEMS - 1,store_placed_monst.extra_item,837,"Which item?"); 
			if (i >= 0)
				store_placed_monst.extra_item = i;
			put_placed_monst_in_dlog();
			break;

		case 35:
			i = choose_text_res(-2,0,NUM_SCEN_ITEMS - 1,store_placed_monst.extra_item_2,837,"Which item?"); 
			if (i >= 0)
				store_placed_monst.extra_item_2 = i;
			put_placed_monst_in_dlog();
			break;		

		default:
			cd_hit_led_range(837,27,30,item_hit);
			break;
		}
}

void edit_placed_monst(short which_m)
{
	short item_hit,i;
	
	store_placed_monst = town.creatures[which_m];
	store_which_placed_monst = which_m;
	
	cd_create_dialog_parent_num(837,0);
	
	cd_set_item_num(837,37,which_m + 6);

	put_placed_monst_in_dlog();

	for (i = 0; i < 4; i++) 
		cd_add_label(837,27 + i,attitude_types[i],57);
	//for (i = 0; i < 10; i++) {
	//	sprintf((char *) temp_str,"Cell %d",i);
	//	cd_add_label(837,15 + i,(char *) temp_str,30);
	//	}

	while (dialog_not_toast)
		ModalDialog(main_dialog_UPP, &item_hit);

	cd_kill_dialog(837,0);
	set_tool(40);

}

void edit_sign(short which_sign, bool handleUndo)
{
	char new_text[256];
	if (editing_town)
		get_str_dlog(town.sign_text[which_sign],"What should this sign say?",new_text);
	else
		get_str_dlog(current_terrain.sign_text[which_sign],"What should this sign say?",new_text);

	if (editing_town){
		if(handleUndo && strcmp(new_text,town.sign_text[which_sign]))
			pushUndoStep(new Undo::SignTextChange(new_text,town.sign_text[which_sign],which_sign));
		strcpy(town.sign_text[which_sign],new_text);
	}
	else{
		if(handleUndo && strcmp(new_text,current_terrain.sign_text[which_sign]))
			pushUndoStep(new Undo::SignTextChange(new_text,current_terrain.sign_text[which_sign],which_sign));
		strcpy(current_terrain.sign_text[which_sign],new_text);
	}
}

void get_a_number_event_filter (short item_hit)
{	
	switch (item_hit) {
		case 3:
			dialog_answer = CDGN(store_which_dlog,2);
			dialog_answer = minmax(store_dlog_min,store_dlog_max,dialog_answer);

			dialog_not_toast = FALSE; 
			break;
		case 4:
			dialog_answer = -1;
			dialog_not_toast = FALSE; 
			break;

		}
}

// which_dlog is the number of a dialog box that takes a number in text edit field 2
// gets a numnebr and returns it. returns -1 if canceled
short get_a_number(short which_dlog,short default_value,short min,short max)
{
	short town_strs_hit;
	Str255 temp_str,str2;
	
	 store_which_dlog = which_dlog;
	store_dlog_min = min;
	store_dlog_max = max;
	cd_create_dialog_parent_num(store_which_dlog,0);
	
	CDSN(store_which_dlog,2,default_value);

	if ((which_dlog == 842) || (which_dlog == 855) || (which_dlog == 856) ){
		cd_get_item_text(which_dlog,6,(char *) temp_str);
		sprintf((char *) str2,"%s (%d - %d)",(char *) temp_str,min,max);
		cd_set_item_text(which_dlog,6,(char *) str2);
	}		

	while (dialog_not_toast)
		ModalDialog(main_dialog_UPP, &town_strs_hit);

	cd_kill_dialog(store_which_dlog,0);
	return dialog_answer;
}

void change_ter_event_filter (short item_hit)
{
	short i;
	
	switch (item_hit) {
		case 5:
			tn_a = CDGN(857,2);
			tn_b = CDGN(857,3);
			tn_c = CDGN(857,4);
			if (current_drawing_mode == 0) {
				if (cre(tn_a,0,255,"Both floor types must be from 0 to 255.","",857) == TRUE) break;
				if (cre(tn_b,0,255,"Both floor types must be from 0 to 255.","",857) == TRUE) break;
			}
			else {
				if (cre(tn_a,0,511,"Both terrain types must be from 0 to 511.","",857) == TRUE) break;
				if (cre(tn_b,0,511,"Both terrain types must be from 0 to 511.","",857) == TRUE) break;
			}
			if (cre(tn_c,0,100,"The Chance must be from 0 to 100.","",857) == TRUE) break;

			 dialog_not_toast = FALSE; 
			break;
		case 6:
			tn_a = -1; tn_b = -1; tn_c = -1;
			dialog_not_toast = FALSE; 
			break;
		case 9: case 10:
			i = CDGN(857,item_hit - 7);
			if (current_drawing_mode == 0)
				i = choose_text_res(-6,0,255,i,857,"Which Floor?");
				else i = choose_text_res(-4,0,511,i,857,"Which Terrain?");
			if (i >= 0)
				CDSN(857,item_hit - 7,i);
			break;
		}
}

void change_ter(short *change_from,short *change_to,short *chance)
// ignore parent in Mac version
{
	short town_strs_hit;
	
	cd_create_dialog_parent_num(857,0);
	
	CDSN(857,2,0);
	CDSN(857,3,0);
	CDSN(857,4,100);
	
	while (dialog_not_toast)
		ModalDialog(main_dialog_UPP, &town_strs_hit);

	*change_from = tn_a;
	*change_to = tn_b; 
	*chance = tn_c;
	
	cd_kill_dialog(857,0);
}

void outdoor_details_event_filter (short item_hit)
{
	Str255 str;
	
	switch (item_hit) {
		case 10:
			CDGT(851,3,(char *) str);
			if (string_not_clean((char *) str,SCRIPT_NAME_LEN,1,"The outdoor section script name",851))
				break;

			CDGT(851,2,(char *) str);
			str[19] = 0;
			sprintf(current_terrain.name,"%s",(char *) str);
			strcpy(&zone_names.section_names[zone_names.out_width * cur_out.y + cur_out.x][0],&current_terrain.name[0]);
			CDGT(851,3,(char *) str);
			str[SCRIPT_NAME_LEN - 1] = 0;
			sprintf(current_terrain.section_script,"%s",(char *) str);

			current_terrain.extra[0] = CDGN(851,4);

			current_terrain.is_on_surface = cd_get_led(851,8);
			dialog_not_toast = FALSE; 
			break;
		default:
			cd_flip_led(851,8,item_hit);
			break;
		}
}

void outdoor_details()
// ignore parent in Mac version
{
	short town_strs_hit;
	Str255 temp_str;
	
	
	cd_create_dialog_parent_num(851,0);
	
	CDST(851,2,current_terrain.name);
	CDST(851,3,current_terrain.section_script);
	CDSN(851,4,current_terrain.extra[0]);
	sprintf((char *) temp_str,"X = %d, Y = %d",cur_out.x,cur_out.y);
	cd_set_item_text(851,7,(char *) temp_str);
	cd_set_led(851,8,current_terrain.is_on_surface);

	while (dialog_not_toast)
		ModalDialog(main_dialog_UPP, &town_strs_hit);
	
	cd_kill_dialog(851,0);
}

void put_out_wand_in_dlog()
{
	Str255 str;
	short i;
	
	cd_set_item_num(852,41,store_which_out_wand);
	for (i = 0; i < 4; i++) {
		if (store_out_wand.hostile[i] <= 0)
			cd_set_item_text(852, i + 31, "Empty");
			else {
				get_str(str,-1,store_out_wand.hostile[i]);
				cd_set_item_text(852,i + 31,(char *) str);
				}
		CDSN(852,2 + i,store_out_wand.hostile_amount[i]);
		}
	for (i = 0; i < 3; i++) {
		if (store_out_wand.friendly[i] <= 0)
			cd_set_item_text(852, i + 35, "Empty");
			else {
				get_str(str,-1,store_out_wand.friendly[i]);
				cd_set_item_text(852,i + 35,(char *) str);
				}
		CDSN(852,6 + i,store_out_wand.friendly_amount[i]);
		}
	cd_set_led(852,38,store_out_wand.cant_flee); 
	cd_set_led(852,39,store_out_wand.forced); 
	cd_set_led(852,40,store_out_wand.check_every_turn); 
	CDSN(852,12,store_out_wand.end_spec1);
	CDSN(852,13,store_out_wand.end_spec2);
	CDSN(852,14,store_out_wand.killed_flag1);
	CDSN(852,15,store_out_wand.killed_flag2);
	CDSN(852,16,store_out_wand.move_type);
	CDSN(852,17,store_out_wand.start_loc.x);
	CDSN(852,18,store_out_wand.start_loc.y);
	CDSN(852,19,store_out_wand.random_move_chance);
	CDSN(852,9,store_out_wand.start_state_when_encountered);
	CDSN(852,10,store_out_wand.start_state_when_defeated);
	CDSN(852,11,store_out_wand.start_state_when_fled);
}
Boolean get_out_wand_in_dlog()
{
	short i;
	
	store_out_wand.cant_flee = cd_get_led(852,38); 
	store_out_wand.forced = cd_get_led(852,39); 
	store_out_wand.check_every_turn = cd_get_led(852,40); 

	for (i = 0; i < 4; i++) {
		store_out_wand.hostile_amount[i] = CDGN(852,2 + i);
		}
	for (i = 0; i < 3; i++) {
		store_out_wand.friendly_amount[i] = CDGN(852,6 + i);
		}
	
	store_out_wand.end_spec1 = CDGN(852,12);
	store_out_wand.end_spec2 = CDGN(852,13);
	store_out_wand.killed_flag1 = CDGN(852,14);
	store_out_wand.killed_flag2 = CDGN(852,15);
	if (cre(store_out_wand.end_spec1,
		-1,SDF_RANGE_X - 1,"First part of Stuff Done flags must be from 0 to 299 (or -1 if it's not used).","",852) == TRUE) return FALSE;
	if (cre(store_out_wand.end_spec2,
		-1,SDF_RANGE_Y - 1,"Second part of Stuff Done flags must be from 0 to 29 (or -1 if it's not used).","",852) == TRUE) return FALSE;
	if (cre(store_out_wand.killed_flag2,
		-1,SDF_RANGE_X - 1,"First part of Stuff Done flags must be from 0 to 299 (or -1 if it's not used).","",852) == TRUE) return FALSE;
	if (cre(store_out_wand.killed_flag2,
		-1,SDF_RANGE_Y - 1,"Second part of Stuff Done flags must be from 0 to 29 (or -1 if it's not used).","",852) == TRUE) return FALSE;

	store_out_wand.move_type = CDGN(852,16);
	store_out_wand.start_loc.x = CDGN(852,17);
	store_out_wand.start_loc.y = CDGN(852,18);
	if (cre(store_out_wand.start_loc.x,
		-1,47,"All locations in outdoors must be in the 0..47 range (or -1 if encounter unused or placed by special script)","",852) == TRUE) return FALSE;
	if (cre(store_out_wand.start_loc.y,
		-1,47,"All locations in outdoors must be in the 0..47 range (or -1 if encounter unused or placed by special script)","",852) == TRUE) return FALSE;

	store_out_wand.random_move_chance = CDGN(852,19);
	store_out_wand.start_state_when_encountered = CDGN(852,9);
	store_out_wand.start_state_when_defeated = CDGN(852,10);
	store_out_wand.start_state_when_fled = CDGN(852,11);

	switch (store_out_wand_mode) {
		case 0: current_terrain.wandering[store_which_out_wand] = store_out_wand; break;
		case 1: current_terrain.special_enc[store_which_out_wand] = store_out_wand;  break;
		case 2: current_terrain.preset[store_which_out_wand] = store_out_wand;  break;
		}
	return TRUE;
}

void edit_out_wand_event_filter (short item_hit)
{
	short i;
	creature_start_type store_m;
	
	switch (item_hit) {
		case 21:
			if (get_out_wand_in_dlog() == FALSE)
				break;
			dialog_not_toast = FALSE;
			break;
		case 20:
			dialog_not_toast = FALSE;
			break;
		case 22:
			if (get_out_wand_in_dlog() == FALSE) break;
			store_which_out_wand--;

			switch (store_out_wand_mode) {
				case 0: 
					if (store_which_out_wand < 0) store_which_out_wand = 3;
					store_out_wand = current_terrain.wandering[store_which_out_wand]; break;
				case 1: 
					if (store_which_out_wand < 0) store_which_out_wand = 3;
					store_out_wand = current_terrain.special_enc[store_which_out_wand];  break;
				case 2: 
					if (store_which_out_wand < 0) store_which_out_wand = 7;
					store_out_wand = current_terrain.preset[store_which_out_wand];  break;
				}
			put_out_wand_in_dlog();
			break;
		case 23:
			if (get_out_wand_in_dlog() == FALSE) break;
			store_which_out_wand++;
			switch (store_out_wand_mode) {
				case 0: 
					if (store_which_out_wand > 3) store_which_out_wand = 0;
					store_out_wand = current_terrain.wandering[store_which_out_wand]; break;
				case 1: 
					if (store_which_out_wand > 3) store_which_out_wand = 0;
					store_out_wand = current_terrain.special_enc[store_which_out_wand];  break;
				case 2: 
					if (store_which_out_wand > 7) store_which_out_wand = 0;
					store_out_wand = current_terrain.preset[store_which_out_wand];  break;
				}
			put_out_wand_in_dlog();
			break;

		default:
			if (get_out_wand_in_dlog() == FALSE)
				 break; 
			cd_flip_led(852,38,item_hit);
			cd_flip_led(852,39,item_hit);
			cd_flip_led(852,40,item_hit);
			if ((item_hit >= 24) && (item_hit <= 27)) {
				i = choose_text_res(-1,0,255,store_out_wand.hostile[item_hit - 24],852,"Choose Which Creature:"); 
				if (i >= 0) store_out_wand.hostile[item_hit - 24] = i;
				put_out_wand_in_dlog();
				}
			if ((item_hit >= 28) && (item_hit <= 30)) {
				i = choose_text_res(-1,0,255,store_out_wand.friendly[item_hit - 28],852,"Choose Which Creature:"); 
				if (i >= 0) store_out_wand.friendly[item_hit - 28] = i;
				put_out_wand_in_dlog();
				}
			break;
		}
}

// mode 0 - wandering 1 - special
void edit_out_wand(short mode)
{
	
	short item_hit;

	store_which_out_wand = 0;
	store_out_wand_mode = mode;
	switch (store_out_wand_mode) {
		case 0: store_out_wand = current_terrain.wandering[store_which_out_wand]; break;
		case 1: store_out_wand = current_terrain.special_enc[store_which_out_wand];  break;
		case 2: store_out_wand = current_terrain.preset[store_which_out_wand];  break;
		}
	
	cd_create_dialog_parent_num(852,0);
	
	if (mode == 1)
		cd_set_item_text(852,53,"Outdoor Special Encounter:");
	if (mode == 2)
		cd_set_item_text(852,53,"Outdoor Preplaced Encounter:");

	put_out_wand_in_dlog();

	while (dialog_not_toast)
		ModalDialog(main_dialog_UPP, &item_hit);

	cd_kill_dialog(852,0);

}

Boolean save_town_details()
{
	char str[256];
	short i;
	short dummy;
	
	// Check for errors before saving anything
	dummy = CDGN(832,10);
	if (cre(dummy,0,2,"The beam type must be 0 (no beams), 1 (damage beams), or 2 (impassable beams).","",832) == TRUE) return FALSE;
	dummy = CDGN(832,11);
	if (cre(dummy,-1,200,"The background sound must be -1 (no sound) or between 0 and 200.","",832) == TRUE) return FALSE;
	dummy = CDGN(832,12);
	if (cre(dummy,-1,10000,"The town death day must be -1 (never dies) or between 0 and 10000.","",832) == TRUE) return FALSE;
	dummy = CDGN(832,13);
	if (cre(dummy,-1,9,"The town death prevention event must be -1 (never dies) or between 0 and 9.","",832) == TRUE) return FALSE;
	dummy = CDGN(832,17);
	if (cre(dummy,-1,255,"The external floor type must be -1 (uses edge terrain) or between 0 and 255.","",832) == TRUE) return FALSE;
	for (i = 0; i < 8; i++) {
		dummy = CDGN(832,18 + i);
		if (cre(dummy,-1,47,"The outdoor exit coordinates must all be -1 (game determines best location) or between 0 and 47.","",832) == TRUE) return FALSE;
		}
	CDGT(832,2,str);
	if (string_not_clean(str,20,0,"The name of the town",832))
		return FALSE;
	CDGT(832,9,str);
	if (string_not_clean(str,SCRIPT_NAME_LEN,1,"The town script",832))
		return FALSE;
	
	// save values
	CDGT(832,2,town.town_name);
	strcpy(&zone_names.town_names[cur_town][0],&town.town_name[0]);
	town.lighting = cd_get_led_range(832,33,36);
	town.is_on_surface = cd_get_led(832,32);
	town.monster_respawn_chance = CDGN(832,3);
	town.wall_1_sheet = CDGN(832,4);
	town.wall_1_height = CDGN(832,5);
	town.wall_2_sheet = CDGN(832,6);
	town.wall_2_height = CDGN(832,7);
	town.cliff_sheet = CDGN(832,8);
	CDGT(832,9,town.town_script);
	town.beam_type = CDGN(832,10);
	town.environmental_sound = CDGN(832,11);
	town.town_kill_day = CDGN(832,12);
	town.town_linked_event = CDGN(832,13);
	scenario.town_starts_hidden[cur_town] = (CDGN(832,14) == 0) ? 0 : 1;
	town.spec_on_entry = CDGN(832,15);
	town.spec_on_entry_if_dead = CDGN(832,16);
	town.external_floor_type = CDGN(832,17);
	for (i = 0; i < 4; i++) {
		town.exit_locs[i].x = CDGN(832,18 + i * 2);
		town.exit_locs[i].y = CDGN(832,19 + i * 2);
		town.exit_specs[i] = CDGN(832,26 + i);
		}		

	
	return TRUE;
}

void put_town_details_in_dlog()
{
	short i;

	CDST(832,2,town.town_name);
	cd_set_led_range(832,33,36,town.lighting);
	cd_set_led(832,32,town.is_on_surface);
	CDSN(832,3,town.monster_respawn_chance);
	CDSN(832,4,town.wall_1_sheet);
	CDSN(832,5,town.wall_1_height);
	CDSN(832,6,town.wall_2_sheet);
	CDSN(832,7,town.wall_2_height);
	CDSN(832,8,town.cliff_sheet);
	CDST(832,9,town.town_script);
	CDSN(832,10,town.beam_type);
	CDSN(832,11,town.environmental_sound);
	CDSN(832,12,town.town_kill_day);
	CDSN(832,13,town.town_linked_event);
	CDSN(832,14,scenario.town_starts_hidden[cur_town]);
	CDSN(832,15,town.spec_on_entry);
	CDSN(832,16,town.spec_on_entry_if_dead);
	CDSN(832,17,town.external_floor_type);
	for (i = 0; i < 4; i++) {
		CDSN(832,18 + i * 2,town.exit_locs[i].x);
		CDSN(832,19 + i * 2,town.exit_locs[i].y);
		CDSN(832,26 + i,town.exit_specs[i]);
		}		
}

void edit_town_details_event_filter (short item_hit)
{
	switch (item_hit) {
		case 30:
			if (save_town_details() == TRUE)
				 dialog_not_toast = FALSE; 
			break;
		case 31:
			dialog_not_toast = FALSE; 
			break;
		default:
			cd_hit_led_range(832,33,36,item_hit);
			cd_flip_led(832,32,item_hit);
			break;
		}
}

void edit_town_details()
// ignore parent in Mac version
{
	short town_details_hit;
		
	cd_create_dialog_parent_num(832,0);
	
	put_town_details_in_dlog();
	

	while (dialog_not_toast)
		ModalDialog(main_dialog_UPP, &town_details_hit);
	
	cd_kill_dialog(832,0);
}


Boolean save_town_wand()
{
	short i,j;
	
	for (i = 0; i < 4; i++) 
	for (j = 0; j < 6; j++) 
		{
		town.respawn_monsts[i][j] = CDGN(835,2 + i * 6 + j);
		}	
		
	return TRUE;
}

void put_town_wand_in_dlog()
{
	short i,j;
	
	for (i = 0; i < 4; i++) 
	for (j = 0; j < 6; j++) 
		{
		CDSN(835,2 + i * 6 + j,town.respawn_monsts[i][j]);
		}

}

void edit_town_wand_event_filter (short item_hit)
{
	short i,j;
	
	switch (item_hit) {
		case 38:
			if (save_town_wand() == TRUE)
				 dialog_not_toast = FALSE; 
			break;
		case 39: case 40: case 41: case 42:
			for (i = 0; i < 6; i++) {
				j = choose_text_res(-1,0,255,town.respawn_monsts[item_hit - 39][i],835,"Choose Which Monster:"); 
				if (j <= 0)
					j = -1;
				if (j < 0)
					i = 7;
					else {
						CDSN(835,2 + (item_hit - 39) * 6 + i,j);
						}
				}
			break;
		}
}

void edit_town_wand()
// ignore parent in Mac version
{
	short town_wand_hit;
	
	
	cd_create_dialog_parent_num(835,0);
	
	put_town_wand_in_dlog();
	

	while (dialog_not_toast)
		ModalDialog(main_dialog_UPP, &town_wand_hit);

	cd_kill_dialog(835,0);
}


void pick_out_event_filter (short item_hit)
{
	Str255 temp_str;
	
	switch (item_hit) {
		case 2:
			dialog_answer = store_cur_loc.x * 100 + store_cur_loc.y;
			 dialog_not_toast = FALSE; 
			break;
		case 3:
			dialog_answer = -1;
			 dialog_not_toast = FALSE; 
			break;
		case 12:
			if (store_cur_loc.x == 0) EdSysBeep(20);
				else store_cur_loc.x--;
			break;
		case 13:
			if (store_cur_loc.x >= store_max_x - 1) EdSysBeep(20);
				else store_cur_loc.x++;
			break;
		case 14:
			if (store_cur_loc.y == 0) EdSysBeep(20);
				else store_cur_loc.y--;
			break;
		case 15:
			if (store_cur_loc.y >= store_max_y - 1) EdSysBeep(20);
				else store_cur_loc.y++;
			break;
		case 16:
			int i = store_cur_loc.x+ zone_names.out_width*store_cur_loc.y;
			i = choose_text_res(-8,1,store_max_y*store_max_x,i+1,854,"Choose Which Outdoor Section:");
			if (i > 0) {
				i--;
				store_cur_loc.y = i/store_max_x;
				store_cur_loc.x = i-(store_max_x*store_cur_loc.y);
			}
			break;
	}
	sprintf((char *) temp_str,"X = %d",store_cur_loc.x);
	cd_set_item_text(854,8,(char *) temp_str);
	sprintf((char *) temp_str,"Y = %d",store_cur_loc.y);
	cd_set_item_text(854,11,(char *) temp_str);
}

short pick_out(location default_loc,short max_x,short max_y)
// ignore parent in Mac version
{
	short basic_dlog_hit;
	Str255 temp_str;
	
	store_cur_loc = default_loc;
	store_max_x = max_x;
	store_max_y = max_y;
	
	cd_create_dialog_parent_num(854,0);
	
	cd_set_item_num(854,7,store_max_x);	
	cd_set_item_num(854,10,store_max_y);
	sprintf((char *) temp_str,"X = %d",store_cur_loc.x);
	cd_set_item_text(854,8,(char *) temp_str);
	sprintf((char *) temp_str,"Y = %d",store_cur_loc.y);
	cd_set_item_text(854,11,(char *) temp_str);

	while (dialog_not_toast)
		ModalDialog(main_dialog_UPP, &basic_dlog_hit);

	cd_kill_dialog(854,0);
	return dialog_answer;
}

void change_outdoor_size_event_filter (short item_hit)
{
	Str255 temp_str;
	short new_x_size,new_y_size;
	short extra_dialog_answer_add = 0, extra_dialog_answer_index = 0;
	
	new_x_size = extra_dialog_answer[1] + extra_dialog_answer[3] + store_max_x;
	new_y_size = extra_dialog_answer[0] + extra_dialog_answer[2] + store_max_y;
	
	switch (item_hit) {
		case 2:
			dialog_answer = 1;
			 dialog_not_toast = FALSE; 
			break;
		case 3:
			dialog_answer = -1;
			 dialog_not_toast = FALSE; 
			break;
		case 10:
			extra_dialog_answer_index = 0;
			extra_dialog_answer_add = -1;
			new_y_size += extra_dialog_answer_add;
			break;
		case 11:
			extra_dialog_answer_index = 0;
			extra_dialog_answer_add = 1;
			new_y_size += extra_dialog_answer_add;
			break;
		case 13:
			extra_dialog_answer_index = 1;
			extra_dialog_answer_add = -1;
			new_x_size += extra_dialog_answer_add;
			break;
		case 14:
			extra_dialog_answer_index = 1;
			extra_dialog_answer_add = 1;
			new_x_size += extra_dialog_answer_add;
			break;
		case 16:
			extra_dialog_answer_index = 2;
			extra_dialog_answer_add = -1;
			new_y_size += extra_dialog_answer_add;
			break;
		case 17:
			extra_dialog_answer_index = 2;
			extra_dialog_answer_add = 1;
			new_y_size += extra_dialog_answer_add;
			break;
		case 19:
			extra_dialog_answer_index = 3;
			extra_dialog_answer_add = -1;
			new_x_size += extra_dialog_answer_add;
			break;
		case 20:
			extra_dialog_answer_index = 3;
			extra_dialog_answer_add = 1;
			new_x_size += extra_dialog_answer_add;
			break;
		case 31:
			cd_flip_led(991,31,item_hit);
			break;
		}
	if (cre(new_x_size,1,50,"Outdoors width must be between 1 and 50.","",991) == TRUE) return ;
	if (cre(new_y_size,1,50,"Outdoors height must be between 1 and 50.","",991) == TRUE) return ;
	if (cre(new_x_size * new_y_size,1,100,"The total number of outdoor sections (width times height) must be between 1 and 100.","",991) == TRUE) return ;
	
	extra_dialog_answer[extra_dialog_answer_index] += extra_dialog_answer_add;
	
	sprintf((char *) temp_str,"%s%d",extra_dialog_answer[0] < 0 ? "-" : "+",(int)abs(extra_dialog_answer[0]));
	cd_set_item_text(991,12,(char *) temp_str);
	sprintf((char *) temp_str,"%s%d",extra_dialog_answer[1] < 0 ? "-" : "+",(int)abs(extra_dialog_answer[1]));
	cd_set_item_text(991,15,(char *) temp_str);
	sprintf((char *) temp_str,"%s%d",extra_dialog_answer[2] < 0 ? "-" : "+",(int)abs(extra_dialog_answer[2]));
	cd_set_item_text(991,18,(char *) temp_str);
	sprintf((char *) temp_str,"%s%d",extra_dialog_answer[3] < 0 ? "-" : "+",(int)abs(extra_dialog_answer[3]));
	cd_set_item_text(991,21,(char *) temp_str);
	
	cd_set_item_num(991,28,new_x_size);
	cd_set_item_num(991,29,new_y_size);
}

Boolean change_outdoor_size()
{
	short basic_dlog_hit;
	short on_surface_answer;
	
	extra_dialog_answer[0] = extra_dialog_answer[1] = extra_dialog_answer[2] = extra_dialog_answer[3] = 0;
	store_max_x = scenario.out_width;
	store_max_y = scenario.out_height;
	
	cd_create_dialog_parent_num(991,0);
	
	cd_set_item_text(991,12,"+0");
	cd_set_item_text(991,15,"+0");
	cd_set_item_text(991,18,"+0");
	cd_set_item_text(991,21,"+0");
	
	cd_set_item_num(991,26,store_max_x);
	cd_set_item_num(991,27,store_max_y);
	
	cd_set_item_num(991,28,store_max_x);
	cd_set_item_num(991,29,store_max_y);
	
	while (dialog_not_toast)
		ModalDialog(main_dialog_UPP, &basic_dlog_hit);
	
	on_surface_answer = cd_get_led(991,31);
	
	cd_kill_dialog(991,0);
	
	if(dialog_answer == -1)
		return FALSE;
	/*
	//keep the "same" sector selected, if it still exists
	cur_out.x += extra_dialog_answer[1];
	cur_out.y += extra_dialog_answer[0];
	if(cur_out.x < 0 || cur_out.x >= extra_dialog_answer[1] + extra_dialog_answer[3] + store_max_x ||
	cur_out.y < 0 || cur_out.y >= extra_dialog_answer[0] + extra_dialog_answer[2] + store_max_y) {
		cur_out.x = cur_out.y = 0;
		if(editing_town = FALSE)
			cen_x = cen_y = 24;
	}
	//and the same starting place
	scenario.what_outdoor_section_start_in.x += extra_dialog_answer[1];
	scenario.what_outdoor_section_start_in.y += extra_dialog_answer[0];
	if(scenario.what_outdoor_section_start_in.x < 0 || scenario.what_outdoor_section_start_in.x >= extra_dialog_answer[1] + extra_dialog_answer[3] + store_max_x ||
	scenario.what_outdoor_section_start_in.y < 0 || scenario.what_outdoor_section_start_in.y >= extra_dialog_answer[0] + extra_dialog_answer[2] + store_max_y) {
		scenario.what_outdoor_section_start_in.x = scenario.what_outdoor_section_start_in.y = 0;
		scenario.start_where_in_outdoor_section.x = scenario.start_where_in_outdoor_section.y = 24;
	}*/
	
	save_change_to_outdoor_size(extra_dialog_answer[0],extra_dialog_answer[1],extra_dialog_answer[2],extra_dialog_answer[3],on_surface_answer);
	
	return TRUE;
}

void new_town_event_filter (short item_hit)
{
	switch (item_hit) {
		case 3:
			dialog_answer = 1;
			 dialog_not_toast = FALSE; 
			break;
		case 15:
			dialog_answer = -1;
			 dialog_not_toast = FALSE; 
			break;
		default:
			cd_hit_led_range(830,10,12,item_hit);
			cd_flip_led(830,18,item_hit);
			break;
	}
}

Boolean new_town()
{
	short basic_dlog_hit;
	Str255 temp_str;
	short size = 0;
	short on_surface_answer;
	
	short new_town = scenario.num_towns;
	 
	cd_create_dialog_parent_num(830,0);
	
	cd_set_item_num(830,14,new_town);
	cd_set_led(830,11,1);
	cd_set_led(830,18,town.is_on_surface);
	sprintf((char *) temp_str,"New Town Name");
	CDST(830,2,(char *) temp_str);
	
	while (dialog_not_toast)
		ModalDialog(main_dialog_UPP, &basic_dlog_hit);

	size = cd_get_led_range(830,10,12);
	on_surface_answer = cd_get_led(830,18);
	CDGT(830,2,(char *) temp_str);
	temp_str[19] = 0;
	cd_kill_dialog(830,0);
	if (dialog_answer < 0)
		return FALSE;
	
	scenario.num_towns++;
	scenario.town_size[new_town] = size;
	cur_town = new_town;
	scenario.last_town_edited = new_town;
	town.clear_town_record_type();
	town.set_start_locs(size);
	t_d.clear_big_tr_type();
	strcpy(town.town_name,(char *) temp_str);
	strcpy(&zone_names.town_names[new_town][0],&town.town_name[0]);
	town_type = size;
	town.is_on_surface = on_surface_answer;
	if (on_surface_answer) {
		for (short i = 0; i < 64; i++){
			for (short j = 0; j < 64; j++){
				t_d.floor[i][j] = 37;
			}
		}
	}
		
	// set walls appropriately
	if (town.is_on_surface) {
		town.wall_1_sheet = 600;
		town.wall_2_sheet = 605;
		town.cliff_sheet = 650;
	}
	else {
		town.wall_1_sheet = 601;
		town.wall_2_sheet = 600;
		town.cliff_sheet = 651;
	}
	return TRUE;
}

// before calling this, be sure to do all checks to make sure it's safe.
void delete_town()
{
	for(int i=cur_town; i<scenario.num_towns; i++){
		scenario.town_size[i]=scenario.town_size[i+1];
		scenario.town_starts_hidden[i]=scenario.town_starts_hidden[i+1];
	}
	if(scenario.start_in_what_town == cur_town){
		scenario.start_in_what_town=0;
	}
	scenario.num_towns--;
	save_remove_town();
}

void pick_import_town_event_filter (short item_hit)
{
	switch (item_hit) {
		case 3:
			dialog_answer = CDGN(store_which_dlog,2);
			if (dialog_answer < 0) {
				give_error("This number is out of the correct range. It must be at least 0.","",store_which_dlog);
				break;
				}
			 dialog_not_toast = FALSE; 
			break;
		case 7:
			dialog_answer = -1;
			dialog_not_toast = FALSE; 
			break;

	}
}

short pick_import_town(short which_dlog,short def)
// ignore parent in Mac version
{
	short town_strs_hit;
	
	 store_which_dlog = which_dlog;
	
	cd_create_dialog_parent_num(store_which_dlog,0);
	
	CDSN(store_which_dlog,2,0);
	
	while (dialog_not_toast)
		ModalDialog(main_dialog_UPP, &town_strs_hit);
	
	cd_kill_dialog(store_which_dlog,0);
	return dialog_answer;
}

// Used for editing terrain. Called when user calls floor_drawn onto floor_there.
// returns TRUE is editor should erase spot
Boolean floors_match(short floor_drawn,short floor_there)
{
	// matching rough stone
	if ((((floor_drawn >= 4) && (floor_drawn <= 7)) || ((floor_drawn >= 11) && (floor_drawn <= 22))) &&
	  (((floor_there >= 4) && (floor_there <= 7)) || ((floor_there >= 11) && (floor_there <= 22))))
		return TRUE;
	// matching desert
	if ((floor_drawn == 41) &&
	  ((floor_there >= 41) && (floor_there <= 56)))
		return TRUE;
	// matching cave floor
	//if (((floor_drawn >= 0) && (floor_drawn <= 3)) &&
	 // ((floor_there >= 0) && (floor_there <= 3)))
	//	return TRUE;
	// matching grass
	//if (((floor_drawn >= 37) && (floor_drawn <= 40)) &&
	 // ((floor_there >= 37) && (floor_there <= 40)))
	//	return TRUE;
	// matching cave water
	if (((floor_drawn >= 11) && (floor_drawn <= 22)) &&
	  ((floor_there >= 11) && (floor_there <= 22)))
		return TRUE;
	// matching surfacxe water
	if (((floor_drawn >= 57) && (floor_drawn <= 70)) &&
	  ((floor_there >= 57) && (floor_there <= 70)))
		return TRUE;
	if (floor_drawn == floor_there)
		return TRUE;
	return FALSE;
}

// Used for editing terrain. Called when user calls terrain_drawn onto terrain_there.
// returns TRUE is editor should erase spot
Boolean terrains_match(short terrain_drawn,short terrain_there)
{
	if (terrain_drawn == terrain_there)
		return TRUE;
	return FALSE;
}


void edit_area_rect_event_filter (short item_hit)
{
	Str255 str;
	
	switch (item_hit) {
		case 5:
			dialog_answer = TRUE;
			dialog_not_toast = FALSE; 
			CDGT(840,2,(char *) str);
			str[29] = 0;
			if (editing_town == FALSE){
				pushUndoStep(new Undo::DescriptionAreaTextChange((char*)&str[0],&current_terrain.info_rect_text[store_which_str][0],store_which_str));
				sprintf(current_terrain.info_rect_text[store_which_str],"%s",(char *) str);
			}
			else{
				pushUndoStep(new Undo::DescriptionAreaTextChange((char*)&str[0],&town.info_rect_text[store_which_str][0],store_which_str));
				sprintf(town.info_rect_text[store_which_str],"%s",(char *) str);
			}
			break;

		case 3:
			dialog_answer = FALSE;
			dialog_not_toast = FALSE; 
			break;
		}
}

// mode 0 - out 1 - town
Boolean edit_area_rect_str(short which_str,short mode)
// ignore parent in Mac version
{
	short item_hit;
	
	store_which_str = which_str;
	
	cd_create_dialog_parent_num(840,0);
	
	if (editing_town == FALSE)
		CDST(840,2,current_terrain.info_rect_text[store_which_str]);
	else
		CDST(840,2,town.info_rect_text[store_which_str]);

	while (dialog_not_toast)
		ModalDialog(main_dialog_UPP, &item_hit);
	
	cd_kill_dialog(840,0);
	
	return dialog_answer;
}

Boolean save_out_strs()
{
	Str255 str;
	short i;
	
	for (i = 0; i < NUM_OUT_DESCRIPTION_AREAS; i++){
		CDGT(850,2 + i,(char *) str);
		str[29] = 0;
		sprintf((char *)current_terrain.info_rect_text[i],"%s",str);
		if (str_do_delete[i] > 0){
			current_terrain.info_rect[i].right = -1;
			if(selected_object_type==SelectionType::AreaDescription && selected_object_number==i){
				selected_object_type=SelectionType::None;
				selected_object_number=0;
			}
		}
	}
	return TRUE;
}

void put_out_strs_in_dlog()
{
	Str255 str;
	short i;
	
	for (i = 0; i < NUM_OUT_DESCRIPTION_AREAS; i++) {
		if ((current_terrain.info_rect[i].right <= 0) || (str_do_delete[i] > 0)) {
			sprintf((char *) str,"Not yet placed.");
			cd_activate_item(850,25 + i,0);
		}
		else
			sprintf((char *) str,"L = %d, T = %d, R = %d, B = %d",current_terrain.info_rect[i].left,
					current_terrain.info_rect[i].top,current_terrain.info_rect[i].right,current_terrain.info_rect[i].bottom);
		cd_set_item_text(850,13 + i,(char *) str);
		CDST(850,2 + i,current_terrain.info_rect_text[i]);
	}
	
}

void edit_out_strs_event_filter (short item_hit)
{
	switch (item_hit) {
		case 10:
			if (save_out_strs() == TRUE)
				 dialog_not_toast = FALSE; 
			break;
		case 11:
				 dialog_not_toast = FALSE; 
			break;
		default:
			if ((item_hit >= 25) && (item_hit <= 32)) {
				//sprintf((char *)data_store->out_strs[item_hit - 25 + 1],"");
				CDST(850,2 + item_hit - 25,"");
				str_do_delete[item_hit - 25] = 1;
				put_out_strs_in_dlog();
			}
			break;
		}
}

void edit_out_strs()
// ignore parent in Mac version
{
	short out_strs_hit,i;
	
	for (i = 0; i < NUM_OUT_DESCRIPTION_AREAS; i++)
		str_do_delete[i] = 0;
		
	cd_create_dialog_parent_num(850,0);
	
	put_out_strs_in_dlog();
	

	while (dialog_not_toast)
		ModalDialog(main_dialog_UPP, &out_strs_hit);
		
	cd_kill_dialog(850,0);
}


Boolean save_town_strs()
{
	Str255 str;
	short i;
	
	for (i = 0; i < NUM_TOWN_DESCRIPTION_AREAS; i++) {
		CDGT(839,2 + i,(char *) str);
		str[29] = 0;
		sprintf((char *)town.info_rect_text[i],"%s",str);
		if (str_do_delete[i] > 0){
			town.room_rect[i].right = -1;
			if(selected_object_type==SelectionType::AreaDescription && selected_object_number==i){
				selected_object_type=SelectionType::None;
				selected_object_number=0;
			}
		}
	}
	return TRUE;
}

void put_town_strs_in_dlog()
{
	Str255 str;
	short i;
	
	for (i = 0; i < NUM_TOWN_DESCRIPTION_AREAS; i++) {
		if ((town.room_rect[i].right <= 0) || (str_do_delete[i] > 0)) {
			sprintf((char *) str,"Not yet placed.");
			cd_activate_item(839,41 + i,0);
			}
			else sprintf((char *) str,"L = %d, T = %d, R = %d, B = %d",town.room_rect[i].left,
				town.room_rect[i].top,town.room_rect[i].right,town.room_rect[i].bottom);
		cd_set_item_text(839,21 + i,(char *) str);
		CDST(839,2 + i,town.info_rect_text[i]);
		}

}

void edit_town_strs_event_filter (short item_hit)
{
	switch (item_hit) {
		case 18:
			if (save_town_strs() == TRUE)
				 dialog_not_toast = FALSE; 
			break;
		case 19:
				 dialog_not_toast = FALSE; 
			break;
		default:
			if ((item_hit >= 41) && (item_hit <= 56)) {
				//sprintf((char *)data_store->town_strs[item_hit - 41 + 1],"");
				CDST(839,2 + item_hit - 41,"");
				str_do_delete[item_hit - 41] = 1;
				put_town_strs_in_dlog();
				}
			break;
		}
}

void edit_town_strs()
// ignore parent in Mac version
{
	short town_strs_hit,i;
	
	for (i = 0; i < NUM_TOWN_DESCRIPTION_AREAS; i++)
		str_do_delete[i] = 0;
		
	cd_create_dialog_parent_num(839,0);
	
	put_town_strs_in_dlog();

	while (dialog_not_toast)
		ModalDialog(main_dialog_UPP, &town_strs_hit);

	cd_kill_dialog(839,0);
}


Boolean save_item_placement()
{
	short i;
	
	store_storage.property = cd_get_led(812,38);
	store_storage.ter_type = CDGN(812,22);
	for (i = 0; i < 10; i++) {
		store_storage.item_num[i] = CDGN(812,2 + i);
		store_storage.item_odds[i] = CDGN(812,12 + i);
		}
	scenario.storage_shortcuts[cur_shortcut] = store_storage;
	
	return TRUE;
}

void put_item_placement_in_dlog()
{
	short i;
	
	cd_set_item_num(812,27,cur_shortcut);
	cd_set_led(812,38,store_storage.property);
	CDSN(812,22,store_storage.ter_type);
	for (i = 0; i < 10; i++) {
		CDSN(812,2 + i,store_storage.item_num[i]);
		CDSN(812,12 + i,store_storage.item_odds[i]);
		}

}

void edit_item_placement_event_filter (short item_hit)
{
	short i;
	
	switch (item_hit) {
		case 23:
			if (save_item_placement() == TRUE)
				 dialog_not_toast = FALSE; 
			break;
		case 24:
				 dialog_not_toast = FALSE; 
			break;
		case 26:
			if (save_item_placement() == FALSE) break;
			cur_shortcut--;
			if (cur_shortcut < 0) cur_shortcut = 9;
			store_storage = scenario.storage_shortcuts[cur_shortcut];
			put_item_placement_in_dlog();
			break;
		case 25:
			if (save_item_placement() == FALSE) break;
			cur_shortcut++;
			if (cur_shortcut > 9) cur_shortcut = 0;
			store_storage = scenario.storage_shortcuts[cur_shortcut];
			put_item_placement_in_dlog();
			break;
		case 40:
			store_storage.ter_type = CDGN(812,22);
			i = choose_text_res(-4,0,511,store_storage.ter_type,812,"Which Terrain?");
			store_storage.ter_type = i;
			CDSN(812,22,i);
			break;
		default:
			if ((item_hit >= 41) && (item_hit <= 50)) {
				i = CDGN(812,2 + item_hit - 41);
				i = choose_text_res(-2,0,NUM_SCEN_ITEMS - 1,i,812,"Place which item?"); 
				if (i >= 0)
					CDSN(812,2 + item_hit - 41,i);
				break;
				}
			cd_flip_led(812,38,item_hit);
			break;
		}
}

void edit_item_placement()
// ignore parent in Mac version
{
	short item_placement_hit;
	
	store_storage = scenario.storage_shortcuts[0];
	cur_shortcut = 0;
	
	cd_create_dialog_parent_num(812,0);
	
	put_item_placement_in_dlog();
 
	while (dialog_not_toast)
		ModalDialog(main_dialog_UPP, &item_placement_hit);
	cd_kill_dialog(812,0);
}

Boolean save_add_town()
{
	short i;
	
	for (i = 0; i < 10; i++) {
		scenario.town_to_add_to[i] = CDGN(810,2 + i);
		if (cre(scenario.town_to_add_to[i],
			-1,199,"Town number must be from 0 to 199 (or -1 for no effect).","",810) == TRUE) return FALSE;
		scenario.flag_to_add_to_town[i][0] = CDGN(810,12 + i);
		if (cre(scenario.flag_to_add_to_town[i][0],
			0,299,"First part of flag must be from 0 to 299.","",810) == TRUE) return FALSE;
		scenario.flag_to_add_to_town[i][1] = CDGN(810,22 + i);
		if (cre(scenario.flag_to_add_to_town[i][1],
			0,29,"Second part of flag must be from 0 to 29.","",810) == TRUE) return FALSE;
		}
	return TRUE;
}

void put_add_town_in_dlog()
{
	short i;
	
	for (i = 0; i < 10; i++) {
		CDSN(810,2 + i,scenario.town_to_add_to[i]);
		CDSN(810,12 + i,scenario.flag_to_add_to_town[i][0]);
		CDSN(810,22 + i,scenario.flag_to_add_to_town[i][1]);
		}

}

void edit_add_town_event_filter (short item_hit)
{
	switch (item_hit) {
		case 32:
			if (save_add_town() == TRUE)
				 dialog_not_toast = FALSE; 
			break;

		}
}

void edit_add_town()
// ignore parent in Mac version
{
	short item_placement_hit;
		
	cd_create_dialog_parent_num(810,0);
	
	put_add_town_in_dlog();
	
	while (dialog_not_toast)
		ModalDialog(main_dialog_UPP, &item_placement_hit);

	cd_kill_dialog(810,0);
}

void put_make_scen_1_in_dlog()
{	
	CDST(800,2,"Scenario name");
	CDST(800,3,"filename");
}

void edit_make_scen_1_event_filter (short item_hit)
{
	char str[256];
	
	switch (item_hit) {
		case 4:
			CDGT(800,2,str);
			if (string_not_clean(str,30,0,"The scenario name",800))
				return;
			if (strlen(str) == 0) {
				give_error("You've left the scenario name empty.","",800);
				return;
				}
			CDGT(800,3,str);
			if (string_not_clean(str,21,1,"The scenario file name",800))
				return;
			if (strlen(str) == 0) {
				give_error("You've left the scenario file name empty.","",800);
				return;
				}

			dialog_answer = 1;
			dialog_not_toast = FALSE; 
			break;
		case 8:
			dialog_answer = 0;
			dialog_not_toast = FALSE; 
			break;
		default:
			cd_flip_led(800,10,item_hit);
			break;
		}
}

short edit_make_scen_1(char *filename,char *title,short *start_on_surface)
// ignore parent in Mac version
{
	short make_scen_1_hit;
	
	cd_create_dialog_parent_num(800,0);
	
	put_make_scen_1_in_dlog();
	
	while (dialog_not_toast)
		ModalDialog(main_dialog_UPP, &make_scen_1_hit);

	CDGT(800,2,title);
	title[49] = 0;
	CDGT(800,3,filename);
	filename[21] = 0;
	*start_on_surface = cd_get_led(800,10);
	cd_kill_dialog(800,0);
	return dialog_answer;
}

void put_make_scen_2_in_dlog()
{
	CDSN(801,2,1);
	CDSN(801,3,1);
}

void edit_make_scen_2_event_filter (short item_hit)
{
	short i,j;
	
	switch (item_hit) {
		case 7:
			i = CDGN(801,2);
			if (cre(i,
				1,50,"Outdoors width must be between 1 and 50.","",801) == TRUE) return ;
			j = CDGN(801,3);
			if (cre(j,
				1,50,"Outdoors height must be between 1 and 50.","",801) == TRUE) return ;
			if (cre(i * j,
				1,100,"The total number of outdoor sections (width times height) must be between 1 and 100.","",801) == TRUE) return ;
	
			dialog_not_toast = FALSE; 
			break;
		case 6:
			dialog_answer = 0;
				 dialog_not_toast = FALSE; 
			break;
		default:
			cd_flip_led(801,17,item_hit);
			break;
		}
}

short edit_make_scen_2(short *val_array)
// ignore parent in Mac version
{
	short make_scen_2_hit,i;//array[6];
	
	//array = val_array;
	cd_create_dialog_parent_num(801,0);
	
	put_make_scen_2_in_dlog();
	
	while (dialog_not_toast)
		ModalDialog(main_dialog_UPP, &make_scen_2_hit);

	for (i = 0; i < 2; i++)
		val_array[i] = CDGN(801,2 + i);
	val_array[5] = cd_get_led(801,17);
	cd_kill_dialog(801,0);
	return dialog_answer;
}

void build_scenario()
{
	short two_flags[6]; // width, height, large, med, small, default_town
	char f_name[256],f_name2[256],title[256];
	short on_surface;
	
	file_is_loaded = FALSE;

	if (edit_make_scen_1((char *) f_name,(char *) title,&on_surface) == FALSE)
		return;
	sprintf((char *) f_name2,"%s.bas",f_name);
	if (edit_make_scen_2((short *) two_flags) == FALSE)
		return;
	if (fancy_choice_dialog(860,0) == 2)
		return;
	
	if (create_basic_scenario(f_name,f_name2,title,two_flags[0],two_flags[1],
	  on_surface,two_flags[5]))	
		fancy_choice_dialog(874,0);
		else fancy_choice_dialog(875,0);

}

Boolean save_scen_details()
{
	Str255 str;
	short i;
	
	scenario.rating = cd_get_led_range(803,11,14);
	scenario.ver[0] = CDGN(803,2);
	scenario.ver[1] = CDGN(803,3);
	scenario.ver[2] = CDGN(803,4);
	for (i = 0; i < 3; i++)
		if (cre(scenario.ver[i],
			0,9,"The digits in the version number must be in the 0 to 9 range.","",803) == TRUE) return FALSE;

	CDGT(803,5,scenario.credits_text[1]);
	CDGT(803,6,scenario.scen_desc);
	CDGT(803,7,(char *) str);
	str[49] = 0;
	strcpy(scenario.scen_name,(char *) str);

	scenario.min_level = CDGN(803,8);
	if (cre(scenario.min_level,
		0,100,"The minimum level must be between 0 and 100.","",803) == TRUE) return FALSE;
	scenario.max_level = CDGN(803,9);
	if (cre(scenario.max_level,
		0,100,"The maximum level must be between 0 and 100.","",803) == TRUE) return FALSE;
	
	return TRUE;
}

void put_scen_details_in_dlog()
{
	cd_set_led_range(803,11,14,scenario.rating);
	CDSN(803,2,scenario.ver[0]);
	CDSN(803,3,scenario.ver[1]);
	CDSN(803,4,scenario.ver[2]);
	CDST(803,5,scenario.credits_text[1]);
	CDST(803,6,scenario.scen_desc);
	CDST(803,7,scenario.scen_name);
	CDSN(803,8,scenario.min_level);
	CDSN(803,9,scenario.max_level);
}

void edit_scen_details_event_filter (short item_hit)
{
	switch (item_hit) {
		case 10:
			if (save_scen_details() == TRUE)
				 dialog_not_toast = FALSE; 
			break;
		default:
			cd_hit_led_range(803,11,14,item_hit);
			break;
	}
}

void edit_scen_details()
// ignore parent in Mac version
{
	short scen_details_hit;
	cd_create_dialog_parent_num(803,0);
	put_scen_details_in_dlog();
	while (dialog_not_toast)
		ModalDialog(main_dialog_UPP, &scen_details_hit);
	cd_kill_dialog(803,0);
}

void edit_scen_intro_event_filter (short item_hit)
{
	short i;
	switch (item_hit) {
		case 9:
			scenario.intro_pic_resources[store_which_part] = CDGN(804,8);
			if ((scenario.intro_pic_resources[store_which_part] < -1) || (scenario.intro_pic_resources[store_which_part] > 3000)) {
				give_error("WARNING: Intro picture number is out of range.","",804);
				break;
			}
			for (i = 0; i < 6; i++)
				CDGT(804, 2 + i,scenario.intro_text[store_which_part][i]);
			dialog_not_toast = FALSE; 
			break;
		case 10: 
			dialog_not_toast = FALSE; 
			break;
	}
}

void edit_scen_intro(short which_part)
{
	short i,item_hit;
	store_which_part = which_part;
	cd_create_dialog_parent_num(804,0);
	CDSN(804,8,scenario.intro_pic_resources[which_part]);
	for (i = 0; i < 6; i++)
		CDST(804, 2 + i,scenario.intro_text[which_part][i]);
	cd_set_item_num(804,16,store_which_part);
	while (dialog_not_toast)
		ModalDialog(main_dialog_UPP, &item_hit);
	cd_kill_dialog(804,0);
}

void edit_scen_intro_pic_event_filter (short item_hit)
{
	switch (item_hit) {
		case 3:
			scenario.scen_label_pic = CDGN(805,2);
			if ((scenario.scen_label_pic < 0) || (scenario.scen_label_pic > 3000)) {
				give_error("WARNING: Label picture number is out of range.","",8805);
				break;
			}
			dialog_not_toast = FALSE; 
			break;
		case 4: 
			dialog_not_toast = FALSE; 
			break;
	}
}

void edit_scen_intro_pic()
{
	short item_hit;
	
	cd_create_dialog_parent_num(805,0);
	
	CDSN(805,2,scenario.scen_label_pic);
		
	while (dialog_not_toast)
		ModalDialog(main_dialog_UPP, &item_hit);
	cd_kill_dialog(805,0);
}

void pick_town_event_filter (short item_hit, int which_dlg)
{
	switch (item_hit) {
		case 3:
			dialog_answer = CDGN(store_which_dlog,2);
			dialog_answer = minmax(store_dlog_min,store_dlog_max,dialog_answer);
			dialog_not_toast = FALSE; 
			break;
		case 4:
			dialog_answer = -1;
			dialog_not_toast = FALSE; 
			break;
		case 7:
			int i = CDGN(which_dlg,2);
			i = choose_text_res(-7,1,store_dlog_max+1,i+1,which_dlg,"Choose Which Town:");
			if (i > 0) {
				CDSN(which_dlg,2,i-1);
			}
			break;
	}
}

void change_town_size_event_filter(short item_hit)
{
	//7,8,9 are the leds
	//10 is cancel, 11 is ok
	switch (item_hit) {
		case 11:
			dialog_answer = 1;
			dialog_not_toast = FALSE; 
			break;
		case 10:
			dialog_answer = -1;
			dialog_not_toast = FALSE; 
			break;
		default:
			cd_hit_led_range(831,7,9,item_hit);
			cd_flip_led(831,18,item_hit);
			break;
	}
}

Boolean change_town_size()
{
	short i,j;
	int size = -1;// 0 - large, 1 - medium, 2 - small
	short old_size=scenario.town_size[cur_town];
	short basic_dlog_hit;
	cd_create_dialog_parent_num(831,0);
	cd_set_led(831,7+old_size,1);
	while (dialog_not_toast)
		ModalDialog(main_dialog_UPP, &basic_dlog_hit);
	size = cd_get_led_range(831,7,9);
	cd_kill_dialog(831,0);
	if (dialog_answer<0 || size<0 || size>2)
		return FALSE;
	if(old_size==size)
		return(FALSE);//no change made, so do nothing
	scenario.town_size[cur_town] = size;
	scenario.last_town_edited = cur_town;
	town.clear_town_record_type();
	town.set_start_locs(size);
	t_d.clear_big_tr_type();
	town_type = size;
	if (town.is_on_surface) {
		for (i = 0; i < 64; i++){
			for (j = 0; j < 64; j++){
				t_d.floor[i][j] = 37;
			}
		}
	}
	// set walls appropriately
	if (town.is_on_surface) {
		town.wall_1_sheet = 600;
		town.wall_2_sheet = 605;
		town.cliff_sheet = 650;
	}
	else {
		town.wall_1_sheet = 601;
		town.wall_2_sheet = 600;
		town.cliff_sheet = 651;
	}
	return TRUE;
}

void edit_item_properties_event_filter (short item_hit)
{
	switch (item_hit) {
		case 13:		
		case 12:
		case 11:
		case 10:
		case 9:
			cd_flip_led(838,item_hit,item_hit);
			break;
		case 3: 
			town.preset_items[store_which_placed_item].properties=0;
			for(int i=9; i<14; i++){
				if(cd_get_led(838,i)>0)
					town.preset_items[store_which_placed_item].properties|=(1<<(i-9));
			}
			//fall through
		case 2:
			dialog_not_toast = FALSE; 
			break;
	}
}

void edit_item_properties(short which_i)
// ignore parent in Mac version
{
	short item_properties_hit;
	cd_create_dialog_parent_num(838,0);
	//put the properties in the dialog
	for(int i=9; i<14; i++){
		if(town.preset_items[which_i].properties&(1<<(i-9)))
			cd_flip_led(838,i,i);
	}
	store_which_placed_item = which_i;
	while (dialog_not_toast)
		ModalDialog(main_dialog_UPP, &item_properties_hit);
	cd_kill_dialog(838,0);
}