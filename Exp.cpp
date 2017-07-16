#include "stdafx.h"
#include <sstream>
#include <stack>
#include "Exp.h"

Exp::Exp()
{
}


Exp::~Exp()
{
}

string Exp::Out(int Tab)
{
	return "";
}

ExpTree::Type Exp::type()
{
	return ExpTree::Type();
}

bool Exp::Float()
{
	return false;
}

Exp_real::Exp_real(string value)
{
	stringstream ss;
	Status s = Space;
	string c;
	ExpTree::Type createToken;
	ExpTree::Type lastToken = ExpTree::Type::SignTwo;
	bool moveCursor = true;
	bool end = false;
	bool Float = false;
	stack<ExpTree*> expStack = stack<ExpTree*>();
	stack<string> signStack = stack<string>();

	for (int i = 0; i < value.length(); moveCursor ? (i++) : (moveCursor = true)) {
		AscType a = GetType(value.at(i));
		if (s == Space) {
			switch (a) {
			case AscType::Character:
				s = Variable;
				createToken = ExpTree::Type::Var;
				ss << value.at(i);
				end = false;
				continue;
			case AscType::Number:
				s = Number;
				createToken = ExpTree::Type::Num;
				ss << value.at(i);
				end = false;
				continue;
			case AscType::Space:
				continue;
			case AscType::Braket:
				if (value.at(i) == '(') {
					signStack.push("(");
					end = false;
					continue;
				}
				ss << value.at(i);
				end = true;
				break;
			case AscType::Sign:
				s = Sign;
				createToken = ExpTree::Type::SignTwo;
				ss << value.at(i);
				end = false;
				continue;
			default:
				throw("unexpected expression");
			}
		}
		else if (s == Variable) {
			switch (a) {
			case AscType::Character:
			case AscType::Number:
				ss << value.at(i);
				end = false;
				continue;
			case AscType::Space:
				s = Space;
				end = true;
				break;
				/*case AscType::Colon:

				break;*/
			case AscType::Sign:
				s = Space;
				moveCursor = false;
				end = true;
				break;
			case AscType::Braket:
				/*if (value.at(i) == '(')
				throw("unexpected expression");*/
				s = Space;
				moveCursor = false;
				end = true;
				break;
			default:
				throw("unexpected expression");
			}
		}
		else if (s == Number) {
			switch (a) {
			case AscType::Number:
				ss << value.at(i);
				end = false;
				continue;
			case AscType::Dot:
				if (createToken == ExpTree::Type::Num) {
					if (Float)
						throw("this float number has two points");
					Float = true;
				}
				ss << value.at(i);
				end = false;
				continue;
			case AscType::Space:
				s = Space;
				end = true;
				break;
				/*case AscType::Colon:

				break;*/
			case AscType::Sign:
				s = Space;
				moveCursor = false;
				end = true;
				break;
			case AscType::Braket:
				if (value.at(i) == '(')
					throw("unexpected expression");
				s = Space;
				moveCursor = false;
				end = true;
				break;
			default:
				throw("unexpected expression");
			}
		}
		else if (s == Sign) {
			switch (a) {
			case AscType::Character:
				s = Space;
				moveCursor = false;
				end = true;
				break;
			case AscType::Number:
				s = Space;
				moveCursor = false;
				end = true;
				break;
			case AscType::Space:
				s = Space;
				end = true;
				break;
			case AscType::Braket:
				s = Space;
				moveCursor = false;
				end = true;
				break;
			case AscType::Sign:
				ss << value.at(i);
				end = false;
				continue;
			default:
				throw("unexpected expression");
			}
		}
		c = ss.str();

		if (c == ")") {
			while (signStack.empty() || signStack.top() != "(") {
				//取出一对
				if (signStack.empty())
					throw("unclosed braket");
				ExpTree* b2 = expStack.top();
				if (ReadIns::getFirst(signStack.top()) != 1) {
					expStack.pop();
					ExpTree* b1 = expStack.top();
					if (b1->type == ExpTree::Type::Num && b2->type == ExpTree::Type::Num) {
						if (!(b1->Float) && !(b2->Float)) {
							string sign = signStack.top();
							signStack.pop();
							b1->value.valueI = calculate(sign, b1->value.valueI, b2->value.valueI);
						}
						else {
							if (!(b1->Float))
								b1->value.valueF = (float)(b1->value.valueI);
							if (!(b2->Float))
								b2->value.valueF = (float)(b2->value.valueI);
							b1->Float = true;
							string sign = signStack.top();
							signStack.pop();
							b1->value.valueF = calculate(sign, b1->value.valueF, b2->value.valueF);
						}
					}
					else {
						expStack.pop();
						ExpTree* b = (ExpTree*)malloc(sizeof(struct ExpTree));
						b->leftTree = b1;
						b->rightTree = b2;
						b->parentTree = nullptr;
						b->type = ExpTree::Type::SignTwo;
						b->value.sign = new string(signStack.top());
						b->Float = b1->Float || b2->Float;
						if (ReadIns::getFirst(signStack.top()) == 4)//是布尔符号
							b->Float = false;
						signStack.pop();
						b1->parentTree = b;
						b2->parentTree = b;
						expStack.push(b);
					}
				}
				else {
					if (b2->type == ExpTree::Type::Num) {
						string sign = signStack.top();
						signStack.pop();
						if (b2->Float)
							b2->value.valueF = calculate(sign, 0., b2->value.valueF);
						else
							b2->value.valueI = calculate(sign, 0, b2->value.valueI);
					}
					else {
						expStack.pop();
						ExpTree* b = (ExpTree*)malloc(sizeof(struct ExpTree));
						b->leftTree = nullptr;
						b->rightTree = b2;
						b->parentTree = nullptr;
						b->type = ExpTree::Type::SignOne;
						b->value.sign = new string(signStack.top());
						if (signStack.top() == "_f" || signStack.top() == "_S")
							b->Float = !(b2->Float);
						else
							b->Float = (signStack.top() == "sqrt") || b2->Float;
						signStack.pop();
						b2->parentTree = b;
						expStack.push(b);
					}
				}
			}
			signStack.pop();
			end = true;
			ss.str("");
			continue;
		}
		else if (createToken == ExpTree::Type::Var) {
			if (c == "pi") {
				createToken = ExpTree::Type::Num;
				Float = true;
				c = "3.1415926";
			}
			else if (ReadIns::constant.find(c) != ReadIns::constant.end()) {
				createToken = ExpTree::Type::Num;
				c = (*ReadIns::constant.find(c)).second;
			}
			else if (ReadIns::customFunction.find(c) != ReadIns::customFunction.end())
				createToken = ExpTree::Type::SignOne;
			else if (ReadIns::globalVariable.find(c) != ReadIns::globalVariable.end()) {
				pair<int, NumType> d = ReadIns::globalVariable.find(c)->second;
				if (d.second == NumType::Float)
					Float = true;
				char s[10];
				sprintf_s(s, (d.second == NumType::Float) ? "%d.0f]" : "%d]", d.first);
				c = "[-" + string(s);
			}
			else if (ReadIns::varF.find(c) != ReadIns::varF.end())
				Float = true;
			else if (ReadIns::varI.find(c) == ReadIns::varI.end()) {
				if (Float = (c.substr(0, 7) == "STACK_F") || c.substr(0, 7) == "STACK_I") {
					c = "[-" + c.substr(7) + (Float ? ".0f]" : "]");
				}
				else
					throw("variable not found");
			}
		}
		else {
			if (c == "!")
				createToken = ExpTree::Type::SignOne;
			else if (c == "-" && lastToken == ExpTree::Type::SignTwo) {
				createToken = ExpTree::Type::SignOne;
				c = "--";
			}
		}

		if (createToken == ExpTree::Type::SignOne)
			signStack.push(c);
		else if (createToken == ExpTree::Type::SignTwo) {
			if (lastToken == ExpTree::Type::SignTwo)
				throw("unexpected expression");
			while (!signStack.empty() && signStack.top() != "("
				&& ReadIns::getFirst(c) >= ReadIns::getFirst(signStack.top())) {
				//取出一对
				ExpTree* b2 = expStack.top();
				if (ReadIns::getFirst(signStack.top()) != 1) {
					expStack.pop();
					ExpTree* b1 = expStack.top();
					if (b1->type == ExpTree::Type::Num && b2->type == ExpTree::Type::Num) {
						if (!(b1->Float) && !(b2->Float)) {
							string sign = signStack.top();
							signStack.pop();
							b1->value.valueI = calculate(sign, b1->value.valueI, b2->value.valueI);
						}
						else {
							if (!(b1->Float))
								b1->value.valueF = (float)(b1->value.valueI);
							if (!(b2->Float))
								b2->value.valueF = (float)(b2->value.valueI);
							b1->Float = true;
							string sign = signStack.top();
							signStack.pop();
							b1->value.valueF = calculate(sign, b1->value.valueF, b2->value.valueF);
						}
					}
					else {
						expStack.pop();
						ExpTree* b = (ExpTree*)malloc(sizeof(struct ExpTree));
						b->leftTree = b1;
						b->rightTree = b2;
						b->parentTree = nullptr;
						b->type = ExpTree::Type::SignTwo;
						b->value.sign = new string(signStack.top());
						b->Float = b1->Float || b2->Float;
						if (ReadIns::getFirst(signStack.top()) == 4)//是布尔符号
							b->Float = false;
						signStack.pop();
						b1->parentTree = b;
						b2->parentTree = b;
						expStack.push(b);
					}
				}
				else
					if (b2->type == ExpTree::Type::Num) {
						string sign = signStack.top();
						signStack.pop();
						if (b2->Float)
							b2->value.valueF = calculate(sign, 0., b2->value.valueF);
						else
							b2->value.valueI = calculate(sign, 0, b2->value.valueI);
					}
					else {
						expStack.pop();
						ExpTree* b = (ExpTree*)malloc(sizeof(struct ExpTree));
						b->leftTree = nullptr;
						b->rightTree = b2;
						b->parentTree = nullptr;
						b->type = ExpTree::Type::SignOne;
						b->value.sign = new string(signStack.top());
						if (signStack.top() == "_f" || signStack.top() == "_S")
							b->Float = !(b2->Float);
						else
							b->Float = (signStack.top() == "sqrt") || b2->Float;
						signStack.pop();
						b2->parentTree = b;
						expStack.push(b);
					}
			}
			signStack.push(c);
		}
		else {
			if (lastToken <= 1)
				throw("unexpected expression");
			ExpTree* b = (ExpTree*)malloc(sizeof(struct ExpTree));
			b->leftTree = nullptr;
			b->rightTree = nullptr;
			b->parentTree = nullptr;
			b->type = createToken;
			b->Float = Float;
			if (createToken == ExpTree::Type::Num) {
				if (Float)
					b->value.valueF = atof(c.c_str());
				else
					b->value.valueI = atoi(c.c_str());
			}
			else
				b->value.var = new string(c);
			expStack.push(b);
		}
		ss.str("");
		Float = false;
		lastToken = createToken;
	}
	if (!end) {
		c = ss.str();

		if (c == ")") {
			while (signStack.empty() || signStack.top() != "(") {
				//取出一对
				if (signStack.empty())
					throw("unclosed braket");
				ExpTree* b2 = expStack.top();
				if (ReadIns::getFirst(signStack.top()) != 1) {
					expStack.pop();
					ExpTree* b1 = expStack.top();
					if (b1->type == ExpTree::Type::Num && b2->type == ExpTree::Type::Num) {
						if (!(b1->Float) && !(b2->Float)) {
							string sign = signStack.top();
							signStack.pop();
							b1->value.valueI = calculate(sign, b1->value.valueI, b2->value.valueI);
						}
						else {
							if (!(b1->Float))
								b1->value.valueF = (float)(b1->value.valueI);
							if (!(b2->Float))
								b2->value.valueF = (float)(b2->value.valueI);
							b1->Float = true;
							string sign = signStack.top();
							signStack.pop();
							b1->value.valueF = calculate(sign, b1->value.valueF, b2->value.valueF);
						}
					}
					else {
						expStack.pop();
						ExpTree* b = (ExpTree*)malloc(sizeof(struct ExpTree));
						b->leftTree = b1;
						b->rightTree = b2;
						b->parentTree = nullptr;
						b->type = ExpTree::Type::SignTwo;
						b->value.sign = new string(signStack.top());
						b->Float = b1->Float || b2->Float;
						if (ReadIns::getFirst(signStack.top()) == 4)//是布尔符号
							b->Float = false;
						signStack.pop();
						b1->parentTree = b;
						b2->parentTree = b;
						expStack.push(b);
					}
				}
				else {
					if (b2->type == ExpTree::Type::Num) {
						string sign = signStack.top();
						signStack.pop();
						if (b2->Float)
							b2->value.valueF = calculate(sign, 0., b2->value.valueF);
						else
							b2->value.valueI = calculate(sign, 0, b2->value.valueI);
					}
					else {
						expStack.pop();
						ExpTree* b = (ExpTree*)malloc(sizeof(struct ExpTree));
						b->leftTree = nullptr;
						b->rightTree = b2;
						b->parentTree = nullptr;
						b->type = ExpTree::Type::SignOne;
						b->value.sign = new string(signStack.top());
						if (signStack.top() == "_f" || signStack.top() == "_S")
							b->Float = !(b2->Float);
						else
							b->Float = (signStack.top() == "sqrt") || b2->Float;
						signStack.pop();
						b2->parentTree = b;
						expStack.push(b);
					}
				}
			}

		}
		else if (createToken == ExpTree::Type::Var) {
			if (c == "pi") {
				createToken = ExpTree::Type::Num;
				Float = true;
				c = "3.1415926";
			}
			else if (ReadIns::constant.find(c) != ReadIns::constant.end()) {
				createToken = ExpTree::Type::Num;
				c = (*ReadIns::constant.find(c)).second;
			}
			else if (ReadIns::customFunction.find(c) != ReadIns::customFunction.end())
				createToken = ExpTree::Type::SignOne;
			else if (ReadIns::globalVariable.find(c) != ReadIns::globalVariable.end()) {
				pair<int, NumType> d = ReadIns::globalVariable.find(c)->second;
				if (d.second == NumType::Float)
					Float = true;
				char s[10];
				sprintf_s(s, (d.second == NumType::Float) ? "%d.0f]" : "%d]", d.first);
				c = "[-" + string(s);
			}
			else if (ReadIns::varF.find(c) != ReadIns::varF.end())
				Float = true;
			else if (ReadIns::varI.find(c) == ReadIns::varI.end()) {
				if (Float = (c.substr(0, 7) == "STACK_F") || (c.substr(0, 7) == "STACK_I")) {
					c = "[-" + c.substr(7) + (Float ? ".0f]" : "]");
				}
				else
					throw("variable not found");
			}
		}
		else {
			if (c == "!")
				createToken = ExpTree::Type::SignOne;
			else if (c == "-" && lastToken == ExpTree::Type::SignTwo) {
				createToken = ExpTree::Type::SignOne;
				c = "--";
			}
		}


		if (createToken == ExpTree::Type::SignOne) {
			ExpTree* b1 = expStack.top();
			if (b1->type == ExpTree::Type::Num) {
				if (b1->Float)
					b1->value.valueF = calculate(c, 0., b1->value.valueF);
				else
					b1->value.valueI = calculate(c, 0, b1->value.valueI);
			}
			else {
				expStack.pop();
				ExpTree* b = (ExpTree*)malloc(sizeof(struct ExpTree));
				b->leftTree = nullptr;
				b->rightTree = b1;
				b->parentTree = nullptr;
				b->type = createToken;
				b->value.sign = new string(c);
				b1->parentTree = b;
				expStack.push(b);
			}
		}
		else if (createToken == ExpTree::Type::SignTwo) {
			if (lastToken == ExpTree::Type::SignTwo)
				throw("unexpected expression");
			while (!signStack.empty() && signStack.top() != "("
				&& ReadIns::getFirst(c) >= ReadIns::getFirst(signStack.top())) {
				//取出一对
				ExpTree* b2 = expStack.top();
				if (ReadIns::getFirst(signStack.top()) != 1) {
					expStack.pop();
					ExpTree* b1 = expStack.top();
					if (b1->type == ExpTree::Type::Num && b2->type == ExpTree::Type::Num) {
						if (!(b1->Float) && !(b2->Float)) {
							string sign = signStack.top();
							signStack.pop();
							b1->value.valueI = calculate(sign, b1->value.valueI, b2->value.valueI);
						}
						else {
							if (!(b1->Float))
								b1->value.valueF = (float)(b1->value.valueI);
							if (!(b2->Float))
								b2->value.valueF = (float)(b2->value.valueI);
							b1->Float = true;
							string sign = signStack.top();
							signStack.pop();
							b1->value.valueF = calculate(sign, b1->value.valueF, b2->value.valueF);
						}
					}
					else {
						expStack.pop();
						ExpTree* b = (ExpTree*)malloc(sizeof(struct ExpTree));
						b->leftTree = b1;
						b->rightTree = b2;
						b->parentTree = nullptr;
						b->type = ExpTree::Type::SignTwo;
						b->value.sign = new string(signStack.top());
						b->Float = b1->Float || b2->Float;
						if (ReadIns::getFirst(signStack.top()) == 4)//是布尔符号
							b->Float = false;
						signStack.pop();
						b1->parentTree = b;
						b2->parentTree = b;
						expStack.push(b);
					}
				}
				else
					if (b2->type == ExpTree::Type::Num) {
						string sign = signStack.top();
						signStack.pop();
						if (b2->Float)
							b2->value.valueF = calculate(sign, 0., b2->value.valueF);
						else
							b2->value.valueI = calculate(sign, 0, b2->value.valueI);
					}
					else {
						expStack.pop();
						ExpTree* b = (ExpTree*)malloc(sizeof(struct ExpTree));
						b->leftTree = nullptr;
						b->rightTree = b2;
						b->parentTree = nullptr;
						b->type = ExpTree::Type::SignOne;
						b->value.sign = new string(signStack.top());
						if (signStack.top() == "_f" || signStack.top() == "_S")
							b->Float = !(b2->Float);
						else
							b->Float = (signStack.top() == "sqrt") || b2->Float;
						signStack.pop();
						b2->parentTree = b;
						expStack.push(b);
					}
			}
			signStack.push(c);
		}
		else {
			if (lastToken <= 1)
				throw("unexpected expression");
			ExpTree* b = (ExpTree*)malloc(sizeof(struct ExpTree));
			b->leftTree = nullptr;
			b->rightTree = nullptr;
			b->parentTree = nullptr;
			b->type = createToken;
			b->Float = Float;
			if (createToken == ExpTree::Type::Num) {
				if (Float)
					b->value.valueF = atof(c.c_str());
				else
					b->value.valueI = atoi(c.c_str());
			}
			else
				b->value.var = new string(c);
			expStack.push(b);
		}
	}


	while (!signStack.empty()) {
		//取出一对
		if (signStack.top() == "(")
			throw("unclosed braket");
		ExpTree* b2 = expStack.top();
		if (ReadIns::getFirst(signStack.top()) != 1) {
			expStack.pop();
			ExpTree* b1 = expStack.top();
			if (b1->type == ExpTree::Type::Num && b2->type == ExpTree::Type::Num) {
				if (!(b1->Float) && !(b2->Float)) {
					string sign = signStack.top();
					signStack.pop();
					b1->value.valueI = calculate(sign, b1->value.valueI, b2->value.valueI);
				}
				else {
					if (!(b1->Float))
						b1->value.valueF = (float)(b1->value.valueI);
					if (!(b2->Float))
						b2->value.valueF = (float)(b2->value.valueI);
					b1->Float = true;
					string sign = signStack.top();
					signStack.pop();
					b1->value.valueF = calculate(sign, b1->value.valueF, b2->value.valueF);
				}
			}
			else {
				expStack.pop();
				ExpTree* b = (ExpTree*)malloc(sizeof(struct ExpTree));
				b->leftTree = b1;
				b->rightTree = b2;
				b->parentTree = nullptr;
				b->type = ExpTree::Type::SignTwo;
				b->value.sign = new string(signStack.top());
				b->Float = b1->Float || b2->Float;
				if (ReadIns::getFirst(signStack.top()) == 4)//是布尔符号
					b->Float = false;
				signStack.pop();
				b1->parentTree = b;
				b2->parentTree = b;
				expStack.push(b);
			}
		}
		else {
			if (b2->type == ExpTree::Type::Num) {
				string sign = signStack.top();
				signStack.pop();
				if (b2->Float)
					b2->value.valueF = calculate(sign, 0., b2->value.valueF);
				else
					b2->value.valueI = calculate(sign, 0, b2->value.valueI);
			}
			else {
				expStack.pop();
				ExpTree* b = (ExpTree*)malloc(sizeof(struct ExpTree));
				b->leftTree = nullptr;
				b->rightTree = b2;
				b->parentTree = nullptr;
				b->type = ExpTree::Type::SignOne;
				b->value.sign = new string(signStack.top());
				if (signStack.top() == "_f" || signStack.top() == "_S")
					b->Float = !(b2->Float);
				else
					b->Float = (signStack.top() == "sqrt") || b2->Float;
				signStack.pop();
				b2->parentTree = b;
				expStack.push(b);
			}
		}
	}

	tree = expStack.top();
}

Exp_real::~Exp_real()
{
}

string Exp_real::Out(int Tab)
{
	return tree->Out(Tab);
}

ExpTree::Type Exp_real::type()
{
	return tree->type;
}

bool Exp_real::Float()
{
	return tree->Float;
}

bool Exp_real::One()
{
	return tree->value.valueI == 1;
}

Exp_str::Exp_str(string value)
{
	this->value = value;
}

Exp_str::~Exp_str()
{
}

string Exp_str::Out(int Tab)
{
	return value;
}

Exp_diff::Exp_diff(string value)
{
	int n;
	for (int i = 0; i < 3; i++) {
		n = value.find(':');
		if (n == -1)
			throw("difficulty-select expression aren't four expressions");
		this->value[i] = new Exp_real(value.substr(0, n));
		value = value.substr(n + 1);
	}
	this->value[3] = new Exp_real(value);
}

Exp_diff::~Exp_diff()
{
}

string Exp_diff::Out(int Tab)
{
	return Out(Tab, false);
}

string Exp_diff::Out(int Tab, bool ifNext)
{
	if (!this->Float())
		return  "!EWX\n" + value[0]->Out(Tab) + ";\n!NWX\n" + value[1]->Out(Tab) +
		";\n!HWX\n" + value[2]->Out(Tab) + ";\n!LW\n" + value[3]->Out(Tab) + ";\n";
	else {
		string c = "!EWX\n";
		c += value[0]->Out(Tab) + (value[0]->Float() ? "" :
			(";\n" + string(Tab * 4, ' ') + "_f([-1])")) + ";\n!NWX\n";
		c += value[1]->Out(Tab) + (value[1]->Float() ? "" :
			(";\n" + string(Tab * 4, ' ') + "_f([-1])")) + ";\n!HWX\n";
		c += value[2]->Out(Tab) + (value[2]->Float() ? "" :
			(";\n" + string(Tab * 4, ' ') + "_f([-1])")) + ";\n!LW\n";
		c += value[3]->Out(Tab) + (value[3]->Float() ? "" :
			(";\n" + string(Tab * 4, ' ') + "_f([-1])")) + ";\n";
		return c;
	}
}

string Exp_diff::Stack_23(int Tab)
{
	string c = "";
	for (int j = 0; j < 4; j++) {
		if (value[j]->type() != ExpTree::Type::Num && value[j]->type() != ExpTree::Type::Var) {
			c += value[j]->Out(Tab) + ";\n";
		}
	}
	return c;
}

string Exp_diff::Stack_23(int &i, int Tab)
{
	string c = "";
	for (int j = 0; j < 4; j++) {
		if (value[j]->type() != ExpTree::Type::Num && value[j]->type() != ExpTree::Type::Var) {
			i++; c += value[j]->Out(Tab) + ";\n";
		}
	}
	return c;
}

string Exp_diff::Out_23(int &i)
{
	string c = "";
	for (int j = 0; j < 4; j++) {
		if (value[j]->type() != ExpTree::Type::Num && value[j]->type() != ExpTree::Type::Var) {
			i++;
		}
	}
	for (int j = 0; j < 4; j++) {
		if (value[j]->type() == ExpTree::Type::Num || value[j]->type() == ExpTree::Type::Var) {
			c += value[j]->Out(0) + ", ";
		}
		else {
			char a[10];
			if (value[j]->Float())
				sprintf_s(a, "%d.0f], ", --i);
			else
				sprintf_s(a, "%d], ", --i);
			c += "[-" + string(a);
		}
	}
	return c.substr(0, c.length() - 2);
}

string Exp_diff::Out_23()
{
	return value[0]->Out(0) + ", " + value[1]->Out(0) + ", " + value[2]->Out(0) + ", " +
		value[3]->Out(0);
}

string Exp_diff::Out_23(bool Float)
{
	string c = "";
	for (int i = 0; i < 4; i++) {
		if (Float ^ value[i]->Float())
			c += (Float ? "_f(" : "_S(") + value[i]->Out(0) + "), ";
		else
			c += value[i]->Out(0) + ", ";
	}
	return c.substr(0, c.length() - 2);
}

ExpTree::Type Exp_diff::type()
{
	return ExpTree::Type();
}

bool Exp_diff::Float()
{
	return value[0]->Float() || value[1]->Float() || value[2]->Float() ||
		value[3]->Float();
}
