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
	virtual action take_action(const board& b, int& op) { return action(); }
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

	virtual action take_action(const board& after, int& op) {
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

/**
 * dummy player
 * select a legal action randomly
 */
class player : public random_agent {
public:
	player(const std::string& args = "") : random_agent("name=dummy role=player " + args),
		opcode({ 0, 1, 2, 3 }) {}

	virtual action take_action(const board& before, int& op_new) {
		std::shuffle(opcode.begin(), opcode.end(), engine);
		int maxPlace;
		if(false){
			for (int op : opcode) {
				board::reward reward = board(before).slide(op, maxPlace);
				op_new = op;
				//std::cout << "slide score: " << reward << std::endl;
				if (reward != -1) return action::slide(op);
			}
			return action();
		}
		else if(true){
			int MaxReward = -2, MaxOp;
			for (int op : opcode) {
				int getNewReward = board(before).slide(op, maxPlace);
				if(getNewReward >= MaxReward && getNewReward != -1){
					MaxReward = getNewReward;
					MaxOp = op;
				}
				//op_new = op;
				//std::cout << "slide score: " << reward << std::endl;
				//if (reward != -1) return action::slide(op);
			}
			//std::cout << "slide score: " << MaxReward << std::endl;
			if(MaxReward > -1){
					board::reward reward = MaxReward;
					op_new = MaxOp;
					return action::slide(MaxOp);
			}
			return action();
		}
		else{
			int MaxReward[4], MaxOp[4], MaxRewardNum = -2;
			int MaxReturnValue[4];
			//	Initial MaxRwward
			for(int i=0;i<4;i++) MaxReward[i] = -2;
			for (int op : opcode) {
				int getNewReward = board(before).slide(op, maxPlace);
				if(getNewReward >= MaxRewardNum && getNewReward != -1){
					MaxReward[op] = getNewReward;
					MaxRewardNum = getNewReward;
					MaxOp[op] = op;
					MaxReturnValue[op] = maxPlace;
				}
				//op_new = op;
				//std::cout << "slide score: " << reward << std::endl;
				//if (reward != -1) return action::slide(op);
			}
			//std::cout << "slide score: " << MaxReward << std::endl;
			int BestPlaceMaxRewardReturnValue = 4, BestPlaceMaxRewardIndex;
			int i;
			for(i=0;i<4;i++){	// Find Every MaxReward
				if(MaxReward[i] > -1){
					if(MaxReturnValue[i] < BestPlaceMaxRewardReturnValue){
						BestPlaceMaxRewardIndex = i;
						BestPlaceMaxRewardReturnValue = MaxReturnValue[i];
					}
				}
			}
			//std::cout << "slide score: " << MaxRewardNum << std::endl;
			if(MaxRewardNum > -1){	//	Have Some Silde
				board::reward reward = MaxReward[BestPlaceMaxRewardIndex];
				op_new = MaxOp[BestPlaceMaxRewardIndex];
				return action::slide(MaxOp[BestPlaceMaxRewardIndex]);
			}
			return action();
		}
	}

private:
	std::array<int, 4> opcode;
};
