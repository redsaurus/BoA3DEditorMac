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

#include <Sparkle/SUCarbonAPI.h>
#include "global.h"
#include "Undo.h"

extern Rect terrain_rects[516]; //was 264

//tridash - moving stuff to panels
WindowPtr palettePtr;
WindowPtr tilesPtr;
/* Globals */
Rect	windRect;
//tridash - moving stuff to panels
Rect	paletteRect;
Rect	tilesRect;
Boolean dialog_not_toast = FALSE;
WindowPtr	mainPtr;
EventHandlerRef scrollHandler;
EventHandlerRef scrollHandler2;
EventHandlerRef scrollHandler3;
Boolean scroll_dialog_lock = FALSE;
Boolean mouse_button_held = FALSE;
bool delimit_undo_on_mouse_up = false;
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
ControlHandle tiles_zoom_slider;
static ControlActionUPP right_sbar_action_UPP;
pascal void right_sbar_action(ControlHandle bar, short part);
short store_control_value = 0;
bool use_strict_adjusts;
bool always_draw_heights;
bool allow_arrow_key_navigation;
ModalFilterUPP main_dialog_UPP;

// DATA TO EDIT
scenario_data_type scenario;
town_record_type town;
extern outdoor_record_type current_terrain;
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
short previous_tool = 0;
short tile_zoom_level = 1;
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
// 25 - redraw special enc
// 26 - redraw town entrance
// 27 - redraw area description

// regular select modes
// 30 - place north entrance
// 31 - place west entrance 
// 32 - place south entrance 
// 33 - place east entrance

// 40 - select instance
// 41 - delete instance
// 45 - change terrain randomly
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
// 62-66 - barrels, etc.
// 67 - clean space
// 68 - place different floor stains (No longer used see 75-82)
// 69 - edit town entrance
// 70 - place, edit terrain script
// 71 - place outdoor start point
// 72 - place town start point
// 73 - place NE/SW mirror
// 74 - place NW/SE mirror
// 75 - place small blood stain
// 76 - place med. blood stain
// 77 - place large blood stain
// 78 - place small slime pool
// 79 - place large slime pool
// 80 - place dried blood
// 81 - place bones
// 82 - place rocks

// Numbers of current zone being edited

// file selection editing files
SelectionType::SelectionType_e selected_object_type = SelectionType::None;
unsigned short selected_object_number;

item_type copied_item;
creature_start_type copied_creature;
in_town_on_ter_script_type copied_ter_script;

FSSpec default_directory;

extern bool setUpCreaturePalette;
extern bool setUpItemPalette;
extern Rect terrain_buttons_rect;
extern Rect mode_buttons_rect;
extern Rect terrain_viewport_3d;

extern unsigned int terrain_width_2d;
extern unsigned int terrain_height_2d;
extern int TER_RECT_UL_X_2d_big;
extern int TER_RECT_UL_Y_2d_big;

extern Rect kRect3DEditScrn;
Rect tiles_zoom_slider_rect;

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
void doScroll(ControlHandle bar, short delta);
OSStatus paletteScrollHandler(EventHandlerCallRef eventHandlerCallRef,EventRef eventRef, void* userData);
OSStatus paletteScrollHandler2(EventHandlerCallRef eventHandlerCallRef,EventRef eventRef, void* userData);
OSStatus viewScrollHandler(EventHandlerCallRef eventHandlerCallRef,EventRef eventRef, void* userData);
void tiles_zoom_slider_change(short zoom_slider);

extern GWorldPtr ter_draw_gworld;
extern Rect terrain_rect_gr_size;

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
	Set_Window_Drag_Bdry();//TODO: this doesn't seem to actually do anything?
	
	const EventTypeSpec eventList3[] = {
	//{ kEventClassMouse, kEventMouseDown }, 
	{ kEventClassMouse, kEventMouseWheelMoved},
	//{ kEventClassKeyboard, kEventRawKeyDown },
	//{ kEventClassKeyboard, kEventRawKeyRepeat }
	};
	const EventTypeSpec eventList4[] = {
		{ kEventClassMouse, kEventMouseScroll}
	};
	
	InstallWindowEventHandler(tilesPtr,NewEventHandlerUPP((EventHandlerProcPtr) paletteScrollHandler2),GetEventTypeCount(eventList3),eventList3,0,&scrollHandler3);
	InstallWindowEventHandler(mainPtr,NewEventHandlerUPP((EventHandlerProcPtr) paletteScrollHandler),GetEventTypeCount(eventList3),eventList3,0,&scrollHandler);
	InstallWindowEventHandler(mainPtr,NewEventHandlerUPP((EventHandlerProcPtr) viewScrollHandler),GetEventTypeCount(eventList4),eventList4,0,&scrollHandler2);

	init_warriors_grove();
	
	// this next line is for creating dummy data file
	//create_basic_scenario("basicscen",1,1,FALSE);

	Handle menu_bar_handle = GetNewMBar(128);
	if (menu_bar_handle == NIL) {
		printf("Failed to locate main menubar; exiting\n");
		ExitToShell();
	}
	SetMenuBar(menu_bar_handle);
	DisposeHandle(menu_bar_handle);
	
	SUSparkleInitializeForCarbon();
	init_user_prefs();
	Set_up_win();
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

static pascal void ControlUserPaneBackgroundProc (ControlHandle control, ControlBackgroundPtr)
{
	PixPatHandle background = GetPixPat (142);
	BackPixPat(background);
//	printf("setting colour background!\n");
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
	//	will always be different.  Don‚Äôt for each call of Random, or the sequence
	//	will no longer be random.  Only needed once, here in the init.
	//
	unsigned long randseed=(unsigned long)CFAbsoluteTimeGetCurrent();
	srandom(randseed);

	//	Make a new window for drawing in, and it must be a color window.  
	//	The window is full screen size, made smaller to make it more visible.
	//	Don't want dock / top menubar to be included in the space, so use
	//	GetAvailableWindowPositioningBounds rather than bitMap.bounds
	
//	BitMap bitMap;
//	GetQDGlobalsScreenBits(&bitMap);
//	windRect = bitMap.bounds;
	
	Rect activeScreenRect;
	GetAvailableWindowPositioningBounds(NULL, &activeScreenRect);
	windRect = activeScreenRect;	
	
	//find_quickdraw();
	set_pixel_depth();
	
	//InsetRect(&windRect, 5, 34);
	InsetRect(&windRect,((windRect.right - windRect.left) - DEFAULT_WINDOW_WIDTH) / 2,((windRect.bottom - windRect.top) - DEFAULT_WINDOW_HEIGHT) / 2); //tridash - larger window stuff
	OffsetRect(&windRect,0,18);
	// add room for new scrollbar
	InsetRect(&windRect,-9,0);
	//mainPtr = NewCWindow(nil, &windRect, "\pBlades of Avernum Campaign Editor", true, documentProc, 
	//					(WindowPtr) -1, false, 0);
	//windRect = mainPtr->portRect;	
	
	WindowAttributes  windowAttrs;
	windowAttrs = kWindowCloseBoxAttribute | kWindowFullZoomAttribute | kWindowCollapseBoxAttribute | kWindowResizableAttribute;
	
	OffsetRect(&windRect, 0, 0);
	get_saved_window_bounds(0, windRect);

	CreateNewWindow (kDocumentWindowClass, windowAttrs,&windRect, &mainPtr);
	
	ShowWindow (mainPtr);
	ZeroRectCorner(&windRect);
	terrain_viewport_3d.bottom = windRect.bottom - 40;
	terrain_viewport_3d.right = windRect.right - 40;
	kRect3DEditScrn.bottom = windRect.bottom - 20;
	kRect3DEditScrn.right = windRect.right - 20;
	terrain_rect_gr_size=terrain_viewport_3d;
	
	//tridash - moving stuff to panels
	
	paletteRect = activeScreenRect;
	windowAttrs = kWindowFullZoomAttribute | kWindowNoShadowAttribute;
	InsetRect(&paletteRect,((paletteRect.right - paletteRect.left) - TOOL_PALETTE_WIDTH) / 2,((paletteRect.bottom - paletteRect.top) - TOOL_PALETTE_HEIGHT) / 2);
	OffsetRect(&paletteRect, activeScreenRect.right-paletteRect.right, activeScreenRect.top + 16 - paletteRect.top); //TODO: get rid of the magic 16 (window title bar height)
	
	tilesRect = activeScreenRect;
	InsetRect(&tilesRect,((tilesRect.right - tilesRect.left) - TILES_WINDOW_WIDTH) / 2, (paletteRect.bottom + 16 - tilesRect.top) / 2); //tridash - larger window stuff
	OffsetRect(&tilesRect, activeScreenRect.right-tilesRect.right, activeScreenRect.bottom-tilesRect.bottom);

	get_saved_window_bounds(1, paletteRect);
	CreateNewWindow (kFloatingWindowClass, windowAttrs, &paletteRect, &palettePtr);
	
	SetWindowTitleWithCFString(palettePtr, CFSTR("Tools"));
	
	windowAttrs = kWindowFullZoomAttribute | kWindowNoShadowAttribute | kWindowResizableAttribute;
	
	get_saved_window_bounds(2, tilesRect);
	CreateNewWindow (kFloatingWindowClass, windowAttrs, &tilesRect, &tilesPtr);
	ZeroRectCorner(&tilesRect);
	terrain_buttons_rect.bottom=tilesRect.bottom-tilesRect.top-11;
	terrain_buttons_rect.right=tilesRect.right-tilesRect.left-RIGHT_SCROLLBAR_WIDTH;
	mode_buttons_rect.right=tilesRect.right-tilesRect.left-RIGHT_SCROLLBAR_WIDTH;
	resize_recalculate_num_tiles();
	
	SetWindowTitleWithCFString(tilesPtr, CFSTR("Tiles"));
	
	HMInstallWindowContentCallback(mainPtr,NewHMWindowContentUPP(mainWindowTooltipContentCallback));
	HMInstallWindowContentCallback(palettePtr,NewHMWindowContentUPP(paletteWindowTooltipContentCallback));
	HMInstallWindowContentCallback(tilesPtr,NewHMWindowContentUPP(tileWindowTooltipContentCallback));
	
	ShowWindow(palettePtr);
	ShowWindow(tilesPtr);
	
	ZeroRectCorner(&paletteRect);
	ZeroRectCorner(&tilesRect);
	
	SetPort(GetWindowPort(tilesPtr));
	
	Rect right_sbar_rect;
	Get_right_sbar_rect( &right_sbar_rect );
	right_sbar = NewControl(tilesPtr,&right_sbar_rect,title,TRUE,0,0,0,scrollBarProc,1);
	terrain_buttons_rect.bottom=tilesRect.bottom-tilesRect.top-11;
	SizeControl(right_sbar, RIGHT_SCROLLBAR_WIDTH, tilesRect.bottom-tilesRect.top-20-11);
	SetControlMaximum(right_sbar, get_right_sbar_max());
    
	tiles_zoom_slider_rect.top = 0;
	tiles_zoom_slider_rect.left = tilesRect.right - 80;
	tiles_zoom_slider_rect.right = tilesRect.right;
	tiles_zoom_slider_rect.bottom = 10;
	CreateSliderControl(tilesPtr, &tiles_zoom_slider_rect, 1, 0, 3, 0, 4, false, NULL, &tiles_zoom_slider);
	
	ControlUserPaneBackgroundUPP upp = NewControlUserPaneBackgroundUPP (ControlUserPaneBackgroundProc);	

	ControlBackgroundRec rec = { 0, true };
	InvokeControlUserPaneBackgroundUPP(tiles_zoom_slider, &rec, upp);

	load_builtin_images();
	
	const MenuCommand kSparkleUpdate = FOUR_CHAR_CODE('sCUP');
	SetMenuItemCommandID (GetMenuHandle(500),2,kSparkleUpdate);
}

//initializes values according to user's preferences
//requires the menubar to be loaded
void init_user_prefs()
{
	play_sounds = get_should_play_sounds();
	CheckMenuItem (GetMenuHandle(570),18,play_sounds);
	use_strict_adjusts = get_should_use_strict_adjusts();
	CheckMenuItem (GetMenuHandle(570),19,use_strict_adjusts);
	always_draw_heights = get_always_show_heights();
	CheckMenuItem (GetMenuHandle(570),20,always_draw_heights);
	allow_arrow_key_navigation = get_allow_arrow_key_navigation();
	CheckMenuItem (GetMenuHandle(570),21,allow_arrow_key_navigation);
	CheckMenuItem (GetMenuHandle(570),22,get_user_pref_bool_value(4,false));
	tile_zoom_level = get_saved_tile_zoom_level();
	SetControl32BitValue (tiles_zoom_slider,tile_zoom_level);
	zoom_tiles_recalculate();
	get_editor_application();
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
	if ((mouse_button_held == TRUE) && (event.what != 23) && (ActiveNonFloatingWindow() == mainPtr)) {
		GlobalToLocal(&event.where);
		handle_action(event.where,event,MAIN_WINDOW_NUM);
	}
	//printf("%i\n",event.what);
	switch (event.what){
		case keyDown: case autoKey:
				chr = event.message & charCodeMask;
				chr2 = (char) ((event.message & keyCodeMask) >> 8);
				if((event.modifiers & cmdKey) != 0){
					if(chr2>=123 && chr2<=126)
						handle_keystroke(chr,chr2,event);
					if(event.what != autoKey){
						BringToFront(mainPtr);
						SetPort(GetWindowPort(mainPtr));
						if(chr=='=') //turn Command-= into Command-+
							event.modifiers|=shiftKey;
						menu_choice = MenuEvent(&event);//MenuKey(chr);
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
			if(delimit_undo_on_mouse_up){
				pushUndoStep(new Undo::UndoGroupDelimiter(Undo::UndoStep::END_GROUP));
				delimit_undo_on_mouse_up=false;
			}
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
				set_new_item(menu_item + 100 * (menu - 700) - 1);
				set_drawing_mode(4);
				set_tool(0);
				place_right_buttons(0);
				break;
			case 750: case 751: case 752: case 753: 
				set_new_creature(menu_item + 64 * (menu - 750) - 1);
				set_drawing_mode(3);
				set_tool(0);
				place_right_buttons(0);
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
		case 2:
		{
			CFBundleRef mainBundle = CFBundleGetMainBundle();
			if(mainBundle == NULL)
				give_error("Internal Error: Failed to locate license file","",0);
			CFURLRef resourceURL = CFBundleCopyResourceURL(mainBundle, CFSTR("Blades of Avernum Editor License"), CFSTR("pdf"), NULL);
			if(resourceURL == NULL)
				give_error("Internal Error: Failed to locate license file","",0);
			else{
				LSOpenCFURLRef(resourceURL, NULL);
				CFRelease(resourceURL);
			}
		}
			break;
		case 3:
			if (change_made_town || change_made_outdoors) {
				if (save_check(881) == FALSE)
					break;							
			}
			SUSparkleCheckForUpdates(true);
			break;
		case 4:
			select_script_editor();
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
					update_terrain_window_title();
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
			if(editing_town)
				return; //if already editing a town, do nothing
			//TODO: set change_made_outdoors based on state of undo stack
			small_any_drawn = FALSE;
			cen_x = max_zone_dim[town_type] / 2; cen_y = max_zone_dim[town_type] / 2;
			current_height_mode = 0;
			set_drawing_mode(0);
			editing_town = TRUE;
			set_up_terrain_buttons();
			reset_mode_number();
			shut_down_menus();
			//set_string("Drawing mode","");
			DrawMenuBar();
			reset_drawing_mode();
			purgeUndo();
			purgeRedo();
			selected_object_type = SelectionType::None;
			update_terrain_window_title();
			redraw_screen();
			break;
		case 2: // outdoor section
			if(!editing_town)
				return; //if already editing outdoors, do nothing
			//TODO: set change_made_town based on state of undo stack
			small_any_drawn = FALSE;
			cen_x = 24; cen_y = 24;
			current_height_mode = 0;
			set_drawing_mode(0);
			editing_town = FALSE;
			set_up_terrain_buttons();
			reset_mode_number();
			//set_string("Drawing mode","");
			shut_down_menus();
			clear_selected_copied_objects();
			DrawMenuBar();
			reset_drawing_mode();
			purgeUndo();
			purgeRedo();
			selected_object_type = SelectionType::None;
			update_terrain_window_title();
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
				selected_object_type = SelectionType::None;
				change_made_town = TRUE;
				update_terrain_window_title();
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
		case 10: //edit scenario script
			{
				char* script_name=get_scenario_filename();
				if(script_name){
					open_script_with_editor(script_name);
					free(script_name);
				}
			}
			break;
		case 11: //edit scenario data script
			{
				char* base_script_name=get_scenario_filename();
				if(base_script_name){
					char* script_name=(char*)malloc(strlen(base_script_name)+4);
					sprintf(script_name,"%sdata",base_script_name);
					open_script_with_editor(script_name);
					free(base_script_name);
					free(script_name);
				}
			}
			break;
		case 14: // reload scen script
			if (fancy_choice_dialog(871,0) == 2)
				break;
			scen_data.clear_scen_item_data_type();
			if (load_core_scenario_data() == FALSE) {
				file_is_loaded = FALSE;
				return;
			}
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
		case 15: // Clean Up Walls
			if (fancy_choice_dialog(873,0) == 2)
				break;
			clean_walls(); 
			redraw_screen();
			change_made_town = TRUE;
			break;
		case 16: //   Import Town
			clear_selected_copied_objects();
			if(import_boa_town()){
				purgeUndo();
				purgeRedo();
				change_made_town = TRUE;
			}
			redraw_screen();
			break;
		case 17: //   Import Outdoor Section
			clear_selected_copied_objects();
			if(import_boa_outdoors()){
				purgeUndo();
				purgeRedo();
				change_made_outdoors = TRUE;
			}
			redraw_screen();
			break;
		case 18: //   Set Variable Town Entry
			edit_add_town();
			change_made_town = TRUE;
			break;
		case 19: //   Edit Item Placement Shortcuts
			edit_item_placement();
			change_made_town = TRUE;
			break;
		case 20: //   Delete Last Town
			if (change_made_town || change_made_outdoors) {
				give_error("You need to save the changes made to your scenario before you can delete a town.","",0);
				return;
			}
			if (scenario.num_towns == 1) {
				give_error("You can't delete the only town in a scenario. All scenarios must have at least 1 town.","",0);
				return;
			}
			if (fancy_choice_dialog(865,0) == 1) {
				delete_town();
				load_all_town_names(NULL);
				load_town(cur_town);
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
		case 21: //   Write Scenario Data to Text File
			if (fancy_choice_dialog(866,0) == 1)
				start_data_dump();
			break;
		case 22: //   Change Outdoor Size
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
				update_terrain_window_title();
				redraw_screen();
			}
			break;
		case 2: 
			edit_town_details(); 
			change_made_town = TRUE; 
			set_up_terrain_buttons();
			update_terrain_window_title();
			draw_main_screen(); 
			break; // Town Details
		case 3: 
			edit_town_wand(); 
			change_made_town = TRUE; 
			break; //Town Wandering Monsters
		case 4: // Set Town Boundaries							
			set_tool(17);
			break;
		case 5: frill_terrain(); change_made_town = TRUE; break;
		case 6: unfrill_terrain(); change_made_town = TRUE; break;
		case 7:  //Edit Area Descriptions
			 edit_town_strs();
			 break;
		case 8: //edit town script
			if(strlen(town.town_script))
				open_script_with_editor(town.town_script);
			else
				give_error("There is no script specified for this town.", "You can set the town script using Town > Town Details", 0);
			break;
		case 9: //edit town dialogue script
			if(strlen(town.town_script)){
				char* script_name=(char*)malloc(strlen(town.town_script)+3);
				sprintf(script_name,"%sdlg",town.town_script);
				open_script_with_editor(script_name);
				free(script_name);
			}
			else
				give_error("There is no script specified for this town.", "You can set the town script using Town > Town Details", 0);
			break;
		case 11: //Set Starting Location
			if (fancy_choice_dialog(867,0) == 2)
					break;
			set_tool(72);
			break;
		case 13: // add random
			if (fancy_choice_dialog(863,0) == 2)
				break;
			place_items_in_town();
			change_made_town = TRUE; 
			redraw_screen();
			break;
		case 14:
			//TODO: should this continue to exist?
			set_all_items_containment();
			draw_terrain();
			change_made_town = TRUE; 
			break;
		case 15:  // set not prop
			for (i = 0; i < 144; i++)
				town.preset_items[i].properties = town.preset_items[i].properties & 253;
			fancy_choice_dialog(861,0);
			draw_terrain();
			change_made_town = TRUE; 
			break;
		case 16:  // clear all items
			if (fancy_choice_dialog(862,0) == 2)
				break;
			for (i = 0; i < 144; i++)
				town.preset_items[i].which_item = -1;
			draw_terrain();
			change_made_town = TRUE; 
			redraw_screen();
			break;
		case 19: //clear all creatures
			if (fancy_choice_dialog(878,0) == 2)
				break;
			for (i = 0; i < 80; i++)
				town.creatures[i].number = -1;
			draw_terrain();
			change_made_town = TRUE; 
				redraw_screen();
			break;
		case 20: // clear special encs
			if (fancy_choice_dialog(877,0) == 2)
				break;
			for (x = 0; x < NUM_TOWN_PLACED_SPECIALS; x++) {
				town.spec_id[x] = kNO_TOWN_SPECIALS;
				SetRect(&town.special_rects[x],-1,-1,-1,-1);
			}
			change_made_town = TRUE; 
			redraw_screen();
			break;
		case 21://change town size
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
				update_terrain_window_title();
				redraw_screen();
				change_made_outdoors = FALSE;
			}
			break;
		case 2: //Outdoor Details
			change_made_outdoors = TRUE;
			outdoor_details();
			set_up_terrain_buttons();
			update_terrain_window_title();
			draw_main_screen();
			break;

		case 3: edit_out_wand(0); change_made_outdoors = TRUE; break;
		case 4: edit_out_wand(1);  change_made_outdoors = TRUE; break;
		case 5: edit_out_wand(2); change_made_outdoors = TRUE; draw_terrain(); break;
		
		case 6: frill_terrain(); change_made_outdoors = TRUE; break;
		case 7: unfrill_terrain(); change_made_outdoors = TRUE; break;
		case 8: edit_out_strs(); change_made_outdoors = TRUE; break; //Edit Area Descriptions
		case 9:
			if(strlen(current_terrain.section_script))
				open_script_with_editor(current_terrain.section_script);
			else
				give_error("There is no script specified for this outdoor section.", "You can set the section script using Outdoors > Outdoor Details", 0);
			break;
		case 11: //Set outdoor starting point
			if (fancy_choice_dialog(864,0) == 2)
				return;
			set_tool(71);
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
			set_tool(48);
			break;
		case 7: // clear
			delete_selected_instance();
			if(editing_town)
				change_made_town = TRUE;
			else
				change_made_outdoors = TRUE;
			break;
		case 9: //draw floors
			set_drawing_mode(0);
			draw_main_screen();
			break;
		case 10: //draw terrains
			set_drawing_mode(1);
			draw_main_screen();
			break;
		case 11: //draw heights
			set_drawing_mode(2);
			draw_main_screen();
			break;
		case 12: //place creatures
			if(editing_town){
				set_drawing_mode(4);
				draw_main_screen();
			}
			else
				beep();
			break;
		case 13: //place items
			if(editing_town){
				set_drawing_mode(5);
				draw_main_screen();
			}
			else
				beep();
			break;
		case 15: //increase tile size
			tile_zoom_level<3 ? tiles_zoom_slider_change(tile_zoom_level+1) : beep();
			break;
		case 16: //decrease tile size
			tile_zoom_level>0 ? tiles_zoom_slider_change(tile_zoom_level-1) : beep();
			break;
		case 18: // toggle sound
			play_sounds = !play_sounds;
			CheckMenuItem (GetMenuHandle(570),9,play_sounds);
			write_should_play_sounds(play_sounds);
			play_sound(0);
			break;
		case 19: // toggle adjust usage
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
		case 20: // toggle height labelling
			always_draw_heights = !always_draw_heights;
			CheckMenuItem (GetMenuHandle(570),11,always_draw_heights);
			write_always_show_heights(always_draw_heights);
			break;
		case 21: //toggle arrow key handling
			allow_arrow_key_navigation = !allow_arrow_key_navigation;
			CheckMenuItem (GetMenuHandle(570),12,allow_arrow_key_navigation);
			write_allow_arrow_key_navigation(allow_arrow_key_navigation);
			break;
		case 22: //toggle auto-update checking
			{
				bool temp = !get_user_pref_bool_value(4,false);
				CheckMenuItem (GetMenuHandle(570),13,temp);
				write_user_pref_bool_value(4,temp);
			}
			break;
	}
	draw_main_screen();		
}

void doScroll(ControlHandle bar, short delta){
	short old_setting,new_setting;
	old_setting = GetControlValue(bar);
	new_setting = minmax(0,GetControlMaximum(bar),old_setting+delta);
	SetControlValue(bar,new_setting);
	store_control_value = new_setting;
	if (new_setting != old_setting)
	//	place_right_buttons(0);
        redraw_screen();
}

OSStatus paletteScrollHandler(EventHandlerCallRef eventHandlerCallRef,EventRef eventRef, void* userData) {
	OSStatus result = eventNotHandledErr;
	if(!file_is_loaded)
		return(result);
	UInt32      eventKind, eventClass;
	eventClass = GetEventClass(eventRef);
	eventKind  = GetEventKind(eventRef);
	if(eventKind==kEventMouseWheelMoved){ //scroll wheel events
		SInt32 delta;
		GetEventParameter(eventRef,kEventParamMouseWheelDelta,typeLongInteger,NULL,sizeof(delta),NULL,&delta);
		HIPoint pos;
		GetEventParameter(eventRef,kEventParamWindowMouseLocation,typeHIPoint,NULL,sizeof(pos),NULL,&pos);
		Point ppos = {(short)pos.y,(short)pos.x};
		//Rect paletteRect = terrain_buttons_rect;
		//paletteRect.left+=RIGHT_BUTTONS_X_SHIFT;
		//paletteRect.right+=RIGHT_BUTTONS_X_SHIFT+RIGHT_SCROLLBAR_WIDTH;
		//test if the event was in the right palette
		//if(PtInRect(ppos, &paletteRect))
		 //  doScroll(right_sbar,-1*delta);
		if (1){ //check to see if it fell in the terrian view
			Rect whole_area_rect = terrainViewRect();
			if(PtInRect(ppos, &whole_area_rect)){
				//it was inside the viewport. Now we have to figure out exactly what scroll to do
				//find out which axis it was
				UInt32 modifiers=0;
				GetEventParameter(eventRef, kEventParamKeyModifiers, typeUInt32, NULL, sizeof(modifiers), NULL, &modifiers);
				int axis = ((modifiers & shiftKey) != 0);
				int altaxis = ((modifiers & optionKey) != 0);
				int scrl = eSCRL_NoScrl;
				if(!axis){
					if(delta>0)
						scrl|=eSCRL_Top;
					else if(delta<0)
						scrl|=eSCRL_Bottom;
					if(altaxis){
						if(delta>0)
							scrl|=eSCRL_Left;
						else if(delta<0)
							scrl|=eSCRL_Right;
					}
				}
				else{
					if(delta>0)
						scrl|=eSCRL_Left;
					else if(delta<0)
						scrl|=eSCRL_Right;
					if(altaxis){
						if(delta>0)
							scrl|=eSCRL_Bottom;
						else if(delta<0)
							scrl|=eSCRL_Top;
					}
				}
				handle_scroll( (editing_town) ? max_zone_dim[town_type] : 48, scrl, false, false );
				//printf("done handling scroll (palette)\n");
			}
		}
	}
	return(result);
}

OSStatus paletteScrollHandler2(EventHandlerCallRef eventHandlerCallRef,EventRef eventRef, void* userData) {
	OSStatus result = eventNotHandledErr;
	if(!file_is_loaded)
		return(result);
	UInt32      eventKind, eventClass;
	eventClass = GetEventClass(eventRef);
	eventKind  = GetEventKind(eventRef);
	if(eventKind==kEventMouseWheelMoved){ //scroll wheel events
		SInt32 delta;
		GetEventParameter(eventRef,kEventParamMouseWheelDelta,typeLongInteger,NULL,sizeof(delta),NULL,&delta);
		HIPoint pos;
		GetEventParameter(eventRef,kEventParamWindowMouseLocation,typeHIPoint,NULL,sizeof(pos),NULL,&pos);
		Point ppos = {(short)pos.y,(short)pos.x};
		Rect paletteRect = terrain_buttons_rect;
		paletteRect.left+=RIGHT_TILES_X_SHIFT;
		paletteRect.right+=RIGHT_TILES_X_SHIFT+RIGHT_SCROLLBAR_WIDTH;
		//test if the event was in the right palette
		if(PtInRect(ppos, &paletteRect))
			doScroll(right_sbar,-1*delta);
	}
	return(result);
}

OSStatus viewScrollHandler(EventHandlerCallRef eventHandlerCallRef,EventRef eventRef, void* userData) {
	OSStatus result = eventNotHandledErr;
	if(!file_is_loaded)
		return(result);
	UInt32      eventKind, eventClass;
	eventClass = GetEventClass(eventRef);
	eventKind  = GetEventKind(eventRef);
	if(eventKind==kEventMouseScroll){ //scroll wheel events
		HIPoint pos;
		GetEventParameter(eventRef,kEventParamWindowMouseLocation,typeHIPoint,NULL,sizeof(pos),NULL,&pos);
		Point ppos = {(short)pos.y,(short)pos.x};
		Rect whole_area_rect = terrainViewRect();
		if(PtInRect(ppos, &whole_area_rect)){
			SInt32 dx=0,dy=0;
			GetEventParameter(eventRef,kEventParamMouseWheelSmoothHorizontalDelta,typeSInt32,NULL,sizeof(dx),NULL,&dx);
			GetEventParameter(eventRef,kEventParamMouseWheelSmoothVerticalDelta,typeSInt32,NULL,sizeof(dy),NULL,&dy);
			UInt32 modifiers=0;
			GetEventParameter(eventRef, kEventParamKeyModifiers, typeUInt32, NULL, sizeof(modifiers), NULL, &modifiers);
			dx*=-1;//flip coordinates
			if(abs(dx)>1000)
				dx=pow(-1,dx<0);
			if(abs(dy)>1000)
				dy=pow(-1,dy<0);
			float dir=pi/2;
			if(dx!=0){
				dir = atan(float(abs(dy))/abs(dx));
			}
			if(dx<0)
				dir=pi-dir;
			if(dy<0)
				dir=(2*pi)-dir;
			float smallAngle=.2694;
			float largeAngle=1.0147;
			if(modifiers&shiftKey){
				smallAngle=pi/8;
				largeAngle=(3*pi)/8;
			}
			int scrl = eSCRL_NoScrl;
			if (cur_viewing_mode == 10 || cur_viewing_mode == 11){ //3D
				if(dir>((2*pi)-smallAngle) || dir<(pi-largeAngle)){
					scrl|=eSCRL_Top;
				}
				else if(dir>(pi-smallAngle) && dir<((2*pi)-largeAngle)){
					scrl|=eSCRL_Bottom;
				}
				if(dir>largeAngle && dir<(pi+smallAngle)){
					scrl|=eSCRL_Left;
				}
				else if(dir<smallAngle || dir>(pi+largeAngle)){
					scrl|=eSCRL_Right;
				}
			}
			else{ //2D
				if(dir>(.125*pi) && dir<(.875*pi))
					scrl|=eSCRL_Top;
				else if(dir>(1.125*pi) && dir<(1.875*pi))
					scrl|=eSCRL_Bottom;
				if(dir>(.625*pi) && dir<(1.375*pi))
					scrl|=eSCRL_Left;
				else if(dir<(.375*pi) || dir>(1.625*pi))
					scrl|=eSCRL_Right;
			}
			handle_scroll( (editing_town) ? max_zone_dim[town_type] : 48, scrl, false, false );
		}
	}
	return(result);
}

pascal void right_sbar_action(ControlHandle bar, short part){
	if (part == 0)
		return;
	
	int delta=0;
	
	switch (part) {
		case inUpButton: delta=-1; break;
		case inDownButton: delta=1; break;
		case inPageUp: delta=-21; break;
		case inPageDown: delta=21; break;
	}
	doScroll(bar,delta);
}

void tiles_zoom_slider_change(short zoom_slider){
    if (zoom_slider == tile_zoom_level){
		//this isn't useless, it's needed in case the user drags the slider around and then puts it back where it was
		redraw_screen();
        return;
	}
    
    tile_zoom_level = zoom_slider;
	if(tile_zoom_level==0)
		DisableMenuItem(GetMenuHandle(570), 16);
	else
		EnableMenuItem(GetMenuHandle(570), 16);
	if(tile_zoom_level==3)
		DisableMenuItem(GetMenuHandle(570), 15);
	else
		EnableMenuItem(GetMenuHandle(570), 15);
	//necessary in case this function was called through a keyboard shortcut
	SetControl32BitValue(tiles_zoom_slider, tile_zoom_level);
	write_tile_zoom_level(tile_zoom_level);
    zoom_tiles_recalculate();
    set_up_terrain_rects();
    make_tile_gworlds();
    SetControlMaximum(right_sbar, get_right_sbar_max());
    setUpCreaturePalette = false;
    setUpItemPalette = false;
    set_up_terrain_buttons();
    redraw_screen();
}

Boolean Mouse_Pressed( EventRecord * event )
{
	WindowPtr	the_window;
	short	the_part,content_part;
	long menu_choice;
	ControlHandle control_hit;
	BitMap		screenbits;
	Boolean All_Done = FALSE;
	Rect	resized_rect;
	static const Rect main_window_size_limit_rect = {225, 300, 5000, 5000};
	static const Rect tile_window_size_limit_rect = {225, 326, 5000, 5000};//326}; //TODO: figure out correct limit on maximum height

	the_part = FindWindow( event->where, &the_window);
	
	switch (the_part){
		case inMenuBar:
			menu_choice = MenuSelect(event->where);
			handle_menu_choice(menu_choice);
			break;
		case inSysWindow:
			break;
		case inGrow:
			if (the_window == mainPtr){
				//TODO: enforce a different size limit when in 2D zoomed-out mode
				ResizeWindow(the_window, event->where, &main_window_size_limit_rect, &resized_rect);
				write_window_bounds(0, resized_rect);
				ZeroRectCorner(&resized_rect);
				terrain_viewport_3d.bottom = resized_rect.bottom - 40;
				terrain_viewport_3d.right = resized_rect.right - 40;
				kRect3DEditScrn.bottom = resized_rect.bottom - 20;
				kRect3DEditScrn.right = resized_rect.right - 20;
				windRect = resized_rect;
				DisposeGWorld(ter_draw_gworld);
				NewGWorld(&ter_draw_gworld, 0,&resized_rect, NIL, NIL, kNativeEndianPixMap);
				recalculate_2D_view_details();
				recalculate_draw_distances();
				set_up_view_buttons();
				redraw_screen();
				//TODO: do this properly, not just hacked on: redraw_screen() calls draws terrain with small_any_drawn = TRUE, so zoomed out view doesn't get fully redrawn
				if (cur_viewing_mode == 1)
				{
					small_any_drawn = FALSE;
					draw_terrain();
				}
			}
			else if(the_window == tilesPtr){
				ResizeWindow(the_window, event->where, &tile_window_size_limit_rect, &resized_rect);
				write_window_bounds(2, resized_rect);
				tilesRect = resized_rect;
				ZeroRectCorner(&tilesRect);
				terrain_buttons_rect.bottom=tilesRect.bottom-tilesRect.top-11;
                terrain_buttons_rect.right=tilesRect.right-tilesRect.left-RIGHT_SCROLLBAR_WIDTH;//
                mode_buttons_rect.right=tilesRect.right-tilesRect.left-RIGHT_SCROLLBAR_WIDTH;//
				SizeControl(right_sbar, RIGHT_SCROLLBAR_WIDTH, tilesRect.bottom-tilesRect.top-20-11);
                MoveControl(right_sbar, tilesRect.right - RIGHT_SCROLLBAR_WIDTH, 20);//
                MoveControl(tiles_zoom_slider, tilesRect.right - 80, 0);//
                reset_mode_number();//
                resize_recalculate_num_tiles();//
                set_up_terrain_rects();//
                make_tile_gworlds();//
				SetControlMaximum(right_sbar, get_right_sbar_max());
                setUpCreaturePalette = false;//
                setUpItemPalette = false;//
                set_up_terrain_buttons();//
				redraw_screen();
			}

			break;
		case inZoomOut:
			if (the_window == mainPtr){
				if(cur_viewing_mode>=10){
					if(!EqualRect(&terrain_viewport_3d, &default_terrain_viewport_3d)){
						SizeWindow(the_window, DEFAULT_RECT3DEDIT_WIDTH + 40, DEFAULT_RECT3DEDIT_HEIGHT + 40, FALSE);
						terrain_viewport_3d = default_terrain_viewport_3d;
						
						GetWindowBounds(the_window, kWindowContentRgn, &windRect);
						write_window_bounds(0, windRect);
						windRect = default_terrain_viewport_3d;
						InsetRect(&windRect, -TER_RECT_UL_X, -TER_RECT_UL_Y);
						ZeroRectCorner(&windRect);
						DisposeGWorld(ter_draw_gworld);
						NewGWorld(&ter_draw_gworld, 0,&windRect, NIL, NIL, kNativeEndianPixMap);
						
						kRect3DEditScrn.bottom = DEFAULT_RECT3DEDIT_HEIGHT + TER_RECT_UL_X;
						kRect3DEditScrn.right = DEFAULT_RECT3DEDIT_WIDTH + TER_RECT_UL_Y;
						recalculate_draw_distances();
					}
					else{
						//TODO: this assumes that all windows are on the main screen
						//Should eventually use GetWindowGreatestAreaDevice to determine which screen the main window should be on,
						//and compute intersections of secondary windows with that device
						Rect screenBounds;
						GetAvailableWindowPositioningBounds(NULL,&screenBounds);
						Rect floatingWindows[2];
						GetWindowBounds(palettePtr, kWindowStructureRgn, &floatingWindows[0]);
						GetWindowBounds(tilesPtr, kWindowStructureRgn, &floatingWindows[1]);
						windRect=largestNonOverlapping(screenBounds,&floatingWindows[0],&floatingWindows[0]+2);
						SetWindowBounds (mainPtr,kWindowStructureRgn,&windRect);
						//note that in the next line we switch to working with kWindowContentRgn instaead of kWindowStructureRgn
						GetWindowBounds(mainPtr, kWindowContentRgn, &windRect);
						write_window_bounds(0, windRect);
						ZeroRectCorner(&windRect);
						
						terrain_viewport_3d=windRect;
						InsetRect(&terrain_viewport_3d, 20, 20);
						ZeroRectCorner(&terrain_viewport_3d);
						Rect ter_draw_rect=terrain_viewport_3d;
						InsetRect(&ter_draw_rect, -TER_RECT_UL_X, -TER_RECT_UL_Y);
						ZeroRectCorner(&ter_draw_rect);
						DisposeGWorld(ter_draw_gworld);
						NewGWorld(&ter_draw_gworld, 0,&ter_draw_rect, NIL, NIL, kNativeEndianPixMap);
						
						kRect3DEditScrn.bottom = windRect.bottom - 20;
						kRect3DEditScrn.right = windRect.right - 20;
						recalculate_draw_distances();
					}
				}
				else{ //TODO: specialize this for the 9 by 9 2D size
					if(!EqualRect(&terrain_viewport_3d, &default_terrain_viewport_3d)){
						SizeWindow(the_window, DEFAULT_RECT3DEDIT_WIDTH + 40, DEFAULT_RECT3DEDIT_HEIGHT + 40, FALSE);
						terrain_viewport_3d = default_terrain_viewport_3d;
						
						GetWindowBounds(the_window, kWindowContentRgn, &windRect);
						write_window_bounds(0, windRect);
						windRect = default_terrain_viewport_3d;
						InsetRect(&windRect, -TER_RECT_UL_X, -TER_RECT_UL_Y);
						ZeroRectCorner(&windRect);
						DisposeGWorld(ter_draw_gworld);
						NewGWorld(&ter_draw_gworld, 0,&windRect, NIL, NIL, kNativeEndianPixMap);
						
						kRect3DEditScrn.bottom = DEFAULT_RECT3DEDIT_HEIGHT + TER_RECT_UL_X;
						kRect3DEditScrn.right = DEFAULT_RECT3DEDIT_WIDTH + TER_RECT_UL_Y;
						recalculate_2D_view_details();
					}
					else{
						//TODO: this assumes that all windows are on the main screen
						//Should eventually use GetWindowGreatestAreaDevice to determine which screen the main window should be on,
						//and compute intersections of secondary windows with that device
						Rect screenBounds;
						GetAvailableWindowPositioningBounds(NULL,&screenBounds);
						Rect floatingWindows[2];
						GetWindowBounds(palettePtr, kWindowStructureRgn, &floatingWindows[0]);
						GetWindowBounds(tilesPtr, kWindowStructureRgn, &floatingWindows[1]);
						windRect=largestNonOverlapping(screenBounds,&floatingWindows[0],&floatingWindows[0]+2);
						SetWindowBounds (mainPtr,kWindowStructureRgn,&windRect);
						//note that in the next line we switch to working with kWindowContentRgn instaead of kWindowStructureRgn
						GetWindowBounds(mainPtr, kWindowContentRgn, &windRect);
						write_window_bounds(0, windRect);
						ZeroRectCorner(&windRect);
						
						terrain_viewport_3d=windRect;
						InsetRect(&terrain_viewport_3d, 20, 20);
						ZeroRectCorner(&terrain_viewport_3d);
						Rect ter_draw_rect=terrain_viewport_3d;
						InsetRect(&ter_draw_rect, -TER_RECT_UL_X, -TER_RECT_UL_Y);
						ZeroRectCorner(&ter_draw_rect);
						DisposeGWorld(ter_draw_gworld);
						NewGWorld(&ter_draw_gworld, 0,&ter_draw_rect, NIL, NIL, kNativeEndianPixMap);
						
						kRect3DEditScrn.bottom = windRect.bottom - 20;
						kRect3DEditScrn.right = windRect.right - 20;
						recalculate_2D_view_details();
					}
				}
				set_up_view_buttons();
				redraw_screen();
				//TODO: do this properly, not just hacked on: redraw_screen() calls draws terrain with small_any_drawn = TRUE, so zoomed out view doesn't get fully redrawn
				if (cur_viewing_mode == 1)
				{
					small_any_drawn = FALSE;
					draw_terrain();
				}				
			}
            else if (the_window == tilesPtr){
                SizeWindow(the_window, TILES_WINDOW_WIDTH, tilesRect.bottom - tilesRect.top, FALSE);
				Rect boundsRect;
				GetWindowBounds(the_window, kWindowContentRgn, &boundsRect);
				write_window_bounds(2, boundsRect);
                tilesRect.right = tilesRect.left + TILES_WINDOW_WIDTH;
                terrain_buttons_rect.right=tilesRect.right-tilesRect.left-RIGHT_SCROLLBAR_WIDTH;
                mode_buttons_rect.right=tilesRect.right-tilesRect.left-RIGHT_SCROLLBAR_WIDTH;
                MoveControl(right_sbar, tilesRect.right - RIGHT_SCROLLBAR_WIDTH, 20);
                MoveControl(tiles_zoom_slider, tilesRect.right - 80, 0);//
                reset_mode_number();
                resize_recalculate_num_tiles();
                set_up_terrain_rects();
                make_tile_gworlds();
				SetControlMaximum(right_sbar, get_right_sbar_max());
                setUpCreaturePalette = false;
                setUpItemPalette = false;
                set_up_terrain_buttons();
				redraw_screen();
            }
			break;
		case inDrag:
			GetQDGlobalsScreenBits(&screenbits);
			DragWindow(the_window, event->where, &screenbits.bounds);
			Rect boundsRect;
			GetWindowBounds(the_window, kWindowContentRgn, &boundsRect);
			if(the_window==mainPtr)
				write_window_bounds(0, boundsRect);
			else if(the_window==palettePtr)
				write_window_bounds(1, boundsRect);
			else if(the_window==tilesPtr)
				write_window_bounds(2, boundsRect);
			break;
		case inGoAway:
			All_Done = TRUE;
			break;
		case inContent:
			if (the_window == mainPtr){
			SetPort(GetWindowPort(mainPtr));
			GlobalToLocal(&(event->where));
				handle_action(event->where, *event, MAIN_WINDOW_NUM);
			}
			else if (the_window == palettePtr){
				SetPort(GetWindowPort(palettePtr));
				GlobalToLocal(&(event->where));
				handleToolPaletteClick(event->where, *event);
			}
			else if (the_window == tilesPtr){
				SetPort(GetWindowPort(tilesPtr));
				GlobalToLocal(&(event->where));
				content_part = FindControl(event->where,the_window,&control_hit); // hit sbar?
				if (content_part != 0) {
					switch (content_part) {
						case inThumb:
                            if (control_hit == tiles_zoom_slider){
                                content_part = TrackControl(control_hit,event->where,NIL);
                                tiles_zoom_slider_change(GetControl32BitValue(control_hit));
								ticks_to_wait = SPARSE_TICKS;
								mouse_button_held = FALSE;
                            }
                            else{
							content_part = TrackControl(control_hit,event->where,NIL);
							if (control_hit == right_sbar){
								if (content_part == inThumb) {
									set_up_terrain_buttons();
									place_right_buttons(0);
								}
							}
                            }
							break;
						case inUpButton: case inPageUp: case inDownButton: case inPageDown:
							if (control_hit == right_sbar)
								content_part = TrackControl(control_hit,event->where,(ControlActionUPP)right_sbar_action_UPP);
							break;
					}
				} // a control hit				
				else
					handle_action(event->where, *event, TILES_WINDOW_NUM);
			}
			
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
	
	screen_pixmap_handle = (**(cur_device)).gdPMap;
	pixel_depth = (**(screen_pixmap_handle)).pixelSize;
	
	if ((diff_depth_ok == FALSE) && ((pixel_depth <= 16) && (HasDepth(cur_device,16,1,1)) == 0)) {
		choice = choice_dialog(0,1070);
		if (choice == 1)
			ExitToShell();
		if (choice == 2)
			diff_depth_ok = TRUE;
	}
	
	if ((pixel_depth != 16) && (diff_depth_ok == TRUE))
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