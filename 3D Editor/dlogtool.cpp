// dlogtool.c

// OK. What on Earth is this?

// For various reasons (mainly customization and porting ease), we use a custom
// dialog box engine. This is it. 

// Dialog boxes are created using the calls in this file. Basically, what happens,
// is that the editor loads in a dialog box resource. The text entries in that resource
// tell the editor what the dialog box should look like. An empty dialog box is created by
// the editor, and the editor draws the buttons on top of that.

// Events for that dialog box are handled using the call cd_event_filter. If you make a new
// dialog box, it must have a line hooking it in in that procedure.

// How to make a new dialog box.
// 1. Make the DLOG resource are usual.
// 2. Make item 1 a button. Move that button off of the window.
// 3. If you want text entry fields, make those items 2 - whatever. (i.e.
//    3 text entries their numbers are items 2-4).
// 4. All of the other items are text. Each is a number, then an underscore,
//    then another number. (i.e. "0_1", "3_5").
//    The meanings of the numbers are:
//    First number = 0: Button. The second number is which button, the button types
//      are described in arrays button_type and button_strs below
//    First number = 1: Default Button. Like button, but is the default.
//    First number = 2: Radio button. Next number should be 0.
//    First number = 5: Picture. I don't think this works anymore.
//    One exception: If you just want a chunk of text, you can have the text entry contain
//    that text. If that text begins with ~, =, or *, formats text differently.
//		+ signifies text with an outline but no background
//		~ signifies large bold text
//		* is small bold text

// Of course, you can just ignore all this mess and write regular dialog boxes.

//#include <Carbon/Carbon.h>	// included in pre-compiled header
//#include <TextUtils.h>
//#include <QDOffscreen.h>
//#include <Quickdraw.h>
//#include <Events.h>
//#include "stdio.h"
//#include "string.h"
#include "global.h"

#define	NUM_DLOG_B		53
#define	DIALOG_PIXEL_ADJUST		6
#define ND	15
#define	NI	500
#define	NL	100


extern Boolean play_sounds,cursor_shown,dialog_not_toast;
extern WindowPtr mainPtr;
extern Boolean modeless_exists[18];
extern DialogPtr modeless_dialogs[18];
extern Boolean using_osx;


extern GWorldPtr dlg_buttons_gworld[NUM_BUTTONS][2],tiny_obj_gworld;
extern GWorldPtr pc_gworld,monst_gworld[10],terrain_gworld[12],item_graphics[26];
extern PixPatHandle	bg[14];
extern short geneva_font_num;

static short dlg_keys[ND];
static short dlg_types[ND];
static WindowPtr dlgs[ND];
static WindowPtr dlg_parent[ND];
static short dlg_highest_item[ND];
static Boolean dlg_draw_ready[ND];

static short item_dlg[NI];
static short item_number[NI];
static char dlg_item_type[NI];
static Rect item_rect[NI];
static short item_flag[NI];
static char item_active[NI];
static char item_key[NI];
static short item_label[NI];
static short item_label_loc[NI];

static char text_long_str[10][256];
static char text_short_str[140][40];
static char labels[NL][25];
static Boolean label_taken[NL];


static short store_free_slot;
static short store_dlog_num;

short available_dlog_buttons[NUM_DLOG_B] = {0,63,64,65,1,4,5,8, 
								128,
								9,10,11,12,13,
								14,15,16,17,29, 51,60,61,62,
								66,69,70, 71,72,73,74,79,
								80,83,86,87,88, 91,92,93,99,100,
								101,102,104, 129,130,131,132,133,
								134,135,136,137};

// these four dimensions should be defined as a structure
static short button_type[140] = {1,1,4,5,1,12,0,0,1,1, // how big the button is? example 0 - small, 1 - medium, 2 - large
						 1,1,1,1,1,1,1,1,8,8,
						 9,9,9,1,1,2,1,6,7,1,
						 1,12,1,1,2,0,0,0,0,0,
						 0,0,0,0,0,0,0,0,0,0,
						 0,1,1,1,2,1,1,1,2,2, // 50
						 1,1,1,11,1,1,2,3,1,1,
						 1,1,1,1,2,2,2,2,2,1,
						 1,1,1,1,2,2,1,1,1,2,
						 0,1,1,1,14,13,12,12,12,1,
						 1,1,1,2,1,2,2,2,2,1, // 100
						 2,2,2,2,2,2,2,2,2,2,
						 2,2,2,2,2,2,2,2,0,0,
						 1,1,1,1,1,1,0,0,2,0};
const char *button_strs[140] = {"Done ","Ask"," "," ","Keep", " ","+","-","Buy","Leave", // text on the button? what text
						"Get","1","2","3","4","5","6","Cast"," "," ",
						" "," "," ","Buy","Sell","Other Spells","Buy x10"," "," ","Save",
						"Race","Train","Items","Spells","Heal Party","1","2","3","4","5",
						"6","7","8","9","10","11","12","13","14","15",
				/*50*/  "16","Take","Create","Delete","Race/Special","Skill","Name","Graphic","Bash Door","Pick Lock",
						"Leave","Steal","Attack"," ","Yes","No","Step In"," ","Record","Climb",
						"Flee","Onward","Answer","Drink","Approach","Mage Spells","Priest Spells","Advantages","New Game","Land",
						"Under","Restore","Restart","Quit","Save First","Just Quit","Rest","Read","Pull","Alchemy",
						"17","Cancel","Pray","Wait","","","Delete","Graphic","Create","Give",
				/*100*/		"Destroy","Pay","Free","Next Tip","Touch", "Select Icon","Create/Edit","Clear Special","Edit Abilities","Choose",
						"Go Back","Create New","General","One Shots","Affect PCs","If-Thens","Town Specs","Out Specs","Advanced","Weapon Abil",
						"General Abil.","NonSpell Use","Spell Usable","Reagents","Missiles","Abilities","Pick Picture","Animated","Enter","Burn",
						"Insert","Remove","Accept","Refuse","Open","Close","Sit","Stand","Don't Save",""};
static short button_left_adj[140] = {2,0,0,0,6,0,0,0,0,5,
							0,0,0,0,0,0,0,4,0,0,
							0,0,0,0,0,5,0,0,0,6,
							6,6,0,0,0,0,0,0,0,0,
							0,0,0,0,6,6,6,6,6,6,
							6,6,0,2,0,0,0,2,3,3, // 50
							6,6,0,7,5,5,0,0,2,6,
							4,2,0,0,0,4,10,4,0,6,
							6,2,1,6,4,3,0,4,6,4,
							6,0,6,6,0,0,0,2,0,0,
							2,6,6,3,6,7,3,0,0,3, // 100
							0,5,3,3,3,3,3,4,4,0,
							6,4,4,2,2,3,6,5,0,0,
							4,4,4,4,6,4,0,0,5,0
							};
static char button_def_key[140] = {0,0,20,21,'k', 24,0,0,0,0,
							'g','1','2','3','4', '5','6',0,0,0,
							0,0,0,0,0,' ',0,22,23,0,
							0,0,0,0,0,'1','2','3','4','5',
							'6','7','8','9','a', 'b','c','d','e','f',  
							'g',0,0,0,0,0,0,0,0,0,
							0,0,0,0,'y','n',0,'?','r',0,
							0,0,0,0,0,0,0,0,0, 0,
							0,0,0,0,0,0,0,0,0,0,
							'g',0,0,0,0,0,0,0,0,0,
							0,0,0,0,0,0,0,0,0,0,
							0,0,0,0,0,0,0,0,0,0,
							0,0,0,0,0,0,0,0,0,0,
							0,0,0,0,0,0,0,0,0,0
							};
							// specials ... 20 - <-  21 - ->  22 up  23 down  24 esc
							// 25-30  ctrl 1-6  31 - return"\"

void beep() 
{
	SysBeep(20);
}

void cd_init_dialogs()
{
	short i;

	for (i = 0; i < ND; i++) {
		dlg_keys[i] = -1;
		dlg_types[i] = 0;
		dlgs[i] = NULL;
		dlg_highest_item[i] = 0;
	}
	for (i = 0; i < NI; i++) {
		item_dlg[i] = -1;
	}
	for (i = 0; i < NL; i++) {
		label_taken[i] = FALSE;
	}
}

short cd_create_dialog_parent_num(short dlog_num,short parent)
{
	short i;

	if ((parent == 0) || (parent == 1))
		return cd_create_dialog(dlog_num,mainPtr);
	i = cd_get_dlg_index(parent);
	if (i < 0)
		return -1;
	return cd_create_dialog(dlog_num,dlgs[i]);
}

short cd_create_dialog(short dlog_num,WindowPtr parent)
{
	short i,free_slot = -1;
	WindowPtr dlg;
	static short current_key = 0;

	store_dlog_num = dlog_num;
	//store_parent = parent;
	for (i = 0; i < ND; i++) {
		if ((dlg_keys[i] >= 0) && (dlg_types[i] == dlog_num))
			return -1;
		}
	for (i = 0; i < ND; i++) {
		if (dlg_keys[i] < 0) {
			free_slot = i;
			i = 500;
			}
		}
	if (free_slot < 0)
		return -2;
	current_key++;
	dlg_keys[free_slot] = current_key;
	dlg_types[free_slot] = dlog_num;
	dlg_highest_item[free_slot] = 1;
	dlg_draw_ready[free_slot] = FALSE;
	dlgs[free_slot] = NULL;

	// first, create dummy dlog
	store_free_slot = free_slot;
	//dlg = CreateDialog(store_hInstance,MAKEINTRESOURCE(dlog_num),0,d_proc);
	dlg = (WindowPtr) GetNewDialog (dlog_num, NIL, IN_FRONT);
	dlgs[free_slot] = dlg;
	if (dlgs[free_slot] == NULL) {
		play_sound(3);
		return -3;
	}
	//center_window(dlgs[free_slot]);

	dlg_parent[free_slot] = parent;
	
	process_new_window (dlgs[free_slot]);

	HideFloatingWindows();

	ShowWindow(GetDialogWindow((DialogPtr)dlgs[free_slot]));
	SetPort(GetWindowPort(GetDialogWindow((DialogPtr)dlgs[free_slot])));
	//TextFont(geneva_font_num);
	//TextFace(bold);
	//TextSize(10);
	ForeColor(blackColor);
	BackColor(whiteColor);
	dialog_not_toast = TRUE;

	set_cursor(0);
	
	return 0;
}

void process_new_window (WindowPtr hDlg) {
	short i = -1,j,free_slot = -1,free_item = -1,type,flag;
	Str255 item_str;
	Boolean str_stored = FALSE;
	Rect store_rect;
	short win_height = 0, win_width = 0;
	short the_type;
	Handle the_handle = NULL;
	Rect small_rect;
	short num_items;
	short str_offset = 1;

	free_slot = store_free_slot;
	num_items = CountDITL((DialogPtr)hDlg);

	dlg_highest_item[free_slot] = 0;
	
	// Now, give the window its items
	for (i = 0; i < num_items; i++) {
		str_offset = 1;
		GetDialogItem((DialogPtr) hDlg, i + 1, &the_type, &the_handle, &small_rect);
		if (the_type % 128 == 8) {
			GetDialogItemText(the_handle,item_str);
			p2c(item_str);
			dlg_highest_item[free_slot]++;
			str_stored = FALSE;
			if (strlen((char *)item_str) == 0) {
				sprintf((char *) item_str, "+");
				type = 3;
				flag = 1;
	            str_stored = TRUE;
			}
			else if (item_str[0] == '+') { // default is framed text
				type = 3;
				flag = 1;
	            str_stored = TRUE;
			}
			else if (item_str[0] == '*') {
				type = 3;
				flag = 0;
				str_stored = TRUE;
			}
			else if (item_str[0] == '~') {
				type = 7;
				flag = 0;
				str_stored = TRUE;
			}
			else if (item_str[0] == '!') {
				type = 4;
				flag = 0;
				str_stored = TRUE;
			}
			else if (item_str[0] == '=') {
				type = 9;
				flag = 1;
				str_stored = TRUE;
			}
			else if (((item_str[0] >= 65) && (item_str[0] <= 122)) || (item_str[0] == '"')) {
				type = 9;
				flag = 0;
				str_offset = 0;
				str_stored = TRUE;
			}
			else if ((item_str[0] == '^') || (item_str[0] == '&')) {
				type = (item_str[0] == '^') ? 10 : 11;
				flag = 1;
				if (string_length((char *) item_str) > 55)
					flag = 2;
				str_stored = TRUE;
			}
			else {
#ifndef EXILE_BIG_GUNS
				sscanf((char *) item_str,"%d_%d",&type,&flag);
#endif		
#ifdef EXILE_BIG_GUNS
				sscanf((char *) item_str,"%hd_%hd",&type,&flag);
				//type = typel; flag = flagl;
#endif		
			}

			free_item = -1;
			// find free item
			switch (type) {
				case 0: case 1: case 2: case 5: case 6:
					for (j = 150; j < NI; j++)
						if (item_dlg[j] < 0) {
							free_item = j;
							j = NI + 1;
						}
					break;
				default:
					if ((type == 9) || 
					 ((str_stored == TRUE) && (strlen((char *) item_str) > 35))) {
						for (j = 0; j < 10; j++)
							if (item_dlg[j] < 0) {
								free_item = j;
								j = NI + 1;
							}
					}
					else {
						for (j = 10; j < 140; j++)
							if (item_dlg[j] < 0) {
								free_item = j;
								j = NI + 1;
							}
					}
					break;
			}

			if (free_item >= 0) {
					item_dlg[free_item] = store_dlog_num;
					dlg_item_type[free_item] = type;
					item_number[free_item] = i + 1;

					item_rect[free_item] = get_item_rect(hDlg,i + 1);

					item_flag[free_item] = flag;
					item_active[free_item] = 1;
					item_label[free_item] = 0;
               		item_label_loc[free_item] = -1;
               		item_key[free_item] = 0;
					switch (type) {
						case 0: case 1:
							store_rect = get_graphic_rect(dlg_buttons_gworld[button_type[flag]][0]);
							item_rect[free_item].right = item_rect[free_item].left + store_rect.right;
							item_rect[free_item].bottom = item_rect[free_item].top + store_rect.bottom;
							item_key[free_item] = button_def_key[flag];
							if (type == 1)
								item_key[free_item] = 31;
							break;
						case 2:
							item_rect[free_item].right = item_rect[free_item].left + 14;
							item_rect[free_item].bottom = item_rect[free_item].top + 10;
                    		item_key[free_item] = 255;
							break;
						case 3: case 4: case 7: case 8: case 9: case 10: case 11: 
						//	sprintf(((free_item < 10) ? text_long_str[free_item] : text_short_str[free_item - 10]),"");
							if (free_item < 10)
								*(text_long_str[free_item]) = '\0';
							else
								*(text_short_str[free_item - 10]) = '\0';
							if (str_stored == TRUE) {
								if (free_item < 10)
								sprintf(text_long_str[free_item],"%s",
								  (char *) (item_str + str_offset));
								else
								sprintf(text_short_str[free_item - 10],"%-39.39s",
								  (char *) (item_str + str_offset));
							}
							item_key[free_item] = 255; 
							if (type >= 10) {
								store_rect = get_graphic_rect(dlg_buttons_gworld[1][0]);
								item_rect[free_item].right = item_rect[free_item].left + store_rect.right;
								item_rect[free_item].bottom = item_rect[free_item].top + store_rect.bottom;
								if (type == 11)
									item_key[free_item] = 31;
							}
							break;
					}
					win_height = max(win_height, item_rect[free_item].bottom + 5);
					win_width = max(win_width, item_rect[free_item].right + 6);

				}

			}
		}
		
	win_width += 25;
	win_height += 25;
	
	ShortenDITL((DialogPtr) hDlg,dlg_highest_item[free_slot]);
	if (using_osx)
		SizeWindow(GetDialogWindow((DialogPtr)hDlg),win_width + 13 + DIALOG_PIXEL_ADJUST,win_height + 13 + DIALOG_PIXEL_ADJUST,FALSE);
		else SizeWindow(hDlg,win_width - 5 + DIALOG_PIXEL_ADJUST,win_height - 5 + DIALOG_PIXEL_ADJUST,FALSE);
	dlg_highest_item[free_slot] = num_items;
}


short cd_kill_dialog(short dlog_num,short parent_message)
{
	short i,which_dlg = -1;

	for (i = 0; i < ND; i++)
		if ((dlg_keys[i] >= 0) && (dlg_types[i] == dlog_num))
			which_dlg = i;
	if (which_dlg < 0)
		return -1;

	for (i = 0; i < NI; i++)
		if (item_dlg[i] == dlog_num) {
			if (item_label[i] > 0)
				label_taken[item_label_loc[i]] = FALSE;
			item_dlg[i] = -1;
		}

/*	if (dlg_parent[which_dlg] != NULL) {
		EnableWindow(dlg_parent[which_dlg],TRUE);
		if (dlg_parent[which_dlg] == mainPtr)
			for (i = 0; i < 18; i++)
				if (modeless_exists[i] == TRUE)
					EnableWindow(modeless_dialogs[i],TRUE);
		SetFocus(dlg_parent[which_dlg]);
		SetWindowPos(dlg_parent[which_dlg],WindowPtr_TOP,0,0,100,100,
			SWP_NOMOVE | SWP_NOSIZE);
		cd_set_edit_focus();
		} */
//	if (parent_message > 0)
//		SendMessage(dlg_parent[which_dlg],WM_COMMAND,parent_message,0);

	DisposeDialog((DialogPtr) dlgs[which_dlg]);
	dlg_keys[which_dlg] = -1;
	if (dlg_parent[which_dlg] != NULL){
//  BUG FIX:  In OS X 10.4, there was an error on what used to be this line.  Instead of checking for the mainPtr, 
//            the editor just used GetDialogWindow, which screws up the editor.  The following code fixes that bug.
        if (dlg_parent[which_dlg] != mainPtr){
            SetPort(GetWindowPort(GetDialogWindow((DialogPtr)dlg_parent[which_dlg])));
		}
        else {
            SetPort(GetWindowPort(mainPtr));        // NO IDEA IF THIS WORKS!!
			ShowFloatingWindows();
		}
	}
	if (FrontWindow() != mainPtr)
		redraw_screen();
	dialog_not_toast = TRUE;
	return 0;
}

// patch to change the mouse click behavior on the dialog button/radio button
//		cd_press_button(): replaced to cd_process_mousetrack()
short cd_process_click(WindowPtr window,Point the_point, short mods,short *item)
{
	short i,which_dlg,dlg_num=-1,item_id;
	short dlog_key;

	if ((which_dlg = cd_find_dlog(window,&dlg_num,&dlog_key)) < 0)
		return -1;

	for (i = 0; i < dlg_highest_item[which_dlg] + 1; i++){
		if ((item_id = cd_get_item_id(dlg_num,i)) >= 0) {
			if ((PtInRect(the_point,&item_rect[item_id])) && (item_active[item_id] > 0)
				&& ((dlg_item_type[item_id] < 3) || (dlg_item_type[item_id] == 8)
				|| (dlg_item_type[item_id] == 10)|| (dlg_item_type[item_id] == 11))) {
					*item = i;
					if ((mods & 2048) != 0) 
						*item += 100;
					if (dlg_item_type[item_id] != 8)
						dlg_num = cd_process_mousetrack( dlg_num, i, item_rect[item_id] );
					return dlg_num;
			}
		}
	}
	return -1;
}

// patch to change the mouse click behavior on the dialog button/radio button
// Tracking mouse while mouse button is held.
short cd_process_mousetrack( short dlg_num, short item_num, Rect theRect )
{
	short result = -1;
	Boolean prevInside = FALSE;
	Boolean currInside = TRUE;
	cd_press_button(dlg_num, item_num, eDLGBtnResChange);	// give click response

	RgnHandle clipRgn = NewRgn();	// Get copy of clipping region, seems not to use currently
	GetClip( clipRgn );

	Point currPt;					// Track the mouse while it is down
	while ( StillDown() ) {
		GetMouse( &currPt );
		prevInside = currInside;
		currInside = PtInRect(currPt, &theRect) && PtInRgn(currPt, clipRgn);
		if ( !prevInside && currInside )
			cd_press_button(dlg_num, item_num, eDLGBtnResChange);
		if ( prevInside && !currInside )
			cd_press_button(dlg_num, item_num, eDLGBtnResRecover);
	}

	EventRecord	anEvent;			// Get location from MouseUp event
	if ( WaitNextEvent( mUpMask, &anEvent, SLEEP_TICKS, MOUSE_REGION) ) {
		currPt = anEvent.where;
		GlobalToLocal( &currPt );
		if ( PtInRect(currPt, &theRect) && PtInRgn(currPt, clipRgn) )
			result = dlg_num;
	}
	else{
		//printf("no mouse-up makes cyclops sad: @[ \n");
		//no mouse up event. Not clear why this happens, but we have to do something, so brute force check where the cursor is
		GetMouse( &currPt );
		currInside = PtInRect(currPt, &theRect) && PtInRgn(currPt, clipRgn);
		if(currInside)
			result = dlg_num;
		if ( !prevInside && currInside )
			cd_press_button(dlg_num, item_num, eDLGBtnResChange);
		if ( prevInside && !currInside )
			cd_press_button(dlg_num, item_num, eDLGBtnResRecover);
	}
	cd_press_button(dlg_num, item_num, eDLGBtnResRecover);
	DisposeRgn( clipRgn );
	return result;
}

// patch to change the mouse click behavior on the dialog button/radio button
//		Add parameter eDLGBtnResCompatible to cd_press_button() 
short cd_process_keystroke(WindowPtr window,char char_hit,short *item)
{
	short i,which_dlg,dlg_num=-1,dlg_key,item_id;

	if ((which_dlg = cd_find_dlog(window,&dlg_num,&dlg_key)) < 0)
		return -1;
							// specials ... 20 - <-  21 - ->  22 up  23 down  24 esc
							// 25-30  ctrl 1-6


	for (i = 0; i < dlg_highest_item[which_dlg] + 1; i++)
		if ((item_id = cd_get_item_id(dlg_num,i)) >= 0) {
			if ((item_key[item_id] == char_hit) && (item_active[item_id] > 0)
				&&  ((dlg_item_type[item_id] < 3) || (dlg_item_type[item_id] == 8)
				 || (dlg_item_type[item_id] == 10) || (dlg_item_type[item_id] == 11))) {
					*item = i;
					if (dlg_item_type[item_id] != 8)
						cd_press_button(dlg_num,i, eDLGBtnResCompatible);
					return dlg_num;
			}
		}
			
	// kludgy. If you get an escape and is isn't processed, make it an enter
	if (char_hit == 24) {
   	char_hit = 31;
		for (i = 0; i < dlg_highest_item[which_dlg] + 1; i++)
			if ((item_id = cd_get_item_id(dlg_num,i)) >= 0) {
				if ((item_key[item_id] == char_hit) && (item_active[item_id] > 0)
					&&  ((dlg_item_type[item_id] < 3) || (dlg_item_type[item_id] == 8))) {
						*item = i;
						if (dlg_item_type[item_id] != 8)
							cd_press_button(dlg_num,i, eDLGBtnResCompatible);
						return dlg_num;
				}
			}
	}
	return -1;
}

/*
void cd_init_button(short dlog_num,short item_num, short button_num, short status)
{
	short dlg_index,item_index;

	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;
	if (dlg_item_type[item_index] > 1) {
		beep();
		return;
		}
	item_flag[item_index] = button_num;
	item_active[item_index] = status;
	item_rect[item_index].right = item_rect[item_index].left + button_width[button_num];
	item_rect[item_index].bottom = item_rect[item_index].top + button_width[button_num];
	item_key[item_index] = button_def_key[button_num];
	cd_draw_item(dlog_num,item_num);
}
*/

void cd_attach_key(short dlog_num,short item_num,char key)
{
	short dlg_index,item_index;

	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;

	if ((dlg_item_type[item_index] > 2) && (dlg_item_type[item_index] != 8)) {
		beep();
		return;
	}
	item_key[item_index] = key;
}

void cd_set_pict(short dlog_num, short item_num, short pict_num)
{
	short dlg_index,item_index;
	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;
	if (dlg_item_type[item_index] != 5) {
		beep();
		return;
	}
	item_flag[item_index] = pict_num;
	if (pict_num == -1)
		cd_erase_item(dlog_num,item_num);
		else cd_draw_item(dlog_num,item_num);
}

void cd_activate_item(short dlog_num, short item_num, short status)
{
	short dlg_index,item_index;
	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;

	item_active[item_index] = status;
	cd_draw_item(dlog_num,item_num);
}

short cd_get_active(short dlog_num, short item_num)
{
	short dlg_index,item_index;
	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return -1;

	return item_active[item_index];
}


void cd_get_item_text(short dlog_num, short item_num, char *str)
{
	short dlg_index,item_index;
	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return ;
	if (item_index >= 150) {
		beep();
		return;
	}
	if (item_index < 10)
		sprintf(str,"%s",text_long_str[item_index]);
		else sprintf(str,"%s",text_short_str[item_index - 10]);
}

void cd_retrieve_text_edit_str(short dlog_num, short item_num, char *str)
{
	short dlg_index;
	short the_type;
	Handle the_handle = NULL;
	Rect the_rect;
	Str255 store_ptr;
	
	*str = '\0';
	if ((dlg_index = cd_get_dlg_index(dlog_num)) < 0) {
		SysBeep(50); return ;
	}
	GetDialogItem((DialogPtr)  dlgs[dlg_index], item_num, &the_type, &the_handle, &the_rect);
	GetDialogItemText(the_handle,store_ptr);
	p2c(store_ptr);
	strcpy(str,(char *) store_ptr);
}
	
short cd_retrieve_text_edit_num(short dlog_num, short item_num)
{
	short dlg_index;
	short the_type;
	Handle the_handle = NULL;
	Rect the_rect;
	Str255 store_ptr;
	long num_given;
	
	if ((dlg_index = cd_get_dlg_index(dlog_num)) < 0) {
		SysBeep(50);
		return -1;
	}
	GetDialogItem((DialogPtr) dlgs[dlg_index], item_num, &the_type, &the_handle, &the_rect);
	GetDialogItemText(the_handle,store_ptr);
	StringToNum (store_ptr,&num_given);
	return (short) num_given;
}	


// NOTE!!! Expects a c string
void cd_set_text_edit_str(short dlog_num, short item_num, const char *str)
{
	short dlg_index;
	short the_type;
	Handle the_handle = NULL;
	Rect the_rect;
	Str255 store_ptr;
	
	if ((dlg_index = cd_get_dlg_index(dlog_num)) < 0) {
		SysBeep(50);
		return ;
	}
	strcpy((char *) store_ptr,str);
	c2p(store_ptr);
	GetDialogItem( (DialogPtr) dlgs[dlg_index], item_num, &the_type, &the_handle, &the_rect );
	//printf("item #%i, type=%i, text=%s\n",item_num,dlg_item_type[item_num],str);
	SetDialogItemText ( the_handle, store_ptr);
	SelectDialogItemText ((DialogPtr) dlgs[dlg_index], item_num, (int)strlen(str),(int)strlen(str));
}
// NOTE!!! Expects a c string
void cd_set_text_edit_num(short dlog_num, short item_num, short num)
{
	short dlg_index;
	short the_type;
	Handle the_handle = NULL;
	Rect the_rect;
	Str255 store_ptr;
	
	if ((dlg_index = cd_get_dlg_index(dlog_num)) < 0) {
		SysBeep(50);
		return ;
	}
		
	sprintf((char *) store_ptr,"%d",num);
	c2p(store_ptr);
	GetDialogItem((DialogPtr) dlgs[dlg_index], item_num, &the_type, &the_handle, &the_rect );
	SetDialogItemText ( the_handle, store_ptr);	
	SelectDialogItemText ((DialogPtr) dlgs[dlg_index], item_num,32767,32767);
}

void cd_set_item_text(short dlog_num, short item_num, const char *str)
{
	short dlg_index,item_index;
	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return ;
	if (item_index >= 150) {
		beep();
		return;
	}
	if (item_index < 10)
		sprintf(text_long_str[item_index],"%s",str);
	else
		sprintf(text_short_str[item_index - 10],"%-39.39s",str);
	cd_draw_item( dlog_num,item_num);
}

void cd_set_item_num(short dlog_num, short item_num, short num)
{
	short dlg_index,item_index;
	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return ;
	if (item_index >= 150) {
		beep();
		return;
	}
	if (item_index < 10)
		sprintf(text_long_str[item_index],"%d",num);
	else
		sprintf(text_short_str[item_index - 10],"%d",num);
	cd_draw_item( dlog_num,item_num);
}

void cd_set_led(short dlog_num,short item_num,short state)
{
	short dlg_index,item_index;

	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;

	if (dlg_item_type[item_index] != 2) {
		beep();
		return;
	}
	item_flag[item_index] = state;
	cd_draw_item(dlog_num,item_num);
}

// Use when checking for events ... if item hit matches item num, this led was
// hit, and flip it to other thing.
void cd_flip_led(short dlog_num,short item_num,short item_hit)
{
	if (item_hit != item_num)
		return;
	if (cd_get_led(dlog_num,item_num) > 0)
		cd_set_led(dlog_num,item_num,0);
	else
		cd_set_led(dlog_num,item_num,1);
}

void cd_set_led_range(short dlog_num,short first_led,short last_led,short which_to_set)
{
	short i;
	
	for (i = first_led; i <= last_led; i++) {
		if (i - first_led == which_to_set)
			cd_set_led(dlog_num,i,1);
		else
			cd_set_led(dlog_num,i,0);
	}
}

// Use when someone presses something in the range, to light up the right button
// Also has an error check ... if which_to_set is outside range, do nothing
void cd_hit_led_range(short dlog_num,short first_led,short last_led,short which_to_set)
{
	short i;
	
	if ((which_to_set < first_led) || (which_to_set > last_led))
		return;
	for (i = first_led; i <= last_led; i++) {
		if (i == which_to_set)
			cd_set_led(dlog_num,i,1);
		else
			cd_set_led(dlog_num,i,0);
	}
}
short cd_get_led_range(short dlog_num,short first_led,short last_led)
{
	short i;
	
	for (i = first_led; i <= last_led; i++) {
		if (cd_get_led(dlog_num,i) == 1)
			return i - first_led;
	}
	return 0;
}

void cd_set_flag(short dlog_num,short item_num,short flag)
{
	short dlg_index,item_index;

	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;

	item_flag[item_index] = flag;
	cd_draw_item(dlog_num,item_num);
}


short cd_get_led(short dlog_num,short item_num)
{
	short dlg_index,item_index;

	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return 0;

	if (dlg_item_type[item_index] != 2) {
		beep();
		return 0;
	}
	return item_flag[item_index];
}


void cd_text_frame(short dlog_num,short item_num,short frame)
{
	short dlg_index,item_index;

	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;

	if (item_index >= 150) {
		beep();
		return;
	}
	item_flag[item_index] = frame;
	cd_draw_item(dlog_num,item_num);
}

void cd_add_label(short dlog_num, short item_num, char *label, short label_flag)
{
	short dlg_index,item_index,label_loc = -1;
	short i;

	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;

	if (item_label_loc[item_index] < 0) {
		item_label[item_index] = label_flag;
		for (i = 0; i < 100; i++)
			if (label_taken[i] == FALSE) {
				label_loc = i;
				label_taken[i] = TRUE;
				i = 100;
			}
		if (label_loc < 0) {
			beep();
			return;
		}
		item_label_loc[item_index] = label_loc;
	}
	else 
		cd_erase_item(dlog_num,item_num + 100);
	label_loc = item_label_loc[item_index];
	sprintf((char *) labels[label_loc],"%-24s",label);
	if (item_active[item_index] > 0)
		cd_draw_item(dlog_num,item_num);
}

void cd_take_label(short dlog_num, short item_num)
{
	short dlg_index,item_index;

	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;
	item_label[item_index] = 0;
	label_taken[item_label_loc[item_index]] = FALSE;
}

void cd_key_label(short dlog_num, short item_num,short loc)
{
	short dlg_index,item_index;
	char str[10];
	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;
	sprintf((char *) str,"  ");
	str[0] = item_key[item_index];
	cd_add_label(dlog_num,item_num, str, 7 + loc * 100);
}

void cd_draw_item(short dlog_num,short item_num)
{
	short dlg_index,item_index,store_label;
	RGBColor c[4] = {{0,0,0},{65535,0,0},{61184,52530,12544},{15000,3000,3000}};
	Rect from_rect,to_rect;
	GrafPtr old_port;

	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;
	if (dlg_draw_ready[dlg_index] == FALSE)
		return;
	
	GetPort(&old_port);
	SetPort(GetWindowPort(GetDialogWindow((DialogPtr)dlgs[dlg_index])));
	TextFont(geneva_font_num);
	TextFace(bold);
	TextSize(10);
	
	if (item_active[item_index] == 0) {
		cd_erase_item(dlog_num,item_num);
		cd_erase_item(dlog_num,item_num + 100);
	}
	else {
		switch (dlg_item_type[item_index]) {
			case 0: case 1: case 10: case 11:
				from_rect = get_graphic_rect(dlg_buttons_gworld[button_type[item_flag[item_index]]][0]);
				rect_draw_some_item(dlg_buttons_gworld[button_type[item_flag[item_index]]][0],from_rect,
				 dlg_buttons_gworld[button_type[item_flag[item_index]]][0],item_rect[item_index],0,2);
				RGBForeColor(&c[2]);
				TextSize(12);
				if (dlg_item_type[item_index] == 10){
					ForeColor(whiteColor);
					TextSize(8);
				}
				if (dlg_item_type[item_index] < 2)
					OffsetRect(&item_rect[item_index],-1 * button_left_adj[item_flag[item_index]],0);
				if (dlg_item_type[item_index] < 2) {
					char_win_draw_string(GetWindowPort(GetDialogWindow((DialogPtr)dlgs[dlg_index])),item_rect[item_index],
					 (char *) (button_strs[item_flag[item_index]]),1,8);
				}
				else {
					char_win_draw_string(GetWindowPort(GetDialogWindow((DialogPtr)dlgs[dlg_index])),item_rect[item_index],
						 (char *) ((item_index < 10) ? text_long_str[item_index] : 
					text_short_str[item_index - 10]),1,8);
				}
				if (dlg_item_type[item_index] < 2)
					OffsetRect(&item_rect[item_index],button_left_adj[item_flag[item_index]],0);
				TextSize(10);
				ForeColor(blackColor);
				break;

			case 2:
				from_rect = get_graphic_rect(dlg_buttons_gworld[9][0]);
				switch (item_flag[item_index]) {
					case 0: rect_draw_some_item( dlg_buttons_gworld[10][0],from_rect, dlg_buttons_gworld[10][0],item_rect[item_index],0,2); break;
					case 1: rect_draw_some_item( dlg_buttons_gworld[9][1],from_rect, dlg_buttons_gworld[9][1],item_rect[item_index],0,2); break;
					case 2: rect_draw_some_item( dlg_buttons_gworld[9][0],from_rect, dlg_buttons_gworld[9][0],item_rect[item_index],0,2); break;
				}
				break;

			case 3: case 4: case 7: case 8: case 9:
				cd_erase_item(dlog_num,item_num);
				if (dlg_item_type[item_index] == 4)
					TextFace(0);
				if (dlg_item_type[item_index] == 7)
					TextSize(12); 
				ForeColor(blackColor);
				if (item_flag[item_index] % 10 == 1)
					cd_frame_item(dlog_num,item_num,2);

				if (item_flag[item_index] >= 10) {
					RGBForeColor(&c[1]);
				}
				if (item_rect[item_index].bottom - item_rect[item_index].top < 20) {
					item_rect[item_index].left += 3;
						char_win_draw_string(GetWindowPort(GetDialogWindow((DialogPtr)dlgs[dlg_index])),item_rect[item_index],
						 (char *) ((item_index < 10) ? text_long_str[item_index] : 
						  text_short_str[item_index - 10]),3,12);
					item_rect[item_index].left -= 3;
				}
				else {
					InsetRect(&item_rect[item_index],4,4);
					char_win_draw_string(GetWindowPort(GetDialogWindow((DialogPtr)dlgs[dlg_index])),item_rect[item_index],
					 (char *) ((item_index < 10) ? text_long_str[item_index] : 
					  text_short_str[item_index - 10]),0,(dlg_item_type[item_index] == 7) ? 14 : 12);
					InsetRect(&item_rect[item_index],-4,-4);
				}
				TextFont(geneva_font_num);
				TextFace(0);
				TextFace(bold);
				TextSize(10); 
				ForeColor(blackColor);
				break;

			case 5:
				if (item_flag[item_index] == -1)
					cd_erase_item(dlog_num,item_num);
				else
					draw_dialog_graphic(GetWindowPort(GetDialogWindow((DialogPtr)dlgs[dlg_index])), item_rect[item_index], item_flag[item_index],(item_flag[item_index] >= 2000) ? FALSE : TRUE,0);
				break;
		}
	}

	if (item_label[item_index] != 0) {
		store_label = item_label[item_index];
		if (store_label >= 1000) {
			store_label -= 1000;
		}
		else {
			TextFace(0);
		}
		to_rect = item_rect[item_index];
		switch (store_label / 100) {
			case 0:
				to_rect.right = to_rect.left;
				to_rect.left -= 2 * (store_label % 100);
				break;
			case 1:
				to_rect.bottom = to_rect.top;
				to_rect.top -= 2 * (store_label % 100);
				break;
			case 2:
				to_rect.left = to_rect.right;
				to_rect.right += 2 * (store_label % 100);
				break;
			case 3:
				to_rect.top = to_rect.bottom;
				to_rect.bottom += 2 * (store_label % 100);
				break;
		}

		if (to_rect.bottom - to_rect.top < 14) {
			to_rect.bottom += (14 - (to_rect.bottom - to_rect.top)) / 2 + 1;
			to_rect.top -= (14 - (to_rect.bottom - to_rect.top)) / 2 + 1;
			}
			else OffsetRect(&to_rect, 0,(to_rect.bottom - to_rect.top) / 6);
		//cd_erase_rect(dlog_num,to_rect);
		if (item_active[item_index] != 0) {
			char_win_draw_string(GetWindowPort(GetDialogWindow((DialogPtr)dlgs[dlg_index])),to_rect,
				labels[item_label_loc[item_index]],2,12);

		}
		TextFace(bold);
	}

	TextFont(0);
	TextFace(0);
	TextSize(0);
	SetPort(old_port);
}

void cd_initial_draw(short dlog_num)
{
	short i,which_dlg = -1;

	for (i = 0; i < ND; i++)
		if ((dlg_keys[i] >= 0) && (dlg_types[i] == dlog_num))
			which_dlg = i;
	if (which_dlg < 0) {
		return;
		}
	dlg_draw_ready[which_dlg] = TRUE;

	cd_erase_item(dlog_num, 0);
	cd_draw(dlog_num);
}

void cd_draw(short dlog_num)
{
	short i,which_dlg = -1;

	for (i = 0; i < ND; i++){
		if ((dlg_keys[i] >= 0) && (dlg_types[i] == dlog_num))
			which_dlg = i;
	}
	if (which_dlg < 0)
		return;
	for (i = 0; i < dlg_highest_item[which_dlg] + 1; i++) {
		cd_draw_item(dlog_num,i);
	}
}

void cd_redraw(WindowPtr window)
{
	short which_dlg,dlg_num=-1,dlg_key;

	if ((which_dlg = cd_find_dlog(window,&dlg_num,&dlg_key)) < 0)
		return;
	dlg_draw_ready[which_dlg] = TRUE;
	cd_initial_draw(dlg_num);
}

void cd_frame_item(short dlog_num, short item_num, short width)
{
	short dlg_index,item_index;

	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;
	frame_dlog_rect(GetWindowPort(GetDialogWindow((DialogPtr)dlgs[dlg_index])), item_rect[item_index], width);
}

void cd_erase_item(short dlog_num, short item_num)
// if item_num is 0, nail whole window
// item_num + 100  just erase label
{
	short i,dlg_index,item_index,store_label;
	Rect to_fry;
	Boolean just_label = FALSE;
	GrafPtr old_port;

	if (item_num >= 100) {
		item_num -= 100;
		just_label = TRUE;
	}

	if (item_num == 0) {
		for (i = 0; i < ND; i++)
			if ((dlg_keys[i] >= 0) && (dlg_types[i] == dlog_num))
				dlg_index = i;
			//GetWindowRect(dlgs[dlg_index],&to_fry);
			GetWindowPortBounds (GetDialogWindow((DialogPtr) dlgs[dlg_index]), &to_fry);
			//OffsetRect(&to_fry,-1 * to_fry.left,-1 * to_fry.top);
	}
	else {
		if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
			return;
		to_fry = item_rect[item_index];
		if (just_label == TRUE) {
			if (item_label[item_index] != 0) {
				store_label = item_label[item_index];
				if (store_label >= 1000)
					store_label -= 1000;

				switch (store_label / 100) {
					case 0:
						to_fry.right = to_fry.left;
						to_fry.left -= 2 * (store_label % 100);
						break;
					case 1:
						to_fry.bottom = to_fry.top;
						to_fry.top -= 2 * (store_label % 100);
						break;
					case 2:
						to_fry.left = to_fry.right;
						to_fry.right += 2 * (store_label % 100);
						break;
					case 3:
						to_fry.top = to_fry.bottom;
						to_fry.bottom += 2 * (store_label % 100);
						break;
				}
				if ((i = 12 - (to_fry.bottom - to_fry.top)) > 0) {
					// adjust rect ... but doesn't work for bold letters
					to_fry.bottom += i / 2;
					to_fry.bottom++; // extra pixel to get dangly letters
					to_fry.top -= i / 2;
				}
			}
		}
		InsetRect(&to_fry,-1,-1);

	}
	if (dlg_draw_ready[dlg_index] == FALSE) {
		return;
	}
	//GetPort(&old_port);
	//SetPort((GrafPtr) dlgs[dlg_index]);
	//FillCRect(&to_fry,bg[5]);

	//paint_pattern(GetWindowPort(GetDialogWindow((DialogPtr)dlgs[dlg_index])),2,to_fry,1);
	GetPort(&old_port);
	SetPort(GetWindowPort(GetDialogWindow((DialogPtr)dlgs[dlg_index])));
	paint_pattern(GetWindowPort(GetDialogWindow((DialogPtr)dlgs[dlg_index])),2,to_fry,1);

	// Now 2nd part of frame processing
	if (item_num == 0) {
		place_dlog_borders_around_rect(NULL,dlgs[dlg_index],to_fry);
	}
		
	SetPort(old_port);
}

void cd_erase_rect(short dlog_num,Rect to_fry)
{
	short dlg_index;
	GrafPtr old_port;

	if ((dlg_index = cd_get_dlg_index(dlog_num)) < 0)
		return;
	if (dlg_draw_ready[dlg_index] == FALSE)
		return;
	
	GetPort(&old_port);
	SetPort(GetWindowPort(GetDialogWindow((DialogPtr)dlgs[dlg_index])));
	FillCRect(&to_fry,bg[5]);
	SetPort(old_port);
}


// patch to change the mouse click behavior on the dialog button/radio button
//	mode
//	eDLGBtnResCompatible,	// change graphics of the button - sound and delay - recover graphics
//	eDLGBtnResChange,		// change graphics of the button - sound (no delay)
//	eDLGBtnResRecover,		// recover graphics of the button

void cd_press_button(short dlog_num, short item_num, EDLGBtnRes mode )
{
	short dlg_index,item_index;
	unsigned long dummy;
	Rect from_rect;
	GrafPtr old_port;
	RGBColor c[2] = {{26265,18944,6272},{61184,52530,12544}};//{{0,0,4096},{0,0,8192}}; 206,148,49
	
	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;

	// no press action for radio buttons
	//removed the restriction to eDLGBtnResCompatible mode so that radio buttons don't get random bits of text drawn over them when clicked
	if (dlg_item_type[item_index] == 2 /*&& mode == eDLGBtnResCompatible*/) {
		play_sound(34);
		return;
	}
	
	GetPort(&old_port);
	SetPort(GetWindowPort( GetDialogWindow((DialogPtr)dlgs[dlg_index])));
	TextFont(geneva_font_num);
	TextFace(bold);
	TextSize(10);

	// change graphics of the button
	if ( (mode == eDLGBtnResCompatible) || (mode == eDLGBtnResChange) ){
		ForeColor(blackColor);
		TextSize(12);
		from_rect = get_graphic_rect(dlg_buttons_gworld[button_type[item_flag[item_index]]][0]);
		rect_draw_some_item(dlg_buttons_gworld[button_type[item_flag[item_index]]][1],from_rect,
		 dlg_buttons_gworld[button_type[item_flag[item_index]]][1],item_rect[item_index],0,2);
		TextFace(bold);
		RGBForeColor(&c[0]);
		if (dlg_item_type[item_index] < 2) {
			OffsetRect(&item_rect[item_index],-1 * button_left_adj[item_flag[item_index]],0);
			char_win_draw_string(GetWindowPort(GetDialogWindow((DialogPtr)dlgs[dlg_index])),item_rect[item_index],
			 (char *) (button_strs[item_flag[item_index]]),1,8);
			OffsetRect(&item_rect[item_index],button_left_adj[item_flag[item_index]],0);
		}
		else {
			char_win_draw_string(GetWindowPort(GetDialogWindow((DialogPtr)dlgs[dlg_index])),item_rect[item_index],
			 (char *) ((item_index < 10) ? text_long_str[item_index] : 
					   text_short_str[item_index - 10]),1,8);
		}


		// refresh graphis for this dialog
		if (using_osx) {
			RgnHandle  rgnHandle = NewRgn();	
			QDFlushPortBuffer(GetWindowPort(GetDialogWindow((DialogPtr)dlgs[dlg_index])), 
			  GetPortVisibleRegion(GetWindowPort(GetDialogWindow((DialogPtr)dlgs[dlg_index])), rgnHandle));
			DisposeRgn(rgnHandle);	
		}
	}
	
	// sound
	if ( mode == eDLGBtnResCompatible ){
		if (play_sounds == TRUE)
			play_sound(37);
	}
	if ( mode == eDLGBtnResChange ){
		if (dlg_item_type[item_index] == 2)		// radio button
			play_sound(34);
		else if (play_sounds == TRUE)
			play_sound(37);
	}
	
	// delay
	if ( mode == eDLGBtnResCompatible ) {
		if (play_sounds == TRUE)
			Delay(6,&dummy);
		else
			Delay(14,&dummy);
	}

	// recover graphics of the button
	if ( (mode == eDLGBtnResCompatible) || (mode == eDLGBtnResRecover) ) {
		ForeColor(blackColor);
		TextSize(12);
		rect_draw_some_item(dlg_buttons_gworld[button_type[item_flag[item_index]]][0],from_rect,
		 dlg_buttons_gworld[button_type[item_flag[item_index]]][0],item_rect[item_index],0,2);

		RGBForeColor(&c[1]);
		if (dlg_item_type[item_index] < 2) {
			OffsetRect(&item_rect[item_index],-1 * button_left_adj[item_flag[item_index]],0);
					char_win_draw_string(GetWindowPort(GetDialogWindow((DialogPtr)dlgs[dlg_index])),item_rect[item_index],
					 (char *) (button_strs[item_flag[item_index]]),1,8);
			OffsetRect(&item_rect[item_index],button_left_adj[item_flag[item_index]],0);
		}
		else {
			char_win_draw_string(GetWindowPort(GetDialogWindow((DialogPtr)dlgs[dlg_index])),item_rect[item_index],
			 (char *) ((item_index < 10) ? text_long_str[item_index] : 
			  text_short_str[item_index - 10]),1,8);
		}

		// refresh graphis for this dialog
		if (using_osx) {
			RgnHandle  rgnHandle = NewRgn();	
			QDFlushPortBuffer(GetWindowPort(GetDialogWindow((DialogPtr)dlgs[dlg_index])), 
			  GetPortVisibleRegion(GetWindowPort(GetDialogWindow((DialogPtr)dlgs[dlg_index])), rgnHandle));
			DisposeRgn(rgnHandle);	
			}
	}

	TextFont(0);
	TextFace(0);
	TextSize(12);
	ForeColor(blackColor);
	// delay
	if ( mode == eDLGBtnResCompatible )
		Delay(8,&dummy);

	SetPort(old_port);
}

// LOW LEVEL

short cd_get_indices(short dlg_num, short item_num, short *dlg_index, short *item_index)
{
	if ((*dlg_index = cd_get_dlg_index(dlg_num)) < 0)
		return -1;
	if ((*item_index = cd_get_item_id(dlg_num,item_num)) < 0)
		return -1;
	return 0;
}

short cd_get_dlg_index(short dlog_num)
{
	short i;

	for (i = 0; i < ND; i++){
		if ((dlg_keys[i] >= 0) && (dlg_types[i] == dlog_num))
			return i;
	}
	return -1;
}

short cd_find_dlog(WindowPtr window, short *dlg_num, short *dlg_key)
{
	short i;
	for (i = 0; i < ND; i++){
		if ((dlg_keys[i] >= 0) && (dlgs[i] == window)) {
			*dlg_num = dlg_types[i];
			*dlg_key = dlg_keys[i];
			return i;
		}
	}
	return -1;
}

short cd_get_item_id(short dlg_num, short item_num)
{
	short i;

	for (i = 0; i < NI; i++){
		if ((item_dlg[i] == dlg_num) && (item_number[i] == item_num))
			return i;
	}
	return -1;
}

Rect get_item_rect(WindowPtr hDlg, short item_num)
{
	short the_type;
	Handle the_handle = NULL;
	Rect small_rect;

	GetDialogItem((DialogPtr) hDlg, item_num, &the_type, &the_handle, &small_rect);
	
	return small_rect;
}

void frame_dlog_rect(GrafPtr hDlg, Rect rect, short val)
{
	RGBColor lt_gray = {20320,8825,8825},dk_gray = {16320,3825,3825};
	GrafPtr old_port;
	
	GetPort(&old_port);
	SetPort((GrafPtr) hDlg);
	
	InsetRect(&rect,-1 * val,-1 * val);

	RGBForeColor(&dk_gray);
	MoveTo(rect.left,rect.top);
	LineTo(rect.right,rect.top);
	RGBForeColor(&lt_gray);
	LineTo(rect.right,rect.bottom);
	LineTo(rect.left,rect.bottom);
	RGBForeColor(&dk_gray);
	LineTo(rect.left,rect.top);
	ForeColor(blackColor);
	SetPort(old_port);

/*	RGBColor lt_gray = {57344,57344,57344},dk_gray = {12287,12287,12287},med_gray = {24574,24574,24574};
	GrafPtr old_port;

	GetPort(&old_port);
	SetPort((GrafPtr) hDlg);
	
	InsetRect(&rect,-1 * val,-1 * val);

	RGBForeColor(&dk_gray);
	MoveTo(rect.left,rect.top);
	LineTo(rect.right,rect.top);
	if (hDlg == mainPtr)
		RGBForeColor(&med_gray);
		else RGBForeColor(&lt_gray);
	LineTo(rect.right,rect.bottom);
	LineTo(rect.left,rect.bottom);
	RGBForeColor(&dk_gray);
	LineTo(rect.left,rect.top);
	ForeColor(blackColor);
	SetPort(old_port);*/
}

void draw_dialog_graphic(GrafPtr hDlg, Rect rect, short which_g, Boolean do_frame,short win_or_gworld)
// Won't be any graphics in dialogs for this editor
{
}

Rect calc_rect(short i, short j)
{
	Rect base_rect = {0,0,36,28};
	
	OffsetRect(&base_rect,i * 28,j * 36);
	return base_rect;
}

Rect calc_from_rect(short i, short j)
{
	Rect base_rect = {0,0,36,28};
	
	OffsetRect(&base_rect,1 + i * 29,1 + j * 37);
	return base_rect;
}

Rect get_graphic_rect(GWorldPtr graf)
{
	Rect dummy = {0,0,0,0};
	if (graf == NULL)
		return dummy;
	GetPortBounds(graf, &dummy);
	return dummy;
}