#include "stdafx.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <list>
#include <map>
#include <set>
#include "ReadIns.h"

using namespace std;

set<string> ReadIns::keyWord = set<string>();
multimap<string, pair<int, deque<NumType>>> ReadIns::ins =
	multimap<string, pair<int, deque<NumType>>>();
map<string, pair<int, deque<NumType>>> ReadIns::mode = 
	map<string, pair<int, deque<NumType>>>();
map<string, pair<int, NumType>> ReadIns::globalVariable =
	map<string, pair<int, NumType>>();
map<string, string> ReadIns::constant = map<string, string>();
set<string> ReadIns::customFunction = set<string>();
set<string> ReadIns::integratedFunction = set<string>();
set<string> ReadIns::varI = set<string>();
set<string> ReadIns::varF = set<string>();
list<string> ReadIns::subVar = list<string>();
//set<string> ReadIns::declareVar = set<string>();
set<string> ReadIns::defaultList = set<string>();
map<string, list<NumType>> ReadIns::subName = map<string, list<NumType>>();
int ReadIns::MUA_num = 0;
int ReadIns::line = 1;

void ReadIns::Read()
{
	ifstream in("ins.ini");
	char buffer[255];
	string mode = "";
	while (!in.eof()) {
		in.getline(buffer, 255);
		//注释
		if (buffer[0] == '%') {
			continue;
		}
		//切换mode
		else if (buffer[0] == '#') {
			mode = string(buffer).substr(1, mode.rfind('#') - 1);
		}
		//读取ins
		else if (mode == "ins") {
			stringstream s = stringstream(buffer);
			int n; s >> n;
			string name; s >> name;
			string c;
			deque<NumType> lnt; //输入参数类型表
			while (!s.eof()) {
				s >> c;
				lnt.push_back(c == "str" ? NumType::String : (c == "int" ? NumType::Int : (
					c == "float" ? NumType::Float : NumType::Anything)));
			} 
			ReadIns::ins.insert(pair<string, pair<int, deque<NumType>>>(name, 
				pair<int, deque<NumType>>(n, lnt)));
		}
		//读取全局变量
		else if (mode == "variable") {
			stringstream s = stringstream(buffer);
			int n; string name; string c;
			s >> n >> name >> c;
			ReadIns::globalVariable.insert(pair<string, pair<int, NumType>>(name,
				pair<int, NumType>(n, c == "int" ? NumType::Int : NumType::Float)));
		}
		//读取符号常量
		else if (mode == "const") {
			stringstream s = stringstream(buffer);
			string name; string key;
			s >> name >> key;
			ReadIns::constant.insert(pair<string, string>(name, key));
		}
		//读取变换mode
		else if (mode == "mode") {
			stringstream s = stringstream(buffer);
			int n; s >> n;
			string name; s >> name;
			string c; deque<NumType> lnt;
			while (!s.eof()) {
				s >> c;
				lnt.push_back(c == "int" ? NumType::Int : (
					c == "float" ? NumType::Float : NumType::Anything));
			}
			ReadIns::mode.insert(pair<string, pair<int, deque<NumType>>>(name,
				pair<int, deque<NumType>>(n, lnt)));
		}
		//读取集成函数（包括在关键字里）
		else if (mode == "integrated function") {
			ReadIns::keyWord.insert(string(buffer));
			ReadIns::integratedFunction.insert(string(buffer));
		}
		//读取关键字
		else if (mode == "keyword" || mode == "preserve keyword") {
			ReadIns::keyWord.insert(string(buffer));
		}
		//读取自定义函数（包括在关键字里）
		else if (mode == "custom function") {
			ReadIns::keyWord.insert(string(buffer));
			ReadIns::customFunction.insert(string(buffer));
		}
	}
	ifstream indef("default.ini");
	//读取default.ecl中的函数名
	while (!indef.eof()) {
		indef.getline(buffer, 255);
		defaultList.insert(string(buffer));
	}
}
