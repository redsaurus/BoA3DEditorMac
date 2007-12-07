//============================================================================
//----------------------------------------------------------------------------
//									EdUtils.c
//----------------------------------------------------------------------------
//============================================================================

// General utility functions.

//#include <Carbon/Carbon.h>	// included in pre-compiled header
//#include "string.h"
//#include "stdio.h"
#include "global.h"

extern town_record_type town;
extern scen_item_data_type scen_data;

// DBUG UTILITIES

// Used to stop game when an error happens
void debug_stop(short which_error)
{
//	short i = which_error;
}

void RedAlert_c (char *theStr)
{
	Str255 new_str;
	
	strcpy((char *) new_str,theStr);
	c2p(new_str);
	display_error_dialog(new_str,TRUE);
}

void ASB (char *theStr)
{
	Str255 new_str;
	
	strcpy((char *) new_str,theStr);
	c2p(new_str);
	display_error_dialog(new_str,FALSE);
}

void ASB_big (char *str1,char *str2,char *str3,char *str4,short num,char *str5)
{
	Str255 str;
	
	if (num >= 0)
		sprintf((char *) str,"%s%s%s%s%d%s",str1,str2,str3,str4,num,str5);
		else sprintf((char *) str,"%s%s%s%s%s",str1,str2,str3,str4,str5);
	c2p(str);
	display_error_dialog(str,FALSE);
}

void ASB_big_color (char *str1,char *str2,char *str3,char *str4,short num,char *str5,short dummy)
{
	Str255 str;
	
	if (num >= 0)
		sprintf((char *) str,"%s%s%s%s%d%s",str1,str2,str3,str4,num,str5);
		else sprintf((char *) str,"%s%s%s%s%s",str1,str2,str3,str4,str5);
	c2p(str);
	display_error_dialog(str,FALSE);
}
void RedAlert (char * theStr)
{
	Str255 str;

	c2pstrcpy( str, theStr );
	display_error_dialog(str,TRUE);
}

void RedAlert_big_color (char *str1,char *str2,char *str3,char *str4,short num,char *str5,short color)
{
	Str255 str;
	
	if (num >= 0)
		sprintf((char *) str,"%s%s%s%s%d%s",str1,str2,str3,str4,num,str5);
		else sprintf((char *) str,"%s%s%s%s%s",str1,str2,str3,str4,str5);
	c2p(str);
	display_error_dialog(str,TRUE);
}

void RedAlert_big (char *str1,char *str2,char *str3,char *str4,short num,char *str5)
{
	Str255 str;
	
	if (num >= 0)
		sprintf((char *) str,"%s%s%s%s%d%s",str1,str2,str3,str4,num,str5);
		else sprintf((char *) str,"%s%s%s%s%s",str1,str2,str3,str4,str5);
	c2p(str);
	display_error_dialog(str,TRUE);
}

void display_error_dialog(StringPtr theStr,Boolean shut_down)
{
	#define		kRedAlertID		128
	short		whoCares;

	ParamText(theStr, "\p", "\p", "\p");		// Replace ^0 in alert with error mssg.
	whoCares = Alert(kRedAlertID, 0L);			// Bring up alert.
	if (shut_down == TRUE)
		ExitToShell();								// Quit to Finder.
}

void ZeroRectCorner (Rect *theRect)
{
	theRect->right -= theRect->left;	// Move right edge by amount of left.
	theRect->bottom -= theRect->top;	// Move bottom edge by amount of top.
	theRect->left = 0;					// Can now set left to zero.
	theRect->top = 0;					// Can set top edge to zero as well.
}


// Handy function for returning the absolute width of a rectangle.
short rect_width (Rect *theRect)
{
	return (theRect->right - theRect->left);
}

// Handy function for returning the absolute height of a rectangle.
short rect_height (Rect *theRect)
{
	return (theRect->bottom - theRect->top);
}

// Other rectangle functions
Boolean rects_touch(Rect *r1,Rect *r2)
{
	if (r1->right <= r2->left)
		return (FALSE);
		
	if (r1->left >= r2->right)
		return (FALSE);
		
	if (r1->top >= r2->bottom)
		return (FALSE);
		
	if (r1->bottom <= r2->top)
		return (FALSE);
		
	return TRUE;

}

short rect_dist(Rect *r1,Rect *r2)
{
	if (rects_touch(r1,r2))
		return 0;

	short cur_dist = 0;
		
	if ((r2->left >= r1->right) && (r2->left - r1->right > cur_dist))
		cur_dist = r2->left - r1->right;
		
	if ((r1->left >= r2->right) && (r1->left - r2->right > cur_dist))
		cur_dist = r1->left - r2->right;
		
	if ((r1->top >= r2->bottom) && (r1->top - r2->bottom > cur_dist))
		cur_dist = r1->top - r2->bottom;
		
	if ((r2->top >= r1->bottom) && (r2->top - r1->bottom > cur_dist))
		cur_dist = r2->top - r1->bottom;
		
	return cur_dist;
}

Boolean rect_empty(Rect *r)
{
	if ((r->top >= r->bottom) || (r->right <= r->left))
		return TRUE;
	return FALSE;
}

Rect rect_union(Rect *r1,Rect *r2)
{
	Rect r;
	
	UnionRect(r1,r2,&r);
	return r;
}

Rect rect_sect(Rect *r1,Rect *r2)
{
	Rect r;
	
	SectRect(r1,r2,&r);
	return r;
}

Boolean r1_in_r2(Rect r1,Rect r2)
{
	if ((r1.left >= r2.left) && (r1.top >= r2.top) && 
		(r1.bottom <= r2.bottom) && (r1.right <= r2.right))
			return TRUE;
	return FALSE;
}

// returns the given rect, but centered around point p
Rect rect_centered_around_point(Rect r, location l)
{
	short w = rect_width(&r);
	short h = rect_height(&r);
	SetRect(&r,l.x - (w / 2),l.y - (h / 2),l.x - (w / 2) + w,l.y - (h / 2) + h);
	return r;
}

void CenterRectInRect (Rect *rectA, Rect *rectB)
{
	short	widthA, tallA;
	
	widthA = rect_width(rectA);				// Get width of 1st rect.
	tallA = rect_height(rectA);				// Get height of 1st rect.
											// Do the math (center horizontally).
	rectA->left = rectB->left + (rect_width(rectB) - widthA) / 2;
	rectA->right = rectA->left + widthA;
											// Do the math (center vertically).
	rectA->top = rectB->top + (rect_height(rectB) - tallA) / 2;
	rectA->bottom = rectA->top + tallA;
}

Boolean same_string(char *str1,char *str2)
{
	short i = 0;
	
	while ((str1[i] == str2[i]) && (str1[i] != 0) && (str2[i] != 0))
		i++;
	if ((str1[i] == 0) && (str2[i] == 0))
		return TRUE;
	return FALSE;
}

// Returns TRUE is this location is in the active region of the zone
Boolean loc_in_active_area(location loc)
{
	return loc_touches_rect(loc,town.in_town_rect);
}

Boolean loc_in_rect(location loc,Rect r)
{
	if ((loc.x >= r.left) && (loc.x < r.right) && (loc.y >= r.top) && (loc.y < r.bottom))
		return TRUE;
	return FALSE;
}

// Returns TRUE is this location touches (is not strictly inside of) rectangle
Boolean loc_touches_rect(location loc,Rect r)
{
	if ((loc.x >= r.left) && (loc.x <= r.right) && (loc.y >= r.top) && (loc.y <= r.bottom))
		return TRUE;
	return FALSE;
}

Boolean loc_touch_rect(location loc,Rect r)
{
	if ((loc.x >= r.left) && (loc.x <= r.right) && (loc.y >= r.top) && (loc.y <= r.bottom))
		return TRUE;
	return FALSE;
}

short a_v(short x)
{
	if (x < 0)
		return (-1 * x);
		else return x;
}

terrain_type_type get_ter(short which_ter)
{
	terrain_type_type ter;
		ter = scen_data.scen_ter_types[which_ter];
	return ter;
}