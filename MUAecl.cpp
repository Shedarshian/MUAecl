// MUAecl.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include "ReadIns.h"//subVar
#include "Line.h"
#include "Block.h"
#include "ExpTree.h"
#include "Exp.h"
#include "CommaExp.h"

#define GET(next, in) if(!in.eof()) next = in.get()
#define LINEUP(buffer, line) if (buffer == '\n') line++

using namespace std;

int main(int argc, char* argv[])
{
	try {
		ReadIns::Read();	//��ȡins.ini
		ifstream in(argv[1]);
		char buffer;
		//ǰ����������anim��ecli�������Ķ��벢���
		string include = "";
		do {
			buffer = in.get();
			while (buffer != in.eof() && (buffer == ' ' || buffer == '\t' || buffer == '\n')) {
				LINEUP(buffer, ReadIns::line);
				buffer = in.get();
			}
			in.unget();
			if (buffer == 'a' || buffer == 'e') {
				char a[100];
				in.getline(a, 99);
				include += string(a) + "\n";
				ReadIns::line++;
			}
		} while (buffer == 'a' || buffer == 'e');

		char next = in.get();			//��һ���ַ�
		stringstream lineBuffer;		//��ȡ��
		bool annotationLine = false;	//�Ƿ�����ע����
		bool annotationBlock = false;	//�Ƿ��ڶ�ע����
		string c;

		Block* root = (Block*)malloc(sizeof(struct Block));	//Block���ĸ�
		int Tab = 1;										//�����������ѷϣ�
		root->line = new Line_Root();
		root->childBlock = nullptr;
		root->nextBlock = nullptr;
		root->parentBlock = root;
		Block* now = root;						//Block���ĵ�ǰ����λ��
		Line_Sub* thisSub = nullptr;			//��ǰ����sub
		bool isDown = true;						//Block�����Ƿ�����һ������
		while (!in.eof())
		{
			int i = 0;
			bool begin = true;
			//��ȡһ�У���;��}����һ��
			while (1) {
				buffer = next;
				GET(next, in);
				LINEUP(buffer, ReadIns::line);
				if (!annotationLine && !annotationBlock) {
					if (buffer == '/' && next == '*') {
						annotationBlock = true;
						buffer = next;
						GET(next, in);
						continue;
					}
					else if (buffer == '/' && next == '/') {
						annotationLine = true;
						continue;
					}
					else if (begin && GetType(buffer) == AscType::Space)
						continue;
					else if (begin)
						begin = false;
					if (GetType(buffer) == AscType::Endline ||
						GetType(buffer) == AscType::RightBigBraket) {
						lineBuffer << buffer;
						break;
					}
					else
						lineBuffer << buffer;
					if (in.eof())
						break;
				}
				else if (buffer == '*' && next == '/') {
					annotationBlock = false;
					buffer = next;
					GET(next, in);
				}
				else if (buffer == '\n')
					annotationLine = false;
				else if (in.eof()) {
					annotationBlock = false;
					annotationLine = false;
					break;
				}
			}
			if (in.eof())
				break;

			//����ȡ����ת����string
			c = lineBuffer.str();
			if (c == "")
				continue;
			lineBuffer.str("");

			//�Ҵ������ս�һ��Block��
			if (c == "}") {
				now = now->parentBlock;
				if (isDown)
					throw("empty big braket");
				//Tab--;
				continue;
			}

			//���б�����������ֱ���ڴ˴�����
			bool ifDeclare = false;
			bool ifFloat = false;
			if (c.substr(0, 4) == "int ") {
				c = c.substr(4);	//ȥ��ǰ׺
				ifDeclare = true;
			}
			else if (c.substr(0, 6) == "float ") {
				c = c.substr(6);	//ȥ��ǰ׺
				ifDeclare = true;
				ifFloat = true;
			}
			if (ifDeclare) {
				c = c.substr(0, c.length() - 1);	//ȥ�ֺ�
				int n, m;
				if (c == "")
					throw("no expression");
				//TODO,���Ų��,�ĳ�tree
				do {
					n = c.find(',');
					string s = (n == -1) ? c : (c.substr(0, c.find_last_not_of(' ', n - 1) + 1));
					s = s.substr(s.find_first_not_of(' '));
					if (s == "")
						throw("no expression");
					m = s.find('=');
					if (m == -1) {
						if (ifFloat)
							ReadIns::varF.insert(s);
						else
							ReadIns::varI.insert(s);
						thisSub->Declare(s);
					}
					else {
						string var = s.substr(0, s.find_last_not_of(' ', m - 1) + 1);
						if (ifFloat)
							ReadIns::varF.insert(var);
						else
							ReadIns::varI.insert(var);
						thisSub->Declare(var);
						Block* b = (Block*)malloc(sizeof(struct Block));
						b->childBlock = nullptr;
						b->nextBlock = nullptr;
						if (isDown) {
							//Tab++;
							b->parentBlock = now;
							now->childBlock = b;
							isDown = false;
						}
						else {
							b->parentBlock = now->parentBlock;
							now->nextBlock = b;
						}
						b->line = new Line_Value_Real(var, s.substr(m + 1, s.length() - m - 1),
							ReadIns::varF.find(var) != ReadIns::varF.end(), Tab);
						now = b;
					}
					c = c.substr(n + 1);
				} while (n != -1);
				//������ڵ��ˣ������´�ѭ��
				continue;
			}

			//��ȡ�׸�Token
			stringstream buffers;
			AscType bb = GetType(c.at(0));
			for (int i = 0; i < c.length(); i++) {
				AscType aa = GetType(c.at(i));
				if (aa != bb && !(aa == AscType::Number && bb == AscType::Character))
					break;
				buffers << c.at(i);
			}
			string firstToken;
			buffers >> firstToken;

			//��������ڵ�
			Block* b = (Block*)malloc(sizeof(struct Block));
			b->childBlock = nullptr;
			b->nextBlock = nullptr;
			if (isDown) {
				//Tab++;
				b->parentBlock = now;
				now->childBlock = b;
				isDown = false;
			}
			else {
				b->parentBlock = now->parentBlock;
				now->nextBlock = b;
			}
			bool BOOL;	//�м����
			//��sub
			if (firstToken == "sub") {
				ReadIns::varI.clear();
				ReadIns::varF.clear();
				ReadIns::subVar.clear();
				thisSub = new Line_Sub(c.substr(4, c.find('(') - 4),
					c.substr(c.find('(') + 1, c.rfind(')') - c.find('(') - 1));
				b->line = thisSub;
			}
			//���ñ�ǩ
			else if (firstToken == "SetLabel") {
				b->line = new Line_Label(c.substr(c.find('(') + 1, c.rfind(')') - c.find('(') - 1));
			}
			//if���
			else if (firstToken == "if") {
				b->line = new Line_If(b, c.substr(c.find('(') + 1, c.rfind(')') - c.find('(') - 1), Tab);
			}
			//elsif�Ӿ�
			else if (firstToken == "elsif") {
				b->line = new Line_Elsif(b, c.substr(c.find('(') + 1, c.rfind(')') - c.find('(') - 1), Tab);
			}
			//else�Ӿ�
			else if (firstToken == "else") {
				b->line = new Line_Else(b, Tab);
			}
			//for���
			else if (firstToken == "for") {
				b->line = new Line_For(c.substr(c.find('(') + 1, c.rfind(')') - c.find('(') - 1)
					, thisSub, Tab);
			}
			//while���
			else if (firstToken == "while") {
				b->line = new Line_While(c.substr(c.find('(') + 1, c.rfind(')') - c.find('(') - 1), Tab);
			}
			//��ת���
			else if (firstToken == "Jmp" || firstToken == "Jz" || firstToken == "Jnz") {
				b->line = new Line_Jump(firstToken, c.substr(c.find(' ') + 1, c.length() - c.find(' ') - 2), Tab);
			}
			//�Ѷ�ѡ�����
			else if (firstToken == "!") {
				b->line = new Line_Diff(c.substr(1, c.length() - 2));
			}
			//��Ļ�任���
			else if (firstToken == "@") {
				b->line = new Line_Mode(c.substr(1, c.find('(') - 1), c.substr(c.find('(') + 1,
					c.rfind(')') - c.find('(') - 1), Tab);
			}
			//ִ�����
			else if (ReadIns::ins.find(firstToken) != ReadIns::ins.end()) {
				b->line = new Line_Ins(firstToken, 
					c.substr(c.find('(') + 1, c.rfind(')') - c.find('(') - 1), Tab);
			}
			//���ɺ���
			else if (ReadIns::integratedFunction.find(firstToken) != ReadIns::integratedFunction.end()) {
				b->line = new Line_Inte(firstToken,
					c.substr(c.find('(') + 1, c.rfind(')') - c.find('(') - 1), Tab);
			}
			//TODO,������ֵ,�ĳ�tree,�޷�����B C=1�����
			else if ((BOOL = ReadIns::varF.find(firstToken) != ReadIns::varF.end())
				|| ReadIns::varI.find(firstToken) != ReadIns::varI.end()) {
				int eq = c.find('=');
				//+=�ȷ���
				if (c.at(eq - 1) == '+' || c.at(eq - 1) == '-' || c.at(eq - 1) == '*'
					|| c.at(eq - 1) == '/')
					b->line = new Line_Value_Sign(c.substr(0, eq - 1),
						c.substr(eq + 1, c.length() - eq - 2), string(1, c.at(eq - 1)),
						BOOL, Tab);
				else {
					b->line = new Line_Value_Real(c.substr(0, eq),
						c.substr(eq + 1, c.length() - eq - 2), BOOL, Tab);
				}
			}
			//TODO,ȫ�ֱ�����ֵ,�ĳ�tree,�޷�����B C=1�����
			else if (ReadIns::globalVariable.find(firstToken) != ReadIns::globalVariable.end()) {
				pair<int, NumType> p = ReadIns::globalVariable.find(firstToken)->second;
				int eq = c.find('=');
				//+=�ȷ���
				if (c.at(eq - 1) == '+' || c.at(eq - 1) == '-' || c.at(eq - 1) == '*'
					|| c.at(eq - 1) == '/')
					b->line = new Line_Value_Sign(p.first,
						c.substr(eq + 1, c.length() - eq - 2), string(1, c.at(eq - 1)),
						p.second == NumType::Float, Tab);
				else {
					b->line = new Line_Value_Real(p.first, c.substr(eq + 1, c.length() - eq - 2),
						p.second == NumType::Float, Tab);
				}
			}
			//�̵߳���
			else {
				b->line = new Line_Thread(firstToken,
					c.substr(c.find('(') + 1, c.rfind(')') - c.find('(') - 1), Tab);
			}

			//�����sub���˳���ʱ�����varlist

			isDown = (dynamic_cast<Line_For*>(b->line) || dynamic_cast<Line_If*>(b->line) ||
				dynamic_cast<Line_Else*>(b->line) || dynamic_cast<Line_Elsif*>(b->line) ||
				dynamic_cast<Line_While*>(b->line) || dynamic_cast<Line_Sub*>(b->line));

			now = b;
		}

		ofstream out(argv[2]);
		out << include << root->Out();

		return 0;
	}
	catch (char* c) {
		cout << ReadIns::line << "  " << string(c) << endl;
		return 0;
	}
}



