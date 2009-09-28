#include "StdAfx.h"
#include "user.h"
#include <string>

using namespace std;

user::user(void)
{
}

user::user(string nm, string tg)
{
	name = nm;
	tag = tg;
	notes = "0";
}

user::user(string nm, string tg, string note = "0")
{
	name = nm;
	tag = tg;
	if(note == "")
		notes = "0";
	else
		notes = note;
}

//user::user(string nm, string tg, string note, bac level)
//{
//	name = nm;
//	tag = tg;
//	notes = note;
//	stats = level;
//}
user::~user(void)
{
}

string user::getname()
{
	return name;
}

string user::gettag()
{
	return tag;
}

string user::getnote()
{
	return notes;
}

void user::setname(string n)
{
	name = n;
}

void user::settag(string t)
{
	tag = t;
}

void user::setnote(string nt)
{
	notes = nt;
}

//void user::setstats(bac in)
//{
//	stats = in;
//}

//bac user::getstats()
//{
//	return stats;
//}