// RFIDCounter.cpp : Defines the entry point for the console application.
// i'm not gonna lie, the main structure of this is a direct copy from the phidgets example, even the comments
//

#include "stdafx.h"
#include "phidget21.h"
#include "usertags.h"
#include "utilities.h"
#include <windows.h>
#include <shellapi.h>
#include <sstream>
#include <ctime>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <time.h>

using namespace std;

int __stdcall DetachHandler(CPhidgetHandle RFID, void *userptr);
int __stdcall ErrorHandler(CPhidgetHandle RFID, void *userptr, int ErrorCode, const char *unknown);
int __stdcall TagHandler(CPhidgetRFIDHandle RFID, void *usrptr, unsigned char *TagVal);
int __stdcall TagLostHandler(CPhidgetRFIDHandle RFID, void *usrptr, unsigned char *TagVal);
bool allowentry(string tag);
void unlockdoor(CPhidgetRFIDHandle RFID);
void showhelp();
void listall();
void adduser(string input);
bool removeuser(string input);
void modifyuser(string input);
string lowercase(string s);
void docommand(string input);
string whitespace(string fill, int size, char touse);
string trailing(string fill, int space, char touse = ' ');
void showtaglist();
void addtag(string input);
bool removetag(string input);
string read(string& in, string readto);
void outputfile(string user, string in);
void swipe(string user);
void reset();
bool isNumber(string test);
void calcBAC();
DWORD begin;

usertags users;
usertags tagIDs;
string add[2];
bool connected = false;
bool adding = false;
bool tagadd = false;
//Declare an RFID handle
CPhidgetRFIDHandle rfid = 0;

int __stdcall AttachHandler(CPhidgetHandle RFID, void *userptr)
{
	int serialNo;
	const char *name;
	int result;
	const char *err;

	CPhidget_getDeviceName (RFID, &name);
	CPhidget_getSerialNumber(RFID, &serialNo);
	printf("%s %10d attached!\n", name, serialNo);

	
	CPhidgetRFID_create(&rfid);
	CPhidget_open((CPhidgetHandle)rfid, -1);	
	if((result = CPhidget_waitForAttachment((CPhidgetHandle)rfid, 5000)))
	{
		CPhidget_getErrorDescription(result, &err);
		printf("Problem waiting for attachment: %s\n", err);
	}
	else
	{
		connected = true;
		CPhidget_set_OnAttach_Handler((CPhidgetHandle)rfid, AttachHandler, NULL);
		CPhidget_set_OnDetach_Handler((CPhidgetHandle)rfid, DetachHandler, NULL);
		CPhidget_set_OnError_Handler((CPhidgetHandle)rfid, ErrorHandler, NULL);
		CPhidgetRFID_set_OnTag_Handler(rfid, TagHandler, NULL);
		CPhidgetRFID_set_OnTagLost_Handler(rfid, TagLostHandler, NULL);
	}

	return 0;
}

int __stdcall DetachHandler(CPhidgetHandle RFID, void *userptr)
{
	int serialNo;
	const char *name;

	CPhidget_getDeviceName (RFID, &name);
	CPhidget_getSerialNumber(RFID, &serialNo);
	printf("%s %10d detached!\n", name, serialNo);

	return 0;
}

int __stdcall ErrorHandler(CPhidgetHandle RFID, void *userptr, int ErrorCode, const char *unknown)
{
	printf("Error handled. %d - %s\n", ErrorCode, unknown);
	return 0;
}

bool allowentry(string tag)
{	
	if(users.getuser(tag) != tag)
		return true;
	return false;
}

void unlockdoor(CPhidgetRFIDHandle RFID)
{
	cout << "Door Changing\n";
	CPhidgetRFID_setOutputState(RFID, 0, 1);
	//Sleep(500);	
	//CPhidgetRFID_setOutputState(RFID, 0, 0);
}

int __stdcall TagHandler(CPhidgetRFIDHandle RFID, void *usrptr, unsigned char *TagVal)
{
	//turn on the Onboard LED
	CPhidgetRFID_setLEDOn(RFID, 1);
	char s[20];
	sprintf_s(s,"%02x%02x%02x%02x%02x",TagVal[0], TagVal[1], TagVal[2], TagVal[3], TagVal[4]);
	string tag = (string)s;

	if(adding)
	{
		if(tagadd)
		{
			tagIDs.adduser(add[0], tag, add[1]);
			cout << add[0] << " added with tag:" << tag;
		}
		else
		{
			tag = tagIDs.gettag(tag);
			users.adduser(add[0], tag, add[1]);
			cout << add[0] << " added with tag:" << tagIDs.getuser(tag) << "(" << tag << ")";
		}
		if(add[1] != "") 
			cout << " and note:" << add[1];
		cout << "\n";
	}
	else
	{
		/*time_t rawtime;
		struct tm * timeinfo;
		time ( &rawtime );
		timeinfo = localtime ( &rawtime );*/
		
		char t[100];
		sprintf_s(t,"%ld",time(NULL));

		cout << "Tag Read: " << users.getuser(tag) << " with tag: " << tagIDs.getuser(tag) << " at " << (string)t;//asctime(timeinfo);
		if(allowentry(tag))
		{
			swipe(users.getuser(tag));
			cout << ">";
		}
		else
			cout << "Access denied\n>";

	}
		adding = false;
		tagadd = false;

	//printf("Tag Read: %02x%02x%02x%02x%02x\n", TagVal[0], TagVal[1], TagVal[2], TagVal[3], TagVal[4]);
	return 0;
}

int __stdcall TagLostHandler(CPhidgetRFIDHandle RFID, void *usrptr, unsigned char *TagVal)
{
	//turn off the Onboard LED
	CPhidgetRFID_setLEDOn(RFID, 0);
	return 0;
}

void swipe(string user)
{
	string tag;
	if(user == (tag = users.gettag(user)))
	{
		cout << "no such person\n";
		return;
	}
	stringstream out;
	out << users.getnote(user);
	stringstream in;
	in << atoi(out.str().c_str())+1;
	users.setnote(user, in.str());
	listall();
	char t[100];
	sprintf_s(t,"%ld",time(NULL));
	//char* formatted = new char;
	//time_t rawtime;
	//struct tm * timeinfo;
	//time ( &rawtime );
	//timeinfo = localtime ( &rawtime );
	//strftime(formatted,20,"%H:%M-%m/%d",timeinfo);
	//string toput = (string)formatted + "," + (string)t;
	string toput = "," + (string)t;
	string names = ",";
	for (int i = 0; i < users.size(); i++)
	{
		names += "," + users.getuser(i);
		toput += "," + users.getnote(users.getuser(i));
	}
	//outputfile("log", users.getuser(tag) + " -- " + (string)asctime(timeinfo));
	outputfile("log"+names.substr(1),toput);
	outputfile(users.getuser(tag), (string)t + "," + users.getnote(users.getuser(tag)));
	//cout << ">";
}


void showhelp()
{
	int space = 10;
	int tag = 10;
	cout << "-----------------------------------------------------------------\n";
	cout << trailing("Command",space,' ') << "Description\n\n";
	cout << trailing("LIST",space,' ') << "list all users and their tag name\n";
	cout << trailing("TAGLIST",space,' ') << "lists all tags associated with system\n";
	cout << "ADD name [-T tag] [-NOTE note]\n" << whitespace("",space,' ') << "adds someone to the list\n";
	cout << "EDIT name [-T new tag] [-NOTE note]\n" << whitespace("",space,' ') << "changes the tag of the given person\n" << whitespace("",space,' ') << "\"--\" in note field uses old note\n";
	cout << "REMOVE name\n" << whitespace("",space,' ') << "removes someone from list\n";
	cout << "ADDTAG name [-NOTE note]\n" << whitespace("",space,' ') << "adds a new tag to the system\n";
	cout << "REMOVETAG name\n" << whitespace("",space,' ') << "removes a tag from the system\n";
	cout << trailing("SWIPE",space,' ') << "swipe the person\n";
	cout << trailing("RESET",space,' ') << "resets values for a new session\n";
	cout << trailing("HELP",space,' ') << "displays this list of commands\n";
	cout << trailing("QUIT",space,' ') << "closes the program\n";
	cout << "-----------------------------------------------------------------\n";
}


void outputfile(string user, string in)
{
	string filename = "";
	string titles = "";
	if(user.substr(0,3) == "log")
	{
		filename = "log.csv";
		titles = user.substr(3);
	}
	else if(user.substr(0,3) == "bac")
	{
		filename = "bac.csv";
		titles = user.substr(3);
	}
	else
	{
		filename = user + ".csv";
		titles = user;
	}
	string logstring = "";
	const char* fname = filename.c_str();
	if(ifstream(fname))
	{
		ifstream infile;
		infile.open(fname);
		string temp;
		if(!infile.eof())
			getline(infile,temp);
		while(!infile.eof())
		{
			getline(infile,temp);
			logstring += temp + "\n";
		}
		infile.close();
	}

	ofstream ofile(fname);
	ofile << "," << titles << "\n" << logstring << in;
	ofile.close();
}

string trailing(string fill, int space, char touse)
{
	return fill + whitespace(fill,space,touse);
}

string whitespace(string fill, int size, char touse)
{
	string white;
	int i = size-fill.length();
	if(i > 0)
		white.append(i,touse);
	return white;
}

void listall()
{
	cout << "-----------------------------------------------------------------\n";
	int namemax = 4;
	int tagmax = 8;
	for(int i = 0; i < users.size(); i++)
	{
		int temp;
		if((temp = users.getuser(i).length()) > namemax)
			namemax = temp;
		if((temp = tagIDs.getuser(users.gettag(i)).length()) > tagmax)
			tagmax = temp;
	}
	int space = namemax + 6;
	int buffer = tagmax + 3;
	//int back = 8;
	cout << trailing("    User",space,' ') << trailing("Tag Name",buffer,' ') << trailing("Hex Code",13) << "Count\n";	//<< trailing("BAC",back) << "Notes\n";
	char t[100];
	sprintf_s(t,"%ld",time(NULL)-1234567890);			
	string toput = (string)t;
	string names = "";
	for(int i = 0; i < users.size(); i++)
	{
		user temp = users.getwholeuser(i);
		//bac level = temp.getstats();
		//if(users.getnote(i) == "")
		//	level.start = time(NULL);
		//level.hours = ((double)time(NULL) - (double)level.start)/3600;
		stringstream out;
		out << (i+1);
		//char *outputbac = new char[6];
		char *lognum = new char[20];
		//double content = level.getlevel(atoi(users.getnote(i).c_str()));
		//sprintf_s(outputbac, 6, "%.3f", content);
		//sprintf_s(lognum,20,"%f",content);
		cout << trailing(trailing(out.str()+":",4,' ')+temp.getname(),space,' ')	//name
			<< trailing(tagIDs.getuser(temp.gettag()),buffer,' ')	//tag name
			<< trailing(temp.gettag(),13); //tag hex
			//<< trailing(outputbac,back);	//bac level
		//if(temp.getnote() != "")
			cout << temp.getnote();
		cout << "\n";
		//users.setbac(i,level);
		names += "," + users.getuser(i);
		toput += "," + (string)lognum;
	}
	//outputfile("bac"+names,toput);
	cout << "-----------------------------------------------------------------\n";
}

void showtaglist()
{
	cout << "-----------------------------------------------------------------\n";
	int namemax = 0;
	for(int i = 0; i < tagIDs.size(); i++)
	{
		int temp;
		if((temp = tagIDs.getuser(i).length()) > namemax)
			namemax = temp;
	}
	int space = namemax + 7;
	cout << trailing("    RFID Tag",space,' ') << "Hex Code\tNotes\n\n";
	for(int i = 0; i < tagIDs.size(); i++)
	{
		user temp = tagIDs.getwholeuser(i);
		stringstream out;
		out << (i+1);
		cout << trailing(trailing(out.str()+":",4,' ')+temp.getname(),space,' ') << temp.gettag();
		if(temp.getnote() != "")
			cout << "\t" << temp.getnote();
		cout << "\n";
	}
	cout << "-----------------------------------------------------------------\n";
}

string read(string& in, string readto)
{
	string ret = "";
	bool go = true;
	if(readto.length() == 1)
	{
		char readchar = readto[0];
		while(in.length() > 0 && go)
		{
			if(in[0] != readchar)
				ret.push_back(in[0]);
			else
				go = false;
			in.erase(0,1);
		}
	}
	else
	{
		int pos;
		ret = in;
		string temp = lowercase(in);
		if((pos = temp.find(readto)) != -1)
		{
			ret = in.substr(0,pos);
			in = in.substr(pos+readto.length());
		}
		else
			in = "";
	}
	return ret;
}

void adduser(string input)
{
	string in = input;
	string name, note;
	read(in," ");

	string temp = lowercase(in);
	if(temp.find(" -t ") != -1)
	{
		name = read(in, " -t ");
		string tag = read(in," -note ");
		note = in;

		tag = tagIDs.gettag(tag);
		users.adduser(name, tag, note);
		cout << name << " added with tag:" << tag;
		if(note != "")
			cout << " and note:" << note;
		cout << "\n";
	}
	else
	{
		add[0] = read(in, " -note ");
		add[1] = in;
		adding = true;
		cout << "Swipe RFID for it to be read into system\n";
		time_t start = time(NULL);
		while(adding)
		{
			if(difftime(time(NULL),start) == 10)
				adding = false;
		}
	}
}

void addtag(string input)
{
	string in = input;
	string name;
	read(in," ");

	add[0] = read(in," -note ");
	
	add[1] = in;
	adding = true;
	tagadd = true;
	cout << "Swipe RFID tag for it to be read into system\n";
	int i = 0;
	time_t start = time(NULL);
	while(adding)
	{
		if(difftime(time(NULL),start) == 10)
			adding = false;
	}
}

bool removeuser(string input)
{
	string name = input.substr(7);
	bool number = true;
	bool success;
	for each (char c in name)
		if(!isdigit(c))
			number = false;
	if(number)
		success = users.removeuser(atoi(name.c_str())-1);
	else
		success = users.removeuser(name);
	return success;
}

bool removetag(string input)
{
	string in = input;
	read(in, " ");
	string name = in;
	bool number = true;
	bool success;
	for each (char c in name)
		if(!isdigit(c))
			number = false;
	if(number)
		success = tagIDs.removeuser(atoi(name.c_str())-1);
	else
		success = tagIDs.removeuser(name);
	return success;
}

void modifyuser(string input)
{
	string in,name,tag,note;
	in = input;
	read(in," ");
	string temp = lowercase(in);
	if(temp.find(" -t ") == -1)
	{
		bool number = true;
		name = read(in," -note ");
		for each (char c in name)
			if(!isdigit(c))
				number = false;
		if(number)
			name = users.getuser(atoi(name.c_str())-1);
		note = in;
		if(note == "--")
			note = users.getnote(name);
		if(!removeuser("remove "+name))
			return;
		adduser("add " + name + " -note " + note);
	}
	else
	{
		bool number = true;
		name = read(in, " -t ");
		for each (char c in name)
			if(!isdigit(c))
				number = false;
		if(number)
			name = users.getuser(atoi(name.c_str())-1);
		tag = read(in, " -note ");
		note = in;
		if(note == "--")
			note = users.getnote(name);
		if(!removeuser("remove " + name))
			return;
		adduser("add " + name + " -t " + tag + " -note " + note);
	}
}

string lowercase(string s)
{
	string temp = s;
	for(int i = 0; i < (int)s.length(); i++)
		temp[i] = tolower(s[i]);
	return temp;
}

void reset()
{
	for(int i = 0; i < users.size(); i++)
	{
		//bac temp = users.getbac(i);
		//temp.reset();
		//users.setbac(i,temp);
		users.setnote(i,"0");
	}
}

bool isNumber(string test)
{
	for(int i = 0; i < test.length(); i++)
		if(!isdigit(test[i]) && test[i] != '-' && test[i] != '.' && test[i] != '\n')
			return false;
	return true;
}

void calcBAC()
{
	int count=0;
	string number, wt, hrs, gender, temp;
	double weight=1, hours=1, ratio;
	cout << "Count: ";
	cin >> number;
	cout << "Weight(kg): ";
	cin >> wt;
	cout << "Gender(m/f): ";
	cin >> gender;
	cout << "Hours: ";
	cin >> hrs;
	if(isNumber(number))
		count = atoi(number.c_str());
	getline(cin,temp);
	if(wt[0] == '-')
		weight = atof(wt.substr(1).c_str())/2.2;
	else
		weight = atof(wt.c_str());
	hours = atof(hrs.c_str());
	if(tolower(gender[0]) == 'm')
		ratio = .73;
	else
		ratio = .66;
	cout << "BAC: " << (count*.55 * (5.14/weight) * ratio) - (.015 * hours) << endl;
}

void docommand(string input)
{
	string lowerinput = lowercase(input);
	if(lowerinput.substr(0,4) == "help"){showhelp();return;}
	else if(lowerinput.substr(0,4) == "list"){listall();return;}
	else if(lowerinput.substr(0,4) == "add "){adduser(input);return;}
	else if(lowerinput.substr(0,7) == "remove "){removeuser(input);return;}
	else if(lowerinput.substr(0,5) == "edit "){modifyuser(input);return;}
	else if(lowerinput.substr(0,7) == "taglist"){showtaglist();return;}
	else if(lowerinput.substr(0,7) == "addtag "){addtag(input);return;}
	else if(lowerinput.substr(0,6) == "swipe "){swipe(input.substr(6));return;}
	else if(lowerinput.substr(0,5) == "reset"){reset();return;}
	else if(lowerinput.substr(0,10) == "removetag "){removetag(input);return;}
	else if(lowerinput.substr(0,3) == "bac"){calcBAC();return;}
	else if(lowerinput == "hello"){cout << "hello yourself\n";return;}
	else if(lowerinput == "why"){cout << "because i can\n";return;}
	cout << "That command doesn't exist\n";
}

//Display the properties of the attached phidget to the screen.  We will be displaying the name, serial number and version of the attached device.
//We will also display the nu,mber of available digital outputs
int display_properties(CPhidgetRFIDHandle phid)
{
	int serialNo, version, numOutputs, antennaOn, LEDOn;
	const char* ptr;

	CPhidget_getDeviceType((CPhidgetHandle)phid, &ptr);
	CPhidget_getSerialNumber((CPhidgetHandle)phid, &serialNo);
	CPhidget_getDeviceVersion((CPhidgetHandle)phid, &version);

	CPhidgetRFID_getOutputCount (phid, &numOutputs);
	CPhidgetRFID_getAntennaOn (phid, &antennaOn);
	CPhidgetRFID_getLEDOn (phid, &LEDOn);


	printf("%s\n", ptr);
	printf("Serial Number: %10d\nVersion: %8d\n", serialNo, version);
	printf("# Outputs: %d\n\n", numOutputs);
	printf("Antenna Status: %d\nOnboard LED Status: %d\n", antennaOn, LEDOn);

	return 0;
}

int rfid_simple()
{
	int result;
	const char *err;

	char* filename = "tagids.txt";
	tagIDs.importfile(filename);
	filename = "users.txt";
	users.importfile(filename);


	//create the RFID object
	CPhidgetRFID_create(&rfid);
	
	//open the RFID for device connections
	CPhidget_open((CPhidgetHandle)rfid, -1);

	//get the program to wait for an RFID device to be attached, if no device, it checks for a remote device
	printf("Waiting for RFID to be attached....\n");
	if((result = CPhidget_waitForAttachment((CPhidgetHandle)rfid, 50)))
	{
		CPhidget_getErrorDescription(result, &err);
		printf("Problem waiting for attachment: %s\n", err);
	}
	else
	{
		connected = true;
		//Set the handlers to be run when the device is plugged in or opened from software, unplugged or closed from software, or generates an error.
		CPhidget_set_OnAttach_Handler((CPhidgetHandle)rfid, AttachHandler, NULL);
		CPhidget_set_OnDetach_Handler((CPhidgetHandle)rfid, DetachHandler, NULL);
		CPhidget_set_OnError_Handler((CPhidgetHandle)rfid, ErrorHandler, NULL);

		//Registers a callback that will run when a Tag is read.
		//Requires the handle for the PhidgetRFID, the function that will be called, and an arbitrary pointer that will be supplied to the callback function (may be NULL).
		CPhidgetRFID_set_OnTag_Handler(rfid, TagHandler, NULL);

		//Registers a callback that will run when a Tag is lost (removed from antenna read range).
		//Requires the handle for the PhidgetRFID, the function that will be called, and an arbitrary pointer that will be supplied to the callback function (may be NULL).
		CPhidgetRFID_set_OnTagLost_Handler(rfid, TagLostHandler, NULL);


		//Display the properties of the attached RFID device
		display_properties(rfid);

		CPhidgetRFID_setAntennaOn(rfid, 1);
		CPhidgetRFID_setOutputState(rfid, 0, 0);
		CPhidgetRFID_setOutputState(rfid, 1, 0);
	}

	//read RFID event data
	printf("Ready.....\n");
	
	string input;
	cout << ">";
	getline(cin,input);
	
	//waits for input and executes commands, until input is "quit"
	while(lowercase(input) != "quit")
	{
		docommand(input);
		cout << ">";
		getline(cin,input);
	}

	//since user input has been read, this is a signal to terminate the program so we will close the phidget and delete the object we created
	printf("Closing...\n");
	if(connected)
	{
		CPhidget_close((CPhidgetHandle)rfid);
		CPhidget_delete((CPhidgetHandle)rfid);
	}

	//all done, exit
	return 0;
}

int main(int argc)
{
	rfid_simple();
	return 0;
}