#ifndef _YHSTL_COMPLEX_H_
#define _YHSTL_COMPLEX_H_
#include<iostream>
class complex
{
public:
	complex(double r = 0, double i = 0) :re(r), im(i) {};    //构造函数，注意要使用初始化列表
	complex& operator +=(const complex&);					 //先声明+=函数
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

//复数加复数
inline complex operator+(const complex& x, const complex& y)
{
	return complex(x.real() + y.real(), x.imag() + y.imag());
}

//复数加实数
inline complex operator+(const complex& x, double y)
{
	return complex(x.real() + y, x.imag());
}

//实数加复数
inline complex operator+(double x, const complex& y)
{
	return complex(x + y.real(), y.imag());
}
//如果把+这个操作设置为成员函数，那么只能应付复数加复数并不能进行复数加实数等操作。所以要设置为全局函数

//操作符重载有两种，一个是写成成员函数，一个是写成非成员函数


//在类外重载<<   为了连串赋值，需要返回ostream引用
std::ostream& operator <<(std::ostream& os, const complex& x)
{
	return os << '(' << x.real() << ',' << x.imag() << ')';
}

#endif // !_YHSTL_COMPLEX_H_

