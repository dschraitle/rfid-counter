#pragma once
#include <string>
#include <vector>
#include "user.h"
#include "bac.h"

using namespace std;

class usertags
{
public:
	usertags(void);
	~usertags(void);
	void adduser(string user, string tag);
	void adduser(string user, string tag, string note);
	//void adduser(string user, string tag, string note, bac level);
	void importfile(char* filename);
	void exportfile();
	string gettag(string user);
	string gettag(int user);
	string getuser(string tag);
	string getuser(int tag);
	string getnote(string user);
	string getnote(int user);
	//bac getbac(string user);
	//bac getbac(int user);
	//void setbac(string user, bac in);
	//void setbac(int user, bac in);
	void setnote(string user, string note);
	void setnote(int user, string note);
	user getwholeuser(string name);
	user getwholeuserfromtag(string tag);
	user getwholeuser(int user);
	void modifyuser(string user, string newtag);
	bool removeuser(string user);
	bool removeuser(int user);
	void listall();
	int size();
	string notes;
private:
	vector<user> users;
	char* file;
	bool importing;
};
