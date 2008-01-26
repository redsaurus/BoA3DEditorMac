// Blades of Avernum Code

// Welcome to the Blades of Avernum Editor source code.

// I am providing this code because I suspect that some heavy users of the editor
// would like to customize it to their own tastes. Even if we at Spiderweb had
// the resources to place every desired feature in the editor, we would not
// be able to appeal to everyone's individual tastes. Thus the code. We hope
// that people will be able to make different flavors of editor to suit
// different sets of users.

// Before you get started, some things you should know.

// 1. This is just the editor code, not the game. You can't use this to add new calls
// or features to the game itself. Along these lines, you must make sure to preserve
// the data structures.

// 2. This is basically the same editor code, heavily modified, that I wrote 10 years
// ago. It is, in parts, clumsy, kludgy, and, in general,  written to my own personal tastes
// and style. The inevitable E-mails explaning to me how my coding would be better 
// if I did this or that will be read with interest and probably make no difference
// whatsoever. If I wrote this from scratch today, it would look much different.

// 3. Also, if I wrote this today, it would be commented much better than it is.
// I would like to comment this heavily, but I just don't have the time.

// Some files of great importance.

// global.h - This is where all of the data structures are stored.
// dlogool.c - This is the custom dialog engine. There are comments on this in
// that file.

//#include <Carbon/Carbon.h>	// included in pre-compiled header
//#include <Memory.h>
//#include <Quickdraw.h>
//#include <QuickdrawText.h>
//#include <Fonts.h>
//#include <Events.h>
//#include <Menus.h>
//#include <Windows.h>
//#include <TextEdit.h>
//#include <Dialogs.h>
//#include <OSUtils.h>
//#include <ToolUtils.h>
//#include <stdio.h>
//#include <QDOffscreen.h>
//#include <Palettes.h>

#include "global.h"

extern Rect terrain_rects[516]; //was 264

/* Globals */
Rect	windRect;
Boolean dialog_not_toast = FALSE;
WindowPtr	mainPtr;	
Boolean mouse_button_held = FALSE;
Boolean play_sounds = TRUE;
short cen_x, cen_y;
short ticks_to_wait = SPARSE_TICKS;

short mode_count = 0;
short geneva_font_num;

short old_depth = 16;
Boolean change_made_town = FALSE, change_made_outdoors = FALSE;
Boolean file_is_loaded = FALSE;

short max_zone_dim[3] = {64,48,32};

ControlHandle right_sbar;
static ControlActionUPP right_sbar_action_UPP;
pascal void right_sbar_action(ControlHandle bar, short part);
short store_control_value = 0;
bool use_strict_adjusts;
bool always_draw_heights;
ModalFilterUPP main_dialog_UPP;

// DATA TO EDIT
scenario_data_type scenario;
town_record_type town;
// big_tr_type t_d;
// outdoor_record_type current_terrain;
scen_item_data_type scen_data;
zone_names_data_type zone_names;
// short borders[4][50];
// unsigned char border_floor[4][50];
// unsigned char border_height[4][50];
// outdoor_record_type border_terrains[3][3];
short cur_town;
location cur_out;

Boolean small_any_drawn = FALSE;
Boolean using_osx;
 
// MAIN WHAT'S GOING ON VARIABLES
short current_drawing_mode = 0; // 0 - floor 1 - terrain 2 - height
short town_type = 0;  // 0 - big 1 - ave 2 - small
short current_height_mode = 0; // 0 - no autohills, 1 - autohills
Boolean editing_town = FALSE;
short cur_viewing_mode = 10; // 0 - big icons 1 - small icons 10 - big 3D icons 11 - 3D view as in game
short overall_mode = 0;
// 0 - 9 - different terrain painting modes
// 0 - neutral state, editing terrain/spaces with pencil
// 1 - large paintbrush
// 2 - small paintbrush
// 3 - large spray can
// 4 - small spray can
// 5 - paste terrain
// 6 - eyedropper
// 7 - paintbucket
//
// 10 - 29 - editing rectangle modes:
// 10 - frame rectangle
// 11 - fill rectangle
// 16 - place special enc
// 17 - town boundaries
// 18 - swap wall types
// 19 - add walls
// 20 - set height rectangle (absolute)
// 21 - place text rectangle
// 22 - outdoor only - town entrance
// 23 - copy terrain
// 24 - change height rectangle (delta)

// regular select modes
// 30 - place north entrance
// 31 - place west entrance 
// 32 - place south entrance 
// 33 - place east entrance

// 40 - select instance
// 41 - delete instance
// 46 - placing creature
// 47 - placing item
// 48 - pasting instance
// 49 - delete special enc
// 50 - set special enc
// 51 - GONE creating path
// 52 - GONE shift path point
// 53 - GONE insert path pt
// 54 - GONE delete path
// 55 - GONE toggle waypoint
// 56 - GONE delete waypoint
// 57 - place nav point
// 58 - delete nav point
// 59 - edit sign
// 60 - wandering monster pts
// 61 - blocked spot
// 62-66 - barrels, atc
// 67 - clean space
// 68 - place different floor stains
// 69 - edit town entrance
// 70 - place, edit terrain script
// 71 - place outdoor start point
// 72 - place town start point

// Numbers of current zone being edited

// file selection editing files
short selected_item_number = -1;
// codes for selected items
// 7000 + x - creature x
// 9000 + x - terrain script x
// 11000 + x - items x
item_type copied_item;
creature_start_type copied_creature;
in_town_on_ter_script_type copied_ter_script;

extern linkedList undo;
extern linkedList redo;

FSSpec default_directory;

extern bool setUpCreaturePalette;
extern bool setUpItemPalette;

/* Prototypes */
int main(void);
void Initialize(void);
void init_user_prefs();
void Set_Window_Drag_Bdry();
Boolean Handle_One_Event();
void Handle_Activate();
void Handle_Update( EventRecord * event );
void handle_menu_choice(long choice);
void handle_apple_menu(int item_hit);
void handle_file_menu(int item_hit);
void handle_campaign_menu(int item_hit);
void handle_town_menu(int item_hit);
void handle_outdoor_menu(int item_hit);
void handle_item_menu(int item_hit);
void handle_monst_menu(int item_hit);
void handle_edit_menu(int item_hit);
Boolean Mouse_Pressed( EventRecord * event );
void close_program();
 pascal Boolean cd_event_filter (DialogPtr hDlg, EventRecord *event, short *dummy_item_hit);
void set_pixel_depth();
void restore_depth();
//void find_quickdraw();
Boolean SWIsOSX( void );
void set_up_apple_events();
static pascal OSErr handle_open_app(const AppleEvent *theAppleEvent,AppleEvent *reply, long handlerRefcon);
static pascal OSErr handle_open_doc(const AppleEvent *theAppleEvent,AppleEvent *reply, long handlerRefcon);
static pascal OSErr handle_quit(const AppleEvent *theAppleEvent,AppleEvent *reply, long handlerRefcon);

//MW specified argument and return type.
int main(void)
{
	/*MaxApplZone();
	MoreMasters();*/
	using_osx = SWIsOSX();

	open_Appl_resource( "3D Blades of Avernum Editor.rsrc" );

	cd_init_dialogs();
	init_scripts();
	reset_small_drawn();
	init_screen_locs();
	
	cen_x = 24; cen_y = 24;
	
	right_sbar_action_UPP = NewControlActionUPP(right_sbar_action);
	Initialize();

	main_dialog_UPP = NewModalFilterUPP(cd_event_filter);
	load_sounds();
	
	set_up_apple_events();
	
	if ( !init_directories() )
		return -1;

	make_cursor_sword();
	Set_Window_Drag_Bdry();
	Set_up_win();

	init_warriors_grove();
	
	// this next line is for creating dummy data file
	//create_basic_scenario("basicscen",1,1,FALSE);

	Handle menu_bar_handle = GetNewMBar(128);
	if (menu_bar_handle == NIL) {
		SysBeep(50); SysBeep(50); SysBeep(50);
		ExitToShell();
	}
	SetMenuBar(menu_bar_handle);
	DisposeHandle(menu_bar_handle);
	init_user_prefs();
	//apple_menu = GetMenuHandle(500);
	//AppendResMenu(apple_menu, 'DRVR');
	shut_down_menus();
	DrawMenuBar();
	// Multifinder_Present = TRUE;
	//(NGetTrapAddress(_WaitNextEvent, ToolTrap) != 
		//NGetTrapAddress(_Unimplemented, ToolTrap));

	TextFace(bold);
	set_up_terrain_buttons();
	
	redraw_screen();

	while (Handle_One_Event() == FALSE);
	
	close_program();	
	return 0;
}

Boolean SWIsOSX( void )
{
	long	gestaltResponse;
	OSErr	err;
	
	err = Gestalt( gestaltSystemVersion, &gestaltResponse );
	return ( err == noErr && ((unsigned long)gestaltResponse >= 0x1000));
}

//	Initialize everything for the program, make sure we can run
void Initialize(void)
{
	Str255 title = "  ";
	/* Initialize all the needed managers. */
	//InitGraf(&qd.thePort);
	//InitFonts();
	//InitWindows();
	//InitMenus();
	//TEInit();
	//InitDialogs(nil);
	InitCursor();
	
	//	To make the Random sequences truly random, we need to make the seed start
	//	at a different number.  An easy way to do this is to put the current time
	//	and date into the seed.  Since it is always incrementing the starting seed
	//	will always be different.  DonÕt for each call of Random, or the sequence
	//	will no longer be random.  Only needed once, here in the init.
	//
	//GetDateTime((unsigned long*) &qd.randSeed);
	unsigned long randseed;
	GetDateTime(&randseed);
	SetQDGlobalsRandomSeed(randseed);

	//	Make a new window for drawing in, and it must be a color window.  
	//	The window is full screen size, made smaller to make it more visible.
	BitMap bitMap;
	GetQDGlobalsScreenBits(&bitMap);
	windRect = bitMap.bounds;
	
	//find_quickdraw();
	set_pixel_depth();
	
	//InsetRect(&windRect, 5, 34);
	InsetRect(&windRect,(windRect.right - 750) / 2,(windRect.bottom - 550) / 2);
	OffsetRect(&windRect,0,18);
	// add room for new scrollbar
	InsetRect(&windRect,-9,0);
	//mainPtr = NewCWindow(nil, &windRect, "\pBlades of Avernum Campaign Editor", true, documentProc, 
	//					(WindowPtr) -1, false, 0);
	//windRect = mainPtr->portRect;	
	
	WindowAttributes  windowAttrs;
	windowAttrs = kWindowCloseBoxAttribute | kWindowFullZoomAttribute | kWindowCollapseBoxAttribute;
	
	CreateNewWindow (kDocumentWindowClass, windowAttrs,&windRect, &mainPtr);
	
	ShowWindow (mainPtr);
	ZeroRectCorner(&windRect);
	
	SetPort(GetWindowPort(mainPtr));	/* set window to current graf port */
	
	Rect right_sbar_rect;
	Get_right_sbar_rect( &right_sbar_rect );
	right_sbar = NewControl(mainPtr,&right_sbar_rect,title,TRUE,0,0,0,scrollBarProc,1);
	
	undo = linkedList();
	redo = linkedList();
}

//initializes values according to user's preferences
//requires the menubar to be loaded
void init_user_prefs()
{
	play_sounds = get_should_play_sounds();
	CheckMenuItem (GetMenuHandle(570),9,play_sounds);
	use_strict_adjusts = get_should_use_strict_adjusts();
	CheckMenuItem (GetMenuHandle(570),10,use_strict_adjusts);
	always_draw_heights = get_always_show_heights();
	CheckMenuItem (GetMenuHandle(570),11,always_draw_heights);
}

void Set_Window_Drag_Bdry()
{
	BitMap bitMap;
	Rect Drag_Rect;

	//GetPortBounds(GetQDGlobalsThePort(), &Drag_Rect);
	GetQDGlobalsScreenBits(&bitMap);
	Drag_Rect = bitMap.bounds;

	//Drag_Rect = qd.screenBits.bounds;
	//Drag_Rect = (**(GrayRgn)).rgnBBox;
	Drag_Rect.left += DRAG_EDGE;
	Drag_Rect.right -= DRAG_EDGE;
	Drag_Rect.bottom -= DRAG_EDGE;
}

Boolean Handle_One_Event()
{
	short chr,chr2;
	long menu_choice;
	EventRecord event;
	Boolean All_Done = FALSE;
	//if(mouse_button_held)
	//	ticks_to_wait = SPARSE_TICKS;
	WaitNextEvent(everyEvent, &event, ticks_to_wait, MOUSE_REGION);	
	//printf("%d\n",event.what);
	//if(event.what == osEvt)
	//	fprintf(stdout,"   %i\n", event.message&1);
	// the order of event handler is recovered to original JV version
	if ((mouse_button_held == TRUE) && (event.what != 23) && (FrontWindow() == mainPtr)) {
		GlobalToLocal(&event.where);
		handle_action(event.where,event);
	}
	switch (event.what)
	{
		case keyDown: case autoKey:
				chr = event.message & charCodeMask;
				chr2 = (char) ((event.message & keyCodeMask) >> 8);
				if ((event.modifiers & cmdKey) != 0) {
					if (event.what != autoKey) {
						BringToFront(mainPtr);
						SetPort(GetWindowPort(mainPtr));
						if(chr>='1' && chr<=(editing_town ? '5' : '3')){ //TODO: remove this temporary hack
							set_drawing_mode(chr-'1');
							draw_main_screen();
						}
						menu_choice = MenuKey(chr);
						handle_menu_choice(menu_choice);
					}
				}
				else if (chr == 'Q')
					All_Done = TRUE;
				else 
					handle_keystroke(chr,chr2,event);
			break;
		case mouseDown:
			ticks_to_wait = 0L;
			All_Done = Mouse_Pressed( &event );
			break;
		case mouseUp:
			ticks_to_wait = SPARSE_TICKS;
			mouse_button_held = FALSE;
			lockLatestStep();
			break;
		case activateEvt:
			Handle_Activate();
			break;
		case updateEvt:
			Handle_Update( &event );
			break;
		case kHighLevelEvent: 
			AEProcessAppleEvent(&event);
			break;
		case osEvt:
			if(event.message & 1)
				ticks_to_wait = SPARSE_TICKS;
			else
				ticks_to_wait = VERY_SPARSE_TICKS;
			break;
	}
	return All_Done;
}

void Handle_Activate()
{
	restore_cursor();
}

void Handle_Update( EventRecord * event )
{
	WindowPtr the_window;
	GrafPtr		old_port;
	
	the_window = (WindowPtr) event->message;
	
	GetPort (&old_port);
	SetPort (GetWindowPort(the_window));
	
	BeginUpdate(the_window);
	
	redraw_screen();
	EndUpdate(the_window);
	
	SetPort(old_port);
	restore_cursor();
}

void handle_menu_choice(long choice)
{
	int menu,menu_item;

	if (choice != 0) {
		menu = HiWord(choice);
		menu_item = LoWord(choice);
		set_cursor(0);
		switch (menu) {
			case 500:
				handle_apple_menu(menu_item);
				break;
			case 550:
				handle_file_menu(menu_item);
				break;
			case 570:
				handle_edit_menu(menu_item);
				break;
			case 600: 
				handle_campaign_menu(menu_item);
				break;
			case 650:
				handle_town_menu(menu_item);
				break;
			case 651:
				handle_outdoor_menu(menu_item);
				break;
			case 700: case 701: case 702: case 703: case 704:
				handle_item_menu(menu_item + 100 * (menu - 700) - 1);
				break;
			case 750: case 751: case 752: case 753: 
				handle_monst_menu(menu_item + 64 * (menu - 750) - 1);
				break;
		}
	}
	HiliteMenu(0);
}

void handle_apple_menu(int item_hit)
{
	switch (item_hit) {
		case 1:
			fancy_choice_dialog(1062,0);
			break;
		default:
			//GetMenuItemText (apple_menu,item_hit,desk_acc_name);
			//desk_acc_num = OpenDeskAcc(desk_acc_name);
			break;
	}
}

void handle_file_menu(int item_hit)
{
	switch (item_hit) {
		case 1: // open
			if (change_made_town || change_made_outdoors) {
				if (save_check(858) == FALSE)
					break;							
			}
            FSSpec fileToOpen;
            if(SelectSaveFileToOpen(&default_directory,&fileToOpen,true)){
				setUpCreaturePalette = false;
				setUpItemPalette = false;
                load_campaign(&fileToOpen);
                if (file_is_loaded) {
                    update_item_menu();
                    purgeUndo();
                    purgeRedo();
                    redraw_screen();
                    shut_down_menus();
                }
            }
			break;
		case 2: // save
			//modify_lists();
			save_campaign();
			break;
		case 3: // new scen
			if ((file_is_loaded) &&  (change_made_town || change_made_outdoors)) {
				give_error("You need to save the changes made to your scenario before you can create a new scenario.","",0);
				return;
			}
			build_scenario();
			redraw_screen();
			break;
		case 4: // import blades
			if ((file_is_loaded) &&  (change_made_town || change_made_outdoors)) {
				give_error("You need to save the changes made to your scenario before you can import a Blades of Exile scenario.","",0);
				return;
			}
			if (fancy_choice_dialog(876,0) == 2)
				break;
			import_blades_of_exile_scenario();
			break;
		case 6: // quit
			if (save_check(869) == FALSE)
				break;
			ExitToShell();
			break;
	}
}

void handle_campaign_menu(int item_hit)
{
	switch (item_hit) {
		case 1: // edit town
			small_any_drawn = FALSE;
			cen_x = max_zone_dim[town_type] / 2; cen_y = max_zone_dim[town_type] / 2;
			current_drawing_mode = current_height_mode = 0;
			editing_town = TRUE;
			set_up_terrain_buttons();
			shut_down_menus();
			//set_string("Drawing mode","");
			DrawMenuBar();
			reset_drawing_mode();
			purgeUndo();
			purgeRedo();
			redraw_screen();
			break;
		case 2: // outdoor section
			small_any_drawn = FALSE;
			cen_x = 24; cen_y = 24;
			current_drawing_mode = current_height_mode = 0;
			editing_town = FALSE;
			set_up_terrain_buttons();
			//set_string("Drawing mode","");
			shut_down_menus();
			clear_selected_copied_objects();
			DrawMenuBar();
			reset_drawing_mode();
			purgeUndo();
			purgeRedo();
			redraw_screen();
			break;
		case 3: // new town
			if (change_made_town || change_made_outdoors) {
				give_error("You need to save the changes made to your scenario before you can add a new town.","",0);
				return;
			}
			if (scenario.num_towns >= 199) {
				give_error("You have reached the limit of 200 zones you can have in one campaign.","",0);
				return;
			}
			if(new_town()){
				reset_drawing_mode();
				cen_x = max_zone_dim[town_type] / 2; cen_y = max_zone_dim[town_type] / 2;
				purgeUndo();
				purgeRedo();
				change_made_town = TRUE;
			}
			redraw_screen();
			break;
		case 5: // basic scen data
			edit_scen_details();
			break;
		case 6: // label icon
			edit_scen_intro_pic();
			break;
		case 7: // intro text 1
			edit_scen_intro(0);
			break;
		case 8: // intro text 2
			edit_scen_intro(1);
			break;
		case 9: // intro text 3
			edit_scen_intro(2);
			break;
		case 12: // reload scen script
			if (fancy_choice_dialog(871,0) == 2)
				break;
			char file_name[256];
			get_name_of_current_scenario(file_name);
			if (load_individual_scenario_data(file_name,TRUE) == FALSE) {
				file_is_loaded = FALSE;
				return;
			}
			setUpCreaturePalette = false;
			setUpItemPalette = false;
			refresh_graphics_library();
			update_item_menu();
			set_up_terrain_buttons();
			place_right_buttons(0);
			redraw_screen();
			break;
		case 13: // Clean Up Walls
			if (fancy_choice_dialog(873,0) == 2)
				break;
			clean_walls(); 
			redraw_screen();
			change_made_town = TRUE;
			break;
		case 14: //   Import Town
			clear_selected_copied_objects();
			if(import_boa_town()){
				purgeUndo();
				purgeRedo();
				change_made_town = TRUE;
			}
			redraw_screen();
			break;
		case 15: //   Import Outdoor Section
			clear_selected_copied_objects();
			if(import_boa_outdoors()){
				purgeUndo();
				purgeRedo();
				change_made_outdoors = TRUE;
			}
			redraw_screen();
			break;
		case 16: //   Set Variable Town Entry
			edit_add_town();
			change_made_town = TRUE;
			break;
		case 17: //   Edit Item Placement Shortcuts
			edit_item_placement();
			change_made_town = TRUE;
			break;
		case 18: //   Delete Last Town
			if (change_made_town || change_made_outdoors) {
				give_error("You need to save the changes made to your scenario before you can delete a town.","",0);
				return;
			}
			if (scenario.num_towns == 1) {
				give_error("You can't delete the last town in a scenario. All scenarios must have at least 1 town.","",0);
				return;
			}
			if (scenario.num_towns - 1 == cur_town) {
				give_error("You can't delete the last town in a scenario while you're working on it. Load a different town, and try this again.","",0);
				return;
			}
			if (fancy_choice_dialog(865,0) == 1) {
				delete_last_town();
				clear_selected_copied_objects();
				redraw_screen();
				change_made_town = TRUE;
			}
			break;
		case 19: //   Write Scenario Data to Text File
			if (fancy_choice_dialog(866,0) == 1)
				start_data_dump();
			break;
		case 20: //   Change Outdoor Size
			if (change_made_town || change_made_outdoors) {
				give_error("You need to save the changes made to your scenario before you can change the outdoor size.","",0);
				return;
			}
			if(change_outdoor_size()) {
				purgeUndo();
				purgeRedo();
				reset_drawing_mode();
				redraw_screen();
			}
			break;
	}
}

void handle_town_menu(int item_hit)
{
	short i,x;	

	switch (item_hit) {
		case 1: // laod new town
			if (change_made_town) {
				if (save_check(859) == FALSE)
					break;							
			}
			x =  get_a_number(855,cur_town,0,scenario.num_towns - 1);
			if (x >= 0) {
				load_town(x);
				clear_selected_copied_objects();
				set_up_terrain_buttons();
				change_made_town = FALSE;
				cen_x = max_zone_dim[town_type] / 2; cen_y = max_zone_dim[town_type] / 2;
				reset_drawing_mode();
				reset_small_drawn();
				purgeUndo();
				purgeRedo();
				redraw_screen();
			}
			break;
		case 2: 
			edit_town_details(); 
			change_made_town = TRUE; 
			set_up_terrain_buttons(); 
			draw_main_screen(); 
			break; // Town Details
		case 3: 
			edit_town_wand(); 
			change_made_town = TRUE; 
			break; //Town Wandering Monsters
		case 4: // Set Town Boundaries							
			overall_mode = 17;
			mode_count = 2;
			set_cursor(5);
			set_string("Set town boundary","Select upper left corner");
			break;
		case 5: frill_terrain(); change_made_town = TRUE; break;
		case 6: unfrill_terrain(); change_made_town = TRUE; break;
		case 7:  //Edit Area Descriptions
			 edit_town_strs();
			 break;
			
		case 9: //Set Starting Location
			if (fancy_choice_dialog(867,0) == 2)
					break;
			set_string("Set Town Start Point","Where will party start?");
			overall_mode = 72;
			set_cursor(7);
			break;
		case 11: // add random
			if (fancy_choice_dialog(863,0) == 2)
				break;
			place_items_in_town();
			change_made_town = TRUE; 
			redraw_screen();
			break;
		case 12:
			set_all_items_contained();
			draw_terrain();
			change_made_town = TRUE; 
			break;
		case 13:  // set not prop
			for (i = 0; i < 144; i++)
				town.preset_items[i].properties = town.preset_items[i].properties & 253;
			fancy_choice_dialog(861,0);
			draw_terrain();
			change_made_town = TRUE; 
			break;
		case 14:  // clear all items
			if (fancy_choice_dialog(862,0) == 2)
				break;
			for (i = 0; i < 144; i++)
				town.preset_items[i].which_item = -1;
			draw_terrain();
			change_made_town = TRUE; 
			redraw_screen();
			break;
		case 17: //clear all creatures
			if (fancy_choice_dialog(878,0) == 2)
				break;
			for (i = 0; i < 80; i++)
				town.creatures[i].number = -1;
			draw_terrain();
			change_made_town = TRUE; 
				redraw_screen();
			break;
		case 18: // clear special encs
			if (fancy_choice_dialog(877,0) == 2)
				break;
			for (x = 0; x < NUM_TOWN_PLACED_SPECIALS; x++) {
				town.spec_id[x] = kNO_TOWN_SPECIALS;
				SetRect(&town.special_rects[x],-1,-1,-1,-1);
			}
			change_made_town = TRUE; 
			redraw_screen();
			break;
		case 19://change town size
			if (change_made_town || change_made_outdoors) {
				give_error("You need to save the changes made to your scenario before you can add a new town.","",0);
				return;
			}
			if(change_town_size()){
				purgeUndo();
				purgeRedo();
				change_made_town = TRUE;
			}
			redraw_screen();
			break;
	}
}

void handle_outdoor_menu(int item_hit)
{
	short x;
	switch (item_hit) {
		case 1: // load new
			if (change_made_outdoors == TRUE) {
				if (save_check(859) == FALSE)
					break;
			}
			x = pick_out(cur_out,scenario.out_width,scenario.out_height);
			if (x >= 0) {
				location spot_hit = {x / 100,x % 100};
				clear_selected_copied_objects();
				//load_outdoors(spot_hit,0);
				augment_terrain(spot_hit);
				set_up_terrain_buttons();
				cen_x = 24; cen_y = 24;
				reset_drawing_mode();
				purgeUndo();
				purgeRedo();
				redraw_screen();
				change_made_outdoors = FALSE;
			}
			break;
		case 2:  change_made_outdoors = TRUE; outdoor_details(); set_up_terrain_buttons(); draw_main_screen(); break; //Outdoor Details

		case 3: edit_out_wand(0); change_made_outdoors = TRUE; break;
		case 4: edit_out_wand(1);  change_made_outdoors = TRUE; break;
		case 5: edit_out_wand(2); change_made_outdoors = TRUE; draw_terrain(); break;
		
		case 6: frill_terrain(); change_made_outdoors = TRUE; break;
		case 7: unfrill_terrain(); change_made_outdoors = TRUE; break;
		case 8: edit_out_strs(); change_made_outdoors = TRUE; break; //Edit Area Descriptions
		case 10: 
			if (fancy_choice_dialog(864,0) == 2)
				return;
			set_string("Set Outdoor Start Point","Where will party start?");
			overall_mode = 71;
			set_cursor(7);
			break; //Set Starting Location
	}
}

void handle_edit_menu(int item_hit)
{
	switch (item_hit) {
		case 1: // undo
			performUndo();
			break;
		case 2: // redo
			performRedo();
			break;
		case 4: // cut
			cut_selected_instance();
			if(editing_town)
				change_made_town = TRUE;
			else
				change_made_outdoors = TRUE;
			break;
		case 5: // copy
			copy_selected_instance();
			if(editing_town)
				change_made_town = TRUE;
			else
				change_made_outdoors = TRUE;
			break;
		case 6: // paste
			if ((copied_creature.exists() == FALSE) && (copied_item.exists() == FALSE) && (copied_ter_script.exists == FALSE)) {
				beep();
				break;
			}
			set_string("Paste copied instance","Select location to place");
			set_cursor(7);
			overall_mode = 48;			
			break;
		case 7: // clear
			delete_selected_instance();
			if(editing_town)
				change_made_town = TRUE;
			else
				change_made_outdoors = TRUE;
			break;
		case 9: // toggle sound
			play_sounds = !play_sounds;
			CheckMenuItem (GetMenuHandle(570),9,play_sounds);
			write_should_play_sounds(play_sounds);
			play_sound(0);
			break;
		case 10: // toggle adjust usage
			use_strict_adjusts = !use_strict_adjusts;
			CheckMenuItem (GetMenuHandle(570),10,use_strict_adjusts);
			write_should_use_strict_adjusts(use_strict_adjusts);
			set_up_terrain_buttons();
			if (cur_viewing_mode == 1){
				small_any_drawn=FALSE;
				draw_ter_small();
			}
			else
				redraw_screen();
			break;
		case 11: // toggle height labelling
			always_draw_heights = !always_draw_heights;
			CheckMenuItem (GetMenuHandle(570),11,always_draw_heights);
			write_always_show_heights(always_draw_heights);
			break;
	}
	draw_main_screen();		
}

void handle_item_menu(int item_hit)
{
	//if (same_string("Unused",scen_data.scen_items[item_hit].full_name)) {
	//	give_error("You can't place an item named 'Unused'.","",0);
	//	return;
	//	}
	//overall_mode = 47;
	//mode_count = item_hit;
	//set_string("Place New Item","Select location to place");
	set_new_item(item_hit);
}

void handle_monst_menu(int item_hit)
{
	/*if (same_string("Unused",scen_data.scen_creatures[item_hit].name)) {
		give_error("You can't place an creature named 'Unused'.","",0);
		return;
	}
	overall_mode = 46;
	mode_count = item_hit;
	set_string("Place New Creature","Select location to place");*/
	set_new_creature(item_hit);
}

pascal void right_sbar_action(ControlHandle bar, short part)
{
	short old_setting,new_setting,max;
	
	if (part == 0)
		return;
	
	old_setting = GetControlValue(bar);
	new_setting = old_setting;
	max = GetControlMaximum(bar);
	
	switch (part) {
		case inUpButton: new_setting--; break;
		case inDownButton: new_setting++; break;
		case inPageUp: new_setting -= 21; break;
		case inPageDown: new_setting += 21; break;
	}
	new_setting = minmax(0,max,new_setting);
	SetControlValue(bar,new_setting);
	store_control_value = new_setting;
	if (new_setting != old_setting) {
		//set_up_terrain_buttons();
		place_right_buttons(0);
	}
}

Boolean Mouse_Pressed( EventRecord * event )
{
	WindowPtr	the_window;
	short	the_part,content_part;
	long menu_choice;
	ControlHandle control_hit;
	BitMap		screenbits;
	Boolean All_Done = FALSE;	

	the_part = FindWindow( event->where, &the_window);
	
	switch (the_part){
		case inMenuBar:
			menu_choice = MenuSelect(event->where);
			handle_menu_choice(menu_choice);
			break;
		case inSysWindow:
			break;
		case inDrag:
			GetQDGlobalsScreenBits(&screenbits);
			DragWindow(the_window, event->where, &screenbits.bounds);
			break;
		case inGoAway:
			All_Done = TRUE;
			break;
		case inContent:
			SetPort(GetWindowPort(mainPtr));
			GlobalToLocal(&(event->where));
			content_part = FindControl(event->where,the_window,&control_hit); // hit sbar?
			if (content_part != 0) {
				switch (content_part) {
					case inThumb:
						content_part = TrackControl(control_hit,event->where,NIL);
						if (control_hit == right_sbar){
							if (content_part == inThumb) {
								set_up_terrain_buttons();
								place_right_buttons(0);
							}
						}
						break;
					case inUpButton: case inPageUp: case inDownButton: case inPageDown:
						if (control_hit == right_sbar)
							content_part = TrackControl(control_hit,event->where,(ControlActionUPP)right_sbar_action_UPP);
						break;
				}
			} // a control hit
			else  // ordinary click
				handle_action(event->where, *event);
			break;
	}
	return All_Done;
}

void close_program()
{
	close_current_scenario_resources();
	restore_depth();
}
 
pascal Boolean cd_event_filter (DialogPtr hDlg, EventRecord *theEvent, short *dummy_item_hit)
{	
	char chr,chr2;
	short wind_hit,item_hit;
	Point the_point;
	dummy_item_hit = 0;
	
	switch (theEvent->what) {
		case updateEvt:
			BeginUpdate((WindowPtr) hDlg);
			cd_redraw((WindowPtr) hDlg);
			EndUpdate((WindowPtr) hDlg);
			DrawDialog(hDlg);
			return FALSE;
			break;
		case keyDown:
			chr = theEvent->message & charCodeMask;
			chr2 = (char) ((theEvent->message & keyCodeMask) >> 8);
			switch (chr2) {
				case 126: chr = 22; break;
				case 124: chr = 21; break;
				case 123: chr = 20; break;
				case 125: chr = 23; break;
				case 53: chr = 24; break;
				case 36: chr = 31; break;
				case 76: chr = 31; break;
			}
			// specials ... 20 - <-  21 - ->  22 up  23 down  24 esc
			// 25-30  ctrl 1-6  31 - return

			wind_hit = cd_process_keystroke((WindowPtr) hDlg,chr,&item_hit);
			break;
	
		case mouseDown:
			the_point = theEvent->where;
			GlobalToLocal(&the_point);	
			wind_hit = cd_process_click((WindowPtr) hDlg,the_point, theEvent->modifiers,&item_hit);
		break;

		default: 
			wind_hit = -1; 
			break;
	}
	switch (wind_hit) {
		case -1: break;
		case 970: case 971: case 972: case 973: display_strings_event_filter(item_hit); break;
		case 857: change_ter_event_filter(item_hit); break;
		case 800: edit_make_scen_1_event_filter(item_hit); break;
		case 801: edit_make_scen_2_event_filter(item_hit); break;
		case 803: edit_scen_details_event_filter(item_hit); break;
		case 804: edit_scen_intro_event_filter(item_hit); break;
		case 805: edit_scen_intro_pic_event_filter(item_hit); break;
		case 810: edit_add_town_event_filter(item_hit); break;
		case 812: edit_item_placement_event_filter(item_hit); break;
		case 820: choose_text_res_event_filter(item_hit); break;

		case 825: edit_special_num_event_filter(item_hit); break;
		case 828: how_many_dlog_event_filter(item_hit); break;
		case 829: get_str_dlog_event_filter(item_hit); break;
		case 830: new_town_event_filter(item_hit); break;
		case 831: change_town_size_event_filter(item_hit); break;
		case 832: edit_town_details_event_filter(item_hit); break;
		case 835: edit_town_wand_event_filter(item_hit); break;
		case 837: edit_placed_monst_event_filter(item_hit); break;
		case 838: edit_item_properties_event_filter(item_hit); break;
		case 839: edit_town_strs_event_filter(item_hit); break;
		case 840: edit_area_rect_event_filter(item_hit); break;
		case 841: pick_import_town_event_filter(item_hit); break;
		case 850: edit_out_strs_event_filter(item_hit); break;
		case 851: outdoor_details_event_filter(item_hit); break;
		case 852: edit_out_wand_event_filter(item_hit); break;
		case 854: pick_out_event_filter(item_hit); break;
		case 842: case 855: case 856: pick_town_event_filter(item_hit,wind_hit); break;
		case 872: case 880: get_a_number_event_filter(item_hit); break;
		
		case 991: change_outdoor_size_event_filter(item_hit); break;
		
		default: fancy_choice_dialog_event_filter (item_hit); break;
	}
	if (wind_hit == -1)
		return FALSE;
	return TRUE;
}

void set_pixel_depth() {
	GDHandle cur_device;
	PixMapHandle screen_pixmap_handle;
	OSErr err;
	short choice;
	static Boolean diff_depth_ok = FALSE;
	short pixel_depth;

	cur_device = GetGDevice();	
	
	if ((diff_depth_ok == FALSE) && ((HasDepth(cur_device,16,1,1)) == 0)) {
		choice_dialog(0,1070);
		ExitToShell();
	}
	
	screen_pixmap_handle = (**(cur_device)).gdPMap;
	pixel_depth = (**(screen_pixmap_handle)).pixelSize;
	
	if ((pixel_depth <= 16) && (diff_depth_ok == TRUE))
		return;
	
	if (pixel_depth < 16) {
		choice = choice_dialog(0,1071);
		if (choice == 3)
			diff_depth_ok = TRUE;
		if (choice == 2)
			ExitToShell();
		if (choice == 1) {
			err = SetDepth(cur_device,16,1,1);
			old_depth = pixel_depth;
		}
	}
}

void restore_depth()
{
	GDHandle cur_device;
	PixMapHandle screen_pixmap_handle;
	OSErr err;
	cur_device = GetGDevice();	
	screen_pixmap_handle = (**(cur_device)).gdPMap;
	if (old_depth != 16)
		err = SetDepth(cur_device,old_depth,1,1);
}

void set_up_apple_events()
{
	OSErr myErr;

	AEInstallEventHandler(kCoreEventClass, kAEOpenApplication, NewAEEventHandlerUPP(&handle_open_app), 0, false);
	AEInstallEventHandler(kCoreEventClass, kAEOpenDocuments, NewAEEventHandlerUPP(&handle_open_doc), 0, false);
	myErr = AEInstallEventHandler(kCoreEventClass, kAEQuitApplication, NewAEEventHandlerUPP(&handle_quit), 0, false);
    if (myErr != noErr) ExitToShell();
}

static pascal OSErr handle_open_app(const AppleEvent *theAppleEvent,AppleEvent *reply, long handlerRefcon)
//AppleEvent *theAppleEvent,*reply;
//long handlerRefcon;
{
	return noErr;
}

static pascal OSErr handle_open_doc(const AppleEvent *theAppleEvent,AppleEvent *reply, long handlerRefcon)
//AppleEvent *theAppleEvent,*reply;
//long handlerRefcon;
{
	AEDescList docList;
    FSRef theFSRef;
	FSSpec theFSSpec;
	FSCatalogInfo fileInfo;
	FInfo fileInfo2;
    long index;
    long count = 0;
    OSErr err = AEGetParamDesc(theAppleEvent, keyDirectObject, typeAEList, &docList);
		
	err = AECountItems(&docList, &count);
			
	for(index = 1; index <= count; index++){
		err = AEGetNthPtr(&docList, index, typeFSRef, NULL, NULL, &theFSRef, sizeof(FSRef), NULL);
		
		FSGetCatalogInfo(&theFSRef,kFSCatInfoNone,&fileInfo,NULL,&theFSSpec,NULL);
		FSpGetFInfo(&theFSSpec,&fileInfo2);
		if(fileInfo2.fdType!='BoAX'){
			char name[255];
			p2cstrcpy(name,theFSSpec.name);
			char* extension=strrchr((char*)name,'.');
			if(extension==NULL)
				return(false);
			if(strcmp(".bas",extension)!=0){
				return errAEEventNotHandled;
			}
		}
        if (change_made_town || change_made_outdoors) {
            if (save_check(858) == FALSE)
                return(errAEEventNotHandled);							
        }
		load_campaign(&theFSSpec);
        if (file_is_loaded) {
			setUpCreaturePalette = false;
			setUpItemPalette = false;
            update_item_menu();
            purgeUndo();
            purgeRedo();
            redraw_screen();
            shut_down_menus();
        }
	}
	AEDisposeDesc(&docList);
	return(err);
}

static pascal OSErr handle_quit(const AppleEvent *theAppleEvent,AppleEvent *reply, long handlerRefcon)
{
	short choice;

	if (change_made_town == FALSE && change_made_outdoors == FALSE) {
		ExitToShell();     
   		return noErr;
	}
	choice = fancy_choice_dialog(869,0);
	if (choice == 3) {
		ExitToShell();     
		return noErr;
	}
	if (choice == 2) {
		return userCanceledErr;
	}
	save_campaign();
	ExitToShell();     
	return noErr;
}