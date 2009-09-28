#pragma once

//http://www.ctduiattorney.com/dui_information/calculating_bac.html
class bac
{
public:
	bac(void);
	bac(int st, int wt, double dst, double hrs);
	~bac(void);
	double getlevel(int count);
	void reset();
	int start;		//start time
	int weight;		//weight in pounds
	double dist;	//gender distribution ratio men: .73, women: .66
	double hours;	//time in seconds from start
};
