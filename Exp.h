#pragma once

#include "ExpTree.h"

class Exp
{
public:
	Exp();
	~Exp();
	virtual string Out(int Tab);
	virtual ExpTree::Type type();
	virtual bool Float();

protected:
	static enum Status {
		Space, Number, Variable, Sign
	};
};

class Exp_real :public Exp {
public:
	Exp_real(string value);
	~Exp_real();
	string Out(int Tab);
	ExpTree::Type type();
	bool Float();
	bool One();

private:
	ExpTree* tree;
};

class Exp_str :public Exp {
public:
	Exp_str(string value);
	~Exp_str();
	string Out(int Tab);

private:
	string value;
};

class Exp_diff :public Exp {
public:
	Exp_diff(string value);
	~Exp_diff();
	string Out(int Tab);//非23,600多等用
	string Out(int Tab, bool ifNext);
	string Out_23();//ins_23，600多等用
	string Out_23(bool Float);
	string Out_23(int &i);
	string Stack_23(int &i, int Tab);
	string Stack_23(int Tab);
	ExpTree::Type type();
	bool Float();

private:
	Exp_real* value[4];
};