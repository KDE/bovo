/*******************************************************************
*
* This file is part of the KDE project "Bovo"
*
* Bovo is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2, or (at your option)
* any later version.
*
* Bovo is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with Bovo; see the file COPYING.  If not, write to
* the Free Software Foundation, 51 Franklin Street, Fifth Floor,
* Boston, MA 02110-1301, USA.
*
********************************************************************/

#include "standing.h"

#include <cstdlib>
#include <memory.h>
#include <assert.h>
#include <QString>

typedef QString string;

// these are used by refresh() and getSuggestions()
const static PatternCount suggestValues = { {0, 8, 8, 6, 5, 3}, {0, 7, 7, 4, 2, 1} };
const static index_T suggestValueCount = 9;

// these are used by refresh()
// '0' : player's mark
// '@' : wall (edge) or enemy's mark
// '+' : free cell, where the player can continue the position or the enemy can block the position
// '-' : free cell, where the enemy can block the position
// '.' : free cell, but nobody should place here
// '?' : everything but player's mark, ie. wall, enemy or empty
const static string level0[] = {QStringLiteral("00000")};
const static string level1[] = {QStringLiteral(".0000+")};
const static string level2[] = {QStringLiteral("@0000+"), QStringLiteral("+0000@"), QStringLiteral("00+00"), QStringLiteral("?000+0?"), QStringLiteral("?0+000?")};
const static string level3[] = {QStringLiteral(".-000+."), QStringLiteral("-+000-@"), QStringLiteral("@-000+-"), QStringLiteral("?-00+0-"), QStringLiteral("@0-00+0."), QStringLiteral("-0+00-?"), QStringLiteral(".0+00-0@"), QStringLiteral(".0-00+0.")};
const static string level4[] = {QStringLiteral("@000++"), QStringLiteral("++000@"), QStringLiteral("@0+00+0@"), QStringLiteral("@0+00+?"), QStringLiteral("?+00+0@"), QStringLiteral("@00+0+"), QStringLiteral("+0+00@"), QStringLiteral("@+000+@"), QStringLiteral("@00++0"), QStringLiteral("0++00@"), QStringLiteral("?.00++0?"), QStringLiteral("?0++00.?"), QStringLiteral("?.00++00.?"), QStringLiteral("?0+0+0?")};
const static string level5[] = {QStringLiteral("?++00++?"), QStringLiteral("@-00++."), QStringLiteral(".++00-@"), QStringLiteral(".+0+0+."), QStringLiteral("@-0+0+."), QStringLiteral(".+0+0-@"), QStringLiteral("@0-0++0-0@"), QStringLiteral("@0-0++0-?"), QStringLiteral("?-0++0-0@"), QStringLiteral("?-0++0-?")};
const static index_T patternTotals[heurLevels] = {1, 1, 5, 8, 14, 10};
const static string* patterns[heurLevels] = {level0, level1, level2, level3, level4, level5};

static bool refresh_inited = false;
const static index_T max_pattern_length = 10;
const static index_T patterns_total = 39;
static index_T mark_inv[2][256];
struct PatternData {
	index_T length;
	index_T level;
	mark_T mark[max_pattern_length];
	index_T next[4][max_pattern_length + 1];
};
static PatternData patternData[patterns_total];
const static PatternData* patternDataEnd = &patternData[patterns_total];

inline heur_T max(heur_T a, heur_T b) {
	return a > b ? a : b;
}

inline heur_T max(heur_T a, heur_T b, heur_T c) {
	return max(max(a, b), c);
}

inline heur_T min(heur_T a, heur_T b) {
	return a < b ? a : b;
}

inline heur_T min(heur_T a, heur_T b, heur_T c) {
	return min(min(a, b), c);
}

inline heur_T norm(heur_T a) {
	return max(-WinTreshold + 1, min(WinTreshold - 1, a));
}

inline count_T max(count_T a, count_T b) {
	return a > b ? a : b;
}

inline count_T min(count_T a, count_T b) {
	return a < b ? a : b;
}

void Standing::initRefresh() {
	if (!refresh_inited) {
		refresh_inited = true;
		mark_inv[0][mark[0]] = 0;
		mark_inv[0][mark[1]] = 1;
		mark_inv[0][mark[2]] = 2;
		mark_inv[0][mark[3]] = 3;
		mark_inv[1][mark[0]] = 1;
		mark_inv[1][mark[1]] = 0;
		mark_inv[1][mark[2]] = 2;
		mark_inv[1][mark[3]] = 3;

		index_T patternDataIndex = 0;
		for (index_T level = 0; level < heurLevels; ++level) {
			for (index_T patternIndex = 0; patternIndex < patternTotals[level]; ++patternIndex) {
				patternData[patternDataIndex].length = (index_T) patterns[level][patternIndex].size();
				assert(patternData[patternDataIndex].length <= max_pattern_length);
				memcpy(&patternData[patternDataIndex].mark[0], patterns[level][patternIndex].toLatin1().data(), patternData[patternDataIndex].length);
				patternData[patternDataIndex].level = level;

				for (index_T q = 0; q <= patternData[patternDataIndex].length; ++q) {
					for (index_T a = 0; a < 4; ++a) {
						index_T k = min(patternData[patternDataIndex].length, q + 1);
						mark_T current_mark = mark[a];
						while (true) {
							mark_T current_pattern = patternData[patternDataIndex].mark[k - 1];
							bool good = !current_mark ? current_pattern == '-' || current_pattern == '+' || current_pattern == '.' || current_pattern == '?' :
									current_mark == mark[0] ? current_pattern == '0' :
									current_pattern == '@' || current_pattern == '?';
							if (good) for (index_T i = 0; i < k - 1; ++i) {
								mark_T mark1 = patternData[patternDataIndex].mark[i];
								mark_T mark2 = patternData[patternDataIndex].mark[i + (q + 1 - k)];
								if (mark1 == '-' || mark1 == '.') mark1 = '+';
								if (mark2 == '-' || mark2 == '.') mark2 = '+';
								// I do not want to duplicate every ? into + and @ because it would slow things down
								// ? is accepted for now blindly, but extra check is needed for every match
								if (mark1 == '?' && mark2 != '0') mark2 = '?';
								if (mark2 == '?' && mark1 != '0') mark1 = '?';
								if (mark1 != mark2) {
									good = false;
									break;
								}
							}
							if (good) break;
							k--;
							if (!k) break;
						}
						patternData[patternDataIndex].next[a][q] = k;
					}
				}

				++patternDataIndex;
			}
		}
		assert(patternDataIndex == patterns_total);
	}
}

Standing::Standing(pos_T _table_size_x, pos_T _table_size_y) :
	table_size_x(_table_size_x), table_size_y(_table_size_y), hval(0),
	heur_seed(normal_heur_seed), target(false), stepCount(0), current(0), lastx(0), lasty(0)
{
	memset(table, 0, sizeof(table));
	memset(suggest, 0, sizeof(suggest));
	memset(matchCount, 0, sizeof(matchCount));
	memset(matchCountRow, 0, sizeof(matchCountRow));
	memset(matchCountColumn, 0, sizeof(matchCountColumn));
	memset(matchCountDiagonalSum, 0, sizeof(matchCountDiagonalSum));
	memset(matchCountDiagonalDiff, 0, sizeof(matchCountDiagonalDiff));
	memset(suggestRow, 0, sizeof(suggestRow));
	memset(suggestColumn, 0, sizeof(suggestColumn));
	memset(suggestDiagonalSum, 0, sizeof(suggestDiagonalSum));
	memset(suggestDiagonalDiff, 0, sizeof(suggestDiagonalDiff));
}

void Standing::step(pos_T x, pos_T y) {
	assert(x < table_size_x && y < table_size_y);
	table[x][y] = mark[current];
	current = 1 - current;
	lastx = x;
	lasty = y;
	++stepCount;
	evaluate();
}

void Standing::step_server(pos_T x, pos_T y) {
	assert(x < table_size_x && y < table_size_y);
	table[x][y] = mark[2];
	++stepCount;
	evaluate();
}

void Standing::posfRow(pos_T pos, int y, count_T value0, count_T value1) {
	pos_T x = pos - 1;
	assert(x < table_size_x && y < table_size_y);
	suggestRow[0][x][y] = max(suggestRow[0][x][y], value0);
	suggestRow[1][x][y] = max(suggestRow[1][x][y], value1);
	assert(!((suggestRow[0][x][y] || suggestRow[1][x][y]) && table[x][y]));
}

void Standing::posfColumn(pos_T pos, int x, count_T value0, count_T value1) {
	pos_T y = pos - 1;
	assert(x < table_size_x && y < table_size_y);
	suggestColumn[0][x][y] = max(suggestColumn[0][x][y], value0);
	suggestColumn[1][x][y] = max(suggestColumn[1][x][y], value1);
	assert(!((suggestColumn[0][x][y] || suggestColumn[1][x][y]) && table[x][y]));
}

void Standing::posfDiagonalSum(pos_T pos, int sum, count_T value0, count_T value1) {
	pos_T y0 = sum < table_size_x ? 0 : sum - table_size_x + 1;
	pos_T y = y0 + pos - 1;
	pos_T x = sum - y;
	assert(x < table_size_x && y < table_size_y);
	suggestDiagonalSum[0][x][y] = max(suggestDiagonalSum[0][x][y], value0);
	suggestDiagonalSum[1][x][y] = max(suggestDiagonalSum[1][x][y], value1);
	assert(!((suggestDiagonalSum[0][x][y] || suggestDiagonalSum[1][x][y]) && table[x][y]));
}

void Standing::posfDiagonalDiff(pos_T pos, int diff, count_T value0, count_T value1) {
	pos_T y0 = -diff > 0 ? -diff : 0;
	pos_T y = y0 + pos - 1;
	pos_T x = diff + y;
	assert(x < table_size_x && y < table_size_y);
	suggestDiagonalDiff[0][x][y] = max(suggestDiagonalDiff[0][x][y], value0);
	suggestDiagonalDiff[1][x][y] = max(suggestDiagonalDiff[1][x][y], value1);
	assert(!((suggestDiagonalDiff[0][x][y] || suggestDiagonalDiff[1][x][y]) && table[x][y]));
}

void Standing::evaluate() {
	countMatches();
	decide();
	if (current) hval *= -1;
	int current_seed = qrand() % (2 * (int) heur_seed + 1) - (int) heur_seed;
	int hval_int = (int) hval + current_seed;
	hval = hval_int > MaxHeur ? MaxHeur : hval_int < MinHeur ? MinHeur : hval_int;
}

void Standing::countMatches() {
	sample_T sample;
	sample.reserve(max(table_size_x, table_size_y) + 2);

	sample.push_back(mark[2]);
	for (pos_T x = 0; x < table_size_x; ++x) {
		sample.push_back(table[x][lasty]);
		suggestRow[0][x][lasty] = 0;
		suggestRow[1][x][lasty] = 0;
	}
	sample.push_back(mark[2]);
	refresh(sample, matchCountRow[lasty], lasty, &Standing::posfRow);

	sample.clear();
	sample.push_back(mark[2]);
	for (pos_T y = 0; y < table_size_y; ++y) {
		sample.push_back(table[lastx][y]);
		suggestColumn[0][lastx][y] = 0;
		suggestColumn[1][lastx][y] = 0;
	}
	sample.push_back(mark[2]);
	refresh(sample, matchCountColumn[lastx], lastx, &Standing::posfColumn);

	sample.clear();
	sample.push_back(mark[2]);
	int sum = lastx + lasty;
	assert(0 <= sum && sum < table_size_x + table_size_y - 1);
	pos_T y0 = sum < table_size_x ? 0 : sum - table_size_x + 1;
	pos_T ym = sum < table_size_y ? sum + 1 : table_size_y;
	assert(y0 < ym);
	for (pos_T y = y0; y < ym; ++y) {
		sample.push_back(table[sum - y][y]);
		suggestDiagonalSum[0][sum - y][y] = 0;
		suggestDiagonalSum[1][sum - y][y] = 0;
	}
	sample.push_back(mark[2]);
	refresh(sample, matchCountDiagonalSum[sum], sum, &Standing::posfDiagonalSum);

	sample.clear();
	sample.push_back(mark[2]);
	int diff = lastx - lasty;
	assert(-table_size_y < diff && diff < table_size_x);
	y0 = diff < 0 ? -diff : 0;
	ym = min(table_size_y, table_size_x - diff);
	assert(y0 < ym);
	for (pos_T y = y0; y < ym; ++y) {
		sample.push_back(table[y + diff][y]);
		suggestDiagonalDiff[0][y + diff][y] = 0;
		suggestDiagonalDiff[1][y + diff][y] = 0;
	}
	sample.push_back(mark[2]);
	refresh(sample, matchCountDiagonalDiff[diff + table_size_y - 1], diff, &Standing::posfDiagonalDiff);
}

void Standing::refresh(sample_T& sample_vect, PatternCount& local, int inv, posf_T posf) {
	PatternCount newCount;
	memset(newCount, 0, sizeof(newCount));

	pos_T sample_size = (pos_T) sample_vect.size();
	mark_T sample[2 * max_table_size - 1];
	for (pos_T i = 0; i < sample_size; ++i) {
		sample[i] = sample_vect[i];
	}

	const pos_T range = 3;
	pos_T begin_pos = 1;
	while (!sample[begin_pos]) ++begin_pos;
	if (begin_pos < range) begin_pos = 0; else begin_pos -= range;
	int end_pos = sample_size - 2;
	while (!sample[end_pos]) end_pos--;
	if (end_pos + range > sample_size - 1) end_pos = sample_size - 1; else end_pos += range;

	index_T correct[2][patterns_total];
	for (index_T i = 0; i < patterns_total; ++i) {
		correct[0][i] = 0;
		correct[1][i] = 0;
	}

	index_T player = current;
	do {
		index_T* correct_player = correct[player];
		index_T* mark_inv_player = mark_inv[player];
		for (pos_T i = begin_pos; i <= end_pos; ++i) {
			index_T sample_a = mark_inv_player[sample[i]];
			assert(sample_a < 4);

			index_T* current_correct = correct_player;
			for (PatternData* current_pattern = patternData; current_pattern != patternDataEnd; ++current_pattern) {
				index_T current_correct_val = current_pattern->next[sample_a][*current_correct];
				*current_correct = current_correct_val;
				if (current_correct_val == current_pattern->length) {
					mark_T* pattern = current_pattern->mark;
					pos_T pattern_size = current_pattern->length;
					index_T level = current_pattern->level;
					pos_T match_start_pos = i + 1 - pattern_size;

					// probably a match, but check once again because ? symbols
					bool good_match = true;
					for (pos_T j = 0; j < pattern_size; ++j) {
						assert(match_start_pos + j < sample_size);
						if ((pattern[j] == '+' || pattern[j] == '-' || pattern[j] == '.') && sample[match_start_pos + j]) {
							good_match = false;
							break;
						}
						if (pattern[j] == '@' && !sample[match_start_pos + j]) {
							good_match = false;
							break;
						}
					}

					if (good_match) {
						++newCount[player][level];
						for (pos_T j = 0; j < pattern_size; ++j) {
							assert(match_start_pos + j < sample_size);
							if (pattern[j] == '+') {
								(this->*posf)(match_start_pos + j, inv, suggestValues[player][level], suggestValues[1 - player][level]);
							} else
							if (pattern[j] == '-') {
								(this->*posf)(match_start_pos + j, inv, player == 0 ? 0 : suggestValues[1][level], player == 1 ? 0 : suggestValues[1][level]);
							}
						}
					}
				}
				++current_correct;
			}
		}
		player ^= 1;
	} while (player != current);

	for (index_T k = 0; k < heurLevels; ++k) {
		assert(matchCount[0][k] >= local[0][k] && matchCount[1][k] >= local[1][k]);
		matchCount[0][k] = matchCount[0][k] + newCount[0][k] - local[0][k];
		matchCount[1][k] = matchCount[1][k] + newCount[1][k] - local[1][k];
		local[0][k] = newCount[0][k];
		local[1][k] = newCount[1][k];
		assert(matchCount[0][k] >= local[0][k] && matchCount[1][k] >= local[1][k]);
	}
}

void Standing::decide() {
	// I have a 5
	if (matchCount[  current][0]) {target = true; hval = MaxHeur; return;}
	// the opponent has a 5
	if (matchCount[1-current][0]) {target = true; hval = MinHeur; return;}
	// the table is full
	if (stepCount >= table_size_x * table_size_y) {target = true; hval = 0; return;}
	// I have an open or a closed 4
	if (matchCount[  current][1] || matchCount[  current][2])      {hval = MaxHeur - depth_decay; return;}
	// the opponent has an open 4 or two closed 4-s
	if (matchCount[1-current][1] || matchCount[1-current][2] >= 2) {hval = MinHeur + 2 * depth_decay; return;}
/*
	0 0
	0 0
	0 1
	? ?
	? ?
	? ?
*/
	heur_T     self_attack_4 = (-90 + 40 * matchCount[current][4] +  7 * matchCount[current][5] - 10 * matchCount[1 - current][4] -  7 * matchCount[1 - current][5]) * HeurPercent;
	heur_T opponent_attack_4 = ( 90 + 10 * matchCount[current][4] +  7 * matchCount[current][5] - 40 * matchCount[1 - current][4] -  7 * matchCount[1 - current][5]) * HeurPercent;
	heur_T     self_attack_3 = (-90 + 40 * matchCount[current][4] + 28 * matchCount[current][5] - 10 * matchCount[1 - current][4] -  7 * matchCount[1 - current][5]) * HeurPercent;
	heur_T opponent_attack_3 = ( 90 + 10 * matchCount[current][4] +  7 * matchCount[current][5] - 40 * matchCount[1 - current][4] - 28 * matchCount[1 - current][5]) * HeurPercent;
	heur_T      self_defense = (  8 + 10 * matchCount[current][4] +  7 * matchCount[current][5] - 10 * matchCount[1 - current][4] -  7 * matchCount[1 - current][5]) * HeurPercent;
	heur_T  opponent_defense = ( -8 + 10 * matchCount[current][4] +  7 * matchCount[current][5] - 10 * matchCount[1 - current][4] -  7 * matchCount[1 - current][5]) * HeurPercent;

	// the opponent has a 4 and a 3
	if (matchCount[1-current][2] == 1 && matchCount[1-current][3] >= 1) {hval = MinHeur + 4 * depth_decay; return;}
	// the opponent has a 4 and I don't have a 3
	if (matchCount[1-current][2] == 1 && matchCount[current][3] == 0)
		{hval = norm(min(opponent_attack_4, max(opponent_attack_3, self_attack_4), max(opponent_defense, self_attack_3))); return;}
	// the opponent has a 4 and I have one 3
	if (matchCount[1-current][2] == 1 && matchCount[current][3] == 1)
		{hval = norm(min(opponent_attack_4, max(self_attack_3, min(self_defense, opponent_attack_3)))); return;}
	// the opponent has a 4 and I have two 3-s
	if (matchCount[1-current][2] == 1 && matchCount[current][3] >= 2)
		{hval = norm(opponent_attack_4); return;}
/*
	? ?
	? ?
	? ?
*/
	// I have a 3
	if (matchCount[current][3] >= 1) {hval = MaxHeur - 3 * depth_decay; return;}
/*
	0 ?
	? ?
	? ?
*/
	// the opponent has two 3-s and I don't have 4 possibilities
	if (matchCount[1-current][3] >= 2 && matchCount[current][4] == 0)
		{hval = MinHeur + 4 * depth_decay; return;}
	// the opponent has two 3-s and I have 4 possibilities
	if (matchCount[1-current][3] >= 2 && matchCount[current][4] >= 1)
		{hval = norm(self_attack_4); return;}
/*
	0 1
	? ?
	? ?
*/
	// the opponent has a 3
	if (matchCount[1-current][3] == 1)
		{hval = norm(max(self_attack_4, min(opponent_attack_3, max(opponent_defense, self_attack_3)))); return;}
/*
	? ?
	? ?
*/
	// I can attack
	hval = norm(max(self_attack_4, min(self_attack_3, opponent_attack_4), min(self_defense, opponent_attack_3)));
}

void Standing::getSuggestions(suggestions_T& suggestions) {
	// should we use smart cut-offs or play like a fool?
	if (heur_seed < MaxHeur) {
		Field suggestPos[suggestValueCount];
		assert((memset(suggestPos, 255, sizeof(suggestPos)), true));
	
		for (pos_T x = 0; x < table_size_x; ++x) {
			for (pos_T y = 0; y < table_size_y; ++y) {
				count_T val = max(suggestRow[current][x][y], suggestColumn[current][x][y]);
				val = max(val, suggestDiagonalSum[current][x][y]);
				val = max(val, suggestDiagonalDiff[current][x][y]);
				suggestPos[val] = Field(x, y);
				suggest[current][x][y] = val;
				assert(!(val && table[x][y]));
			}
		}
	
		suggestions.clear();
		// if I can win now, do it
		if (matchCount[current][1] || matchCount[current][2]) { assert(suggestPos[8].x != 255); suggestions.push_back(suggestPos[8]); return; }
		// if the opponent can win next turn, stop him
		if (matchCount[1-current][1] || matchCount[1-current][2]) { assert(suggestPos[7].x != 255); suggestions.push_back(suggestPos[7]); return; }
		// if I can make a clear 4, do it
		if (matchCount[current][3]) { assert(suggestPos[6].x != 255); suggestions.push_back(suggestPos[6]); return; }
	
		index_T treshold;
		// if the opponent has at least two 3-s, try creating 4-s as a last chance
		if (matchCount[1-current][3] >= 2 && matchCount[current][4]) treshold = 5; else
		// if the opponent has a 3, close it
		if (matchCount[1-current][3]) treshold = 4; else
		// if I can make a lot of 3-s and 4-s then attack, or destroy opponent's 4 possibilities
		if (matchCount[current][4] + matchCount[current][5] >= 3) treshold = 2; else
		// if I can make 3-s and 4-s, attack or defend
		if (matchCount[current][4] || matchCount[current][5]) treshold = 1; else
		// if the opponent can make 3-s and 4-s, defend
		if (matchCount[1-current][4] || matchCount[1-current][5]) treshold = 1; else
		// if nobody has nothing, try some close positions
		treshold = 0;
	
		if (treshold <= 2) {
			int count = 0;
			// filter far away defensive positions
			if (treshold > 0) {
				for (pos_T x = 0; x < table_size_x; ++x) {
					for (pos_T y = 0; y < table_size_y; ++y) {
						if (suggest[current][x][y] && suggest[current][x][y] <= 2) {
							pos_T x1 = x > 0 ? x - 1 : 0;
							pos_T x2 = x < table_size_x - 1 ? x + 2 : table_size_x;
							pos_T y1 = y > 0 ? y - 1 : 0;
							pos_T y2 = y < table_size_y - 1 ? y + 2 : table_size_y;
							bool far = true;
							for (pos_T xx = x1; xx < x2 && far; ++xx) {
								for (pos_T yy = y1; yy < y2 && far; ++yy) {
									if (table[xx][yy]) far = false;
								}
							}
							if (far) suggest[current][x][y] = 0;
						}
						if (suggest[current][x][y]) ++count;
					}
				}
			}
	
			// if there are not enough smart positions
			if (count < 4) {
				// check every near position
				treshold = 1;
				for (pos_T x = 0; x < table_size_x; ++x) {
					for (pos_T y = 0; y < table_size_y; ++y) {
						if (table[x][y] && table[x][y] != mark[2]) {
							pos_T x1 = x > 0 ? x - 1 : 0;
							pos_T x2 = x < table_size_x - 1 ? x + 2 : table_size_x;
							pos_T y1 = y > 0 ? y - 1 : 0;
							pos_T y2 = y < table_size_y - 1 ? y + 2 : table_size_y;
							for (pos_T xx = x1; xx < x2; ++xx) {
								for (pos_T yy = y1; yy < y2; ++yy) {
									if (!table[xx][yy]) suggest[current][xx][yy] = 1;
								}
							}
						}
					}
				}
			}
		}
	
		for (pos_T x = 0; x < table_size_x; ++x) {
			for (pos_T y = 0; y < table_size_y; ++y) {
				if (suggest[current][x][y] >= treshold) {
					suggestions.push_back(Field(x, y));
				}
			}
		}
	} else {
		for (pos_T x = 0; x < table_size_x; ++x) {
			for (pos_T y = 0; y < table_size_y; ++y) {
				if (table[x][y] && table[x][y] != mark[2]) {
					pos_T x1 = x > 0 ? x - 1 : 0;
					pos_T x2 = x < table_size_x - 1 ? x + 2 : table_size_x;
					pos_T y1 = y > 0 ? y - 1 : 0;
					pos_T y2 = y < table_size_y - 1 ? y + 2 : table_size_y;
					for (pos_T xx = x1; xx < x2; ++xx) {
						for (pos_T yy = y1; yy < y2; ++yy) {
							if (!table[xx][yy]) suggest[current][xx][yy] = 1;
						}
					}
				}
			}
		}
	
		for (pos_T x = 0; x < table_size_x; ++x) {
			for (pos_T y = 0; y < table_size_y; ++y) {
				if (suggest[current][x][y]) {
					suggestions.push_back(Field(x, y));
				}
			}
		}
	}

	assert(suggestions.size() > 0);
}
