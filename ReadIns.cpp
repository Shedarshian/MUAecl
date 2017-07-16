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
		//ע��
		if (buffer[0] == '%') {
			continue;
		}
		//�л�mode
		else if (buffer[0] == '#') {
			mode = string(buffer).substr(1, mode.rfind('#') - 1);
		}
		//��ȡins
		else if (mode == "ins") {
			stringstream s = stringstream(buffer);
			int n; s >> n;
			string name; s >> name;
			string c;
			deque<NumType> lnt; //����������ͱ�
			while (!s.eof()) {
				s >> c;
				lnt.push_back(c == "str" ? NumType::String : (c == "int" ? NumType::Int : (
					c == "float" ? NumType::Float : NumType::Anything)));
			} 
			ReadIns::ins.insert(pair<string, pair<int, deque<NumType>>>(name, 
				pair<int, deque<NumType>>(n, lnt)));
		}
		//��ȡȫ�ֱ���
		else if (mode == "variable") {
			stringstream s = stringstream(buffer);
			int n; string name; string c;
			s >> n >> name >> c;
			ReadIns::globalVariable.insert(pair<string, pair<int, NumType>>(name,
				pair<int, NumType>(n, c == "int" ? NumType::Int : NumType::Float)));
		}
		//��ȡ���ų���
		else if (mode == "const") {
			stringstream s = stringstream(buffer);
			string name; string key;
			s >> name >> key;
			ReadIns::constant.insert(pair<string, string>(name, key));
		}
		//��ȡ�任mode
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
		//��ȡ���ɺ����������ڹؼ����
		else if (mode == "integrated function") {
			ReadIns::keyWord.insert(string(buffer));
			ReadIns::integratedFunction.insert(string(buffer));
		}
		//��ȡ�ؼ���
		else if (mode == "keyword" || mode == "preserve keyword") {
			ReadIns::keyWord.insert(string(buffer));
		}
		//��ȡ�Զ��庯���������ڹؼ����
		else if (mode == "custom function") {
			ReadIns::keyWord.insert(string(buffer));
			ReadIns::customFunction.insert(string(buffer));
		}
	}
	ifstream indef("default.ini");
	//��ȡdefault.ecl�еĺ�����
	while (!indef.eof()) {
		indef.getline(buffer, 255);
		defaultList.insert(string(buffer));
	}
}
