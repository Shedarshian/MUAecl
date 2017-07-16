#pragma once

#include <stdlib.h>
#include <string>
#include "ReadIns.h"

using namespace std;

struct ExpTree {
	static enum Type {
		Num, Var, SignOne, SignTwo
	};
	Type type;
	bool Float;
	union {
		int valueI;
		float valueF;
		string* var;
		string* sign;
	} value;
	ExpTree* leftTree;
	ExpTree* rightTree;
	ExpTree* parentTree;
	string Out(int Tab) {
		if (type == Type::Num) {
			char c[20];
			if (Float)
				sprintf_s(c, "%.7ff", value.valueF);
			else
				sprintf_s(c, "%d", value.valueI);
			return string(Tab * 4, ' ') + string(c);
		}
		else if (type == Type::SignOne) {
			string c;
			if (*(value.sign) == "_f")
				c = rightTree->Out(Tab) + ";\n" + string(Tab * 4, ' ') + "_f([-1])";
			else if (*(value.sign) == "_S")
				c = rightTree->Out(Tab) + ";\n" + string(Tab * 4, ' ') + "_S([-1.0f])";
			else
				c = rightTree->Out(Tab) + ";\n" + string(Tab * 4, ' ') + signIns(rightTree->Float, *(value.sign));
			return c;
		}
		else if (type == Type::SignTwo) {
			return leftTree->Out(Tab) + ";\n" + rightTree->Out(Tab) + ";\n" + string(Tab * 4, ' ')
				+ signIns(leftTree->Float || rightTree->Float, *(value.sign));
		}
		else {
			return string(Tab * 4, ' ') + ((*(value.var)).at(0) == '[' ? "" :
				(Float ? "%" : "$")) + *(value.var);
		}
	}
};