#ifndef _YHSTL_COMPLEX_H_
#define _YHSTL_COMPLEX_H_
#include<iostream>
class complex
{
public:
	complex(double r = 0, double i = 0) :re(r), im(i) {};    //���캯����ע��Ҫʹ�ó�ʼ���б�
	complex& operator +=(const complex&);					 //������+=����
	double real() const { return re; };
	double imag() const { return im; };



private:
	double re, im;

	friend complex& _doap1(complex*, const complex&);
};


inline complex& _doap1(complex* ths, const complex& r)
{
	ths->re += r.re;
	ths->im += r.im;
	return *ths;
}


inline complex& complex::operator+=(const complex& r)
{
	return _doap1(this, r);

}

//�����Ӹ���
inline complex operator+(const complex& x, const complex& y)
{
	return complex(x.real() + y.real(), x.imag() + y.imag());
}

//������ʵ��
inline complex operator+(const complex& x, double y)
{
	return complex(x.real() + y, x.imag());
}

//ʵ���Ӹ���
inline complex operator+(double x, const complex& y)
{
	return complex(x + y.real(), y.imag());
}
//�����+�����������Ϊ��Ա��������ôֻ��Ӧ�������Ӹ��������ܽ��и�����ʵ���Ȳ���������Ҫ����Ϊȫ�ֺ���

//���������������֣�һ����д�ɳ�Ա������һ����д�ɷǳ�Ա����


//����������<<   Ϊ��������ֵ����Ҫ����ostream����
std::ostream& operator <<(std::ostream& os, const complex& x)
{
	return os << '(' << x.real() << ',' << x.imag() << ')';
}

#endif // !_YHSTL_COMPLEX_H_

