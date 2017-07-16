#pragma once

#include <map>
#include <list>
#include <set>
#include <deque>
#include <math.h>

using namespace std;

//数据类型
static enum NumType {
	Anything, Int, Float, String
};

//字符类型
static enum AscType {
	Number, Character, Sign, Dot, Comma, Braket, Colon, RightBigBraket,
	Quote, Space, Endline
};

//获取字符类型
static AscType GetType(char c) {
	if (c >= '0' && c <= '9')
		return Number;
	else if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_')
		return Character;
	else if (c == '.')
		return Dot;
	else if (c == ',')
		return Comma;
	else if (c == '(' || c == ')')
		return Braket;
	else if (c == ':')
		return Colon;
	else if (c == '}')
		return RightBigBraket;
	else if (c == '\"')
		return Quote;
	else if (c == ' ' || c == '\t' || c == '\n')
		return Space;
	else if (c == ';' || c == '{')
		return Endline;
	else
		return Sign;
}

//计算两个数的符号运算
static int calculate(string sign, int left, int right) {
	if (sign == "--")
		return -right;
	else if (sign == "!")
		return ~right;
	else if (sign == "&")
		return left & right;
	else if (sign == "|")
		return left | right;
	else if (sign == "^")
		return left ^ right;
	else if (sign == "%")
		return left % right;
	else if (sign == "*")
		return left * right;
	else if (sign == "/")
		return left / right;
	else if (sign == "+")
		return left + right;
	else if (sign == "-")
		return left - right;
	else if (sign == "&&")
		return left && right;
	else if (sign == "||")
		return left || right;
	else if (sign == "==")
		return left == right;
	else if (sign == "!=")
		return left != right;
	else if (sign == "<")
		return left < right;
	else if (sign == "<=")
		return left <= right;
	else if (sign == ">")
		return left > right;
	else if (sign == ">=")
		return left >= right;
	else
		throw("unexpected sign");
}

//计算两个数的符号运算
static float calculate(string sign, float left, float right) {
	if (sign == "--")
		return -right;
	else if (sign == "sin")
		return sin(right);
	else if (sign == "cos")
		return cos(right);
	else if (sign == "sqrt")
		return sqrt(right);
	else if (sign == "*")
		return left * right;
	else if (sign == "/")
		return left / right;
	else if (sign == "+")
		return left + right;
	else if (sign == "-")
		return left - right;
	else if (sign == "==")
		return left == right;
	else if (sign == "!=")
		return left != right;
	else if (sign == "<")
		return left < right;
	else if (sign == "<=")
		return left <= right;
	else if (sign == ">")
		return left > right;
	else if (sign == ">=")
		return left >= right;
	else
		throw("unexpected sign");
}

//将符号转换成ins串
static string signIns(bool Float, string sign) {
	if (sign == "--")
		return Float ? "ins_84()" : "ins_83()";
	else if (sign == "!")
		return Float ? "ins_72()" : "ins_71()";
	else if (sign == "sin")
		return "ins_79()";
	else if (sign == "cos")
		return "ins_80()";
	else if (sign == "sqrt")
		return "ins_88()";
	else if (sign == "&")
		return "ins_77()";
	else if (sign == "|")
		return "ins_76()";
	else if (sign == "^")
		return "ins_75()";
	else if (sign == "%")
		return "ins_58()";
	else if (sign == "*")
		return Float ? "ins_55()" : "ins_54()";
	else if (sign == "/")
		return Float ? "ins_57()" : "ins_56()";
	else if (sign == "+")
		return Float ? "ins_51()" : "ins_50()";
	else if (sign == "-")
		return Float ? "ins_53()" : "ins_52()";
	else if (sign == "&&")
		return "ins_74()";
	else if (sign == "||")
		return "ins_73()";
	else if (sign == "==")
		return Float ? "ins_60()" : "ins_59()";
	else if (sign == "!=")
		return Float ? "ins_62()" : "ins_61()";
	else if (sign == "<")
		return Float ? "ins_64()" : "ins_63()";
	else if (sign == "<=")
		return Float ? "ins_66()" : "ins_65()";
	else if (sign == ">")
		return Float ? "ins_68()" : "ins_67()";
	else if (sign == ">=")
		return Float ? "ins_70()" : "ins_69()";
	else
		throw("unexpected sign");
}

class ReadIns
{
public:
	static multimap<string, pair<int, deque<NumType>>> ins;	//ins表
	static map<string, pair<int, deque<NumType>>> mode;		//mode表
	static map<string, pair<int, NumType>> globalVariable;	//全局变量表
	static map<string, string> constant;					//符号常量表
	static set<string> keyWord;								//关键字表
	static set<string> integratedFunction;					//集成函数表
	static set<string> customFunction;						//自定义函数表，为保留关键字
	static set<string> varI;								//整数变量表，sub局域
	static set<string> varF;								//浮点变量表，sub局域
	static list<string> subVar;								//sub的所有变量，sub局域
	//static set<string> declareVar;						//声明的变量
	static set<string> defaultList;							//default.ecl中的线程名表
	static map<string, list<NumType>> subName;				//sub名表
	static int MUA_num;										//内部变量名计数器
	static int line;										//行数，用于debug

	//读取ins.ini与default.ini
	static void Read();
	//获取符号的优先级
	static int getFirst(string sign) {
		if (sign == "/" || sign == "*" || sign == "%")
			return 2;
		else if (sign == "+" || sign == "-")
			return 3;
		else if (sign == "<" || sign == "<=" || sign == ">" || sign == ">=" ||
			sign == "==" || sign == "!=")
			return 4;
		else if (sign == "&")
			return 5;
		else if (sign == "^")
			return 6;
		else if (sign == "|")
			return 7;
		else if (sign == "&&")
			return 8;
		else if (sign == "||")
			return 9;
		else if (sign == "--" || sign == "!" ||
			(customFunction.find(sign) != customFunction.end()))
			return 1;
		else
			throw("unexpected sign");
	}
};

