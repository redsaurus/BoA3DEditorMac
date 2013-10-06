//#include <Carbon/Carbon.h>	// included in pre-compiled header
//#include "math.h"
//#include <OSUtils.h>
#include "global.h"

short get_ran (short times,short  min,short  max)
{
	unsigned int store;
	short i, to_ret = 0;
	
	for (i = 1; i < times + 1; i++) {
		store = random() & 0x7fff;
		to_ret = to_ret + min + ((store * (max - min + 1)) / 0x7fff);
		}
	return to_ret;
}

short s_pow(short x,short y)
{
	return (short) pow((double) x, (double) y);
}

short dist(location p1,location p2)
{
	return (short) sqrt((double)((p1.x - p2.x) * (p1.x - p2.x) +
							(p1.y - p2.y) * (p1.y - p2.y)));
}

short max(short a,short b)
{
	if (a > b)
		return a;
	else
		return b;
}

short min(short a,short b)
{
	if (a < b)
		return a;
	else
		return b;
}

short minmax(short min,short max,short k)
{
	if (k < min)
		return min;
	if (k > max)
		return max;
	return k;
}

Boolean same_point(location l1,location l2)
{
	return((l1.x == l2.x) && (l1.y == l2.y));
}