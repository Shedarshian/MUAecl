#pragma once

#include "ExpTree.h"

//表达式
class Exp
{
public:
	Exp();
	~Exp();
	virtual string Out(int Tab);	//输出
	virtual ExpTree::Type type();	//返回树的类型
	virtual bool Float();			//返回树是否为浮点

protected:
	//读取时的状态
	static enum Status {
		Space, Number, Variable, Sign
	};
};

//真的表达式
class Exp_real :public Exp {
public:
	Exp_real(string value);
	~Exp_real();
	string Out(int Tab);
	ExpTree::Type type();
	bool Float();
	bool One();				//返回表达式是不是就是1

private:
	ExpTree* tree;			//表达式树
};

//字符串型表达式
class Exp_str :public Exp {
public:
	Exp_str(string value);
	~Exp_str();
	string Out(int Tab);

private:
	string value;			//字符串的值
};

//分难度表达式
class Exp_diff :public Exp {
public:
	Exp_diff(string value);
	~Exp_diff();
	string Out(int Tab);				//非23,600多等输出
	string Out(int Tab, bool ifNext);	//这啥？？？
	string Out_23();					//23,600多等输出
	string Out_23(bool Float);
	string Out_23(int &i);				//23,600多等输出，i为用于[-1]的堆栈参数个数
	string Stack_23(int &i, int Tab);	//堆栈上的输出，i为堆栈参数个数
	string Stack_23(int Tab);
	ExpTree::Type type();
	bool Float();

private:
	Exp_real* value[4];					//4个表达式树
};