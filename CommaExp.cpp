#include "stdafx.h"
#include "CommaExp.h"


CommaExp::CommaExp()
{
}


CommaExp::~CommaExp()
{
}

CommaExp_Exp::CommaExp_Exp(string value, bool if23)
{
	string c = value;
	this->if23 = if23;
	this->line = ReadIns::line;
	int n, m;
	if (c == "")
		return;
	while ((n = c.find(',')) != -1) {
		string s = c.substr(0, c.find_last_not_of(' ', n - 1) + 1);
		if (s == "")
			throw("no expression");
		if (s.at(m = s.find_first_not_of(' ')) == '\"' && s.at(s.length() - 1) == '\"')
			this->value.push_back(new Exp_str(s.substr(m)));
		else if (s.find(':') != -1)
			this->value.push_back(new Exp_diff(s));
		else
			this->value.push_back(new Exp_real(s));
		c = c.substr(n + 1);
	}
	if (c == "")
		throw("no expression");
	if (c.at(m = c.find_first_not_of(' ')) == '\"' && c.at(c.length() - 1) == '\"')
		this->value.push_back(new Exp_str(c.substr(m)));
	else if (c.find(':') != -1)
		this->value.push_back(new Exp_diff(c));
	else
		this->value.push_back(new Exp_real(c));
}

CommaExp_Exp::~CommaExp_Exp()
{
}

string CommaExp_Exp::Stack(int Tab)
{
	if (!chg23()) {
		string c; bool ed = false;
		for (list<Exp*>::iterator it = value.end(); it != value.begin();) {
			it--;
			bool thised = dynamic_cast<Exp_diff*>(*it);
			if (thised) {
				c += (*it)->Out(Tab);
				ed = thised;
			}
			else if ((*it)->type() != ExpTree::Type::Num && (*it)->type() != ExpTree::Type::Var) {
				if (ed)
					c += "!ENHL\n";
				c += (*it)->Out(Tab) + ";\n";
				ed = thised;
			}
		}
		if (ed)
			c += "!ENHL\n";
		return c;
	}
	else {
		Exp_diff* ed = dynamic_cast<Exp_diff*>(*(value.begin()));
		return ed->Stack_23(Tab);
	}
}

string CommaExp_Exp::Out()
{
	Exp_diff* ed = dynamic_cast<Exp_diff*>(*(value.begin()));
	if (if23 && ed) {
		int i = 1;
		ed->Stack_23(i, 0);
		return ed->Out_23(i);
	}
	else {
		string c;
		int i = 1;
		/*for (list<Exp*>::iterator it = value.begin(); it != value.end(); it++) {
			if (dynamic_cast<Exp_diff*>(*it) ||
				(*it)->type() != ExpTree::Type::Num && (*it)->type() != ExpTree::Type::Var)
				i++;
		}*/
		for (list<Exp*>::iterator it = value.begin(); it != value.end(); it++) {
			if (!dynamic_cast<Exp_diff*>(*it) &&
				((*it)->type() == ExpTree::Type::Num || (*it)->type() == ExpTree::Type::Var)) {
				c += (*it)->Out(0) + ", ";
			}
			else {
				char a[10];
				if ((*it)->Float())
					sprintf_s(a, "%d.0f], ", i++);
				else
					sprintf_s(a, "%d], ", i++);
				c += "[-" + string(a);
			}
		}
		c = c.substr(0, c.length() - 2);
		return c;
	}
}

string CommaExp_Exp::Out(deque<NumType> var)
{
	Exp_diff* ed = dynamic_cast<Exp_diff*>(*(value.begin()));
	if (if23 && ed) {
		int i = 1;
		ed->Stack_23(i);
		return ed->Out_23(i);
	}
	string c;
	int i = 1;
	if (value.size() != var.size()) {
		ReadIns::line = line;
		throw("wrong arg list");
	}
	/*for (list<Exp*>::iterator it = value.begin(); it != value.end(); it++) {
		if (dynamic_cast<Exp_diff*>(*it) ||
			(*it)->type() != ExpTree::Type::Num && (*it)->type() != ExpTree::Type::Var)
			i++;
	}*/
	deque<NumType>::iterator it2 = var.begin();
	bool ifChange;
	for (list<Exp*>::iterator it = value.begin(); it != value.end(); it++, it2++) {
		ifChange = false;
		if ((*it2) == String) {
			c += (*it)->Out(0) + ", ";
			continue;
		}
		if ((*it)->Float() && (*it2) == Int) {
			c += "_S("; ifChange = true;
		}
		else if (!(*it)->Float() && (*it2) == Float) {
			c += "_f("; ifChange = true;
		}
		if (!dynamic_cast<Exp_diff*>(*it) &&
			((*it)->type() == ExpTree::Type::Num || (*it)->type() == ExpTree::Type::Var)) {
			c += (*it)->Out(0);
		}
		else {
			char a[10];
			if ((*it)->Float())
				sprintf_s(a, "%d.0f]", i++);//TODO，其他世代，可能一直是-1
			else
				sprintf_s(a, "%d]", i++);
			c += "[-" + string(a);
		}
		c += ifChange ? "), " : ", ";
	}
	c = c.substr(0, c.length() - 2);
	return c;
}

string CommaExp_Exp::Out(bool ifLong, bool haveNum, int mode)
{
	return string();
}

string CommaExp_Exp::Out(bool ifLong, bool haveNum, int modeint, deque<NumType> var)
{
	Exp_diff* ed = dynamic_cast<Exp_diff*>(*(value.begin()));
	string c;
	int i = 1, numAllI = (ifLong ? 4 : 2), numAllF = numAllI, 
		num = (haveNum ? 3 : 2), numI = 0, numF = 0;
	for (deque<NumType>::iterator it = var.begin(); it != var.end(); ++it)
		(*it) == Int ? numI++ : numF++;
	/*for (list<Exp*>::iterator it = value.begin(); it != value.end(); ++it) {
		if (dynamic_cast<Exp_diff*>(*it) ||
			(*it)->type() != ExpTree::Type::Num && (*it)->type() != ExpTree::Type::Var)
			i++;
	}*/
	bool ifChange;
	int mode = 0;
	list<Exp*>::iterator it = value.begin();
	while (mode < 3) {
		ifChange = false;
		if ((*it)->Float() && mode <= 1) {
			c += "_S("; ifChange = true;
		}
		else if (!(*it)->Float() && mode >= 2) {
			c += "_f("; ifChange = true;
		}
		if (!dynamic_cast<Exp_diff*>(*it) &&
			((*it)->type() == ExpTree::Type::Num || (*it)->type() == ExpTree::Type::Var)) {
			c += (*it)->Out(0);
		}
		else {
			char a[10];
			if ((*it)->Float())
				sprintf_s(a, "%d.0f]", i++);
			else
				sprintf_s(a, "%d]", i++);
			c += "[-" + string(a);
		}
		c += ifChange ? "), " : ", ";

		if (mode == 0) {
			--num;
			if (num <= 0) {
				mode = 1;
				char a[13];
				sprintf_s(a, "%d", modeint);
				c += string(a) + ", ";
			}
			else/* if (num == 1) {
				it++;
			}
			else*/
				it++;
		}
		if (mode == 1) {
			if (numI <= 0) {
				mode = 2;
				for (int j = 0; j < numAllI; j++)
					c += "-999999, ";
			}
			else {
				it++;
				--numI;
				--numAllI;
			}
		}
		if (mode == 2) {
			if (numF <= 0) {
				mode = 3;
				for (int j = 0; j < numAllF; j++)
					c += "-999999.0f, ";
			}
			else {
				it++;
				--numF;
				--numAllF;
			}
		}
	}
	c = c.substr(0, c.length() - 2);
	return c;
}

CommaExp_Declare::CommaExp_Declare(string value, string sub_name)
{
	this->sub_name = sub_name;
	list<NumType> argList = list<NumType>();
	if (value != "") {
		string c = value, s;
		int n;
		while ((n = c.find(',')) != -1) {
			s = c.substr(0, c.find_last_not_of(' ', n - 1) + 1);
			int a = s.find_first_not_of(' ');
			int b = s.find(' ', a);
			string sign = s.substr(a, b - a);
			if (sign == "int") {
				ReadIns::varI.insert(s.substr(b + 1));
				argList.push_back(NumType::Int);
			}
			else if (sign == "float") {
				ReadIns::varF.insert(s.substr(b + 1));
				argList.push_back(NumType::Float);
			}
			else
				throw("unexpected declare");
			ReadIns::subVar.push_back(s.substr(b + 1));
			c = c.substr(n + 1);
		}
		s = c;
		int a = s.find_first_not_of(' ');
		int b = s.find(' ', a);
		string sign = s.substr(a, b - a);
		if (sign == "int") {
			ReadIns::varI.insert(s.substr(b + 1));
			argList.push_back(NumType::Int);
		}
		else if (sign == "float") {
			ReadIns::varF.insert(s.substr(b + 1));
			argList.push_back(NumType::Float);
		}
		else
			throw("unexpected declare");
		ReadIns::subVar.push_back(s.substr(b + 1));
	}
	ReadIns::subName.insert(pair<string, list<NumType>>(sub_name, argList));
}

CommaExp_Declare::~CommaExp_Declare()
{
}

string CommaExp_Declare::Out()
{
	string c;
	for (list<string>::iterator it = ReadIns::subVar.begin(); it != ReadIns::subVar.end(); it++) {
		c += (*it) + " ";
	}
	c = c.substr(0, c.length() - 1);
	return c;
}

CommaExp_Thread::CommaExp_Thread(string value, string thread_name)
{
	this->thread_name = thread_name;
	string c = value;
	int n;
	while ((n = c.find(',')) != -1) {
		this->value.push_back(new Exp_real(c.substr(0, c.find_last_not_of(' ', n - 1) + 1)));
		c = c.substr(n + 1);
	}
	if (c != "")
		this->value.push_back(new Exp_real(c));
	this->line = ReadIns::line;
}

CommaExp_Thread::~CommaExp_Thread()
{
}

string CommaExp_Thread::Stack(int Tab)
{
	string c;
	for (list<Exp_real*>::iterator it = value.begin(); it != value.end(); it++) {
		if ((*it)->type() != ExpTree::Type::Num && (*it)->type() != ExpTree::Type::Var) {
			c += (*it)->Out(Tab) + ";\n";
		}
	}
	return c;
}

string CommaExp_Thread::Out()
{
	string c;
	int i = 1;
	map<string, list<NumType>>::iterator it3 = ReadIns::subName.find(thread_name);
	if (ReadIns::defaultList.find(thread_name) != ReadIns::defaultList.end()) {
		for (list<Exp_real*>::iterator it = value.begin(); it != value.end(); it++) {
			c += ((*it)->Float()) ? "_ff " : "_SS ";
			if ((*it)->type() == ExpTree::Type::Num || (*it)->type() == ExpTree::Type::Var) {
				c += (*it)->Out(0) + ", ";
			}
			else {
				char a[10];
				if ((*it)->Float())
					sprintf_s(a, "%d.0f], ", --i);
				else
					sprintf_s(a, "%d], ", --i);
				c += "[-" + string(a);
			}
		}
		c = c.substr(0, c.length() - 2);
		return c;
	}
	if (it3 == ReadIns::subName.end()) {
		ReadIns::line = line;
		throw("unexpected name");
	}
	list<NumType> argList = it3->second;
	for (list<Exp_real*>::iterator it = value.begin(); it != value.end(); it++) {
		if ((*it)->type() != ExpTree::Type::Num && (*it)->type() != ExpTree::Type::Var)
			i++;
	}
	if (argList.size() != value.size()) {
		ReadIns::line = line;
		throw("wrong arg list");
	}
	list<NumType>::iterator it2 = argList.begin();
	for (list<Exp_real*>::iterator it = value.begin();
		it != value.end() && it2 != argList.end(); it++, it2++) {
		c += "_";
		c += ((*it)->Float()) ? "f" : "S";
		c += ((*it2) == NumType::Float) ? "f " : "S ";
		if ((*it)->type() == ExpTree::Type::Num || (*it)->type() == ExpTree::Type::Var) {
			c += (*it)->Out(0) + ", ";
		}
		else {
			char a[10];
			if ((*it)->Float())
				sprintf_s(a, "%d.0f], ", --i);
			else
				sprintf_s(a, "%d], ", --i);
			c += "[-" + string(a);
		}
	}
	c = c.substr(0, c.length() - 2);
	return c;
}

CommaExp_600::CommaExp_600(string value):CommaExp_Exp(value, false)
{
	if (this->value.size() != 10)
		throw("wrong arg list");
}

CommaExp_600::~CommaExp_600()
{
}

string CommaExp_600::Out(int Tab)
{
	list<Exp*>::iterator it = value.begin();
	if (!((*it)->type() == ExpTree::Type::Num || (*it)->type() == ExpTree::Type::Var))
		throw("using expression in set_et is forbidden");
	string c = "", sins = "";
	bool Float = (*it)->Float();
	//600
	sins += (Float ? "_S(" : "") + (*it)->Out(0) + (Float ? ")" : "");
	c += string(Tab * 4, ' ') + "ins_600(" + sins + ");\n";
	++it;
	//607
	if (!((*it)->type() == ExpTree::Type::Num || (*it)->type() == ExpTree::Type::Var))
		throw("using expression in set_et is forbidden");
	c += string(Tab * 4, ' ') + "ins_607(";
	c += sins + ", ";
	c += ((*it)->Float() ? "_S(" : "") + (*it)->Out(0) + ((*it)->Float() ? ")" : "");
	c += ");\n";
	++it;
	//602, 606
	string ins[3] = { "ins_602(", "ins_606(", "ins_625(" };
	bool thised1[2];
	for (int k = 0; k < 2; k++) {
		thised1[0] = dynamic_cast<Exp_diff*>(*it);
		if (!(thised1[0] || (*it)->type() == ExpTree::Type::Num || (*it)->type() == ExpTree::Type::Var))
			throw("using expression in set_et is forbidden");
		++it;
		thised1[1] = dynamic_cast<Exp_diff*>(*it);
		if (!(thised1[1] || (*it)->type() == ExpTree::Type::Num || (*it)->type() == ExpTree::Type::Var))
			throw("using expression in set_et is forbidden");
		--it;
		if (k == 1 && (thised1[0] || thised1[1]))
			k++;
		c += string(Tab * 4, ' ') + ins[k];
		c += sins + ", ";
		for (int j = 0; j < 2; j++) {
			if (thised1[j] && k >= 1)
				c += dynamic_cast<Exp_diff*>(*it)->Out_23();
			else if ((thised1[0] || thised1[1]) && k >= 1)
				c += (*it)->Out(0) + ", " + (*it)->Out(0) + ", " +
				(*it)->Out(0) + ", " + (*it)->Out(0);
			else
				c += ((*it)->Float() ? "_S(" : "") + (*it)->Out(0) + ((*it)->Float() ? ")" : "");
			c += (j ? ");\n" : ", ");
			++it;
		}
	}
	//604, 605
	ins[0] = "ins_604(";
	ins[1] = "ins_605(";
	ins[2] = "ins_624(";
	for (int k = 0; k < 2; k++) {
		thised1[0] = dynamic_cast<Exp_diff*>(*it);
		if (!(thised1[0] || (*it)->type() == ExpTree::Type::Num || (*it)->type() == ExpTree::Type::Var))
			throw("using expression in set_et is forbidden");
		++it;
		thised1[1] = dynamic_cast<Exp_diff*>(*it);
		if (!(thised1[1] || (*it)->type() == ExpTree::Type::Num || (*it)->type() == ExpTree::Type::Var))
			throw("using expression in set_et is forbidden");
		--it;
		if (k == 1 && (thised1[0] || thised1[1]))
			k++;
		c += string(Tab * 4, ' ') + ins[k];
		c += sins + ", ";
		for (int j = 0; j < 2; j++) {
			if (thised1[j] && k >= 1)
				c += dynamic_cast<Exp_diff*>(*it)->Out_23();
			else if ((thised1[0] || thised1[1]) && k >= 1)
			c += (*it)->Out(0) + ", " + (*it)->Out(0) + ", " +
				(*it)->Out(0) + ", " + (*it)->Out(0);
			else
				c += ((*it)->Float() ? "" : "_f(") + (*it)->Out(0) + ((*it)->Float() ? "" : ")");
			c += (j ? ");\n" : ", ");
			if (!(k && j))
				++it;
		}
	}
	return c;
}
