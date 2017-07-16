#pragma once
#include "Exp.h"
#include "CommaExp.h"

using namespace std;

class Line
{
public:
	Line(int Tab = 0);
	~Line();
	virtual string ToString() = 0;
	virtual string GetOut();

protected:
	int Tab;
};

class Line_Sub :public Line {
public:
	Line_Sub(string sub_name, string value);
	~Line_Sub();
	string ToString();
	string GetOut();
	void Declare(string c);

private:
	string sub_name;
	string value;
	list<string> declare;
};

class Line_Value :public Line {
public:
	Line_Value(string var, string value, bool Float, int Tab);
	Line_Value(int globalNo, string value, bool Float, int Tab);
	~Line_Value();

protected:
	bool Float;
	string var;
	Exp* value;
};

class Line_Value_Real : public Line_Value {
public:
	Line_Value_Real(string var, string value, bool Float, int Tab);
	Line_Value_Real(int globalNo, string value, bool Float, int Tab);
	~Line_Value_Real();
	string ToString();
};

class Line_Value_Sign :public Line_Value {
public:
	Line_Value_Sign(string var, string value, string sign, bool Float, int Tab);
	Line_Value_Sign(int globalNo, string value, string sign, bool Float, int Tab);
	~Line_Value_Sign();
	string ToString();

private:
	string sign;
};

class Line_Diff :public Line {
public:
	Line_Diff(string value);
	~Line_Diff();
	string ToString();

private:
	string value;//TODO
};

class Line_Ins :public Line {
public:
	Line_Ins(string ins_name, string value, int Tab);
	~Line_Ins();
	string ToString();

private:
	int ins;
	deque<NumType> var;
	CommaExp_Exp* value;
};

class Line_Mode :public Line {
public:
	Line_Mode(string mode_name, string value, int Tab);
	~Line_Mode();
	string ToString();

private:
	int mode;
	bool ifLong; bool haveNum;
	deque<NumType> var;
	CommaExp_Exp* value;
};

class Line_Inte :public Line {
public:
	Line_Inte(string name, string value, int Tab);
	~Line_Inte();
	string ToString();

private:
	string value;
};

class Line_Thread :public Line {
public:
	Line_Thread(string sub_name, string value, int Tab);
	~Line_Thread();
	string ToString();

private:
	bool wait;
	int line;
	string sub_name;
	CommaExp_Thread* value;
};

class Line_Label :public Line {
public:
	Line_Label(string label);
	~Line_Label();
	string ToString();

private:
	string label;
};

struct Block;

class Line_If_all : public Line {
public:
	Line_If_all(Block* block, int Tab);
	~Line_If_all();
	virtual string ToString() = 0;
	virtual string GetOut();
	string label;
	static Line_If_all* gotoEnd(Line_If_all* thisLine);
	Block* block;
};

class Line_If_E : public Line_If_all {
public:
	Line_If_E(Block* block, string value, int Tab);
	~Line_If_E();
	string ToString();
	string GetOut();

protected:
	Exp_real* value;
};

class Line_If :public Line_If_E {
public:
	Line_If(Block* block, string value, int Tab);
	~Line_If();
};

class Line_Elsif :public Line_If_E {
public:
	Line_Elsif(Block* block, string value, int Tab);
	~Line_Elsif();
};

class Line_Else :public Line_If_all {
public:
	Line_Else(Block* block, int Tab);
	~Line_Else();
	string ToString();
	string GetOut();
};

class Line_Loop :public Line {
public:
	Line_Loop(string value, int Tab);
	~Line_Loop();
	string label_end;

protected:
	string label_begin;
};

class Line_For :public Line_Loop {
public:
	Line_For(string value, Line_Sub* sub, int Tab);
	~Line_For();
	string ToString();
	string GetOut();

private:
	Line_Value_Real* value;
	string var;
};

class Line_While :public Line_Loop {
public:
	Line_While(string value, int Tab);
	~Line_While();
	string ToString();
	string GetOut();

private:
	Exp_real* value;
};

class Line_Jump :public Line {
public:
	Line_Jump(string type, string label, int Tab/*, int time*/);
	~Line_Jump();
	string ToString();

private:
	string ins;
	string label;
};

class Line_Root :public Line {
public:
	Line_Root();
	~Line_Root();
	string ToString();
};