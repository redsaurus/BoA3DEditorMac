#define NUM_BLOCK_DEFINERS	40
#define NUM_VAR_DEFINERS	220
#define NUM_ARRAY_VAR_DEFINERS	40
#define NUM_STRING_VAR_DEFINERS	20
#define NUM_VAR_TYPE_DEFINERS	10
#define NUM_CONST_TYPE_DEFINERS	30
#define NUM_FLOW_CONTROLLERS	10
#define NUM_OPERATORS	20
#define NUM_NO_PARAM_FUNCTIONS	40
#define NUM_UNARY_FUNCTIONS	60
#define NUM_BINARY_FUNCTIONS	30
#define NUM_TRINARY_FUNCTIONS	30
#define NUM_LOCATION_FUNCTIONS	30
#define NUM_PROCEDURES	130

#define BLOCK_TYPE	1
#define	FLOW_CONTROLLER_TYPE	2
#define UNARY_FUNCTION_TYPE	10
#define BINARY_FUNCTION_TYPE	11
#define TRINARY_FUNCTION_TYPE	12
#define LOCATION_FUNCTION_TYPE	13
#define NO_PARAM_FUNCTION_TYPE	13
#define NUMBER_TYPE	17
#define LEFTPARAN_TYPE	18
#define RIGHTPARAN_TYPE	19
#define COMMA_TYPE	20
#define	INT_VARIABLE_TYPE	22
#define	LOCATION_VARIABLE_TYPE	23
#define	STRING_VARIABLE_TYPE	24
#define PROCEDURE_TYPE	30
#define BINARY_OPERATOR_TYPE	32
#define EQUALS_TYPE	40
#define SEMICOLON_TYPE	41
#define LEFTBRACKET_TYPE	42
#define RIGHTBRACKET_TYPE	43
#define STRING_TYPE	47

#define PLUS_OPERATOR	0
#define MINUS_OPERATOR	1
#define TIMES_OPERATOR	2
#define DIVIDE_OPERATOR	3
#define AND_OPERATOR	4
#define OR_OPERATOR	5
#define EQUALS_OPERATOR	6

// Token types
// 0 - unused
// 1 - block definer
// 2 - flow controller

// 10 - int-returning function, unary
// 11 - int-returning function, binary
// 12 - int-returning function, trinary
// 13 - location returning function, passed 1 integer
// 14 - int-returning function, no parameters
// 16 - 
// 17 - number
// 18 - (
// 19 - )
// 20 - ,
// 21 - constant
// 22 - int variable
// 23 - location variable
// 24 - string variable

// 30 - procedure
// 31 - 
// 32 - binary operator
// 33 - 

// 40 - =
// 41 - ;
// 42 - {
// 43 - }
// 44 - (scenario data only) variable definer, used for defining a value in a single variable in the scenario data
// 45 - (scenario data only) array variable definer, used for defining a value in an array in the scenario data
// 46 - (scenario data only) string variable definer, used for defining a value in a string in the scenario data
// 47 - string data
// 48 - new variable definer (e.g. "short","location","string")


// Token describers

// Used to define a block of code
token_describer_type block_definers[NUM_BLOCK_DEFINERS] = {
	{"beginscendatascript"},
	{"begindefinecreature"},	
	{"begindefinefloor"},	
	{"begindefineterrain"},	
	{"begindefineitem"},	

	{""},	
	{""},	
	{"beginoutdlogscript"},	
	{"begintowndlogscript"},	
	{""},	

	{""},	// 10
	{"variables"},	
	{"body"},	
	{"begincreaturescript"},	
	{"beginoutdoorscript"},	

	{"begintownscript"},	
	{"beginstate"},	
	{"beginterrainscript"},	
	{"break"},	
	{"beginscenarioscript"},

	{"begintalkscript"}, // 20
	{"begintalknode"},	
	{"personality"},	
	{"state"},	
	{"nextstate"},

	{"condition"},	
	{"text1"},	
	{"text2"},	
	{"text3"},	
	{"text4"},	

	{"text5"},	// 30
	{"text6"},	
	{"text7"},	
	{"text8"},	
	{"action"},	

	{"question"},	
	{"code"},	
	{""},	
	{""},	
	{""}
	};

short constant_values[NUM_CONST_TYPE_DEFINERS] = {
	0,1,2,1,0, -1,1,100,100,110,
	1,2,3,4,5, 6,7,101,102,0,
	0,0,0,0,0, 0,0,0,0,0
	};
token_describer_type constant_definers[NUM_CONST_TYPE_DEFINERS] = {
{"init_state"},
{"dead_state"},
{"start_state"},
{"true"},
{"false"},
	
{"me"},
{"exit_state"},
{"search_state"},
{""},
{"talking_state"},
	
{"dep_on_sdf"},
{"set_sdf"},
{"shop"},
{"set_to_1"},
{"pay"},
	
{"end_talk"},
{"intro"},
{"unlock_spell_state"},
{"sanctification_state"},
{""},
	
{""},
{""},
{""},
{""},
{""},
	
{""},
{""},
{""},
{""},
{""}
	
	};
	
token_describer_type no_parameter_function_definers[NUM_NO_PARAM_FUNCTIONS] = {
{"pc_num"},
{"current_unlock_strength"},
{"random_party_member"},
{"is_group"},
{"is_combat"},
	
{"get_crime_level"},
{"coins_amount"},
{"get_current_tick"},
{"current_personality"},
{"current_zone"},
	
{"dist_to_pc"}, // 10 // creature/object
{"am_i_doing_action"},
{"my_current_message"},
{"my_number"},
{"item_used_on_me"},
	
{""},
{""},
{""},
{""},
{""},
	
{"target_ok"}, // 20 // creature
{"get_target"},
{"my_group"},
{"my_dist_from_start"},
{"friendly_target_ok"},
	
{"who_shot_me"},
{"my_ap"},
{"get_friend_target"},
{""},
{""},

{"get_pc_skill_pts"}, // 30
{""},
{""},
{""},
{""},

{""},
{""},
{""},
{""},
{""}

};

token_describer_type unary_function_definers[NUM_UNARY_FUNCTIONS] = {
	{"char_ok"}, // general
	{"what_group_in"},
	{"num_chars_in_group"},
	{"get_attitude"},
	{"object_type"},
	
	{"creature_type"},
	{"has_spec_item"},
	{"get_stat"},
	{"get_courage"},
	{"get_strategy"},
	
	{"get_aggression"}, // 10
	{"pay_coins"},
	{"has_item"},
	{"take_all_of_item"},
	{"get_level"},
	
	{"zone_clear"},
	{"can_see_char"},
	{"spawn_creature"},
	{"creature_on_obj"},
	{""},
	
	{"get_memory_cell"}, // 20 // creature/object only
	{"dist_to_char"},
	{"get_nearest_char"},
	{"get_nearest_good_char"},
	{"get_nearest_evil_char"},
	
	{"get_nearest_like_char"},
	{"get_nearest_hate_char"},
	{""},
	{""},
	{""},
	
	{""}, // 30
	{""},
	{""},
	{""},
	{""},
	
	{""},
	{""},
	{""},
	{""},
	{""},
	
	{"friends_nearby"}, // 40 // creature only
	{"enemies_nearby"},
	{""},
	{""},
	{""},
	
	{""},
	{""},
	{""},
	{""},
	{""},
	
	{"get_health"}, // 50 more general
	{"get_max_health"},
	{"num_of_item"},
	{"get_obj_tool_difficulty"},
	{"get_obj_num_tools_needed"},
	
	{"pc_can_disarm_obj"},
	{""},
	{""},
	{""},
	{""}
	
	};
	
token_describer_type binary_function_definers[NUM_BINARY_FUNCTIONS] = {
	{"get_sdf"},
	{"char_in_group"},
	{"get_flag"},
	{"tick_difference"},
	{"get_terrain"},
	
	{"get_floor"},
	{"get_char_status"},
	{"dist_to_nav_point"},
	{""},
	{""},
	
	{""}, // 10
	{""},
	{""},
	{""},
	{""},
	
	{""},
	{""},
	{""},
	{""},
	{""},
	
	{"return_to_start"}, // 20
	{""},
	{""},
	{""},
	{""},
	
	{""},
	{""},
	{""},
	{""},
	{""}
	
	};

token_describer_type trinary_function_definers[NUM_TRINARY_FUNCTIONS] = {
	{"get_ran"},
	{"middle_num"},
	{"last_num"},
	{""},
	{""},
	
	{""},
	{""},
	{""},
	{""},
	{""},
	
	{""}, // 10
	{""},
	{""},
	{""},
	{""},
	
	{""},
	{""},
	{""},
	{""},
	{""},
	
	{"follow_path"}, // 20 // begin creature only
	{"approach_char"},
	{"approach_object"},
	{"approach_nav_point"},
	{""},
	
	{"maintain_dist_to_char"},
	{"flee_char"},
	{"get_foe_target"},
	{"get_friendly_target"},
	{""}
	
	};

token_describer_type location_returning_function_definers[NUM_LOCATION_FUNCTIONS] = {
	{"character_loc"},
	{"my_loc"},
	{""},
	{""},
	{""},
	
	{""},
	{""},
	{""},
	{""},
	{""},
	
	{""},
	{""},
	{""},
	{""},
	{""},
	
	{""},
	{""},
	{""},
	{""},
	{""},
	
	{""},
	{""},
	{""},
	{""},
	{""},
	
	{""},
	{""},
	{""},
	{""},
	{""}
	
	};
// 0 - int, 1 - pix_loc, 2 - some string type
char procedure_expected_variable_types[NUM_PROCEDURES][6] = {
	{0,-1,-1,-1,-1},
	{0,0,0,-1,-1},
	{2,-1,-1,-1,-1},
	{2,0,-1,-1,-1},
	{1,-1,-1,-1,-1},

	{-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1},
	{0,-1,-1,-1,-1},
	{0,0,-1,-1,-1},
	{0,0,-1,-1,-1},

	{0,-1,-1,-1,-1}, // 10
	{0,0,0,-1,-1},
	{0,0,0,-1,-1},
	{0,0,-1,-1,-1},
	{0,0,0,-1,-1},

	{0,0,0,0,-1},
	{0,0,-1,-1,-1},
	{-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1},
	{0,-1,-1,-1,-1},

	{0,-1,-1,-1,-1}, // 20
	{0,-1,-1,-1,-1},
	{0,-1,-1,-1,-1},
	{2,-1,-1,-1,-1},
	{2,0,-1,-1,-1},

	{2,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1},
	{0,0,-1,-1,-1},
	{0,0,-1,-1,-1},
	{0,0,-1,-1,-1},

	{0,0,-1,-1,-1}, // 30
	{0,1,-1,-1,-1},
	{-1,-1,-1,-1,-1},
	{0,0,-1,-1,-1},
	{0,0,-1,-1,-1},

	{0,0,-1,-1,-1},
	{0,0,0,-1,-1},
	{0,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1},

	{-1,-1,-1,-1,-1}, // 40
	{-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1},

	{-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1},

	{0,0,0,0,-1}, // 50
	{-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1},

	{-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1},

	{0,0,0,-1,-1}, // 60
	{-1,-1,-1,-1,-1},
	{0,0,-1,-1,-1},
	{0,0,-1,-1,-1},
	{-1,-1,-1,-1,-1},

	{0,0,-1,-1,-1},
	{0,0,-1,-1,-1},
	{0,0,-1,-1,-1},
	{0,0,-1,-1,-1},
	{0,0,-1,-1,-1},

	{0,0,-1,-1,-1}, // 70
	{0,0,-1,-1,-1},
	{0,0,0,-1,-1},
	{0,0,-1,-1,-1},
	{0,0,-1,-1,-1},

	{0,0,-1,-1,-1},
	{0,-1,-1,-1,-1},
	{0,2,-1,-1,-1},
	{0,0,-1,-1,-1},
	{0,-1,-1,-1,-1},

	{0,0,-1,-1,-1}, // 80
	{2,0,2,-1,-1},
	{2,0,2,0,-1},
	{0,0,0,0,-1},
	{0,0,0,0,-1},

	{0,-1,-1,-1,-1},
	{0,0,-1,-1,-1},
	{0,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1},

	{0,-1,-1,-1,-1}, // 90
	{0,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1},

	{-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1},

	{0,0,0,0,-1}, // 100
	{0,0,0,-1,-1},
	{-1,-1,-1,-1,-1},
	{0,0,-1,-1,-1},
	{0,-1,-1,-1,-1},

	{0,-1,-1,-1,-1},
	{0,-1,-1,-1,-1},
	{0,0,-1,-1,-1},
	{-1,-1,-1,-1,-1},
	{2,2,0,0,0},

	{0,-1,-1,-1,-1}, // 110
	{0,0,0,-1,-1},
	{0,-1,-1,-1,-1},
	{0,0,0,-1,-1},
	{0,0,0,-1,-1},

	{0,0,-1,-1,-1},
	{0,-1,-1,-1,-1},
	{0,2,-1,-1,-1},
	{-1,-1,-1,-1,-1},
	{0,0,-1,-1,-1},

	{0,-1,-1,-1,-1}, // 120
	{0,0,-1,-1,-1},
	{0,0,0,0,0},
	{0,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1},

	{0,0,-1,-1,-1},
	{0,0,-1,-1,-1},
	{0,2,-1,-1,-1},
	{-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1}
	};
token_describer_type procedure_definers[NUM_PROCEDURES] = {
	{"print_num"},
	{"print_nums"},
	{"print_str"},
	{"print_str_color"},
	{"print_loc"},
	
	{"end"},
	{""},
	{"begin_talk_mode"},
	{"set_spec_item"},
	{"alter_stat"},
	
	{"set_state"}, // 10 begin creature/object only
	{"run_object_animation"},
	{"broadcast_char_message"},
	{"broadcast_object_message"},
	{"heal_nearby"},
	
	{"damage_nearby"},
	{"run_reverse_object_animation"},
	{"open_container"},
	{"close_container"},
	{"shift_graphic"},
	
	{"shift_destroyed_graphic"}, // 20
	{"play_sound"},
	{"pc_heard_sound"},
	{"set_terrain_string"},
	{"set_terrain_string_range"},
	
	{"create_text_bubble"},
	{"drain_charge_of_used_object"},
	{"set_memory_cell"},
	{"play_sound_delay"},
	{"pc_heard_sound_delay"},
		
	{"fidget"}, // 30 // begin creature only
	{"move_to_loc"},
	{"do_attack"},
	{"set_foe_target"},
	{"set_friend_target"},
	
	{"kill_char"},
	{"run_char_animation"},
	{"deduct_ap"},
	{"end_combat_turn"},
	{""},
	
	{""}, // 40
	{""},
	{""},
	{""},
	{""},
	
	{""},
	{""},
	{""},
	{""},
	{""},
	
	{"status_nearby"}, // 50 more creature/object only
	{""},
	{""},
	{""},
	{""},
	
	{""},
	{""},
	{""},
	{""},
	{""},
	
	{"set_flag"}, // 60 // begin general
	{"clear_group"},
	{"add_char_to_group"},
	{"remove_char_from_group"},
	{""},
	
	{"kill_object"},
	{"change_object_type"},
	{"give_object_message"},
	{"set_object_icon"},
	{"set_object_destroyed_icon"},
	
	{"give_char_message"}, // 70 Functions which can be fed a group
	{"set_attitude"},
	{"set_char_status"},
	{"set_courage"},
	{"set_strategy"},
	
	{"set_aggression"},
	{"erase_char"},
	{"set_name"},
	{"set_level"},
	{"alert_char"},
	
	{"set_object_blockage"}, // 80 // more general
	{"print_big_str"},
	{"print_big_str_num"},
	{"run_sparkles_on_char"},
	{"run_sparkles_on_object"},
	
	{"activate_hidden_group"},
	{"award_party_xp"},
	{"change_crime_level"},
	{""},
	{""},
	
	{"add_string"}, // 90 // special/talk only
	{"remove_string"},
	{"clear_strings"},
	{""},
	{""},
	
	{""},
	{""},
	{""},
	{""},
	{""},
	
	{"restore_energy_nearby"}, // 100 - more general
	{"inc_flag"},
	{"make_zone_hostile"},
	{"toggle_quest"},
	{"change_coins"},
	
	{"reward_give"},
	{"take_item"},
	{"put_object_on_navpoint"},
	{"revive_party"},
	{"begin_shop_mode"},
	
	{"set_crime_tolerance"}, // 110
	{"add_range_to_group"},
	{"clear_zone"},
	{"set_terrain"},
	{"set_floor"},
	
	{"set_act_at_dist"},
	{"drop_item"},
	{"print_named_str"},
	{"set_items_not_property"},
	{"heal_char"},
	
	{"set_walk_damage"}, // 120
	{"take_num_of_item"},
	{"nav_point_damage_party"},
	{"change_pc_skill_pts"},
	{"turret_heal"},
	
	{"restore_energy_char"},
	{"set_obj_tool_difficulty"},
	{"show_object_disarm_window"},
	{""},
	{""}
	};
	
token_describer_type new_variable_definers[NUM_VAR_TYPE_DEFINERS] = {
	{"short"},
	{"int"},
	{"string"},
	{"location"},
	{""},
	
	{""},
	{""},
	{""},
	{""},
	{""}
	};

token_describer_type flow_controller_definers[NUM_FLOW_CONTROLLERS] = {
	{"if"},
	{"while"},
	{"else"},
	{""},
	{""},
	
	{""},
	{""},
	{""},
	{""},
	{""}
	};
	
// precedence of operators, higher number is more precedence
short operator_precedence[NUM_OPERATORS] = {
	12,12,13,13,5, 4,9,10,10,10,
	10,9,0,0,0, 0,0,0,0,0};

token_describer_type operator_definers[NUM_OPERATORS] = {
	{"+"},
	{"-"},
	{"*"},
	{"/"},
	{"&&"},
	
	{"||"},
	{"=="},
	{">"},
	{"<"},
	{">="},
	
	{"<="},
	{"!="},
	{""},
	{""},
	{""},

	{""},
	{""},
	{""},
	{""},
	{""}	
	};	
	
token_describer_type variable_array_definers[NUM_ARRAY_VAR_DEFINERS] = {
	{"cr_what_stat_adjust"},	
	{"cr_amount_stat_adjust"},	
	{"cr_start_item"},	
	{"cr_start_item_chance"},	
	{"cr_immunities"},	

	{""},	
	{""},	
	{""},	
	{""},	
	{""},
			
	{""},// 10 //
	{""},	
	{""},	
	{""},	
	{""},	
	
	{""},	
	{""},	
	{""},	
	{""},	
	{""},	
	
	{""}, // 20	
	{""},	
	{""},	
	{""},	
	{""},	
	
	{""},	
	{""},	
	{""},	
	{""},	
	{""},	
	
	{""}, // 30
	{""},	
	{""},	
	{""},	
	{""},	
	
	{""},	
	{""},	
	{""},	
	{""},	
	{""}
	};

token_describer_type variable_string_definers[NUM_STRING_VAR_DEFINERS] = {
	{"fl_name"},	
	{""},	
	{"te_name"},	
	{"te_default_script"},	
	{""},	
	
	{"it_name"},	
	{"it_full_name"},	
	{""},	
	{"cr_name"},	
	{"cr_default_script"},	
			
	{""},// 10 
	{""},	
	{""},	
	{""},	
	{""},	
	
	{""},	
	{""},	
	{""},	
	{""},	
	{""}	
	
	};

	
// Used to define an initializer for a variable in the scen data file
token_describer_type variable_definers[NUM_VAR_DEFINERS] = {
	{"fl_which_sheet"},	
	{"fl_which_icon"},	
	{"fl_icon_adjust"},	
	{"fl_ed_which_sheet"},	
	{"fl_ed_which_icon"},	
	
	{"fl_ed_icon_adjust"},	
	{"fl_blocked"},	
	{"fl_step_sound"},	
	{"fl_light_radius"},	
	{"fl_floor_height_pixels"},	

	{"fl_special_property"}, // 10
	{"fl_special_strength"},	
	{"fl_is_water"},	
	{"fl_is_floor"},	
	{"fl_is_ground"},	
	
	{"fl_is_rough"},	
	{"fl_fly_over"},	
	{"fl_shortcut_key"},	
	{"fl_anim_steps"},	
	{"fl_shimmers"},	
	
	{"fl_out_fight_town_used"}, // 20
	{""},	
	{""},	
	{""},	
	{""},	
	
	{"te_which_sheet"},	
	{"te_which_icon"},	
	{"te_icon_adjust"},	
	{"te_ed_which_sheet"},	
	{"te_ed_which_icon"},	

	{"te_ed_icon_adjust"}, // 30
	{"te_cutaway_which_sheet"},	
	{"te_cutaway_which_icon"},	
	{"te_cutaway_icon_adjust"},	
	{"te_second_icon"},	
	
	{"te_second_icon_offset_x"},	
	{"te_second_icon_offset_y"},	
	{"te_cutaway_second_icon"},	
	{"te_cutaway_offset_x"},	
	{"te_cutaway_offset_y"},	

	{"te_anim_steps"}, // 40
	{"te_move_block_n"},	
	{"te_move_block_w"},	
	{"te_move_block_s"},	
	{"te_move_block_e"},	
	
	{"te_look_block_n"},	
	{"te_look_block_w"},	
	{"te_look_block_s"},	
	{"te_look_block_e"},	
	{"te_blocks_view_n"},	
	
	{"te_blocks_view_w"}, // 50
	{"te_blocks_view_s"},	
	{"te_blocks_view_e"},	
	{"te_height_adj_pixels"},	
	{"te_suppress_floor"},	
	
	{"te_light_radius"},	
	{"te_step_sound"},	
	{"te_shortcut_key"},	
	{"te_crumble_type"},	
	{"te_beam_hit_type"},	
	
	{"te_terrain_to_crumble_to"}, // 60
	{"te_hidden_town_terrain"},	
	{"te_swap_terrain"},	
	{"te_is_bridge"},	
	{"te_is_road"},	
	
	{"te_can_look_at"},	
	{"te_special_property"},	
	{"te_special_strength"},	
	{"te_draw_on_automap"},	
	{"te_icon_offset_x"},	
	
	{"te_icon_offset_y"}, // 70
	{"te_full_move_block"},	
	{"te_full_look_block"},	
	{"te_shimmers"},	
	{"te_out_fight_town_used"},	
	
	{"it_variety"},	
	{"it_damage_per_level"},	
	{"it_bonus"},	
	{"it_weapon_skill_used"},	
	{"it_protection"},	
	
	{"it_charges"}, // 80
	{"it_encumbrance"},	
	{"it_floor_which_sheet"},	
	{"it_floor_which_icon"},	
	{"it_icon_adjust"},	
	
	{"it_inventory_icon"},	
	{"it_ability_1"},	
	{"it_ability_2"},	
	{"it_ability_3"},	
	{"it_ability_4"},	
	
	{"it_ability_str_1"}, // 90
	{"it_ability_str_2"},	
	{"it_ability_str_3"},	
	{"it_ability_str_4"},	
	{"it_special_class"},	
	
	{"it_value"},	
	{"it_weight"},	
	{"it_value_class"},	
	{"it_identified"},	
	{"it_magic"},	
	
	{"it_cursed"}, // 100
	{"it_once_per_day"},	
	{"it_junk_item"},	
	{"it_missile_anim_type"},	
	{"it_augment_item_type"},	
	
	{""},	
	{""},	
	{""},	
	{""},	
	{""},	
	
	{"cr_level"}, // 110
	{"cr_hp_bonus"},	
	{"cr_sp_bonus"},	
	{"cr_special_abil"},	
	{"cr_default_attitude"},	
	
	{"cr_species"},	
	{"cr_natural_armor"},	
	{"cr_attack_1"},	
	{"cr_attack_2"},	
	{"cr_attack_3"},	
	
	{"cr_attack_1_type"}, // 120
	{"cr_attack_23_type"},	
	{"cr_ap_bonus"},	
	{"cr_default_strategy"},	
	{"cr_default_aggression"},	
	
	{"cr_default_courage"},	
	{"cr_which_sheet"},	
	{"cr_icon_adjust"},	
	{"cr_small_or_large_template"},	
	{"cr_which_sheet_upper"},	
	
	{"cr_summon_class"}, // 130
	{""},	
	{""},	
	{""},	
	{""},	
	
	{""},	
	{""},	
	{""},	
	{""},	
	{""},	
	
	{""}, // 140
	{""},	
	{""},	
	{""},	
	{""},	
	
	{""},	
	{""},	
	{""},	
	{""},	
	{""},	
	
	{""}, // 150
	{""},	
	{""},	
	{""},	
	{""},	
	
	{""},	
	{""},	
	{""},	
	{""},	
	{""},	
	
	{""}, // 160
	{""},	
	{""},	
	{""},	
	{""},	
	
	{""},	
	{""},	
	{""},	
	{""},	
	{""},	
	
	{""}, // 170
	{""},	
	{""},	
	{""},	
	{""},	
	
	{""},	
	{""},	
	{""},	
	{""},	
	{""},	
	
	{""}, // 180
	{""},	
	{""},	
	{""},	
	{""},	
	
	{""},	
	{""},	
	{""},	
	{""},	
	{""},	
	
	{""}, // 190
	{""},	
	{""},	
	{""},	
	{""},	
	
	{""},	
	{""},	
	{""},	
	{""},	
	{""},	
	
	{"import"},	// 200
	{"clear"},	
	{""},	
	{""},	
	{""},	

	{""},	
	{""},	
	{""},	
	{""},	
	{""},
	
	{""},	// 210
	{""},	
	{""},	
	{""},	
	{""},	

	{""},	
	{""},	
	{""},	
	{""},	
	{""}


	
	};