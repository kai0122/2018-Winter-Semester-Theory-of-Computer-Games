#pragma once
#include <array>
#include <iostream>
#include <iomanip>

/**
 * array-based board for 2048
 *
 * index (1-d form):
 *  (0)  (1)  (2)  (3)
 *  (4)  (5)  (6)  (7)
 *  (8)  (9) (10) (11)
 * (12) (13) (14) (15)
 *
 */
class board {
public:
	typedef uint32_t cell;
	typedef std::array<cell, 4> row;
	typedef std::array<row, 4> grid;
	typedef uint64_t data;
	typedef int reward;

public:
	board() : tile(), attr(0) {after_state = NULL;}
	board(const grid& b, data v = 0) : tile(b), attr(v) {after_state = NULL;}
	board(const board& b) = default;
	board& operator =(const board& b) = default;
	~board(){
		//if(after_state != NULL)
			//delete after_state;
	}

	operator grid&() { return tile; }
	operator const grid&() const { return tile; }
	row& operator [](unsigned i) { return tile[i]; }
	const row& operator [](unsigned i) const { return tile[i]; }
	cell& operator ()(unsigned i) { return tile[i / 4][i % 4]; }
	const cell& operator ()(unsigned i) const { return tile[i / 4][i % 4]; }

	data info() const { return attr; }
	data info(data dat) { data old = attr; attr = dat; return old; }

public:
	bool operator ==(const board& b) const { return tile == b.tile; }
	bool operator < (const board& b) const { return tile <  b.tile; }
	bool operator !=(const board& b) const { return !(*this == b); }
	bool operator > (const board& b) const { return b < *this; }
	bool operator <=(const board& b) const { return !(b < *this); }
	bool operator >=(const board& b) const { return !(*this < b); }

public:

	/**
	 * place a tile (index value) to the specific position (1-d form index)
	 * return 0 if the action is valid, or -1 if not
	 */
	reward place(unsigned pos, cell tile) {
		if (pos >= 16) return -1;
		if (tile != 1 && tile != 2 && tile != 3) return -1;	//	only 1,2,3 for new tile (This time HW)
		operator()(pos) = tile;
		if(tile == 3) return 3;
		return 0;
	}

	/**
	 * apply an action to the board
	 * return the reward of the action, or -1 if the action is illegal
	 */
	unsigned last_slide_side;
	reward slide(unsigned opcode, int& maxPlace, board &nextBoard) {
		last_slide_side = opcode;
		switch (opcode & 0b11) {
		case 0: return slide_up(maxPlace, nextBoard);
		case 1: return slide_right(maxPlace, nextBoard);
		case 2: return slide_down(maxPlace, nextBoard);
		case 3: return slide_left(maxPlace, nextBoard);
		default: return -1;
		}
	}

	reward slide_left(int& maxPlace, board &nextBoard) {
		board prev = *this;
		int maxValue = -2, tempMaxPlace = -1;
		reward score = 0;
		for (int r = 0; r < 4; r++) {
			auto& row = tile[r];
			int tile_prev = 0, prev_c;
			for (int c = 0; c < 4; c++) {
				int tile_current = row[c];
				if (tile_current == 0) {
					if (tile_prev != 0){
						row[c-1] = tile_prev;
						tile_prev = 0;
					}
					continue;
				}
				row[c] = 0;
				if (tile_prev) {
					if ((tile_current == tile_prev && tile_current >= 3) || (tile_current == 1 && tile_prev == 2) || (tile_current == 2 && tile_prev == 1)) {
						if (tile_current < 3) {
							row[c-1] = 3;
							score += 3;
							tile_prev = 0;
						}
						else{
							row[c-1] = ++tile_current;
							int scoreNew = 3;
							for(int s=0;s<tile_current-3;s++) scoreNew *= 3;
							score += (scoreNew - scoreNew*2/3);
							tile_prev = 0;
						}
					} else {
						row[c-1] = tile_prev;
						tile_prev = tile_current;
						prev_c = c;
					}
				} else {
					if (c == 0){
						tile_prev = tile_current;
						prev_c = c;
					}
					else{
						row[c-1] = tile_current;
						tile_prev = 0;
					}
				}
			}
			if (tile_prev) tile[r][prev_c] = tile_prev;
		}
		//	Find Max Value;
		for(int i=0;i<4;i++){
			for(int j=0;j<4;j++){
				if(tile[i][j] > maxValue){
					maxValue = tile[i][j];
					if((i == 0 && j == 3) || (i == 0 && j == 0) || (i == 3 && j == 0) || (i == 3 && j == 3))
						tempMaxPlace = 1;
					else if((i == 1 && j == 2) || (i == 1 && j == 1) || (i == 2 && j == 1) || (i == 2 && j == 2))
						tempMaxPlace = 3;
					else
						tempMaxPlace = 2;
				}
			}
		}
		maxPlace = tempMaxPlace;
		nextBoard = *this;
		return (*this != prev) ? score : -1;
	}
	reward slide_right(int& maxPlace, board &nextBoard) {
		reflect_horizontal();
		reward score = slide_left(maxPlace, nextBoard);
		reflect_horizontal();
		return score;
	}
	reward slide_up(int& maxPlace, board &nextBoard) {
		rotate_right();
		reward score = slide_right(maxPlace, nextBoard);
		rotate_left();
		return score;
	}
	reward slide_down(int& maxPlace, board &nextBoard) {
		rotate_right();
		reward score = slide_left(maxPlace, nextBoard);
		rotate_left();
		return score;
	}

	void transpose() {
		for (int r = 0; r < 4; r++) {
			for (int c = r + 1; c < 4; c++) {
				std::swap(tile[r][c], tile[c][r]);
			}
		}
	}

	void reflect_horizontal() {
		for (int r = 0; r < 4; r++) {
			std::swap(tile[r][0], tile[r][3]);
			std::swap(tile[r][1], tile[r][2]);
		}
	}

	void reflect_vertical() {
		for (int c = 0; c < 4; c++) {
			std::swap(tile[0][c], tile[3][c]);
			std::swap(tile[1][c], tile[2][c]);
		}
	}

	/**
	 * rotate the board clockwise by given times
	 */
	void rotate(int r = 1) {
		switch (((r % 4) + 4) % 4) {
		default:
		case 0: break;
		case 1: rotate_right(); break;
		case 2: reverse(); break;
		case 3: rotate_left(); break;
		}
	}

	void rotate_right() { transpose(); reflect_horizontal(); } // clockwise
	void rotate_left() { transpose(); reflect_vertical(); } // counterclockwise
	void reverse() { reflect_horizontal(); reflect_vertical(); }
	void setcurrentValue(int v) {currentValue = v;}
	void setcurrentReward(int v) {
		currentReward = v;
		//std::cout << "Get v: " << currentReward << std::endl;
	}
	void setafter_state(board v) {
		after_state = new board(v);
		//*after_state = v;
		//std::cout << currentReward << std::endl;
	}

public:
	friend std::ostream& operator <<(std::ostream& out, const board& b) {
		out << "+------------------------+" << std::endl;
		for (auto& row : b.tile) {
			out << "|" << std::dec;
			for (auto t : row){
				int printNum;
				if (t <= 3) printNum = t;
				else printNum = (1 << (t-3)) * 3;
				out << std::setw(6) << printNum;
			}
			out << "|" << std::endl;
		}
		out << "+------------------------+" << std::endl;
		return out;
	}
public:
	int currentValue;
	int currentReward;
	board* after_state;

private:
	grid tile;
	data attr;
};
