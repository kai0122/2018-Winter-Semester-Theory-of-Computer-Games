#pragma once
#include <string>
#include <random>
#include <sstream>
#include <map>
#include <stdlib.h>
#include <time.h>
#include <type_traits>
#include <algorithm>
#include "board.h"
#include "action.h"
#include "weight.h"


class agent {
public:
	agent(const std::string& args = "") {
		std::stringstream ss("name=unknown role=unknown " + args);
		for (std::string pair; ss >> pair; ) {
			std::string key = pair.substr(0, pair.find('='));
			std::string value = pair.substr(pair.find('=') + 1);
			meta[key] = { value };
		}
	}
	virtual ~agent() {}
	virtual void open_episode(const std::string& flag = "") {}
	virtual void close_episode(const std::string& flag = "") {}
	virtual action take_action(weight &myweight, board& b, int& op) { return action(); }
	virtual bool check_for_win(const board& b) { return false; }

public:
	virtual std::string property(const std::string& key) const { return meta.at(key); }
	virtual void notify(const std::string& msg) { meta[msg.substr(0, msg.find('='))] = { msg.substr(msg.find('=') + 1) }; }
	virtual std::string name() const { return property("name"); }
	virtual std::string role() const { return property("role"); }

protected:
	typedef std::string key;
	struct value {
		std::string value;
		operator std::string() const { return value; }
		template<typename numeric, typename = typename std::enable_if<std::is_arithmetic<numeric>::value, numeric>::type>
		operator numeric() const { return numeric(std::stod(value)); }
	};
	std::map<key, value> meta;
};

class random_agent : public agent {
public:
	random_agent(const std::string& args = "") : agent(args) {
		if (meta.find("seed") != meta.end())
			engine.seed(int(meta["seed"]));
	}
	virtual ~random_agent() {}

protected:
	std::default_random_engine engine;
};

/**
 * random environment
 * add a new random tile to an empty cell
 * 1-tile: 33%
 * 2-tile: 33%
 * 3-tile: 33%
 */
class rndenv : public random_agent {
public:
	int bag;
	rndenv(const std::string& args = "") : random_agent("name=random role=environment " + args),
		space({ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 }), popup(0, 2), tileNum({ 1, 2, 3 }) {
			bag = 0;
		}

	virtual action take_action(weight &myweight, board& after, int& op) {
		std::shuffle(space.begin(), space.end(), engine);
		if(bag == 3){
			bag = 0;
			std::shuffle(tileNum.begin(), tileNum.end(), engine);
		}
		for (int pos : space) {
			if (after(pos) != 0) continue;
			if (op == 0 && pos != 12 && pos != 13 && pos != 14 && pos != 15) continue;
			if (op == 1 && pos != 0  && pos != 4  && pos != 8  && pos != 12) continue;
			if (op == 2 && pos != 0  && pos != 1  && pos != 2  && pos != 3 ) continue;
			if (op == 3 && pos != 3  && pos != 7  && pos != 11 && pos != 15) continue;
			board::cell tile;
			/*while(1){
				tile = popup(engine) + 1;
				if (bag[tile-1] == 0) {
					bag[tile-1] = 1;
					break;
				}
			}*/
			int i = 0;
			for (int tilenum : tileNum) {
				if(i == bag) {
					bag++;
					return action::place(pos, tilenum);
				}
				i++;
			}
			//board::cell tile = popup(engine) ? 1 : 2;

		}
		return action();
	}

private:
	std::array<int, 16> space;
	std::array<int, 3> tileNum;
	std::uniform_int_distribution<int> popup;
};

int get2devide(int index){
	int returnValue = 0;
	while(index != 1) {
		index = index >> 1;
		returnValue++;
	}
	return returnValue;
}

void getIJ(int &i, int &j, int index){
	switch(index){
		case 0: i = 0; j = 0; break;
		case 1: i = 0; j = 1; break;
		case 2: i = 0; j = 2; break;
		case 3: i = 0; j = 3; break;
		case 4: i = 1; j = 0; break;
		case 5: i = 1; j = 1; break;
		case 6: i = 1; j = 2; break;
		case 7: i = 1; j = 3; break;
		case 8: i = 2; j = 0; break;
		case 9: i = 2; j = 1; break;
		case 10: i = 2; j = 2; break;
		case 11: i = 2; j = 3; break;
		case 12: i = 3; j = 0; break;
		case 13: i = 3; j = 1; break;
		case 14: i = 3; j = 2; break;
		case 15: i = 3; j = 3; break;
	}
}

/**
 * accumulate the total value of given state
 */
float estimate(weight &myweight, const board& b) {
	//debug << "estimate " << std::endl << b;
	float value = 0;
	for (int i=0;i<32;i++) {
		int arr[6];
		switch(i){
			case 0: arr[0] = 0; arr[1] = 1; arr[2] = 2; arr[3] = 3; arr[4] = 6; arr[5] = 7; break;
			case 1: arr[0] = 4; arr[1] = 5; arr[2] = 6; arr[3] = 7; arr[4] = 10; arr[5] = 11; break;
			case 2: arr[0] = 8; arr[1] = 9; arr[2] = 10; arr[3] = 4; arr[4] = 5; arr[5] = 6; break;
			case 3: arr[0] = 12; arr[1] = 13; arr[2] = 14; arr[3] = 8; arr[4] = 9; arr[5] = 10; break;
			case 4: arr[0] = 3; arr[1] = 2; arr[2] = 1; arr[3] = 0; arr[4] = 5; arr[5] = 4; break;
			case 5: arr[0] = 7; arr[1] = 6; arr[2] = 5; arr[3] = 4; arr[4] = 9; arr[5] = 8; break;
			case 6: arr[0] = 11; arr[1] = 10; arr[2] = 9; arr[3] = 7; arr[4] = 6; arr[5] = 5; break;
			case 7: arr[0] = 15; arr[1] = 14; arr[2] = 13; arr[3] = 11; arr[4] = 10; arr[5] = 9; break;
			case 8: arr[0] = 0; arr[1] = 1; arr[2] = 2; arr[3] = 4; arr[4] = 5; arr[5] = 6; break;
			case 9: arr[0] = 4; arr[1] = 5; arr[2] = 6; arr[3] = 8; arr[4] = 9; arr[5] = 10; break;
			case 10: arr[0] = 8; arr[1] = 9; arr[2] = 10; arr[3] = 11; arr[4] = 6; arr[5] = 7; break;
			case 11: arr[0] = 12; arr[1] = 13; arr[2] = 14; arr[3] = 15; arr[4] = 10; arr[5] = 11; break;
			case 12: arr[0] = 3; arr[1] = 2; arr[2] = 1; arr[3] = 7; arr[4] = 6; arr[5] = 5; break;
			case 13: arr[0] = 7; arr[1] = 6; arr[2] = 5; arr[3] = 11; arr[4] = 10; arr[5] = 9; break;
			case 14: arr[0] = 11; arr[1] = 10; arr[2] = 9; arr[3] = 8; arr[4] = 5; arr[5] = 4; break;
			case 15: arr[0] = 15; arr[1] = 14; arr[2] = 13; arr[3] = 12; arr[4] = 9; arr[5] = 8; break;
			case 16: arr[0] = 12; arr[1] = 8; arr[2] = 4; arr[3] = 0; arr[4] = 5; arr[5] = 1; break;
			case 17: arr[0] = 13; arr[1] = 9; arr[2] = 5; arr[3] = 1; arr[4] = 6; arr[5] = 2; break;
			case 18: arr[0] = 14; arr[1] = 10; arr[2] = 6; arr[3] = 13; arr[4] = 9; arr[5] = 5; break;
			case 19: arr[0] = 15; arr[1] = 11; arr[2] = 7; arr[3] = 14; arr[4] = 10; arr[5] = 6; break;
			case 20: arr[0] = 14; arr[1] = 10; arr[2] = 6; arr[3] = 2; arr[4] = 5; arr[5] = 1; break;
			case 21: arr[0] = 15; arr[1] = 11; arr[2] = 7; arr[3] = 3; arr[4] = 6; arr[5] = 2; break;
			case 22: arr[0] = 12; arr[1] = 8; arr[2] = 4; arr[3] = 13; arr[4] = 9; arr[5] = 5; break;
			case 23: arr[0] = 13; arr[1] = 9; arr[2] = 5; arr[3] = 14; arr[4] = 10; arr[5] = 6; break;
			case 24: arr[0] = 0; arr[1] = 4; arr[2] = 8; arr[3] = 12; arr[4] = 9; arr[5] = 13; break;
			case 25: arr[0] = 1; arr[1] = 5; arr[2] = 9; arr[3] = 13; arr[4] = 10; arr[5] = 14; break;
			case 26: arr[0] = 2; arr[1] = 6; arr[2] = 10; arr[3] = 1; arr[4] = 5; arr[5] = 9; break;
			case 27: arr[0] = 3; arr[1] = 7; arr[2] = 11; arr[3] = 2; arr[4] = 6; arr[5] = 10; break;
			case 28: arr[0] = 2; arr[1] = 6; arr[2] = 10; arr[3] = 14; arr[4] = 9; arr[5] = 13; break;
			case 29: arr[0] = 3; arr[1] = 7; arr[2] = 11; arr[3] = 15; arr[4] = 10; arr[5] = 14; break;
			case 30: arr[0] = 0; arr[1] = 4; arr[2] = 8; arr[3] = 1; arr[4] = 5; arr[5] = 9; break;
			case 31: arr[0] = 1; arr[1] = 5; arr[2] = 9; arr[3] = 2; arr[4] = 6; arr[5] = 10; break;
		}

		int i0, j0, i1, j1, i2, j2, i3, j3, i4, j4, i5, j5;
		getIJ(i0, j0, arr[0]);
		getIJ(i1, j1, arr[1]);
		getIJ(i2, j2, arr[2]);
		getIJ(i3, j3, arr[3]);
		getIJ(i4, j4, arr[4]);
		getIJ(i5, j5, arr[5]);

		unsigned long long int index = b[i0][j0] * 759375 + b[i1][j1] * 50625 + b[i2][j2] * 3375 + b[i3][j3] * 225 + b[i4][j4] * 15 + b[i5][j5] * 1;
		//std::cout << b[i0][j0] << "," << index << "\n";
		if(index > 11390625)
			std::cout << "-----------------------Index: " << index << std::endl;
		value += myweight.operator[](index);
		/*
		int arr[4];
		switch(i){
			case 0: arr[0] = 0; arr[1] = 1; arr[2] = 2; arr[3] = 3; break;
			case 1: arr[0] = 4; arr[1] = 5; arr[2] = 6; arr[3] = 7; break;
			case 2: arr[0] = 8; arr[1] = 9; arr[2] = 10; arr[3] = 11; break;
			case 3: arr[0] = 12; arr[1] = 13; arr[2] = 14; arr[3] = 15; break;
			case 4: arr[0] = 0; arr[1] = 4; arr[2] = 8; arr[3] = 12; break;
			case 5: arr[0] = 1; arr[1] = 5; arr[2] = 9; arr[3] = 13; break;
			case 6: arr[0] = 2; arr[1] = 6; arr[2] = 10; arr[3] = 14; break;
			case 7: arr[0] = 3; arr[1] = 7; arr[2] = 11; arr[3] = 15; break;
		}

		int i0, j0, i1, j1, i2, j2, i3, j3;
		getIJ(i0, j0, arr[0]);
		getIJ(i1, j1, arr[1]);
		getIJ(i2, j2, arr[2]);
		getIJ(i3, j3, arr[3]);
		int index = b[i0][j0] * 1000 + b[i1][j1] * 100 + b[i2][j2] * 10 + b[i3][j3] * 1;
		//std::cout << "Index: " << index << std::endl;
		value += myweight.operator[](index);
		*/
	}
	return value;
}


/**
 * dummy player
 * select a legal action randomly
 */
class player : public random_agent {
public:
	player(const std::string& args = "") : random_agent("name=dummy role=player " + args),
		opcode({ 0, 1, 2, 3 }) {}

	virtual action take_action(weight &myweight, board& before, int& op_new) {
		//std::shuffle(opcode.begin(), opcode.end(), engine);
		int maxPlace;

		if(false){
			for (int op : opcode) {
				board bestBoard;
				board::reward reward = board(before).slide(op, maxPlace, bestBoard);
				op_new = op;
				//std::cout << "slide score: " << reward << std::endl;
				if (reward != -1) return action::slide(op);
			}
			return action();
		}
		else if(true){
			int MaxReward = -2, MaxOp, MaxValue;
			int bestOP;
			int bestValue = -9999;
			board bestBoard, MaxNextBoard;
			board tempNextBoard;
			//std::cout << "-----------------------------\n";
			for (int op : opcode) {
				int getNewReward = board(before).slide(op, maxPlace, tempNextBoard);
				//std::cout << getNewReward << "here!!!\n";
				//board(before).setcurrentReward(getNewReward);
				if (getNewReward != -1){
					float currentValue;
					currentValue = getNewReward + estimate(myweight, tempNextBoard);
					//board(before).setcurrentValue(currentValue);
					//std::cout << "Value: " << currentValue << std::endl;
					//std::cout << "Reward: " << getNewReward << std::endl;
					if(currentValue > bestValue && getNewReward != -1){
						bestOP = op;
						MaxReward = getNewReward;
						MaxValue = currentValue;
						bestValue = currentValue;
						MaxNextBoard = tempNextBoard;
					}
				}

			}
			//std::cout << "OP: !!!" << bestOP << std::endl;
			before.setcurrentReward(MaxReward);
			before.setcurrentValue(MaxValue);
			before.setafter_state(tempNextBoard);
			op_new = bestOP;
			return action::slide(bestOP);

		}
		return action();
	}

private:
	std::array<int, 4> opcode;
};
