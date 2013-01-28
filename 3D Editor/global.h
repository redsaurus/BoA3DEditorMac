// global.h

#ifndef GLOBAL_H
#define GLOBAL_H

// The comments are partial, intermittent, and may be wrong.

//tridash - larger window stuff

#define DEFAULT_WINDOW_WIDTH 516//675//standard value is 750
#define DEFAULT_WINDOW_HEIGHT 455//585//standard value is 550
#define TILES_DRAW_RECT_HEIGHT 520//height of rect drawn to in the tiles window, previously 382
#define TILES_DRAW_RECT_WIDTH 310//width of rect drawn to in the tiles window, previously 210
#define TILES_WINDOW_WIDTH 325//previously 225
#define TILES_DRAW_RECT {20, 0, 20 + TILES_DRAW_RECT_HEIGHT, TILES_DRAW_RECT_WIDTH}//size of rectangle terrain buttons drawn to, standard value is {0, 0, 382, 210}
#define DEFAULT_TERRAIN_RECT_GR_SIZE {0, 0, 512, 512}//size of GWorld for terrain to be drawn to in main window

#define DEFAULT_RECT3DEDIT_HEIGHT 415
#define DEFAULT_RECT3DEDIT_WIDTH 496

extern const Rect default_terrain_viewport_3d;

#define DEFAULT_INDOOR_DRAW_DISTANCE 10//realistic draw distance in towns, used to be 10
#define DEFAULT_OUTDOOR_DRAW_DISTANCE 14//realistic draw distance outdoors, used to be 14

#define TERRAIN_NUM_ROWS 43
#define ITEM_NUM_ROWS 42
#define CREATURE_NUM_ROWS 22
#define FLOOR_NUM_ROWS 22

#define MAIN_WINDOW_NUM 0
#define PALETTE_WINDOW_NUM 1
#define TILES_WINDOW_NUM 2

#define EXILE_BIG_GUNS 1

//lengths of time to wait between events
#define DENSE_TICKS 10
#define SPARSE_TICKS 60
#define VERY_SPARSE_TICKS 300
#define	SLEEP_TICKS 0L

#define	MOUSE_REGION	0L
#define DRAG_EDGE		15
#define IN_FRONT	(WindowPtr)-1L
#define	NIL				0L


#define	NUM_BUTTONS		13
#define	NUM_P_PER_TOWN		20

#define	NUM_DLOG_B		53

#define	MAX_TOWN_SIZE	64
#define OUTDOOR_SIZE	48

#define	TER_RECT_UL_X	20
#define	TER_RECT_UL_Y	20
#define	TERRAIN_BORDER_WIDTH	15
//tridash - larger window stuff
#define	LEFT_TEXT_LINE_ULX	5//default 15
#define	LEFT_TEXT_LINE_ULY	5//default 485
#define	LEFT_TEXT_LINE_WIDTH	240
#define	TEXT_LINE_HEIGHT	12
#define	RIGHT_TEXT_LINE_ULX	25 //default 5
#define	RIGHT_TEXT_LINE_ULY	65 //default 105
#define	RIGHT_BUTTONS_X_SHIFT 500//680//default 540
#define RIGHT_PALETTE_X_SHIFT 500
#define RIGHT_TILES_X_SHIFT 0
#define RIGHT_BUTTONS_Y_SHIFT 0//520//default 382
#define RIGHT_SCROLLBAR_WIDTH 16

#define	PALETTE_BUT_UL_X	0
//#define	PALETTE_BUT_UL_Y	520//default 382

#define NUM_TALK_NODES	100
#define NUM_SCEN_ITEMS	500

#define	SMALL_SPACE_SIZE	8
#define	BIG_SPACE_SIZE	48
#define SPACE_X_DISPLACEMENT_3D  24
#define SPACE_Y_DISPLACEMENT_3D  16
#define ELEVATION_Y_DISPLACEMENT_3D  23
#define PICT_BOX_WIDTH_3D	46
#define PICT_BOX_HEIGHT_3D   55
#define ITEM_BOX_SIZE_3D   28
#define OUTDOOR_CREATURE_WIDTH_3D   11
#define OUTDOOR_CREATURE_HEIGHT_3D	16

#define	TER_BUTTON_HEIGHT_3D	terrain_button_height//default is 19
#define	TER_BUTTON_HEIGHT_3D_OLD	19
#define TER_BUTTON_HEIGHT_3D_STD	27
#define TER_BUTTON_HEIGHT_3D_MID	41
#define TER_BUTTON_HEIGHT_3D_BIG	55

#define TILES_N_COLS tiles_n_columns
#define TILES_N_COLS_STD 12

// new blades consts
#define	SCRIPT_NAME_LEN	14

#define	CDGT	cd_retrieve_text_edit_str
#define	CDGN	cd_retrieve_text_edit_num
#define	CDST	cd_set_text_edit_str
#define	CDSN	cd_set_text_edit_num

#define NUM_WAYPOINTS	10

#define	TER_BUTTON_SIZE	terrain_button_width//default is 16
#define TER_BUTTON_SIZE_BIG 46
#define TER_BUTTON_SIZE_MID 34
#define	TER_BUTTON_SIZE_STD	23//24
#define TER_BUTTON_SIZE_OLD 16

#define	PALETTE_BUT_HEIGHT		17
#define	PALETTE_BUT_WIDTH		25

#define NUM_TER_SCRIPTS	100

#define MAX_NUM_SHEETS_IN_LIBRARY	200

#define NUM_TOWN_PLACED_ITEMS	144
#define NUM_TOWN_PLACED_FIELDS	60
#define NUM_TOWN_PLACED_CREATURES	80
#define NUM_TOWN_PLACED_SPECIALS	60
#define NUM_OUT_PLACED_SPECIALS	30
#define NUM_OUT_TOWN_ENTRANCES	8
#define NUM_TOWN_DESCRIPTION_AREAS 16
#define NUM_OUT_DESCRIPTION_AREAS 8

#define kNO_TOWN_SPECIALS	0xFF	// No Special encounter on town map
#define kNO_OUT_SPECIALS	-1		// No special encounter on outdoor map
#define kNO_OUT_TOWN_ENTRANCE	-1	// No town entrance on outdoor map
#define kNO_SIGN	-1				// No sign in town or outdoors

#define kINVAL_TOWN_LOC_X	-1		// invalid town location
#define kINVAL_TOWN_LOC_Y	-1

#define SDF_RANGE_X	300
#define SDF_RANGE_Y	30

#define TOOL_PALETTE_GUTTER_WIDTH 10
#define TOOL_TITLE_HEIGHT 12
#define TOOL_PALETTE_TEXT_LINE_WIDTH 240
#define TOOL_PALETTE_TEXT_LINE_HEIGHT 12
#define TOOL_PALETTE_TEXT_LINE_SPACING 2
#define TOOL_PALETTE_HEIGHT (4*TOOL_PALETTE_GUTTER_WIDTH + TOOL_TITLE_HEIGHT + TOOL_PALETTE_TEXT_LINE_SPACING + PALETTE_BUT_HEIGHT+1 + 10*(TOOL_PALETTE_TEXT_LINE_HEIGHT+TOOL_PALETTE_TEXT_LINE_SPACING))
#define TOOL_PALETTE_WIDTH (7*TOOL_PALETTE_GUTTER_WIDTH + PALETTE_BUT_WIDTH+1 + TOOL_PALETTE_TEXT_LINE_WIDTH)

const static struct endianness_t{
	bool isLittle;
	endianness_t(){
		uint16_t test=0xFF00;
		isLittle=((uint8_t*)(&test))[0]==0;
	}
} endianness;

// patch to change the mouse click behavior on the dialog button/radio button
// mode selector for cd_press_button()
enum EDLGBtnRes {
	eDLGBtnResCompatible,	// change graphics of the button - sound and delay - recover graphics
	eDLGBtnResChange,		// change graphics of the button - sound (no delay)
	eDLGBtnResRecover,		// recover graphics of the button
};

// bit definition of scroll direction
enum {
	eSCRL_NoScrl	= 0,
	eSCRL_Top		= 0x08,
	eSCRL_Left		= 0x04,
	eSCRL_Bottom	= 0x02,
	eSCRL_Right		= 0x01,
};

//use a wrapping namespace to simulate C++0x strongly typed enum
//when possible this should be switched to an actual strongly type enum
//by removing the namespace and changing 'enum SelectionType_e' to
//'enum class SelectionType'
namespace SelectionType{
	enum SelectionType_e {
		None,
		//Town only:
		Creature,
		Item,
		TerrainScript,
		Waypoint,
		//Town or outdoor:
		SpecialEncounter, 
		AreaDescription,
		Sign,
		//Outdoor only:
		TownEntrance
	};
} //namespace SelectionType

#define	inUpButton	kControlUpButtonPart
#define	inDownButton	kControlDownButtonPart
#define	inPageUp	kControlPageUpPart
#define	inPageDown	kControlPageDownPart
#define	inThumb	kControlIndicatorPart
#define	DisposDialog	DisposeDialog
#define	GetMHandle	GetMenuHandle

typedef struct {
	char x,y;
} location;

//typedef struct {
//	short type,sd1,sd2,pic,m1,m2,ex1a,ex1b,ex2a,ex2b,jumpto;
//	} special_node_type;

typedef struct {
	short which_state,which_personality;
	short condition1[4],condition2[4];
	short action_when_selected,default_jump_state;
	short action_mods[4];
} talking_node_type;

typedef struct {
	unsigned char short_strlens[NUM_TALK_NODES];
	unsigned char strlens[NUM_TALK_NODES * 2];
	char char_names[NUM_P_PER_TOWN][20];
	short p_start_node[NUM_P_PER_TOWN];
	short p_start_state[NUM_P_PER_TOWN];
	talking_node_type talk_nodes[NUM_TALK_NODES];
} talking_record_type;

// The data for the location for an icon in memory.
class graphic_id_type {
public:
	graphic_id_type():
	which_sheet(-1),which_icon(0),graphic_adjust(0){}
	
	graphic_id_type(short sheet, short icon, short adjust):
	which_sheet(sheet),which_icon(icon),graphic_adjust(adjust){}
	
	void clear_graphic_id_type();
	Boolean not_legit();
	
	short which_sheet; // the resource number of the graphic
	short which_icon; // the icon in the resource. icons are in resources 
		// 16 icons per row
	short graphic_adjust;
};
	
class floor_type_type {
public:
	floor_type_type();
	void clear_floor_values();
	
	char floor_name[20];
	graphic_id_type pic; // regular picture location for ground
	graphic_id_type ed_pic; // picture location for editor graphic for icon
	short blocked; // 0 no 1 yes
 	char step_sound; // -1 no sound, otherwise sound when stepped on
 	char light_radius; // radius terrain sends its light out to, 0 - no light
 	short floor_height; // in pixels, positive is up, shows how many vertical pixels
 						// to offset graphics when drawn
	short num_anim_steps;
	
	// special properties of floor
	short special; // floor special property
	short effect_adjust; // intensity of special effect
	// described in the editor docs
	
	// special editor and movement toggles
	Boolean is_water; // water?
	Boolean is_floor; // a human built (stone or wood) floor
	Boolean is_ground; // regular grass, cave floor?
	Boolean is_rough; // rought errain? (i.e. desert or rough cave)
	Boolean fly_over; // can this terrain be flied over?
	Boolean shimmers; // does this graphic get drawn with graphical effects?
		// 0 - none
		// 1 - glows light/dark
		// 2 - water effect
	
	short outdoor_combat_town_used; // The town that is loaded when party fights an outdoor
		// combat on this terrain type. If 0-999, loads the town in the current scenario/
		// If 1000 + x, loads town x in file Blades of Avernum Out Fight
		// 0 - 
	unsigned char shortcut_key;
	short extra[6];
};

class terrain_type_type {
public:
	terrain_type_type();
	void clear_terrain_values();

	char ter_name[20];
	graphic_id_type pic; // regular picture location for when above ground
	graphic_id_type cut_away_pic; // picture when the terrain is being drawn cut away
	graphic_id_type ed_pic; // picture location for editor graphic for icon
	
	// the icon can be drawn with a certain pixel offset.
	short icon_offset_x,icon_offset_y;
	
	// a terrain spot can have a second icon drawn, which must be in the same sheet as the first icon.
	// it affects the outdoor and underground terrain versions equally.
	// there can be a different icon for the front and cut away view.
	short second_icon,second_icon_offset_x,second_icon_offset_y;
	short cut_away_second_icon,cut_away_offset_x,cut_away_offset_y;
		// For now, cut_away_offset_x & cut_away_offset_y are UNUSED
		// Use other offsets instead.
	short anim_steps; // number of icons in this terrain's animation. uses icons after
		// first icon in sheet for animation
	unsigned char move_block[4]; // 0 - no block, 1 - block movement, 0 - n, 1 - w, 2 - s, 3 - e
	unsigned char see_block[4]; // 0 - no block, 1 - sight movement, 0 - n, 1 - w, 2 - s, 3 - e
	unsigned char blocks_view[4]; // 0 - draw after pc in spot, 1 - draw before, 0 - n, 1 - w, 2 - s, 3 - e
	short height_adj; // pixels up to draw chars/items in this space
	unsigned char suppress_floor; // 0 - no, if 1, floor type under terrain type is ignored
	unsigned char light_radius; // radius of light coming from this spot
	char step_sound; // -1 - no sound, takes precedence over floor sound

	unsigned char shortcut_key;
	
	// various special properties
	short crumble_type; // 0 - won't crumble, 1-5 - strength of speel needed to make crumble
	short beam_hit_type; // 0 - blocks beam, 1 - allows beam through
		// 3 - crumbles when beam hits
	short terrain_to_crumble_to;
	short hidden_town_terrain; // -1 - unused. otherwise, terrain to place here if this is
		// icon for a hidden town
	short swap_terrain; // ter to swap to if a swap terrain special call is used
	Boolean is_bridge; // allows people to cross this space even if floor is a blocking type
	Boolean is_road;
	Boolean can_look_at; // a terrain which is always selected to be inspected when player looks
	Boolean draw_on_automap;
	char default_script[SCRIPT_NAME_LEN]; // the name of the default script
	Boolean shimmers; // does this graphic get drawn with graphical effects?
		// 0 - none
		// 1 - glows light/dark
		// 2 - water effect

	short outdoor_combat_town_used; // The town that is loaded when party fights an outdoor
		// combat on this terrain type. If 0-999, loads the town in the current scenario/
		// If 1000 + x, loads town x in file Blades of Avernum Out Fight
		// 0 - 
	
	// special properties of terrain
	short special; // terrain special property
	short effect_adjust; // intensity of special effect
	// below may be out of date. full info inj editor docs
	// special		effect			effect_adjust
	// 0			none
	// 1			fire damage		amt. of damage
	// 2			cold damage		amt. of damage
	// 3			magic damage	amt. of damage
	// 4			poison			amt. of damage
	// 5			disease			amt. of damage
	// 6			blocks monsts
	
	// Hill properties
	// 19			hill, up to west
	// 20			hill, up to southwest
	// 21			hill, up to south
	// 22			hill, up to southeast
	// 23			hill, up to east
	// 24			hill, up to northeast
	// 25			hill, up to north
	// 26			hill, up to northwest
	// 27			hill, down to southeast
	// 28			hill, down to northeast
	// 29			hill, down to northwest
	// 30			hill, down to southwest
	
	// Beam abilities
	// 31			beam power source, if charged by beam	
	// 32			fires beam north
	// 33			fires beam west
	// 34			fires beam south
	// 35			fires beam east
	// 36			nw/se mirror
	// 37			ne/sw mirror
	// 38			beam power source

	// 39			sign
	// 40			container
	// 41			acts as table
	// 42			glows
	// 43			waterfall - south
	// 44			waterfall - west
	// 45			destroyed by quickfire	
};

class out_wandering_type {
public:
	out_wandering_type();
	void clear_out_wandering_type();
	void port();
	
	short hostile[4],hostile_amount[4];
	short friendly[3],friendly_amount[3];

	short cant_flee;
		// this encounter can ever be evaded?
	short end_spec1,end_spec2;
		// if this flag is set, this encounter never appears.
	short killed_flag1,killed_flag2;
		// this flag is set to 1 when enc killed.
		// can, and probably should, be same as end_spec
		
	short forced; // if > 0, party encoutners this immediately
	short check_every_turn; // most encoutners checked every 10 turns.
		// if this is 1, check this one every turn
	short move_type;
		// 0 - seek party  1 - no move  2 - random move  3 - follow roads  4 - flee party
		// +10 - always stays within 10 spaces of starting spot
	location start_loc; // location encounter first appears
	short start_state_when_encountered;
	short start_state_when_defeated;
	short start_state_when_fled;
	short random_move_chance; // chance (0-100) that this creature will move randomly at
		// any move instead of doing what it wants
};

class outdoor_record_type {
public:
	outdoor_record_type();
	void clear_outdoor_record_type();
	void port();

	char name[20];
	unsigned char floor[OUTDOOR_SIZE][OUTDOOR_SIZE],height[OUTDOOR_SIZE][OUTDOOR_SIZE];
	short terrain[OUTDOOR_SIZE][OUTDOOR_SIZE];
	Rect special_rects[NUM_OUT_PLACED_SPECIALS];
	short spec_id[NUM_OUT_PLACED_SPECIALS];
	Rect exit_rects[NUM_OUT_TOWN_ENTRANCES];
	short exit_dests[NUM_OUT_TOWN_ENTRANCES];

	// signs
	location sign_locs[8];
	char sign_text[8][256];
	
	// wandering
	out_wandering_type wandering[4],special_enc[4],preset[8];
	location wandering_locs[4];

	Rect info_rect[NUM_OUT_DESCRIPTION_AREAS];
	char info_rect_text[NUM_OUT_DESCRIPTION_AREAS][30];

	// scripts and special flags
	char section_script[SCRIPT_NAME_LEN]; // the name of the default script
	short is_on_surface;
	
	short extra[10];	// extra[0]: region number
};

class creature_type {
public:
	creature_type();
	void clear_creature_type();
	
	char name[20];
	short level; // level of creature
	short which_to_add_to[6]; // which 6 skills to give bonuses to
	short how_much[6]; // how much to add to each skill
	short items[8]; // numbers of items it has
	short item_chances[8]; // chance, from 0 to 100, of the item being present
	short hp_bonus;
	short sp_bonus;
	short spec_abil; 
		// 0 - 
	short default_attitude; // 0 - PC, 1 - PC aid, 2 friendly, 3 - neutral, 4 - hostile A, 5 - hostile B
	short species;
		// 0 - human
		// 1 - humanoid
		// 2 - nephil
		// 3 - slith
		// 4 - giant
		// 5 - reptile
		// 6 - beast
		// 7 - demon
		// 8 - undead
		// 9 - insect
		// 10 - slime
		// 11 - stone/golem
		// 12 - special
		// 13 - vahnatai
		// 14-19 - other/ scenario designed
	unsigned char immunities[6]; 
		// 0 - fire, 1 - cold, 2 - magic, 3 - mental, 4 - poison/acid, 5 - melee
	short natural_armor; // like armor, starts at 0

	// graphic
	graphic_id_type char_graphic;
	short char_upper_graphic_sheet;
	short small_or_large_template;

	short a[3],a1_type,a23_type;
		// attack types
		// 0 - strike
		// 1 - claw
		// 2 - bite
		// 3 - slimes
		// 4 - punches
		// 5 - stings
		// 6 - clubs
		// 7 - burns
		// 8 - harms
		// 9 - stabs
	short ap_bonus;

	char default_script[SCRIPT_NAME_LEN]; 
	short default_strategy;
		// 0 - default, regular attack
		// 1 - archer/caster, so maintain distance
		// 2-9 - reserved
	short default_aggression,default_courage;
	short summon_class; // -1 - never summoned, otherwise, number from 0-6, higher = harder to summon
	short extra[10];
};

class item_record_type {
public:
	item_record_type();
	void clear_item_record_type();

	short variety;
		// 0 - null item
		// 1 - 1 handed weapon
		// 2 - 2 handed Weapon
		// 3 - Gold
		// 4 - Food
		// 5 - Thrown Missile
		// 6 - Bow
		// 7 - Potion
		// 8 - Scroll
		// 9 - Wand
		// 10 - Tool
		// 11 - Pants
		// 12 - Shield
		// 13 - Armor
		// 14 - Helm
		// 15 - Gloves
		// 16 - Boots
		// 17 - Cloak
		// 18 - Ring
		// 19 - Necklace
		// 20 - Bracelet
		// 21 - Object
		// 22 - Crossbow
		// 23 - Arrows
		// 24 - Bolts
	short damage; // weapons do (1 - damage/2) + (1 - damage/2)
	char bonus; // amt. added to weapon damage, and to hit increased by %5 * bonus
	char weap_type; // number of skill used for weapon, such as 4 for melee
	char protection; // prevents (1 - protection/2) + (1 - protection/2) melee damage
	char charges;
	short encumbrance;
	graphic_id_type item_floor_graphic;
	short inven_icon;
	short missile_anim_type;

	// item abilities
	short ability_in_slot[4]; // -1 or 0 - no abil
		// otherwise, number of ability
		// probably out of date. consult editor docs for full list
		// PASSIVE ABILITIES
		// 1 - 49 - Affects statistic x + 1. So if 3, affects statistic 2 (Intelligence)
		// 50 - Melee to hit chance
		// 51 - Melee damage
		// 52 - Missile to hit chance
		// 53 - Missile damage
		// 54 - Resist all hostile effects
		// 55 - Resist fire
		// 56 - Resist cold
		// 57 - Resist magic
		// 58 - Resist mental
		// 59 - Resist poison
		// 60 - Resist acid
		// 61 - Affect action points
		// 62 - Affect melee statistics
		// 63 - Affect magic statistics
		// 64 - Affect all statistics
		// 65 - Affect rune reading
		// 70 - Extra fire damage (only has effect on a weapon)
		// 71 - Extra acid damage (only has effect on a weapon)
		// 72 - Extra poison damage (only has effect on a weapon)
		// 73 - Extra damage to humanoids (only has effect on a weapon)
		// 74 - Extra damage to undead (only has effect on a weapon)
		// 75 - Extra damage to demons (only has effect on a weapon)
		// 76 - Extra damage to giants (only has effect on a weapon)
		// 77 - Drains life, adds to wielder (only has effect on a melee weapon)
		// 78 - Extra damage to reptiles (only has effect on a weapon)
		// 79 - Extra damage to aliens (only has effect on a weapon)
		// 80 - Encumbers
		// 81 - Resist melee damage
		// 82 - Resist all damage
		// 83 - Affects chance to be hit in combat
		// 84 - Protects from petrification
		// ACTIVE ABILITIES
		// 100-119 - Casts mage spell 100 + x.
		// 120-139 - Casts priest spell 120 + x.
		// 150 - 199 - Affects status 150 + x.
		// 200 - heals damage
		// 201 - cures bad effects
		// 202 - Adds experience.
		// 203 - Adds skill points.
		// 204 - Adds spell energy.
		// 205 - Cleanses webs and disease.
		// 207 - Calls a scenario script state. Doesn't lose a charge when used. Items with this ability can't be carried between scenarios
		// 208 - Calls a scenario script state. Loses a charge when used. Items with this ability can't be carried between scenarios
		// 209 - Whole party resists mental effects
		// 210 - Increases light.
		// 211 - is a lockpick.
		// 212 - Is a First Aid Kit.
		// 213 - Gives whole party flight.
		// 214 - Afflicts user with bad effects.
		// 215 - Permanently change 1 statistic.
	short ability_strength[4]; // strength of ability, -50 to 50
		
	short special_class; // 0 - no class
	short value;
	short weight;
	short value_class; // Represents the item's value for random treasures
		// from 0 - 10, if -1, no class.
	
	// property flags
	unsigned char ident; // if 1, always ident
	unsigned char magic; // if 1, magic item
	unsigned char cursed; // if 1, cursed
	unsigned char once_per_day; // if 1, always ident
	unsigned char junk_item; // if 1, can delete this item to make room in town
	unsigned char extra[4];
	char full_name[30],name[20];
	short augment_item;
};

class item_type {
public:
	item_type();
	void clear_item_type();
	Boolean exists();
	void port();
	
	short which_item; 
		// -1 - null item
		// if 0 - 399, item from scenario list
		// if 1000 + x, is item x is party imported_items list
	location item_loc;
	unsigned char charges;
	unsigned char properties; 
		// bit 0 - ident   1 - property   2 - contained   3 - cursed  4 - used today
	location item_shift;
	
	static unsigned char identified_bit;
	static unsigned char property_bit;
	static unsigned char contained_bit;
	static unsigned char cursed_bit;
	static unsigned char usedToday_bit;
};


class creature_start_type {
public:
	creature_start_type();
	void clear_creature_start_type();
	Boolean exists();
	void port();
	
	short number; // numebr of creature in roster, -1 for none here
	short facing;
	short start_attitude;// 0 - PC, 1 - PC aid, 2 friendly, 3 - neutral, 4 - hostile A, 5 - hostile B
	location start_loc;
	short personality;
	short extra_item;
	short extra_item_chance_1;
	short extra_item_2;
	short extra_item_chance_2;
	short hidden_class;
	short character_id;
		// a number from 0-20000, a unique id for this character

	// timing variable
	short time_flag;
		// Indicates when this char appears. Types 0-9 are only there if
		// the town has not been destroyed.
		// 0 - always here (unless town has been destroyed)
		// 1 - appear at time t, unless town destroyed
		// 2 - disappear at time t
		// 3 - appear if event attached_event not done by time t
		// 4 - disappear if event attached_event not done by time t
		// 5 - appear if event attached_event done
		// 6 - disappear if event attached_event done
		// 7 - here on day 0-2 of every 9 days
		// 8 - here on day 3-5 of every 9 days
		// 9 - here on day 6-8 of every 9 days
		// 10 - here if and only if town has been destroyed
	short creature_time;
	short attached_event;
		// the event that this character is tied to (like a troglo to the
		// troglos killed event)
	short unique_char;
	
	// scripting
	char char_script[SCRIPT_NAME_LEN]; // the name of the default script
	short memory_cells[10];
	short act_at_distance;
	
	short extra[4];
};
	
class preset_field_type {
public:
	preset_field_type();
	void clear_preset_field_type();
	void port();
	
	location field_loc;
	short field_type;
};

// when a terrain spot has a script, the details for that script are remembered in
// this record. It a terrain spot has a script, it MUST has a corresponding
// record of this sort active
class in_town_on_ter_script_type {
public:
	in_town_on_ter_script_type();
	void clear_in_town_on_ter_script_type();
	void port();

	short exists;
	location loc; // location of the terrain spot this is for
	char script_name[SCRIPT_NAME_LEN]; 
	short memory_cells[10];
};


class town_record_type {
public:
	town_record_type();
	void clear_town_record_type();
	void set_start_locs(short town_size);
	void port();

	char town_name[20];
	short respawn_monsts[4][6];
	location respawn_locs[6];
	Rect special_rects[NUM_TOWN_PLACED_SPECIALS];
	unsigned char spec_id[NUM_TOWN_PLACED_SPECIALS];
	short lighting;

	// signs
	location sign_locs[15];
	char sign_text[15][256];

	location start_locs[4];
	location exit_locs[4];
	Rect in_town_rect;
	item_type preset_items[NUM_TOWN_PLACED_ITEMS];	
	preset_field_type preset_fields[NUM_TOWN_PLACED_FIELDS];

	short wall_1_sheet,wall_1_height,wall_2_sheet,wall_2_height,cliff_sheet;
		// how high to make walls and what sheets to look in to get wall graphics
	short beam_type;
		// sort of beams which might be present in level
		// 0 - never, 1 - damage, 2 - impass
	short environmental_sound; // sound to play in background
	short is_on_surface;
	short town_kill_day,town_linked_event;
		// town is cleaned out is event town_linked_event is not done by day town_kill_day
	short external_floor_type;
	short monster_respawn_chance;
	
	// scripts and specials
	char town_script[SCRIPT_NAME_LEN]; 
	in_town_on_ter_script_type ter_scripts[NUM_TER_SCRIPTS];
	
	Rect room_rect[NUM_TOWN_DESCRIPTION_AREAS];
	char info_rect_text[NUM_TOWN_DESCRIPTION_AREAS][30];
	creature_start_type creatures[NUM_TOWN_PLACED_CREATURES];
	short extra[20];
	
	location waypoints[NUM_WAYPOINTS];

	short exit_specs[4];
	short spec_on_entry,spec_on_entry_if_dead;
};

class big_tr_type {
public:
	big_tr_type();
	void clear_big_tr_type();
	void port();
	
	short terrain[64][64];
	unsigned char floor[64][64],height[64][64];
	unsigned char lighting[64][64];
};

class ave_tr_type {
public:
	ave_tr_type();
	void clear_ave_tr_type();
	void port();
	
	short terrain[48][48];
	unsigned char floor[48][48],height[48][48];
	unsigned char lighting[48][48];
};
	
class tiny_tr_type {
public:
	tiny_tr_type();
	void clear_tiny_tr_type();
	void port();
	
	short terrain[32][32];
	unsigned char floor[32][32],height[32][32];
	unsigned char lighting[32][32];
};	


typedef struct {
	short ter_type,item_num[10],item_odds[10],property;
} item_storage_shortcut_type;

class scen_item_data_type {
public:
	scen_item_data_type();
	void clear_scen_item_data_type();
	
	item_record_type scen_items[NUM_SCEN_ITEMS];
	floor_type_type scen_floors[256];
	terrain_type_type scen_terrains[512];
	creature_type scen_creatures[256];
};

//town i's name is stored in town_names[i]
//outdoor section i,j's name is stored in
// section_names[i+out_width*j]
class zone_names_data_type {
public:
	char town_names[200][20];
	char section_names[100][20];
	int out_width;
	int out_height;
	//width and height are stored seperately here as these data structures may 
	//be used to store information about scenarios from which zones are being imported
};

class boat_record_type {
public:
	boat_record_type();
	void clear_boat_record_type();
	void port();
	
	location boat_loc,boat_loc_in_sec,boat_sector;
	short which_town;
	Boolean exists,property;
};
class horse_record_type {
public:
	horse_record_type();
	void clear_horse_record_type();
	void port();
	
	location horse_loc,horse_loc_in_sec,horse_sector;
	short which_town;
	Boolean exists,property;
};
	
class scenario_data_type {
public:
	scenario_data_type();
	void clear_scenario_data_type();
	short scenario_platform();
	void port();
	
	unsigned char flag1, flag2, flag3, flag4;
	// for max, flags are 97 215 7 33
	// for win, flags are 199 61 2 53
	unsigned char ver[3],min_run_ver,prog_make_ver[3],num_towns;
	short out_width,out_height;
	short min_level,max_level;
	short rating;
		
	// labels when selecting scen
	char scen_name[50];
	short scen_label_pic;
	char scen_desc[256];
	
	// credits text
	char credits_text[2][256];
	char comments_text[2][256];
	
	// intro stuff, plays when scen started
	short intro_pic_resources[3];
	char intro_text[3][6][256];
	
	// town data
	unsigned char town_size[200]; // 0 - large, 1 - medium, 2 - small
	unsigned char town_starts_hidden[200];
	
	// starting loc
	short start_in_what_town;
	location what_start_loc_in_town;
	location what_outdoor_section_start_in;
	location start_where_in_outdoor_section;

	// changing town entrances
	short town_to_add_to[10];
	short flag_to_add_to_town[10][2];

	boat_record_type scen_boats[30];
	horse_record_type scen_horses[30];
	item_storage_shortcut_type storage_shortcuts[10];

	location last_out_edited;
	short last_town_edited;
};

// general script records

// Script stuff
#define MAX_DESCRIBER_LEN	30
#define	NUM_SCRIPT_STRINGS	750
#define	SCRIPT_VAR_NAME_LEN	20
#define	NUM_SCRIPT_INTS	20
#define	NUM_SCRIPT_LOCATIONS	10
#define	NUM_SCRIPT_STRING_VARS	20
#define	NUM_PROCEDURE_PASS_VARS	5

// a block of text, loaded in from a file.
class text_block_type
{
public:
	text_block_type();
	~text_block_type();
	Boolean load_text_file_into_buffer(char *file_name_to_load,short file_location);
	void preprocess_text();
	short estimate_num_of_tokens();
	
	char *text_block;
	long block_length;
};

// A token, a basic unit of meaning in a script (like a procedure name, or a number).
// The list of 20-odd different token types can be found in tokntype.h.
class token_type
{
public:
	token_type();
	
	unsigned char type;
	short line;
	short what_sort;
};

// A token, a basic unit of meaning in a script.
typedef struct
{
	char token_text[MAX_DESCRIBER_LEN];
} token_describer_type;

// Stores a loaded script, in tokenized form.
class script_type
{
public:
	script_type();
	~script_type();
	void flush_data();
	Boolean load_script(short type_of_script_to_load,const char *script_to_load,short file_location);
	Boolean IsWhiteSpace(char c);
	Boolean IsIdentifier(char c);
	Boolean IsNumber(char c);
	Boolean IsOperatorCharacter (char c);
	short IsVariable (char * string, int length, token_type *token);
	short IsBlockDefiner (char * string, int length, short *value);
	short IsVarDefiner (char * string, int length, short *value);
	short IsVarArrayDefiner (char * string, int length, short *value);
	short IsVarStringDefiner (char * string, int length, short *value);
	short IsNewVariableDefiner (char * string, int length, short *value);
	short IsFlowController (char * string, int length, short *value);
	short IsConstant (char * string, int length, short *value);
	short IsOperator (char * string, int length, short *value);
	short IsFunction (char * string, int length, short *value);
	short IsProcedure (char * string, int length, short *value);
	short IsBinaryFunction (char * string, int length, short *value);
	short IsTrinaryFunction (char * string, int length, short *value);
	short IsLocationFunction (char * string, int length, short *value);
	Boolean process_scenario_data();
	Boolean process_creature_data();
	Boolean token_type_match(short which_token,short type);
	Boolean token_sort_match(short which_token,short what_sort);
	Boolean token_match(short which_token,short type,short what_sort);
	short find_first_matching_token(short type_of_token,short what_sort = -1);
	short find_next_matching_token(short start_token,short type_of_token,short what_sort = -1);
	Boolean semicolon_check(short check_token);
	short find_start_of_state(short which_state);
	Boolean run_script(short next_token);
	Boolean evaluate_int_expression(short next_token,short *result,short *new_position);
	Boolean higher_precedence (token_type op1, token_type op2);
	short evaluate_unary_function(short fcn_type,short op1,short which_line);
	short evaluate_binary_function(short fcn_type,short op1,short op2,short which_line);
	Boolean run_procedure(short which_procedure,short which_line);
	short find_previous_matching_token(short start_token,short type_of_token,short what_sort);
	short find_matching_bracket(short start_token);
	short end_of_current_line(short start_token);
	short find_conditional_matching_bracket(short start_token);
	location location_function_value(short what_function,short what_passed,short what_line);
	Boolean run_creature_script();
	short evaluate_trinary_function(short fcn_type,short op1,short op2,short op3,short which_line);
	short IsNoParameterFunction (char * string, int length, short *value);
	short evaluate_no_param_function(short fcn_type,short which_line);
	Boolean run_zone_script();
	Boolean process_dialogue_data();
	
	// basic script information
	char script_name[20];
	short type_of_script;
		// -1 - undefined
		// 0 - Scenario data
		// 1 - Scenario script
		// 2 - Town - specials
		// 3 - Town - dialogue
		// 4 - Outdoor - specials
		// 5 - Outdoor - dialogue
		// 6 - Creature
		// 7 - Terrain Spot		
	token_type *token_list;
	short num_tokens;
	Boolean script_killed; // If the script hits a run-time error,
		// this makes sure it's never called again.
	short which_object; // The number of the creature/object this is for.
		// only relevant if type_of_script is 2,3.
			
	// script variables
	char int_var_names[NUM_SCRIPT_INTS][SCRIPT_VAR_NAME_LEN];
	short int_var_values[NUM_SCRIPT_INTS];
	char location_var_names[NUM_SCRIPT_LOCATIONS][SCRIPT_VAR_NAME_LEN];
	location location_var_values[NUM_SCRIPT_LOCATIONS];
	char string_var_names[NUM_SCRIPT_STRING_VARS][SCRIPT_VAR_NAME_LEN];
	char *string_var_values[NUM_SCRIPT_STRING_VARS];

	// temporary holder for the script text
	char *string_data[NUM_SCRIPT_STRINGS];
	
	// stacks for running procedures
	short procedure_passed_variable_types[NUM_PROCEDURE_PASS_VARS];
	//	0 - int 1 - loc 2 - string constant 3 - string variable
	short procedure_passed_values[NUM_PROCEDURE_PASS_VARS];
	location procedure_passed_locations[NUM_PROCEDURE_PASS_VARS];
};

// begin Blades of Exile records
// This junk only used for routines to port BoE scenarios.

typedef struct {
	short type,sd1,sd2,pic,m1,m2,ex1a,ex1b,ex2a,ex2b,jumpto;
} old_blades_special_node_type;

typedef struct {
	short personality,type;
	char link1[4],link2[4];
	short extras[4];
} old_blades_talking_node_type;

typedef struct {
	unsigned char strlens[200];
	old_blades_talking_node_type talk_nodes[60];
} old_blades_talking_record_type;

typedef struct {
	short picture;
	unsigned char blockage,flag1,flag2,special,trans_to_what,fly_over,boat_over;
	unsigned char block_horse,light_radius,step_sound,shortcut_key,res1,res2,res3;
} old_blades_terrain_type_type;
	
typedef	struct {
	unsigned char monst[4];
} old_blades_wandering_type;

typedef struct {
	unsigned char monst[7];
	unsigned char friendly[3];
	short spec_on_meet,spec_on_win,spec_on_flee,cant_flee;
	short end_spec1,end_spec2;
} old_blades_out_wandering_type;

typedef struct {
	unsigned char	terrain[48][48];
	location	special_locs[18];
	unsigned char		special_id[18];
	location	exit_locs[8];
	char		exit_dests[8];
	location	sign_locs[8];
	old_blades_out_wandering_type	wandering[4],special_enc[4];
	location	wandering_locs[4];
	Rect info_rect[8];
	unsigned char strlens[180];
	old_blades_special_node_type specials[60];
} old_blades_outdoor_record_type;

typedef struct {
	unsigned char number;
	unsigned char start_attitude;
	location start_loc;
	unsigned char  mobile;
	unsigned char  time_flag;
	unsigned char  extra1,extra2;
	short spec1, spec2;
	char spec_enc_code,time_code;
	short monster_time,personality;
	short special_on_kill,facial_pic;
} old_blades_creature_start_type;

typedef struct {
	short variety, item_level;
	char awkward, bonus, protection, charges, type;
	unsigned char graphic_num,ability, type_flag, is_special;
	short value;
	Boolean identified, magic;
	unsigned char weight, description_flag;
	char full_name[25], name[15];
	unsigned char reserved1,reserved2;
	unsigned char magic_use_type, ability_strength, treas_class, real_abil;
} old_blades_short_item_record_type;


typedef struct {
	short variety, item_level;
	char awkward, bonus, protection, charges, type, magic_use_type;
	unsigned char graphic_num,ability, ability_strength,type_flag, is_special;
	short value;
	unsigned char weight, special_class;
	location item_loc;
	char full_name[25], name[15];
	unsigned char treas_class,item_properties,reserved1,reserved2;
} old_blades_item_record_type;

typedef struct {
	location item_loc;
	short item_code,ability;
	unsigned char charges,always_there,property,contained;
} old_blades_preset_item_type;
	
typedef struct {
	location field_loc;
	short field_type;
} old_blades_preset_field_type;
	
typedef struct {
	short town_chop_time,town_chop_key;
	old_blades_wandering_type	wandering[4];
	location	wandering_locs[4];
	location	special_locs[50];
	unsigned char		spec_id[50];
	location	sign_locs[15];
	short		lighting;
	location start_locs[4];
	location exit_locs[4];
	short exit_specs[4];
	Rect in_town_rect;
	old_blades_preset_item_type preset_items[64];
	short max_num_monst;
	old_blades_preset_field_type preset_fields[50];
	short spec_on_entry,spec_on_entry_if_dead;
	short timer_spec_times[8];
	short timer_specs[8];
	unsigned char strlens[180];
	old_blades_special_node_type specials[100];
	unsigned char specials1,specials2,res1,res2;
	short difficulty;
} old_blades_town_record_type;

typedef struct {
	unsigned char terrain[64][64];
	Rect room_rect[16];
	old_blades_creature_start_type creatures[60];
	unsigned char lighting[8][64];
} old_blades_big_tr_type;

typedef struct {
	unsigned char terrain[48][48];
	Rect room_rect[16];
	old_blades_creature_start_type creatures[40];
	unsigned char lighting[6][48];
} old_blades_ave_tr_type;
	
typedef struct {
	unsigned char terrain[32][32];
	Rect room_rect[16];
	old_blades_creature_start_type creatures[30];
	unsigned char lighting[4][32];
} old_blades_tiny_tr_type;	

typedef struct {
	old_blades_item_record_type scen_items[400];
	char monst_names[256][20];
	char ter_names[256][30];
} old_blades_scen_item_data_type;

typedef struct {
	short ter_type,item_num[10],item_odds[10],property;
} old_blades_item_storage_shortcut_type;

typedef struct {
	unsigned char m_num,level,m_name[26];
	short health,m_health,mp,max_mp;
	unsigned char armor,skill;
	short a[3];
	unsigned char a1_type,a23_type,m_type,speed,ap,mu,cl,breath,breath_type,treasure,spec_skill,poison;
	short morale,m_morale;
	short corpse_item,corpse_item_chance;
	short status[15];
	unsigned char direction,immunities,x_width,y_width,radiate_1,radiate_2;
	unsigned char default_attitude,summon_type,default_facial_pic,res1,res2,res3;
	short picture_num;
	
} old_blades_monster_record_type;
	
typedef struct {
	short active,attitude;
	unsigned char number;
	location m_loc;
	old_blades_monster_record_type m_d;
	Boolean mobile;
	short summoned;
	old_blades_creature_start_type monst_start;
} old_blades_creature_data_type;	
	
typedef struct {
	location horse_loc,horse_loc_in_sec,horse_sector;
	short which_town;
	Boolean exists,property;
} old_blades_horse_record_type;

typedef struct {
	location boat_loc,boat_loc_in_sec,boat_sector;
	short which_town;
	Boolean exists,property;
} old_blades_boat_record_type;

typedef struct {
	unsigned char flag1, flag2, flag3, flag4;
	unsigned char ver[3],min_run_ver,prog_make_ver[3],num_towns;
	unsigned char out_width,out_height,difficulty,intro_pic,default_ground;
	} scen_header_type;
	
	typedef struct {
	unsigned char flag1, flag2, flag3, flag4;
	unsigned char ver[3],min_run_ver,prog_make_ver[3],num_towns;
	unsigned char out_width,out_height,difficulty,intro_pic,default_ground;
	unsigned char town_size[200];
	unsigned char town_hidden[200];
	short flag_a;
	short intro_mess_pic,intro_mess_len;
	location where_start,out_sec_start,out_start;
	short which_town_start;
	short flag_b;
	short town_data_size[200][5];
	short town_to_add_to[10];
	short flag_to_add_to_town[10][2];
	short flag_c;
	short out_data_size[100][2];
	Rect store_item_rects[3];
	short store_item_towns[3];
	short flag_e;
	short special_items[50];
	short special_item_special[50];
	short rating,uses_custom_graphics;
	short flag_f;
	old_blades_monster_record_type scen_monsters[256];
	//old_blades_boat_record_type scen_boats[30];
	//old_blades_horse_record_type scen_horses[30];
	boat_record_type scen_boats[30];
	horse_record_type scen_horses[30];
	short flag_g;
	old_blades_terrain_type_type ter_types[256];
	short scenario_timer_times[20];
	short scenario_timer_specs[20];
	short flag_h;
	old_blades_special_node_type scen_specials[256];
	old_blades_item_storage_shortcut_type storage_shortcuts[10];
	short flag_d;
	unsigned char scen_str_len[300];
	short flag_i;
	location last_out_edited;
	short last_town_edited;

} old_blades_scenario_data_type;

typedef struct {
	char town_strs[180][256];
	char out_strs[120][256];
	char scen_strs[270][256];
	char talk_strs[170][256];
} old_blades_piles_of_stuff_dumping_type;
	
// end BoE records

typedef struct {
	/*char strings_ls[NLS][40];
	char strings_rs[NRS][50];
	char town_strs[180][256];
	char out_strs[120][256];
	char short_talk_strs[NUM_TALK_NODES][120];
	char talk_strs[NUM_TALK_NODES * 2][256];*/
	scen_item_data_type scen_item_list;
} piles_of_stuff_dumping_type;

/*struct app_refs{
	FSRef textEditorRef;
	FSRef alintRef;
	FSRef dialogueEditorRef;
	FSRef graphicAdjusterRef;
	UInt8 textEditorPath[512];
	UInt8 alintPath[512];
	UInt8 dialogueEditorPath[512];
	UInt8 graphicAdjusterPath[512];
};*/

short get_ran (short times,short  min,short  max);
short s_pow(short x,short y);
short dist(location p1,location p2);

short max(short a,short b);
short min(short a,short b);
short minmax(short min,short max,short k);
Boolean same_point(location l1,location l2);
void EdSysBeep();

// Bl A Fileio
void open_Appl_resource( const char * rsrc_file );
bool init_directories( void );
void save_campaign();
void save_change_to_outdoor_size(short plus_north,short plus_west,short plus_south,short plus_east,short on_surface);
void save_change_to_town_size(int old_town_size);
void save_remove_town();
void load_campaign(FSSpec* file_to_load);
void augment_terrain(location to_create);
void load_outdoor_borders(location which_out);
void load_outdoors(location which_out,short mode);
void load_all_outdoor_names(FSSpec* to_open);
void load_town(short which_town);
void load_all_town_names(FSSpec* to_open);
void oops_error(short error);
void start_data_dump();
short str_to_num(Str255 str) ;
Boolean create_basic_scenario(char *scen_name_short,char *scen_name_with_ext,char *scen_full_name,short out_width,short out_height,short on_surface,Boolean use_warriors_grove);
Boolean import_boa_town();
Boolean import_boa_outdoors();
void EdSysBeep(short duration);
void get_name_of_current_scenario(char *name);
void extract_old_scen_text();
Boolean copy_script(const char *script_source_name,const char *script_dest_name);
void init_warriors_grove();
void import_blades_of_exile_scenario();
void port_boe_scenario_data();
void port_boe_out_data() ;
Boolean is_old_road(short i,short j);
Boolean is_old_wall(short ter);
void port_boe_town_data(short which_town,Boolean is_mac_scen) ;
void boe_port_talk_nodes();
void boe_port_town();
void boe_port_t_d();
void boe_port_scenario();
void boe_port_item_list();
void boe_port_out(old_blades_outdoor_record_type *out);
void boe_flip_spec_node(old_blades_special_node_type *spec);
void port_scenario_script(Str255 script_name,long directory_id);
void port_a_special_node(old_blades_special_node_type *node,short node_num,short file_id,short node_type);
void get_bl_str(char *str,short str_type,short str_num);
void add_short_string_to_file(short file_id,const char *str1,short num,const char *str2);
void add_big_string_to_file(short file_id,const char *str1,short num1,const char *str2,short num2,const char *str3,short num3,const char *str4);
void add_string_to_file(short file_id,const char *str);
void add_cr(short file_id);
void add_string(short file_id,const char *str);
void handle_messages(short file_id,short node_type,short message_1,short message_2);
void port_town_script(Str255 script_name,long directory_id,short which_town);
void trunc_str(char *str);
void port_outdoor_script(Str255 script_name,long directory_id,short sector_x,short sector_y);
void port_town_dialogue_script(Str255 script_name,long directory_id,short which_town);
void port_dialogue_intro_text(short *current_dialogue_node,short which_slot,short file_id,short town_being_ported);
void port_dialogue_node(short *current_dialogue_node,short which_slot,short file_id,short which_node,short town_being_ported);
void clean_str(char *str);
void open_current_scenario_resources();
void close_current_scenario_resources();
Boolean SelectSaveFileToOpen(FSSpecPtr defaultLocationfssPtr,FSSpec *file_picked, bool BoAScen);
void kludge_correct_old_bad_data();
bool get_user_pref_bool_value(int which, bool write_default_if_missing);
void write_user_pref_bool_value(int which, bool value);
bool get_should_play_sounds();
void write_should_play_sounds(bool play);
bool get_should_use_strict_adjusts();
void write_should_use_strict_adjusts(bool use);
bool get_always_show_heights();
void write_always_show_heights(bool show);
bool get_allow_arrow_key_navigation();
void write_allow_arrow_key_navigation(bool allow);
bool get_saved_window_bounds(unsigned int which, Rect& windRect);
void write_window_bounds(unsigned int which, const Rect& windRect);
short get_saved_tile_zoom_level();
void write_tile_zoom_level(short tile_zoom_level);
bool select_script_editor();
bool open_script_with_editor(const char* script_name);
bool get_editor_application();
char* get_scenario_filename();

// dlogtool
void cd_init_dialogs();
short cd_create_dialog_parent_num(short dlog_num,short parent);
short cd_create_dialog(short dlog_num,WindowPtr parent);
void process_new_window (WindowPtr hDlg);
void cd_set_edit_focus();
short cd_kill_dialog(short dlog_num,short parent_message);
short cd_process_click(WindowPtr window,Point the_point, short mods,short *item);
short cd_process_mousetrack( short dlog_num, short item_num, Rect theRect );
short cd_process_keystroke(WindowPtr window,char char_hit,short *item);
void cd_init_button(short dlog_num,short item_num, short button_num, short status);
void cd_attach_key(short dlog_num,short item_num,char key);
void cd_set_pict(short dlog_num, short item_num, short pict_num);
void cd_activate_item(short dlog_num, short item_num, short status);
short cd_get_active(short dlog_num, short item_num);
void cd_get_item_text(short dlog_num, short item_num, char *str);
void cd_retrieve_text_edit_str(short dlog_num,short item_num, char *str);
void cd_set_text_edit_str(short dlog_num, short item_num, const char *str);
void cd_set_item_text(short dlog_num, short item_num, const char *str);
void cd_set_item_num(short dlog_num, short item_num, short num);
void cd_set_led(short dlog_num,short item_num,short state);
void cd_set_flag(short dlog_num,short item_num,short flag);
short cd_get_led(short dlog_num,short item_num);
void cd_text_frame(short dlog_num,short item_num,short frame);
void cd_add_label(short dlog_num, short item_num, char *label, short label_flag);
void cd_take_label(short dlog_num, short item_num);
void cd_key_label(short dlog_num, short item_num,short loc);
void cd_draw_item(short dlog_num,short item_num);
void cd_initial_draw(short dlog_num);
void cd_draw(short dlog_num);
void cd_redraw(WindowPtr window);
void cd_frame_item(short dlog_num, short item_num, short width);
void cd_erase_item(short dlog_num, short item_num);
void cd_erase_rect(short dlog_num,Rect to_fry);
// void cd_press_button(short dlog_num, short item_num);
void cd_press_button(short dlog_num, short item_num, EDLGBtnRes mode );
short cd_get_indices(short dlg_num, short item_num, short *dlg_index, short *item_index);
short cd_get_dlg_index(short dlog_num);
short cd_find_dlog(WindowPtr window, short *dlg_num, short *dlg_key);
short cd_get_item_id(short dlg_num, short item_num);
void center_window(WindowPtr window);
Rect get_item_rect(WindowPtr hDlg, short item_num);
void frame_dlog_rect(GrafPtr hDlg, Rect rect, short val);
void draw_dialog_graphic(GrafPtr hDlg, Rect rect, short which_g, Boolean do_frame,short win_or_gworld);
Rect calc_rect(short i, short j);
void beep();
void cd_hit_led_range(short dlog_num,short first_led,short last_led,short which_to_set);
short cd_get_led_range(short dlog_num,short first_led,short last_led);
void cd_set_led_range(short dlog_num,short first_led,short last_led,short which_to_set);
void cd_set_text_edit_num(short dlog_num, short item_num, short num);
void cd_flip_led(short dlog_num,short item_num,short item_hit);
short cd_retrieve_text_edit_num(short dlog_num, short item_num);
Rect calc_from_rect(short i, short j);
Rect get_graphic_rect(GWorldPtr graf);

// EDFCns
void init_screen_locs();
void handle_action(Point the_point,EventRecord event, short which_window);
void handleToolPaletteClick(Point the_point, EventRecord event);
void handle_ter_spot_press(location spot_hit,Boolean option_hit,Boolean alt_hit,Boolean ctrl_hit);
void play_press_snd();
void swap_terrain();
void set_tool(short tool);
void set_new_terrain(short selected_terrain);
void set_new_floor(short selected_terrain);
void set_new_creature(short selected_creature);
void set_new_item(short selected_item);
void handle_keystroke(char chr,char chr2,EventRecord event);
bool handle_scroll( int map_size, int scrl, bool ctrlKey, bool shftKey );
Boolean is_hill(short i,short j);
Boolean is_rocks(short i,short j);
Boolean is_water(short i,short j);
void shy_change_circle_terrain(location center,short radius,short terrain_type,short probability);
void change_circle_terrain(location center,short radius,short terrain_type,short probability);
void change_circle_height(location center,short radius,short lower_or_raise,short probability);
void change_rect_terrain(Rect r,short terrain_type,short probability,Boolean hollow);
void set_terrain(location l,short terrain_type);
Boolean town_fix_grass_rocks(location l);
Boolean out_fix_grass_rocks(location l);
Boolean town_fix_rocks_water(location l);
Boolean out_fix_rocks_water(location l);
Boolean town_fix_hills(location l);
Boolean out_fix_hills(location l);
short get_corner_height(short x, short y,short out_or_town,short which_corner) ;
short adjust_get_ter(short x, short y,short out_or_town);
void adjust_space(location l);
short coord_to_ter(short x,short y);
void change_height(location l,short lower_or_raise);
void adjust_space_height(location l,short lower_or_raise);
void adjust_space_height_lower(location l);
void adjust_space_height_raise(location l);
void shut_down_menus();
Boolean save_check(short which_dlog);
void update_item_menu();
short locs_to_dir(location l1,location l2);
void delete_selected_instance();
void copy_selected_instance();
void cut_selected_instance();
void paste_selected_instance(location create_loc);
void check_selected_item_number();
location selected_instance_location();
void shift_selected_instance(short dx,short dy);
void rotate_selected_instance(int dir);
void setSelection(SelectionType::SelectionType_e type, unsigned short num, bool jumpTo=false);
void jumpToSelectedInstance();
void create_navpoint(location spot_hit);
void delete_navpoint(location spot_hit);
void frill_terrain();
void unfrill_terrain();
void create_new_creature(short c_to_create,location create_loc,creature_start_type *c_to_make);
Boolean create_new_item(short item_to_create,location create_loc,Boolean property,item_type *i_to_make);
void shift_item_locs(location spot_hit);
void place_items_in_town();
void create_town_entry(Rect rect_hit);
void edit_town_entry(location spot_hit);
void set_rect_height(Rect r);
void add_rect_height(Rect r);
void shy_put_terrain(short i,short j,short ter);
void transform_walls(Rect working_rect);
Boolean is_not_darkness_floor(short i,short j); 
void place_bounding_walls(Rect working_rect);
Boolean is_wall(short x, short y);
void clean_walls();
Boolean is_dumb_terrain(short ter);
short get_height(short x, short y,short out_or_town);
void set_all_items_containment();
void set_items_containment(int x, int y, int mode=0);
void set_up_lights();
Boolean old_can_see_in(location p1,location p2,short check_light,short check_travel);
void old_can_see(location p1,location p2,short check_light,short check_travel,Boolean *see_to,Boolean *see_in);
Boolean can_see_to(location p1,location p2,short check_light,short check_travel);
Boolean can_see_in(location p1,location p2,short check_light,short check_travel);
void can_see(location p1,location p2,short check_light,short check_travel,Boolean *see_to,Boolean *see_in);
Boolean can_see_single(location p1,location p2,short check_light,short check_travel,Boolean *see_to);
Boolean no_block(location l, short direction,short check_light,short check_travel);
Boolean look_block(location l, short direction);
Boolean move_block(location l, short direction);
void clear_selected_copied_objects();
void set_drawing_mode(short new_mode);
void reset_drawing_mode();
Boolean create_new_ter_script(const char* ter_script_name,location create_loc,in_town_on_ter_script_type* script_to_make);
void recursive_clean_terrain(location l);
void recursive_adjust_space_height_raise(location l);
void recursive_adjust_space_height_lower(location l);
Boolean get_see_in(short sector_offset_x, short sector_offset_y, short x, short y);
void set_see_in(short sector_offset_x, short sector_offset_y, short x, short y, Boolean value);
Boolean get_see_to(short sector_offset_x, short sector_offset_y, short x, short y);
void set_see_to(short sector_offset_x, short sector_offset_y, short x, short y, Boolean value);
short get_nw_corner(short sector_offset_x, short sector_offset_y, short x, short y);
void set_nw_corner(short sector_offset_x, short sector_offset_y, short x, short y, short value);
short get_sw_corner(short sector_offset_x, short sector_offset_y, short x, short y);
void set_sw_corner(short sector_offset_x, short sector_offset_y, short x, short y, short value);
short get_se_corner(short sector_offset_x, short sector_offset_y, short x, short y);
void set_se_corner(short sector_offset_x, short sector_offset_y, short x, short y, short value);
short get_ne_corner(short sector_offset_x, short sector_offset_y, short x, short y);
void set_ne_corner(short sector_offset_x, short sector_offset_y, short x, short y, short value);
void set_up_corner_and_sight_map();
void find_out_about_corner_walls(outdoor_record_type* drawing_terrain, short x, short y, short current_size, short *nw_corner, short *ne_corner, short *se_corner, short *sw_corner);
void find_out_about_corner_walls_being_hidden(outdoor_record_type *drawing_terrain, short sector_offset_x, short sector_offset_y, short x, short y, short current_size, Boolean see_in_neighbors[3][3], 
											  Boolean see_to_neighbors[3][3], /*Boolean see_to, */short *nw_corner, short *ne_corner, short *se_corner, short *sw_corner);
Boolean is_wall_drawn(outdoor_record_type *drawing_terrain, short sector_offset_x, short sector_offset_y, short x, short y);
Boolean clean_up_from_scrolling( int map_size, short dx, short dy );
void paste_terrain(location l,Boolean option_hit,Boolean alt_hit,Boolean ctrl_hit);
void copy_rect_terrain(Rect r);
int flood_fill_floor(short new_floor, short old_floor, int x, int y);
int flood_fill_terrain(short new_terrain, short old_terrain, int x, int y);
namespace Undo{ //forward decl
	class UndoStep;
}
void updateUndoMenu();
void pushUndoStep(Undo::UndoStep* s);
void purgeUndo();
void performUndo();
void purgeRedo();
void performRedo();

// EDGlobal.c
short get_ran (short times,short  min,short  max);
short s_pow(short x,short y);
short dist(location p1,location p2);
short max(short a,short b);
short min(short a,short b);
short minmax(short min,short max,short k);
Boolean same_point(location l1,location l2);

// EDParser
void init_scripts();
void load_town_script();
void load_tokens_for_initialized_script(script_type *script_data);
Boolean load_scenario_data(char *scenario_name);
Boolean set_char_variable(short which_char_type,short which_value,short new_value);
Boolean set_char_array_variable(short which_char_type,short which_member,short which_value,short new_value);
Boolean set_char_string(short which_char_type,short which_value,char *new_str);
Boolean set_floor_variable(short which_floor_type,short which_value,short new_value);
Boolean set_floor_string(short which_floor_type,short which_value,char *new_str);
Boolean set_terrain_variable(short which_ter_type,short which_value,short new_value);
Boolean set_terrain_string(short which_ter_type,short which_value,char *new_str);
Boolean set_item_variable(short which_item_type,short which_value,short new_value);
Boolean set_item_array_variable(short which_item_type,short which_member,short which_value,short new_value);
Boolean set_item_string(short which_item_type,short which_value,char *new_str);
short evaluate_operator(short operator_type,short op1,short op2);
short value_limit(short start_value,short min,short max,Boolean *error);
Boolean load_individual_scenario_data(char *scenario_name,Boolean force_reload = FALSE);
Boolean load_core_scenario_data();
Boolean check_script_exists(char *check_file_name,short file_location);

// EDUtils.c
void RedAlert_c (char *theStr);
void RedAlert (char *theStr);
void RedAlert_big_color (char *str1,char *str2,char *str3,char *str4,short num,char *str5,short color);
void RedAlert_big (char *str1,char *str2,char *str3,char *str4,short num,char *str5);
void display_error_dialog(StringPtr theStr,Boolean shut_down);
void ZeroRectCorner (Rect *theRect);
short rect_width (const Rect *theRect);
short rect_height (const Rect *theRect);
Boolean rects_touch(const Rect *r1,const Rect *r2);
short rect_dist(Rect *r1,Rect *r2);
Boolean rect_empty(Rect *r);
Rect rect_union(Rect *r1,Rect *r2);
Rect rect_sect(Rect *r1,Rect *r2);
Boolean r1_in_r2(Rect r1,Rect r2);
Rect rect_centered_around_point(Rect r, location l);
Boolean loc_in_active_area(location loc);
Boolean loc_in_rect(location loc,Rect r);
Boolean loc_touches_rect(location loc,Rect r);
terrain_type_type get_ter(short which_ter);
void ASB (const char *theStr);
void ASB_big (const char *str1,const char *str2,const char *str3,const char *str4,short num,const char *str5);
void ASB_big_color (const char *str1,const char *str2,const char *str3,const char *str4,short num,const char *str5,short dummy);
void CenterRectInRect (Rect *rectA, Rect *rectB);
int coord2Index(int coord, int base, int step);
Rect largestNonOverlapping(Rect bounds, const Rect* obstaclesBegin, const Rect* obstaclesEnd);

// Graphics.c
void Get_right_sbar_rect( Rect * rect );
int get_right_sbar_max();
void Set_up_win ();
void load_main_screen();
void recalculate_2D_view_details();
void recalculate_draw_distances();
void resize_recalculate_num_tiles();
void zoom_tiles_recalculate();
void set_up_terrain_rects();
void make_tile_gworlds(bool first_time=false);
bool set_view_mode(int mode);
void redraw_screen();
void draw_main_screen();
void set_up_terrain_buttons();
void reset_mode_number();
void set_up_view_buttons();
void delete_graphic(GWorldPtr *to_delete);
GWorldPtr load_pict(int picture_to_get);
Rect terrainViewRect();
Boolean place_terrain_icon_into_ter_large(graphic_id_type icon,short in_square_x,short in_square_y);
void draw_wall_3D_sidebar(short t_to_draw, Rect to_rect);
Boolean place_icon_into_3D_sidebar(graphic_id_type icon, Rect to_rect, short unscaled_offset_x, short unscaled_offset_y);
Boolean place_icon_into_ter_3D_large(graphic_id_type icon,short at_point_center_x,short at_point_center_y,Rect *to_whole_area_rect,short lighting,bool selected=false);
Boolean place_creature_icon_into_ter_3D_large(graphic_id_type icon,short at_point_center_x,short at_point_center_y,Rect *to_whole_area_rect,short lighting,short r,short g,short b,bool selected=false);
Boolean place_cliff_icon_into_ter_3D_large(short sheet,short at_point_center_x,short at_point_center_y,
	short direction,Rect *to_whole_area_rect,short lighting);//direction:   0: east/west 1: NW/SE 2:north/south
Boolean place_item_icon_into_ter_3D_large(graphic_id_type icon,short at_point_center_x,short at_point_center_y,Rect *to_whole_area_rect,short lighting,bool selected);
Boolean place_outdoor_creature_icon_into_ter_3D_large(graphic_id_type icon,short at_point_center_x,short at_point_center_y,Rect *to_whole_area_rect,short lighting);
Boolean place_corner_wall_icon_into_ter_3D_large(graphic_id_type icon,short at_point_center_x,short at_point_center_y,Boolean left_side_of_template,Rect *to_whole_area_rect,short lighting);
void place_ter_icon_on_tile_3D(short at_point_center_x,short at_point_center_y,short position,short which_icon,Rect *to_whole_area_rect,bool selected=false);
void draw_ter_script_3D(short at_point_center_x,short at_point_center_y,Rect *to_whole_area_rect,bool selected);
void place_ter_icons_3D(location which_outdoor_sector, outdoor_record_type *drawing_terrain, short square_x, short square_y, short t_to_draw, short floor_to_draw, short at_point_center_x,short at_point_center_y,Rect *to_whole_area_rect);
void draw_ter_icon_3D(short terrain_number,short icon_number,short x,short y,graphic_id_type a,short t_to_draw,Rect *to_whole_area_rect,short lighting,short height,bool selected=false);
void draw_terrain_3D(short t_to_draw, short x, short y, short sector_x, short sector_y, short at_point_center_x, short at_point_center_y, Boolean see_in_neighbors[3][3], Boolean is_wall_corner,Rect *to_whole_area_rect,short lighting,bool selected=false);
void draw_creature_3D(short creature_num,short at_point_center_x,short at_point_center_y, short square_x, short square_y,Rect *to_whole_area_rect,short lighting,bool selected=false);
void draw_item_3D(short item_num,short at_point_center_x,short at_point_center_y, short square_x, short square_y,Rect *to_whole_area_rect,short lighting,bool selected=false);
void put_line_segment_in_gworld_3D(GWorldPtr line_gworld,outdoor_record_type *drawing_terrain,short at_point_center_3D_x,short at_point_center_3D_y,
	short square_2D_x, short square_2D_y, short line_on_2D_x_side, short line_on_2D_y_side, Boolean corner_label_x,
	Boolean corner_label_y, short inset_3D_y, short offset_3D_y,short r,short g, short b,Rect *to_whole_area_rect);
void maybe_draw_part_of_3D_rect(outdoor_record_type *drawing_terrain, short center_of_current_square_x, short center_of_current_square_y, short x, short y,
		Rect rect, short inset, short r, short g, short b,Rect *to_whole_area_rect);
void draw_town_objects_3D(short x, short y, short at_point_center_x, short at_point_center_y,Rect *to_whole_area_rect,short lighting);
void draw_ter_3D_large();
void draw_ter_large();
Rect get_template_from_rect(short x,short y);
void draw_creature(short creature_num,location loc_drawn,short in_square_x,short in_square_y,bool selected=false);
void draw_item(short item_num,location loc_drawn,short in_square_x,short in_square_y,bool selected=false);
Boolean place_terrain_icon_into_ter_small(graphic_id_type icon,short in_square_x,short in_square_y);
void draw_ter_small();
void draw_terrain();
void update_terrain_window_title();
void place_left_text();
void rect_draw_some_item (GWorldPtr src_gworld,Rect src_rect,GWorldPtr targ_gworld,Rect targ_rect,
  char masked,short main_win);
void place_right_buttons(short mode);
void draw_view_buttons();
Boolean container_there(location l);
void char_win_draw_string(GrafPtr dest_window,Rect dest_rect,const char *str,short mode,short line_height);
void win_draw_string(GrafPtr dest_window,Rect dest_rect,Str255 str,short mode,short line_height);
void c2p(Str255 str) ;
void p2c(Str255 str);
void get_str(Str255 str,short i, short j);
short string_length(char *str);
Boolean clear_graphics_library();
Boolean refresh_graphics_library();
Boolean load_sheet_into_library(graphic_id_type *new_sheet);
short get_index_of_sheet(graphic_id_type *sheet);
short safe_get_index_of_sheet(graphic_id_type *sheet);
void load_builtin_images();
//void win_draw_string(CGrafPtr dest_window,Rect dest_rect,char *str,short mode,short line_height,short r, short g, short b);
void put_rect_in_gworld(GWorldPtr line_gworld,Rect rect,short r,short g, short b);
void fill_rect_in_gworld(GWorldPtr line_gworld,Rect to_rect,short r,short g, short b);
void put_line_in_gworld(GWorldPtr line_gworld,short from_x,short from_y,short to_x,short to_y,short r,short g, short b);
void put_rect_on_screen(WindowPtr win,Rect to_rect,short r,short g, short b);
void put_line_on_screen(short from_x,short from_y,short to_x,short to_y,short r,short g, short b);
void put_clipped_rect_on_screen(WindowPtr win,Rect to_rect,Rect clip_rect,short r,short g, short b);
void put_clipped_rect_in_gworld(GWorldPtr line_gworld,Rect to_rect,Rect clip_rect,short r,short g, short b);
void adjust_graphic(GWorldPtr *src_gworld_ptr, Rect *from_rect_ptr, short graphic_adjust/*,
		short light_level, Boolean has_border, short border_r, short border_g, short border_b*/);
void apply_lighting_to_graphic(GWorldPtr *src_gworld_ptr, Rect *from_rect_ptr, short lighting);
void add_border_to_graphic(GWorldPtr *src_gworld_ptr, Rect *from_rect_ptr, short border_r, short border_g, short border_b);
Boolean is_field_type(short i,short j,short field_type);
void make_field_type(short i,short j,short field_type);
void take_field_type(short i,short j,short field_type);
Boolean is_oblique_mirror(short i,short j);
void make_oblique_mirror(short i,short j);
void take_oblique_mirror(short i,short j);
Boolean is_facing_mirror(short i,short j);
void make_facing_mirror(short i,short j);
void take_facing_mirror(short i,short j);
Boolean is_web(short i,short j);
void make_web(short i,short j);
void take_web(short i,short j);
Boolean is_crate(short i,short j);
void make_crate(short i,short j);
void take_crate(short i,short j);
Boolean is_barrel(short i,short j);
void make_barrel(short i,short j);
void take_barrel(short i,short j);
Boolean is_fire_barrier(short i,short j);
void make_fire_barrier(short i,short j);
void take_fire_barrier(short i,short j);
Boolean is_force_barrier(short i,short j);
void make_force_barrier(short i,short j);
void take_force_barrier(short i,short j);
Boolean is_blocked(short i,short j);
void make_blocked(short i,short j);
void take_blocked(short i,short j);
Boolean is_sfx(short i,short j,short type);
void make_sfx(short i,short j,short type);
void take_sfx(short i,short j,short type);
void reset_small_drawn();
void win_draw_string_outline(CGrafPtr dest_window,Rect dest_rect,char *str,short mode,short line_height);
void place_ter_icon_on_tile(short tile_x,short tile_y,short position,short which_icon,bool selected=false);
void place_dlog_borders_around_rect(GWorldPtr to_gworld,WindowPtr win,
	Rect border_to_rect);
void place_dlog_border_on_win(GWorldPtr to_gworld,WindowPtr win,
	Rect border_to_rect,short horiz_or_vert,short bottom_or_top);
void paint_pattern(GWorldPtr dest,short which_mode,Rect dest_rect,short which_pattern);
void draw_ter_script(short script_num,location loc_drawn,short in_square_x,short in_square_y,bool selected);
void cant_draw_graphics_error(graphic_id_type a,const char *bonus_string,short bonus_num);
void refresh_graphics_on_screen();
void drawToolPalette();
void drawToolCategories();
void drawToolDetails();
OSStatus mainWindowTooltipContentCallback(WindowRef inWindow,Point inGlobalMouse,HMContentRequest inRequest,HMContentProvidedType *outContentProvided,HMHelpContentPtr ioHelpContent);
OSStatus paletteWindowTooltipContentCallback(WindowRef inWindow,Point inGlobalMouse,HMContentRequest inRequest,HMContentProvidedType *outContentProvided,HMHelpContentPtr ioHelpContent);
OSStatus tileWindowTooltipContentCallback(WindowRef inWindow,Point inGlobalMouse,HMContentRequest inRequest,HMContentProvidedType *outContentProvided,HMHelpContentPtr ioHelpContent);

// keydlgs
void fancy_choice_dialog_event_filter (short item_hit);
short fancy_choice_dialog(short which_dlog,short parent);
Boolean cre(short val,short min,short max, const char *text1, const char *text2,short parent_num) ;
void give_error(const char *text1, const char *text2,short parent_num);
void display_strings_event_filter (short item_hit);
void display_strings(const char *text1,const char *text2,const char *title,short sound_num,short graphic_num,short parent_num);
void choose_text_res_event_filter (short item_hit);
void put_text_res();
short choose_text_res(short res_list,short first_t,short last_t,short cur_choice,short parent_num,const char *title);
void edit_special_num_event_filter (short item_hit);
short edit_special_num(short mode,short what_start);
void how_many_dlog_event_filter (short item_hit);
short how_many_dlog(short what_start,short minimum,short maximum,const char *what_text);
void get_str_dlog_event_filter (short item_hit);
void get_str_dlog(char *start_str,const char *header_str,char *response,Boolean string_string = FALSE);
void make_cursor_sword() ;
void set_cursor(short which_c) ;
void restore_cursor();
short choice_dialog(short pic,short num);
Boolean string_not_clean(char *str,short max_length,short strict_file_naming,const char *beginning_of_error,short parent_num);

// Library
void flip_short(short *s);
void alter_rect(Rect *r);
void flip_rect(Rect *s);

// sound
void load_sounds ();
void play_sound(short which);
void force_play_sound(short which);
void one_sound(short which);
void clear_sound_memory();
void flip_sound();

// townout
void put_placed_monst_in_dlog();
Boolean get_placed_monst_in_dlog();
void edit_placed_monst_event_filter (short item_hit);
void edit_placed_monst(short which_m);
void put_placed_item_in_dlog();
Boolean get_placed_item_in_dlog();
void edit_placed_item_event_filter (short item_hit);
void edit_placed_item(short which_i);
void edit_sign_event_filter (short item_hit);
void edit_sign(short which_sign, bool handleUndo);
void get_a_number_event_filter (short item_hit);
short get_a_number(short which_dlog,short default_value,short min,short max);
void change_ter_event_filter (short item_hit);
void change_ter(short *change_from,short *change_to,short *chance);
void outdoor_details_event_filter (short item_hit);
void outdoor_details();
void put_out_wand_in_dlog();
Boolean get_out_wand_in_dlog();
void edit_out_wand_event_filter (short item_hit);
void edit_out_wand(short mode);
Boolean save_town_details();
void put_town_details_in_dlog();
void edit_town_details_event_filter (short item_hit);
void edit_town_details();
Boolean save_town_wand();
void put_town_wand_in_dlog();
void edit_town_wand_event_filter (short item_hit);
void edit_town_wand();
void pick_out_event_filter (short item_hit);
short pick_out(location default_loc,short max_x,short max_y);
void change_outdoor_size_event_filter (short item_hit);
Boolean change_outdoor_size();
void new_town_event_filter (short item_hit);
Boolean new_town();
void delete_town();
Boolean change_town_size();
void change_town_size_event_filter(short item_hit);
void pick_import_town_event_filter (short item_hit);
short pick_import_town(short which_dlog,short def);
Boolean terrains_match(short terrain_drawn,short terrain_there);
Boolean floors_match(short floor_drawn,short floor_there);
void edit_area_rect_event_filter (short item_hit);
Boolean edit_area_rect_str(short which_str,short mode);
Boolean save_out_strs();
void put_out_strs_in_dlog();
void edit_out_strs_event_filter (short item_hit);
void edit_out_strs();
Boolean save_town_strs();
void put_town_strs_in_dlog();
void edit_town_strs_event_filter (short item_hit);
void edit_town_strs();
void edit_item_placement();
void edit_item_placement_event_filter (short item_hit);
void put_item_placement_in_dlog();
Boolean save_item_placement();
void edit_add_town();
void edit_add_town_event_filter (short item_hit);
void put_add_town_in_dlog();
Boolean save_add_town();
void put_make_scen_1_in_dlog();
void edit_make_scen_1_event_filter (short item_hit);
short edit_make_scen_1(char *filename,char *title,short *start_on_surface);
void put_make_scen_2_in_dlog();
void edit_make_scen_2_event_filter (short item_hit);
short edit_make_scen_2(short *val_array);
void build_scenario();
Boolean save_scen_details();
void put_scen_details_in_dlog();
void edit_scen_details_event_filter (short item_hit);
void edit_scen_details();
void edit_scen_intro_event_filter (short item_hit);
void edit_scen_intro(short which_part);
void edit_scen_intro_pic_event_filter (short item_hit);
void edit_scen_intro_pic();
void pick_town_event_filter (short item_hit, int which_dlg);
void edit_item_properties_event_filter (short item_hit);
void edit_item_properties(short which_i);

#endif