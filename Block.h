#pragma once

#include <sstream>
#include <stdlib.h>
#include "Line.h"

struct Block {
	Line* line;
	Block* childBlock;
	Block* nextBlock;
	Block* parentBlock;
	string Out() {
		Block* buffer = this->childBlock;
		stringstream c;
		c << line->ToString();
		while (buffer != nullptr) {
			c << buffer->Out();
			buffer = buffer->nextBlock;
		}
		c << line->GetOut();
		return c.str();
	}
};