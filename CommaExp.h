#pragma once
#include <list>
#include <deque>
#include "ReadIns.h"
#include "Exp.h"

using namespace std;

class CommaExp
{
public:
	CommaExp();
	~CommaExp();
	virtual string Stack(int Tab) = 0;
	virtual string Out() = 0;
	static list<string> Comma(string c) {
		list<string> l;
		
		return l;
	}
};

class CommaExp_Exp/* :public CommaExp*/ {
public:
	CommaExp_Exp(string value, bool if23);
	~CommaExp_Exp();
	string Stack(int Tab);
	string Out();
	string Out(deque<NumType> var);
	string Out(bool ifLong, bool haveNum, int mode);
	string Out(bool ifLong, bool haveNum, int mode, deque<NumType> var);//609”√
	int size() { return value.size(); }
	bool chg23() {
		return if23 && value.size() != 0 &&
			dynamic_cast<Exp_diff*>(*(value.begin()));
	};//if23 && ed

protected:
	bool if23;
	int line;
	list<Exp*> value;
};

class CommaExp_Declare/* :public CommaExp*/ {
public:
	CommaExp_Declare(string value, string sub_name);
	~CommaExp_Declare();
	string Out();

private:
	string sub_name;
	list<NumType> expectedArg;
};

class CommaExp_Thread/* :public CommaExp*/ {
public:
	CommaExp_Thread(string value, string thread_name);
	~CommaExp_Thread();
	string Stack(int Tab);
	string Out();

private:
	string thread_name;
	list<Exp_real*> value;
	int line;//±®¥Ì”√
};

class CommaExp_600 :public CommaExp_Exp {
public:
	CommaExp_600(string value);
	~CommaExp_600();
	string Out(int Tab);

private:
};
