#pragma once

#include <stdlib.h>
#include <string>
#include "ReadIns.h"

using namespace std;

//表达式tree结构
struct ExpTree {
	//Token类型
	static enum Type {
		Num, Var, SignOne, SignTwo
	};
	Type type;				//节点token类型
	bool Float;				//节点是否为浮点
	//节点存的值
	union {
		int valueI;			//整型数
		float valueF;		//浮点型数
		string* var;		//变量名
		string* sign;		//符号
	} value;
	ExpTree* leftTree;		//左子树
	ExpTree* rightTree;		//右子树，一元操作符存入右子树
	ExpTree* parentTree;	//母树
	
	//输出
	string Out(int Tab) {
		//数
		if (type == Type::Num) {
			char c[20];
			if (Float)
				sprintf_s(c, "%.7ff", value.valueF);
			else
				sprintf_s(c, "%d", value.valueI);
			return string(Tab * 4, ' ') + string(c);
		}
		//一元操作符
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
		//二元操作符
		else if (type == Type::SignTwo) {
			return leftTree->Out(Tab) + ";\n" + rightTree->Out(Tab) + ";\n" + string(Tab * 4, ' ')
				+ signIns(leftTree->Float || rightTree->Float, *(value.sign));
		}
		//变量
		else {
			return string(Tab * 4, ' ') + ((*(value.var)).at(0) == '[' ? "" :
				(Float ? "%" : "$")) + *(value.var);
		}
	}
};