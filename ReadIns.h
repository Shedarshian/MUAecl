#pragma once

#include <map>
#include <list>
#include <set>
#include <deque>
#include <math.h>

using namespace std;

//��������
static enum NumType {
	Anything, Int, Float, String
};

//�ַ�����
static enum AscType {
	Number, Character, Sign, Dot, Comma, Braket, Colon, RightBigBraket,
	Quote, Space, Endline
};

//��ȡ�ַ�����
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

//�����������ķ�������
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

//�����������ķ�������
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

//������ת����ins��
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
	static multimap<string, pair<int, deque<NumType>>> ins;	//ins��
	static map<string, pair<int, deque<NumType>>> mode;		//mode��
	static map<string, pair<int, NumType>> globalVariable;	//ȫ�ֱ�����
	static map<string, string> constant;					//���ų�����
	static set<string> keyWord;								//�ؼ��ֱ�
	static set<string> integratedFunction;					//���ɺ�����
	static set<string> customFunction;						//�Զ��庯����Ϊ�����ؼ���
	static set<string> varI;								//����������sub����
	static set<string> varF;								//���������sub����
	static list<string> subVar;								//sub�����б�����sub����
	//static set<string> declareVar;						//�����ı���
	static set<string> defaultList;							//default.ecl�е��߳�����
	static map<string, list<NumType>> subName;				//sub����
	static int MUA_num;										//�ڲ�������������
	static int line;										//����������debug

	//��ȡins.ini��default.ini
	static void Read();
	//��ȡ���ŵ����ȼ�
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

