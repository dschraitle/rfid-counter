#pragma once

#include <string>
#include "bac.h"
using namespace std;

class user
{
public:
	user(void);
	user(string nm, string tg);
	user(string nm, string tg, string note);
	//user(string nm, string tg, string note, bac level);
	~user(void);
	string getname();
	string gettag();
	string getnote();
	//bac getstats();
	void setname(string n);
	void settag(string t);
	void setnote(string nt);
	//void setstats(bac in);
private:
	string name;
	string tag;
	string notes;
	//bac stats;
};
