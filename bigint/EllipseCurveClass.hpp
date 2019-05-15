#ifndef ELLIPSECURVECLASS_HPP
#define ELLIPSECURVECLASS_HPP

#include "BigIntegerLibrary.hh"
#include <math.h>
#include <vector>
#include <map>
#include <iterator>


struct ellipse_curve
{
	BigInteger a;
	BigInteger b;
	BigUnsigned m;
};
struct ellipse_curve_point
{
	BigInteger x;
	BigInteger y;
	bool is_infinitive;
	ellipse_curve_point()
	{}
	ellipse_curve_point(BigInteger &&x_val, BigInteger &&y_val):x(x_val),y(y_val)
	{}
	bool operator==(const ellipse_curve_point &point)
	{
		if (x == point.x && y == point.y)
			return 1;
		return 0;
	}
	bool operator!=(const ellipse_curve_point &point)
	{
		return point.x != x || point.y != y;
	}
};

class ellipse_curve_class
{
	ellipse_curve curve;
	public:
	ellipse_curve_class()
	{}
	ellipse_curve_class(BigInteger &&a, BigInteger &&b, BigUnsigned &&m)
	{
		curve.a = a;
		curve.b = b;
		curve.m = m;
	}
	ellipse_curve_class(const ellipse_curve_class &arg)
	{
		curve.a = arg.curve.a;
		curve.b = arg.curve.b;
		curve.m = arg.curve.m;
	}
	~ellipse_curve_class(){}
	BigInteger phi(BigInteger n) {
	BigInteger ret(1);
	for(BigInteger i(2); i * i <= n; i++) {
		BigInteger p(1);
		while(n % i == BigInteger(0)) {
			p = p * i;
			n = n / i;
		}
		p = p / i;
		if(!p.isZero() && p >= BigInteger(1))
		{
			ret = ret * p * (i - BigInteger(1));
			p = p / i;
		}
	}
	n = n - 1;
	if (n != BigInteger(0))
		return n * ret;
	return ret;
}
	ellipse_curve_point curve_point_add(ellipse_curve_point &P, ellipse_curve_point &Q)
	{
		ellipse_curve_point res;
		BigInteger lambda;
		
		if (P != Q)
			lambda = (kar_mul((Q.y - P.y) ,(Q.x - P.x).toPow(phi(curve.m) - 1))) % curve.m;
		else
		{
			//if ((kar_mul(BigInteger(3) , P.x.toPow(2)) + curve.a) < BigInteger(2) * P.y)
				lambda = (kar_mul((kar_mul(BigInteger(3) , kar_mul(P.x, P.x)) + curve.a), (BigInteger(2) * P.y).toPow(phi(curve.m) - 1)))%curve.m;
			//else
			//	lambda = ((kar_mul(BigInteger(3) , P.x.toPow(2)) + curve.a) / (BigInteger(2) * P.y))%curve.m;
		}
		//else
		//	lambda = (Q.y - P.y) / (Q.x - P.x) % curve.m;
		res.x = (kar_mul(lambda, lambda) -P.x -Q.x) % curve.m;
		res.y = (kar_mul(lambda ,(P.x - res.x)) - P.y) %curve.m;
		
		return (res);
	}
	ellipse_curve_point curve_point_mul(ellipse_curve_point &P, unsigned int n)
	{
		ellipse_curve_point res;
		std::vector<int> step;
		std::map<int, ellipse_curve_point> points;
		int i = 0;
		int size = 0;
		int start = 0;
		while (n != 0)
		{
			if (n % 2 == 1)
			{
				size++;
				step.push_back(i);
			}
			n = n / 2;
			i++;
		}
		for (auto _v: step)
			std::cout << _v << std::endl;

		res = P;
		while (start < size)
		{
			if (step[start] == 0)
			{
				res = P;
				points.insert(std::make_pair(step[start], res));
				start++;
				continue;
			}
				auto point = points.find(step[start] - 1);
				if (point == points.end())
				{	
					int start_copy = step[start] - 2;
					while (start_copy != -1)
					{
						auto point_back = points.find(step[start_copy]);
						if (point_back != points.end())
							break;
						start_copy--;
					}
					if (start_copy != -1)
						res = points[start_copy];
					start_copy = start_copy != -1 ? step[start_copy] : 0;
					while (step[start] != start_copy)
					{
						res = curve_point_add(res, res);
						start_copy++;
					}
					points.insert(std::make_pair(step[start], res));
				}
				else
				{
					res = curve_point_add(point->second, point->second);
					points.insert(std::make_pair(step[start], res));
				}
			start++;
		}
		int p = -1;
		for (auto v = step.begin() ; v != step.end() - 1;  v++)
			res = curve_point_add(points[*v], points[*(v + 1)]);
		return (res);
	}
	ellipse_curve_point curve_point_mul(ellipse_curve_point &P, BigInteger n)
	{
		ellipse_curve_point res;
		if (n.isZero())
			return res;
		n--;
		while (!n.isZero())
		{
			res = curve_point_add(P, P);
			n--;
		}
		return res;

	}
/*
    n = max(размер X, размер Y)
    если n = 1: вернуть X * Y
    X_l = левые n/2 цифр X
    X_r = правые n/2 цифр X
    Y_l = левые n/2 цифр Y
    Y_r = правые n/2 цифр Y
    Prod1 = Karatsuba_mul(X_l, Y_l)
    Prod2 = Karatsuba_mul(X_r, Y_r)
    Prod3 = Karatsuba_mul(X_l + X_r, Y_l + Y_r)
    вернуть Prod1 * 10 ^ n + (Prod3 - Prod1 - Prod2) * 10 ^ (n / 2) + Prod2 */
    BigInteger kar_mul(BigInteger P, BigInteger n)
	{
        BigInteger res;
        int div = 1;
        int sign = 1;
        unsigned int len;

        if (P.getSign() == -1 || n.getSign() == -1)
        {
        	sign = -1;
        	if (P.getSign() == -1 && n.getSign() == -1)
        	{
        		P = -P;
        		n = -n;
        		sign = 1;
        	}
        	else if (P.getSign() == -1)
        		P = -P;
        	else if (n.getSign() == -1)
        		n = -n;
        }
        len = (bigIntegerToString(n)).length() > (bigIntegerToString(P)).length() ? (bigIntegerToString(n)).length() : (bigIntegerToString(P)).length();
        if (len == 1)
        {
        	res = P * n;
        	return (res);
        }
        if ((bigIntegerToString(n)).length()%2)
        {
        	n *= BigInteger(10);
        	if (len % 2)
        		len++;
        	div *= 10;
        }
        if ((bigIntegerToString(P)).length()%2)
        {
        	P *= BigInteger(10);
        	if (len % 2)
        		len++;
        	div *= 10;
        }
        BigInteger divider(BigInteger(10).toPow(len/2));
        BigInteger X_l = P / divider;
        BigInteger X_r = P % divider;
        BigInteger Y_l = n / divider;
        BigInteger Y_r = n % divider;
        //std::cout <<"X_l = " << X_l << " X_r = "  << X_r << " Y_l = " << Y_l << " Y_r = " << Y_r<< std::endl;
        BigInteger r1 = kar_mul(X_l , Y_l);
        BigInteger r2 = kar_mul(X_r , Y_r);
        BigInteger ar1(X_l + X_r);
        BigInteger ar2(Y_l + Y_r);
        BigInteger r3 = kar_mul(ar1 , ar2);
        //std::cout <<"r1 = " << r1 << " r2 = "  << r2 << " r3 = " << r3 << std::endl;
        res = r1 * BigInteger(10).toPow(len) + (r3 - r1 - r2) * BigInteger(10).toPow(len / 2) + r2;
        //std::cout << div << std::endl;
        //std::cout <<"res = " << (res / BigInteger(div)) << std::endl;
        return (res / BigInteger(div)) * BigInteger(sign) ;
    }

};

#endif