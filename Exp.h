#pragma once

#include "ExpTree.h"

//���ʽ
class Exp
{
public:
	Exp();
	~Exp();
	virtual string Out(int Tab);	//���
	virtual ExpTree::Type type();	//������������
	virtual bool Float();			//�������Ƿ�Ϊ����

protected:
	//��ȡʱ��״̬
	static enum Status {
		Space, Number, Variable, Sign
	};
};

//��ı��ʽ
class Exp_real :public Exp {
public:
	Exp_real(string value);
	~Exp_real();
	string Out(int Tab);
	ExpTree::Type type();
	bool Float();
	bool One();				//���ر��ʽ�ǲ��Ǿ���1

private:
	ExpTree* tree;			//���ʽ��
};

//�ַ����ͱ��ʽ
class Exp_str :public Exp {
public:
	Exp_str(string value);
	~Exp_str();
	string Out(int Tab);

private:
	string value;			//�ַ�����ֵ
};

//���Ѷȱ��ʽ
class Exp_diff :public Exp {
public:
	Exp_diff(string value);
	~Exp_diff();
	string Out(int Tab);				//��23,600������
	string Out(int Tab, bool ifNext);	//��ɶ������
	string Out_23();					//23,600������
	string Out_23(bool Float);
	string Out_23(int &i);				//23,600��������iΪ����[-1]�Ķ�ջ��������
	string Stack_23(int &i, int Tab);	//��ջ�ϵ������iΪ��ջ��������
	string Stack_23(int Tab);
	ExpTree::Type type();
	bool Float();

private:
	Exp_real* value[4];					//4�����ʽ��
};