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


#include "ai_interface.h"
#include "ai_impl.h"

#include <assert.h>

AiInterface::AiInterface() {
	aiImpl = new AiImpl();
}

AiInterface::~AiInterface() {
	delete aiImpl;
}

void AiInterface::setTableSize(pos_T tableSize) {
	assert(5 <= tableSize && tableSize <= max_table_size);
	aiImpl->table_size_x = aiImpl->table_size_y = tableSize;
}

void AiInterface::setTableSizeX(pos_T tableSizeX) {
	assert(5 <= tableSizeX && tableSizeX <= max_table_size);
	aiImpl->table_size_x = tableSizeX;
}

void AiInterface::setTableSizeY(pos_T tableSizeY) {
	assert(5 <= tableSizeY && tableSizeY <= max_table_size);
	aiImpl->table_size_y = tableSizeY;
}

void AiInterface::setDepth(int depth) {
	assert(depth > 0);
	aiImpl->start_depth = aiImpl->max_depth = depth;
	aiImpl->depth_increment = 2;
}

void AiInterface::setStartDepth(int startDepth) {
	assert(startDepth > 0);
	aiImpl->start_depth = startDepth;
	if (aiImpl->max_depth < aiImpl->start_depth)
		aiImpl->max_depth = aiImpl->start_depth;
}

void AiInterface::setMaxDepth(int maxDepth) {
	assert(maxDepth > 0);
	aiImpl->max_depth = maxDepth;
	if (aiImpl->start_depth > aiImpl->max_depth)
		aiImpl->start_depth = aiImpl->max_depth;
}

void AiInterface::setDepthIncrement(int depthIncrement) {
	assert(depthIncrement > 0);
	aiImpl->depth_increment = depthIncrement;
}

void AiInterface::setForceThinking(bool forceThinking) {
	aiImpl->force_thinking = forceThinking;
}

void AiInterface::setRandomAmount(int randomAmount) {
	aiImpl->heur_seed = randomAmount;
}

void AiInterface::setPrintInfo(bool printInfo) {
	aiImpl->print_info = printInfo;
}

void AiInterface::setTimeOver(AiTimeOver* timeOver) {
	assert(timeOver);
	aiImpl->timeOver = timeOver;
}

void AiInterface::newGame() {
	aiImpl->newGame();
}

void AiInterface::step(pos_T x, pos_T y) {
	aiImpl->step(x, y);
}

void AiInterface::stepServer(pos_T x, pos_T y) {
	aiImpl->stepServer(x, y);
}

void AiInterface::undo() {
	aiImpl->undo();
}

Field AiInterface::think() {
	return aiImpl->think();
}
