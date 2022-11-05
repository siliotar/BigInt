#pragma once

#include <vector>
#include <string>

class BigInt
{
private:
	std::string _digits;
	int _sign;

	void _fromNumber(long long n);
	void _fromString(std::string& s);

	void _increment();
	void _decrement();
	void _trim();

public:
	BigInt(int n);
	BigInt(long  n);
	BigInt(long long n = 0);
	BigInt(const std::string& s);
	BigInt(const char* s);
	BigInt(const BigInt& other);
	BigInt& operator=(const BigInt& other);

	BigInt& operator++();
	BigInt operator++(int);
	BigInt& operator--();
	BigInt operator--(int);

	friend BigInt& operator+=(BigInt& a, const BigInt& b);
	friend BigInt operator+(const BigInt& a, const BigInt& b);
	friend BigInt& operator-=(BigInt& a, const BigInt& b);
	friend BigInt operator-(const BigInt& a, const BigInt& b);

	friend BigInt operator-(const BigInt& a);

	friend BigInt& operator*=(BigInt& a, const BigInt& b);
	friend BigInt operator*(const BigInt& a, const BigInt& b);
	friend BigInt& operator/=(BigInt& a, const BigInt& b);
	friend BigInt operator/(const BigInt& a, const BigInt& b);
	friend BigInt& operator%=(BigInt& a, const BigInt& b);
	friend BigInt operator%(const BigInt& a, const BigInt& b);
	friend BigInt& operator/=(BigInt& a, int v);
	friend BigInt operator/(const BigInt& a, int v);
	friend BigInt& operator%=(BigInt& a, int v);
	friend BigInt operator%(const BigInt& a, int v);

	friend BigInt& operator^=(BigInt& a, const BigInt& b);
	friend BigInt operator^(const BigInt& a, const BigInt& b);
	friend BigInt& operator|=(BigInt& a, const BigInt& b);
	friend BigInt operator|(const BigInt& a, const BigInt& b);
	friend BigInt& operator&=(BigInt& a, const BigInt& b);
	friend BigInt operator&(const BigInt& a, const BigInt& b);

	friend BigInt& operator>>=(BigInt& a, size_t shift);
	friend BigInt operator>>(const BigInt& a, size_t shift);
	friend BigInt& operator<<=(BigInt& a, size_t shift);
	friend BigInt operator<<(const BigInt& a, size_t shift);

	friend BigInt operator~(const BigInt& a);

	friend bool operator==(const BigInt& a, const BigInt& b);
	friend bool operator!=(const BigInt& a, const BigInt& b);
	friend bool operator<(const BigInt& a, const BigInt& b);
	friend bool operator<=(const BigInt& a, const BigInt& b);
	friend bool operator>(const BigInt& a, const BigInt& b);
	friend bool operator>=(const BigInt& a, const BigInt& b);

	friend std::ostream& operator<<(std::ostream& out, const BigInt& bi);

	friend std::pair<BigInt, BigInt> divmod(const BigInt& a, const BigInt& b);

	friend std::string to_string(const BigInt& a);

	long long getLL() const;
};
