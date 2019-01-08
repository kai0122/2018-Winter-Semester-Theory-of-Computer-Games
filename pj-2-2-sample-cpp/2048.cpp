/**
 * Basic Environment for Game 2048
 * use 'g++ -std=c++11 -O3 -g -o 2048 2048.cpp' to compile the source
 *
 * Computer Games and Intelligence (CGI) Lab, NCTU, Taiwan
 * http://www.aigames.nctu.edu.tw
 */

#include <iostream>
#include <fstream>
#include <iterator>
#include <string>
#include "board.h"
#include "action.h"
#include "agent.h"
#include "episode.h"
#include "statistic.h"
#include "weight.h"

bool is_file_exist(const char *fileName)
{
    std::ifstream infile(fileName);
    return infile.good();
}

float update(board* b, float u, weight &myweight, int reward) {
	//debug << "update " << " (" << u << ")" << std::endl << b;
	float u_split = u / 32;
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

		board temp = *b;
		int index = (*b).operator ()(arr[0]) * 759375 + (*b).operator ()(arr[1]) * 50625 + (*b).operator ()(arr[2]) * 3375 + (*b).operator ()(arr[3]) * 225 + (*b).operator ()(arr[4]) * 15 + (*b).operator ()(arr[5]) * 1;
		//b[i0][j0] * 759375 + b[i1][j1] * 50625 + b[i2][j2] * 3375 + b[i3][j3] * 225 + b[i4][j4] * 15 + b[i5][j5] * 1;
		//std::cout << "Index: " << index << std::endl;
		myweight.operator[](index) += u_split;
		value += myweight.operator[](index);
	}
	/*
	float u_split = u / 8;
	float value = 0;
	for (int i=0;i<8;i++) {
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
		//int x = (*b).operator ()(arr[3]) * 1;
		board temp = *b;
		//std::cout << reward << std::endl;
		//getchar();
		size_t index = (*b).operator ()(arr[0]) * 1000 + (*b).operator ()(arr[1]) * 100 + (*b).operator ()(arr[2]) * 10 + (*b).operator ()(arr[3]) * 1;

		myweight.operator[](index) += u_split;
		value += myweight.operator[](index);


	}
	*/
	return value;
}

void update_episode(std::vector<board>& path, float alpha, weight &myweight) {
	float exact = -500;
	//std::cout << "asasd\n" << path.size() << std::endl;
	//std::cout << path.size() << std::endl;
	for (path.pop_back() /* terminal state */; path.size(); path.pop_back()) {
		board& move = path.back();
		float error = exact - (move.currentValue - move.currentReward);
		//debug << "update error = " << error << " for after state" << std::endl << move.after_state();
		//std::cout << "asasd\n" << move << std::endl;
		//std::cout << "asasd\n" << move.currentReward << std::endl;
		//std::cout << "asasd\n" << move.currentValue << std::endl;
		//std::cout << "asasd\n" << exact << std::endl;
		//std::cout << "asasd\n" << *move.after_state << std::endl;
		exact = move.currentReward + update(move.after_state, alpha * error, myweight, move.currentReward);
		//alpha = 0.2;
		//getchar();
	}

}

int main(int argc, const char* argv[]) {
	std::cout << "2048-Demo: ";
	std::copy(argv, argv + argc, std::ostream_iterator<const char*>(std::cout, " "));
	std::cout << std::endl << std::endl;

	size_t total = 1000, block = 0, limit = 0;
	std::string play_args, evil_args;
	std::string load, save;
	bool summary = false;
	bool train = false;
	for (int i = 1; i < argc; i++) {
		std::string para(argv[i]);
		if (para.find("--total=") == 0) {
			total = std::stoull(para.substr(para.find("=") + 1));
		} else if (para.find("--block=") == 0) {
			block = std::stoull(para.substr(para.find("=") + 1));
		} else if (para.find("--limit=") == 0) {
			limit = std::stoull(para.substr(para.find("=") + 1));
		} else if (para.find("--play=") == 0) {
			play_args = para.substr(para.find("=") + 1);
		} else if (para.find("--evil=") == 0) {
			evil_args = para.substr(para.find("=") + 1);
		} else if (para.find("--load=") == 0) {
			load = para.substr(para.find("=") + 1);
		} else if (para.find("--save=") == 0) {
			save = para.substr(para.find("=") + 1);
		} else if (para.find("--summary") == 0) {
			summary = true;
		} else if (para.find("--train") == 0) {
			train = true;
		}

	}

	statistic stat(total, block, limit);

	if (load.size()) {
		std::ifstream in(load, std::ios::in);
		in >> stat;
		in.close();
		summary |= stat.is_finished();
	}

	player play(play_args);
	rndenv evil(evil_args);

	if(!train && !is_file_exist("weight.txt"))
		train = true;

	if(train){
		// train the model
		weight myweight(11390625);
		for(unsigned long long int i=0;i<11390625;i++){
			myweight.operator[](i) = 0;
		}
		std::cout << "Start Train\n";


		//path.reserve(20000);	//分配空間給 vector
		board *boardForPath;
		float alpha = 0.2;
		while (!stat.is_finished()) {
			play.open_episode("~:" + evil.name());
			evil.open_episode(play.name() + ":~");
			evil.bag = 0;

			stat.open_episode(play.name() + ":" + evil.name());
			episode& game = stat.back();
			int op = -1;
			int score = 0;
			std::vector<board> path;
			while (true) {
				//std::cout << game.state() << std::endl;

				//std::cout << game.score() << std::endl;
				agent& who = game.take_turns(play, evil);	//	whos turn
				//std::cout << "pl\n";
				action move = who.take_action(myweight, game.state(), op);
				bool ifBreak = game.apply_action(move);
				if(who.role() == "player"){

					boardForPath = new board(game.state());
					boardForPath->setcurrentValue(game.state().currentValue);
					boardForPath->setcurrentReward(game.state().currentReward);
					boardForPath->setafter_state(*boardForPath);
					path.push_back(*boardForPath);
				}

				score = game.score();
				//std::cout << move << std::endl;
				if (ifBreak != true) {
					//std::cout << "FIRST\n";
					break;
				}

				if (who.check_for_win(game.state())){
					//std::cout << "SECOND\n";
					break;
				}

			}
			agent& win = game.last_turns(play, evil);
			//std::cout << n++ << std::endl;
			update_episode(path, alpha, myweight);
			//make_statistic(n, b, score);

			stat.close_episode(win.name());

			play.close_episode(win.name());
			evil.close_episode(win.name());
			//std::cout << myweight << std::endl;
			/*for(int i=0;i<20000;i++){
				std::cout << myweight.operator[](i) << ", ";
			}*/
			//getchar();
		}

		//	**************************
		//				Save Weight
		//	**************************

		std::cout << "Writing Weight to File...\n";
		std::ofstream outFile("weight.txt");
    // the important part
		int count=10;
		for(unsigned long long int i=0;i<11390625;i++){
			if(i % 11390625/10 == 0){
				std::cout << count << "％...\n";
				count += 10;
			}
			outFile << myweight.operator[](i) << "\n";
		}
		std::cout << "Done...\n";
    //for (const auto &e : my) outFile << e << "\n";
	}
	else{
		//	Not train

		std::cout << "Reading Weight from File...\n";
		std::ifstream inFile("weight.txt");
		weight myweight(11390625);
		int count=10;
		for(unsigned long long int i=0;i<11390625;i++){
			if(i % 11390625/10 == 0){
				std::cout << count << "％...\n";
				count += 10;
			}

			inFile >> myweight.operator[](i);
		}
		std::cout << "Start Run\n";
		while (!stat.is_finished()) {
			play.open_episode("~:" + evil.name());
			evil.open_episode(play.name() + ":~");
			evil.bag = 0;

			stat.open_episode(play.name() + ":" + evil.name());
			episode& game = stat.back();
			int op = -1;
			int score = 0;
			while (true) {
				//std::cout << game.state() << std::endl;

				//std::cout << game.score() << std::endl;
				agent& who = game.take_turns(play, evil);	//	whos turn
				action move = who.take_action(myweight, game.state(), op);
				bool ifBreak = game.apply_action(move);

				score = game.score();
				//std::cout << move << std::endl;
				if (ifBreak != true) {
					//std::cout << "FIRST\n";
					break;
				}

				if (who.check_for_win(game.state())){
					//std::cout << "SECOND\n";
					break;
				}

			}
			agent& win = game.last_turns(play, evil);
			//std::cout << n++ << std::endl;
			//update_episode(path, alpha, myweight);
			//make_statistic(n, b, score);

			stat.close_episode(win.name());

			play.close_episode(win.name());
			evil.close_episode(win.name());
			//std::cout << myweight << std::endl;
			/*for(int i=0;i<20000;i++){
				std::cout << myweight.operator[](i) << ", ";
			}*/
			//getchar();
		}
	}


	if (summary) {
		//std::cout << "jjjjj\n";
		stat.summary();
	}

	if (save.size()) {
		//std::cout << "jjjjjllllll\n";
		std::ofstream out(save, std::ios::out | std::ios::trunc);
		out << stat;
		out.close();
	}

	return 0;
}
