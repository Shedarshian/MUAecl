#include "stdafx.h"
#include <sstream>
#include "Line.h"
#include "Exp.h"
#include "CommaExp.h"
#include "Block.h"


Line::Line(int Tab)
{
	this->Tab = Tab;
}


Line::~Line()
{
}

string Line::GetOut()
{
	return "";
}

Line_Sub::Line_Sub(string sub_name, string value) :Line(Tab)
{
	this->sub_name = sub_name;
	CommaExp_Declare ce = CommaExp_Declare(value, sub_name);
	this->value = ce.Out();
}

Line_Sub::~Line_Sub()
{
}

string Line_Sub::ToString()
{
	string c;
	for (list<string>::iterator it = declare.begin(); it != declare.end(); it++) {
		c += " " + (*it);
	}
	return "\nsub " + sub_name + "(" + value + ")\n{\n    var" + c + ";\n";
}

string Line_Sub::GetOut()
{
	return "}\n";
}

void Line_Sub::Declare(string c)
{
	declare.push_back(c);
}

Line_Value::Line_Value(string var, string value, bool Float, int Tab) :Line(Tab)
{
	this->var = (Float ? "%" : "$") + var;
	if (value.find(':') != -1)
		this->value = new Exp_diff(value);
	else
		this->value = new Exp_real(value);
	this->Float = Float;
}

Line_Value::Line_Value(int globalNo, string value, bool Float, int Tab) :Line(Tab)
{
	char a[10];
	sprintf_s(a, "%d", globalNo);
	this->var = "[-" + string(a) + (Float ? ".0f]" : "]");
	if (value.find(':') != -1)
		this->value = new Exp_diff(value);
	else
		this->value = new Exp_real(value);
	this->Float = Float;
}

Line_Value::~Line_Value()
{
}

Line_Value_Real::Line_Value_Real(string var, string value, bool Float, int Tab)
	:Line_Value(var, value, Float, Tab)
{
}

Line_Value_Real::Line_Value_Real(int globalNo, string value, bool Float, int Tab)
	:Line_Value(globalNo, value, Float, Tab)
{
}

Line_Value_Real::~Line_Value_Real()
{
}

string Line_Value_Real::ToString()
{
	Exp_diff* ed = dynamic_cast<Exp_diff*>(value);
	if (ed) {
		int i = 1;
		return ed->Stack_23(Tab) + string(Tab * 4, ' ') +
			(Float ? "ins_536(" : "ins_535(") + var +
			", " + ed->Out_23(i) + ");\n";//TODO，不同世代
	}
	else {
		if (value->type() == ExpTree::Type::Num || value->type() == ExpTree::Type::Var) {
			if (value->Float() ^ Float) {
				return string(Tab * 4, ' ') + var + " = " +
					(Float ? "_f(" : "_S(") + value->Out(0) + ");\n";
			}
			return string(Tab * 4, ' ') + var + " = " +
				value->Out(0) + ";\n";
		}
		return value->Out(Tab) + ";\n" + string(Tab * 4, ' ') + 
			(Float ? "ins_45(" : "ins_43(") + var + ");\n";
	}
}

Line_Value_Sign::Line_Value_Sign(string var, string value, string sign, bool Float, int Tab)
	:Line_Value(var, value, Float, Tab)
{
	this->sign = sign;
}

Line_Value_Sign::Line_Value_Sign(int globalNo, string value, string sign, bool Float, int Tab)
	: Line_Value(globalNo, value, Float, Tab)
{
	this->sign = sign;
}

Line_Value_Sign::~Line_Value_Sign()
{
}

string Line_Value_Sign::ToString()
{
	if (value->type() == ExpTree::Type::Num || value->type() == ExpTree::Type::Var) {
		if (value->Float() ^ Float) {
			return string(Tab * 4, ' ') + var + " = " + var + " " + sign + " " +
				(Float ? "_f(" : "_S(") + value->Out(0) + ");\n";
		}
		return string(Tab * 4, ' ') + var + " = " + var + " " + sign + " " +
			value->Out(0) + ";\n";
	}
	return string(Tab * 4, ' ') + var + ";\n" + value->Out(Tab) + ";\n" + 
		string(Tab * 4, ' ') + signIns(Float, sign) + ";\n" + string(Tab * 4, ' ') +
		(Float ? "ins_45(" : "ins_43(") + var + ");\n";
}

Line_Diff::Line_Diff(string value)
{
	this->value = value;
}

Line_Diff::~Line_Diff()
{
}

string Line_Diff::ToString()
{
	if (value == "L")
		return "!LW\n";
	else if (value == "E" || value == "N" || value == "H")
		return "!" + value + "WX\n";
	else if (value == "ENHL")
		return "!ENHL\n";
	else
		return "!" + value + "WX\n";
}

Line_Ins::Line_Ins(string ins_name, string value, int Tab) :Line(Tab)
{
	pair<int, deque<NumType>> p = ReadIns::ins.find(ins_name)->second;
	ins = p.first;
	var = p.second;
	this->value = new CommaExp_Exp(value, ins == 23);//TODO，不同世代
}

Line_Ins::~Line_Ins()
{
}

string Line_Ins::ToString()
{
	stringstream c;
	string s = "";
	if (value->chg23())
		ins = 548;
	if (var.size() == 0) {
		c << value->Stack(Tab) << string(Tab * 4, ' ') << "ins_" << ins << "();\n";
		return c.str();
	}
	c << value->Stack(Tab) << string(Tab * 4, ' ') << "ins_" << ins << "(";
	if (var.at(0) == Anything) {
		s += value->Out() + ");\n";
	}
	else {
		s += value->Out(var) + ");\n";
	}
	return c.str() + s;
}

Line_Mode::Line_Mode(string mode_name, string value, int Tab) :Line(Tab)
{
	map<string, pair<int, deque<NumType>>>::iterator it =
		ReadIns::mode.find(mode_name);
	if (it == ReadIns::mode.end())
		throw("unexpected mode name");
	mode = it->second.first;
	var = it->second.second;
	ifLong = (var.size() > 4 || (var.size() != 0 && var.at(0) == Anything));
	//3个int1个float这样的情况无法分辨，不过目前没有这种情况
	this->value = new CommaExp_Exp(value, false);
	haveNum = (this->value->size() - var.size() == 3);
	if (this->value->size() - var.size() != 2 && this->value->size() - var.size() != 3)
		throw("wrong arg list");
}

Line_Mode::~Line_Mode()
{
}

string Line_Mode::ToString()
{
	int ins = 609 + (ifLong ? 1 : 0) + (haveNum ? 0 : 2);//TODO,不同世代
	stringstream c;
	string s = "";
	c << value->Stack(Tab) << string(Tab * 4, ' ') << "ins_" << ins << "(";
	if (var.size() != 0 && var.at(0) == Anything) {
		s += value->Out(ifLong, haveNum, mode) + ");\n";
	}
	else {
		s += value->Out(ifLong, haveNum, mode, var) + ");\n";
	}
	return c.str() + s;
}

Line_Inte::Line_Inte(string name, string value, int Tab)
{
	if (name == "set_et") {
		CommaExp_600* c = new CommaExp_600(value);
		this->value = c->Out(Tab);
	}
	else if (name == "CardStart") {
		this->value = R"(    ins_513();
    ins_21();
    ins_525();
    ins_300("Ecl_EtBreak", 0.0f, 0.0f, 9999, 0, 0);
    ins_523();
    ins_632(0);
    ins_545();
    ins_516(27);
    ins_404(0.0f, 0.0f);
    ins_405(0, 0, 0.0f, 0.0f);
    ins_401(0, 0, 0.0f, 0.0f);
    [-9949] = 0;
    [-9948] = 0;
    [-9947] = 1;
)";
	}//TODO，不同世代
	else if (name == "CardEnd") {
		this->value = R"(    ins_513();
    ins_21();
    ins_525();
    [-9986];
    0;
    ins_59();
    ins_13(MUA_00, 0);
    ins_615(640.0f);
    goto MUA_01 @ 0;
MUA_00:
    ins_616(640.0f);
MUA_01:
    ins_523();
    ins_632(0);
    ins_545();
    ins_516(27);
    ins_404(0.0f, 0.0f);
    ins_405(0, 0, 0.0f, 0.0f);
    ins_401(0, 0, 0.0f, 0.0f);
    [-9949] = 0;
    [-9948] = 0;
    [-9947] = 1;
)";
	}//TODO，不同世代
	else if (name == "EffCharge") {

	}
}

Line_Inte::~Line_Inte()
{
}

string Line_Inte::ToString()
{
	return value;
}

Line_Thread::Line_Thread(string sub_name, string value, int Tab) :Line(Tab)
{//在输出的时候再做检测
	this->sub_name = sub_name;
	line = ReadIns::line;
	int n = ((value.find(',') == -1) ? value.length() : value.find(','));
	string c;
	if (value.find_first_not_of(' ') != -1) {
		c = value.substr(value.find_first_not_of(' '));
	}
	if (wait = (c.substr(0, 4) == "WAIT")) {
		value = value.substr(n);
		if (value != "")
			value = value.substr(1);
	}
	this->value = new CommaExp_Thread(value, sub_name);
}

Line_Thread::~Line_Thread()
{
}

string Line_Thread::ToString()
{
	if (ReadIns::defaultList.find(sub_name) == ReadIns::defaultList.end() &&
		ReadIns::subName.find(sub_name) == ReadIns::subName.end()) {
		ReadIns::line = line;
		throw("unexpected line");
	}
	string v = value->Out();
	if (v != "")
		v = ", " + v;
	return value->Stack(Tab) + string(Tab * 4, ' ') + (wait ? "ins_11(\"" : "ins_15(\"")
		+ sub_name + "\"" + v + ");\n";
}

Line_Label::Line_Label(string label)
{
	this->label = label;
}

Line_Label::~Line_Label()
{
}

string Line_Label::ToString()
{
	return label + ":\n";
}

Line_If_all::Line_If_all(Block* block, int Tab) :Line(Tab)
{
	this->block = block;
	char c[8];
	sprintf_s(c, "MUA_%d", ReadIns::MUA_num++);
	label = c;
}

Line_If_all::~Line_If_all()
{
}

string Line_If_all::GetOut()
{
	return "";
}

Line_If_all* Line_If_all::gotoEnd(Line_If_all* thisLine)
{
	if (!thisLine)
		return nullptr;
	Block *thisBlock, *nextBlock = thisLine->block;
	Line_Elsif* e2 = nullptr;
	do {
		thisBlock = nextBlock;
		nextBlock = thisBlock->nextBlock;
		if (nextBlock) {
			Line_Else* e1 = dynamic_cast<Line_Else*>(nextBlock->line);
			if (e1)
				return e1;
			e2 = dynamic_cast<Line_Elsif*>(nextBlock->line);
		}
	} while (nextBlock && e2);
	return dynamic_cast<Line_If_all*>(thisBlock->line);
}

Line_If_E::Line_If_E(Block * block, string value, int Tab) :Line_If_all(block, Tab)
{
	this->value = new Exp_real(value);
}

Line_If_E::~Line_If_E()
{
}

string Line_If_E::ToString()
{
	//如果是一块结尾且当前if是结尾，则找到对应的label_end再13
	Line_If_all* next = gotoEnd(this);
	if (this == next) {
		string label = "";
		while (label == "") {
			Line_Loop* e1 = dynamic_cast<Line_Loop*>(next->block->parentBlock->line);
			if (!(next->block->nextBlock) && e1)
				label = e1->label_end;
			Line_If_all* e2 = dynamic_cast<Line_If_all*>(next->block->parentBlock->line);
			Line_If_all* e3 = gotoEnd(e2);
			if (!(next->block->nextBlock) && e2 && (e2 == e3))
				next = e2;
			else
				label = next->label;
		}
		return value->Out(Tab) + ";\n" + string(Tab * 4, ' ') + "ins_13(" + label + ", 0);\n";
	}
	return value->Out(Tab) + ";\n" + string(Tab * 4, ' ') + "ins_13(" + label + ", 0);\n";
}

string Line_If_E::GetOut()
{
	//首先判断自己是不是末尾，是的话按else那样：
	//如果是一块结尾且parent是loop，则返回空;
	//如果是一块结尾且parent是If_all，且parent是末尾，则返回空
	//否则如果是第一次循环，则返回自己的label
	//其次next到本if块最后一个块
	//如果是一块结尾且parent是loop，则返回parent的label;
	//如果是一块结尾且parent是If_all，且parent是末尾，则返回“其次”
	//否则返回这个块的label
	Line_If_all* next = gotoEnd(this);
	if (this == next) {
		Line_Loop* e1 = dynamic_cast<Line_Loop*>(this->block->parentBlock->line);
		if (!(next->block->nextBlock) && e1)
			return "";
		Line_If_all* e2 = dynamic_cast<Line_If_all*>(this->block->parentBlock->line);
		Line_If_all* e3 = gotoEnd(e2);
		if (!(next->block->nextBlock) && e2 && (e2 == e3))
			return "";// next = e3;
		else
			return label + ":\n";
	}
	while (1) {
		Line_Loop* e1 = dynamic_cast<Line_Loop*>(next->block->parentBlock->line);
		if (!(next->block->nextBlock) && e1)
			return string(Tab * 4, ' ') + "goto " + e1->label_end + " @ 0;\n" + label + ":\n";
		Line_If_all* e2 = dynamic_cast<Line_If_all*>(next->block->parentBlock->line);
		Line_If_all* e3 = gotoEnd(e2);
		if (!(next->block->nextBlock) && e2 && (e2 == e3))
			next = e2;
		else
			return string(Tab * 4, ' ') + "goto " + next->label + " @ 0;\n" + label + ":\n";
	}


	/*bool ifNotEnd = true;
	while (ifNotEnd = block->nextBlock && (dynamic_cast<Line_Elsif*>(block->nextBlock->line) ||
		dynamic_cast<Line_Else*>(block->nextBlock->line))) {
		block = block->nextBlock;
	}
	//如果if结尾是上级block结尾并且if上级是for或while，则结束的标签直接使用for或while
	//或者，if上级是if elsif else但是是一个if块结尾，则结束标签也直接用
	//结束的标签
	bool ifnotmostout = true;
	bool firstround = true;
	string label_end;
	while (ifnotmostout) {
		Line_For* e3 = dynamic_cast<Line_For*>(block->parentBlock->line);
		Line_While* e4 = dynamic_cast<Line_While*>(block->parentBlock->line);
		Line_If* e5 = dynamic_cast<Line_If*>(block->parentBlock->line);
		Line_Elsif* e6 = dynamic_cast<Line_Elsif*>(block->parentBlock->line);
		Line_Else* e7 = dynamic_cast<Line_Else*>(block->parentBlock->line);
		ifnotmostout = !block->nextBlock && (e3 || e4 || e5 || e6 || e7);
		if (e5 || e6) {
			if (!block->parentBlock->nextBlock &&
				(dynamic_cast<Line_Elsif*>(block->parentBlock->nextBlock->line) ||
					dynamic_cast<Line_Else*>(block->parentBlock->nextBlock->line)))
				ifnotmostout = false;
		}
		if (ifnotmostout) {
			if (e3)
				label_end = e3->label_end;
			else if (e4)
				label_end = e4->label_end;
			else if (e5)
				label_end = e5->label;
			else if (e6)
				label_end = e6->label;
			else
				label_end = e7->label;
			block = block->parentBlock;
		}
		else {
			Line_Elsif* e1 = dynamic_cast<Line_Elsif*>(block->line);
			Line_Else* e2 = dynamic_cast<Line_Else*>(block->line);
			if (e1)
				label_end = e1->label;
			else if (e2)
				label_end = e2->label;
			else {
				if (firstround)
					return label + ":\n";
				else {
					label_end = dynamic_cast<Line_If*>(block->line)->label;
				}
			}
		}
		firstround = false;
	}
	return string(Tab * 4, ' ') + "goto " + label_end + " @ 0;\n" + label + ":\n";*/
}

Line_If::Line_If(Block* block, string value, int Tab) :Line_If_E(block, value, Tab)
{
}

Line_If::~Line_If()
{
}

Line_Elsif::Line_Elsif(Block* block, string value, int Tab) :Line_If_E(block, value, Tab)
{
}

Line_Elsif::~Line_Elsif()
{
}

Line_Else::Line_Else(Block* block, int Tab) :Line_If_all(block, Tab)
{
}

Line_Else::~Line_Else()
{
}

string Line_Else::ToString()
{
	return "";
}

string Line_Else::GetOut()
{
	//如果是一块结尾且parent是loop，则返回空;
	//如果是一块结尾且parent是If_all，且parent是末尾，则返回第二行注释，循环
	//否则如果是第一次循环，则返回这个块的label，否则返回空
	Line_If_all* next = this;
	//while (1) {
		Line_Loop* e1 = dynamic_cast<Line_Loop*>(next->block->parentBlock->line);
		if (!(next->block->nextBlock) && e1)
			return "";
		Line_If_all* e2 = dynamic_cast<Line_If_all*>(next->block->parentBlock->line);
		Line_If_all* e3 = gotoEnd(e2);
		if (!(next->block->nextBlock) && e2 && (e2 == e3))
			return "";// next = e3;
		else
			return label + ":\n";
	//}

	/*//如果if结尾是上级block结尾并且if上级是for或while，则结束的标签直接使用for或while
	//或者，if上级是if elsif else但是是一个if块结尾，则结束标签也直接用
	//结束的标签
	//还要迭代直到找到最外面一个满足这个条件的。
	bool ifnotmostout = true;
	string label_end;
	while (ifnotmostout) {
		Line_For* e3 = dynamic_cast<Line_For*>(block->parentBlock->line);
		Line_While* e4 = dynamic_cast<Line_While*>(block->parentBlock->line);
		Line_If* e5 = dynamic_cast<Line_If*>(block->parentBlock->line);
		Line_Elsif* e6 = dynamic_cast<Line_Elsif*>(block->parentBlock->line);
		Line_Else* e7 = dynamic_cast<Line_Else*>(block->parentBlock->line);
		ifnotmostout = !block->nextBlock && (e3 || e4 || e5 || e6 || e7);
		if (e5 || e6) {
			if (!block->parentBlock->nextBlock &&
				(dynamic_cast<Line_Elsif*>(block->parentBlock->nextBlock->line) ||
					dynamic_cast<Line_Else*>(block->parentBlock->nextBlock->line)))
				ifnotmostout = false;
		}
		if (ifnotmostout) {
			return "";
		}
	}
	return label + ":\n";*/
}

Line_Loop::Line_Loop(string value, int Tab) :Line(Tab)
{
	char c[8];
	sprintf_s(c, "MUA_%d", ReadIns::MUA_num++);
	label_begin = c;
	sprintf_s(c, "MUA_%d", ReadIns::MUA_num++);
	label_end = c;
}

Line_Loop::~Line_Loop()
{
}

Line_For::Line_For(string value, Line_Sub* sub, int Tab) :Line_Loop(value, Tab)
{
	char c[8];
	sprintf_s(c, "MUA_%d", ReadIns::MUA_num++);
	var = c;
	sub->Declare(var);
	this->value = new Line_Value_Real(var, value, false, Tab);
}

Line_For::~Line_For()
{
}

string Line_For::ToString()
{
	return value->ToString() + string(Tab * 4, ' ') + "goto " + label_end + " @ 0;\n"
		+ label_begin + ":\n";
}

string Line_For::GetOut()
{
	return label_end + ":\n" + string(Tab * 4, ' ') + "ins_78($" + var + ");\n" +
		string(Tab * 4, ' ') + "ins_14(" + label_begin + ", 0);\n";
}

Line_While::Line_While(string value, int Tab) :Line_Loop(value, Tab)
{
	this->value = new Exp_real(value);
}

Line_While::~Line_While()
{
}

string Line_While::ToString()
{
	return string(Tab * 4, ' ') + "goto " + label_end + " @ 0;\n" + label_begin + ":\n";
}

string Line_While::GetOut()
{
	if (value->One())
		return label_end + ":\n" + string(Tab * 4, ' ') + "goto " + label_begin + " @ 0;\n";
	else
		return label_end + ":\n" + value->Out(Tab) + ";\n" + string(Tab * 4, ' ') +
		"ins_14(" + label_begin + ", 0);\n";
}

Line_Jump::Line_Jump(string type, string label, int Tab) :Line(Tab)
{
	ins = type;
	this->label = label;
}

Line_Jump::~Line_Jump()
{
}

string Line_Jump::ToString()
{
	if (ins == "Jmp")
		return string(Tab * 4, ' ') + "goto " + label + " @ 0;\n";
	else if (ins == "Jz")
		return string(Tab * 4, ' ') + "ins_13(" + label + ", 0);\n";
	else// if (ins == "Jnz")
		return string(Tab * 4, ' ') + "ins_14(" + label + ", 0);\n";
}

Line_Root::Line_Root()
{
}

Line_Root::~Line_Root()
{
}

string Line_Root::ToString()
{
	return string();
}
