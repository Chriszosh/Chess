#include "PGN.hpp"
#include <ctime>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <iostream>
using namespace std;

namespace {
	void getline(ifstream& s, string& d, char d1, char d2) {
		d.clear();
		char c;
		do {
			c = s.get();
			if (c!=d1 && c!=d2)
				d.push_back(c);
		} while (c!=d1 && c!=d2);
	}
}

int stringToInt(string s) {
		stringstream ss;
		int i;
		ss << s;
		ss >> i;
		return i;
	}

PGN::PGN(string f) {
	file = f;
	time_t t = time(0);   // get time now
    struct tm * now = localtime( & t );
    curYear = now->tm_year+1900;
    curMonth = now->tm_mon+1;
    playerName = "reidben24";
}

void PGN::setPlayer(string nm) {
	playerName = nm;
}

void PGN::parse() {
	Game temp;
	bool keep = true;

	ifstream input(file.c_str());
	string data;

	while (input.good()) {
		if (input.peek()=='[') {
			input.get(); //discard bracket
            getline(input,data,' ');
            if (data=="Date") {
				getline(input,data,'"');
				getline(input,data,'.');
				int y = stringToInt(data);
				getline(input,data,'.');
				int m = stringToInt(data);
				if (y==curYear)
					temp.age = curMonth-m;
				else
					temp.age = curMonth+(curYear-y-1)*12+(12-m);
            }
            else if (data=="White") {
				getline(input,data,'"');
				getline(input,data,'"');
				if (data==playerName)
					temp.isWhite = true;
				else
					temp.isWhite = false;
            }
            else if (data=="Result") {
				getline(input,data,'"');
				getline(input,data,'"');
                if (data=="1-0") {
					if (temp.isWhite)
						temp.result = Game::Win;
					else
						temp.result = Game::Loss;
                }
                else if (data=="0-1") {
					if (temp.isWhite)
						temp.result = Game::Loss;
					else
						temp.result = Game::Win;
                }
                else
					temp.result = Game::Draw;
            }
            else if (data=="BlackElo") {
				getline(input,data,'"');
				getline(input,data,'"');
				int r = stringToInt(data);
				if (!temp.isWhite)
					temp.rating = r;
            }
            else if (data=="WhiteElo") {
				getline(input,data,'"');
				getline(input,data,'"');
				int r = stringToInt(data);
				if (temp.isWhite)
					temp.rating = r;
            }
            else if (data=="Variant") {
				getline(input,data,'"');
				getline(input,data,'"');
				if (data!="Standard")
					keep = false;
            }
            getline(input,data); //move to next line
		}
		else if (input.peek()=='1') {
			while (data!="1-0" && data!="0-1" && data!="1/2-1/2") {
				getline(input,data,' ','\n');
				if (data.find(".")==string::npos && data!="1-0" && data!="0-1" && data!="1/2-1/2")
					temp.moves.push_back(data);
			}
			if (keep)
				games.push_back(temp);
			temp.moves.clear();
			keep = true;
		}
		else {
			if (!input.get())
				break;
		}
	}
}

vector<Game> PGN::getGames() {
	return games;
}
