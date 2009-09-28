#include "StdAfx.h"
#include "bac.h"
#include <ctime>

bac::bac(void)
{
	start = 0;
	weight = 0;
	dist = .73;
	hours = 0;
}

bac::bac(int st, int wt, double dst, double hrs)
{
	start = st;
	weight = wt;
	dist = dst;
	hours = hrs;
}

bac::~bac(void)
{
}

double bac::getlevel(int count)
{
	double alc = (.55 * (count - 1));
	double level = (alc * (5.14/weight) * dist) - (.015 * hours);
	if(level > 0)
		return level;
	else
		return 0;
}

void bac::reset()
{
	start = time(NULL);
	hours = 0;
}