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
#include <vector>

extern town_record_type town;
extern scen_item_data_type scen_data;

// DBUG UTILITIES

void RedAlert_c (char *theStr)
{
	Str255 new_str;
	
	strcpy((char *) new_str,theStr);
	c2p(new_str);
	display_error_dialog(new_str,TRUE);
}

void ASB (const char *theStr)
{
	Str255 new_str;
	
	strcpy((char *) new_str,theStr);
	c2p(new_str);
	display_error_dialog(new_str,FALSE);
}

void ASB_big (const char *str1,const char *str2,const char *str3,const char *str4,short num,const char *str5)
{
	Str255 str;
	
	if (num >= 0)
		sprintf((char *) str,"%s%s%s%s%d%s",str1,str2,str3,str4,num,str5);
		else sprintf((char *) str,"%s%s%s%s%s",str1,str2,str3,str4,str5);
	c2p(str);
	display_error_dialog(str,FALSE);
}

void ASB_big_color (const char *str1,const char *str2,const char *str3,const char *str4,short num,const char *str5,short dummy)
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

	ParamText(theStr, "\p", "\p", "\p");		// Replace ^0 in alert with error mssg.
	Alert(kRedAlertID, 0L);                     // Bring up alert.
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
short rect_width (const Rect *theRect)
{
	return (theRect->right - theRect->left);
}

// Handy function for returning the absolute height of a rectangle.
short rect_height (const Rect *theRect)
{
	return (theRect->bottom - theRect->top);
}

// Other rectangle functions
Boolean rects_touch(const Rect *r1,const Rect *r2)
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

terrain_type_type get_ter(short which_ter)
{
	terrain_type_type ter;
		ter = scen_data.scen_terrains[which_ter];
	return ter;
}

int coord2Index(int coord, int base, int step){
	int numer=coord-base;
	int result=numer/step;
	if(numer<0)
		result-=1;
	return(result);
}

//Finds the largest possible rectangle which can be fit inside of bounds
//without overlapping any of the rectangles in obstacles.
//This function has poor worst case behavior (~O(4^obstacles.size()))
//but should be tolerable when the number of obstacle rectangles is small (~<5)
Rect largestNonOverlapping(Rect bounds, const Rect* obstaclesBegin, const Rect* obstaclesEnd){
	std::vector<Rect> possibilities(1,bounds);
	for(const Rect* obstacle=obstaclesBegin;
	    obstacle!=obstaclesEnd; obstacle++){
		std::vector<Rect> updatedPossibilities;
		for(std::vector<Rect>::const_iterator possible=possibilities.begin(), possibleEnd=possibilities.end();
	        possible!=possibleEnd; possible++){
	    	if(!rects_touch(&*obstacle,&*possible))
	    		updatedPossibilities.push_back(*possible);
	    	else{
	    		if(obstacle->top>possible->top){
	    			Rect r=*possible;
	    			r.bottom=obstacle->top;
	    			updatedPossibilities.push_back(r);
	    		}
	    		if(obstacle->left>possible->left){
	    			Rect r=*possible;
	    			r.right=obstacle->left;
	    			updatedPossibilities.push_back(r);
	    		}
	    		if(obstacle->bottom<possible->bottom){
	    			Rect r=*possible;
	    			r.top=obstacle->bottom;
	    			updatedPossibilities.push_back(r);
	    		}
	    		if(obstacle->right<possible->right){
	    			Rect r=*possible;
	    			r.left=obstacle->right;
	    			updatedPossibilities.push_back(r);
	    		}
	    	}
	    }
		possibilities.swap(updatedPossibilities);
	}
	Rect result=bounds;
	long bestArea=0;
	for(std::vector<Rect>::const_iterator possible=possibilities.begin(), possibleEnd=possibilities.end();
	    possible!=possibleEnd; possible++){
	    long area=possible->bottom-possible->top;
		area*=possible->right-possible->left;
		if(area>bestArea){
			bestArea=area;
			result=*possible;
		}
	}
	return(result);
}