#include "BigInt.hpp"
#include <ostream>

BigInt::BigInt(int n) : _sign(1), _binaryForm(false)
{
	if (n < 0)
		_sign = -1;
	do
	{
		_digits.push_back(n % 10 * _sign);
		n /= 10;
	} while (n);
}

BigInt::BigInt(long n) : _sign(1), _binaryForm(false)
{
	if (n < 0)
		_sign = -1;
	do
	{
		_digits.push_back(n % 10 * _sign);
		n /= 10;
	} while (n);
}

BigInt::BigInt(long long n) : _sign(1), _binaryForm(false)
{
	if (n < 0)
		_sign = -1;
	do
	{
		_digits.push_back(n % 10 * _sign);
		n /= 10;
	} while (n);
}

BigInt::BigInt(const std::string& s) : _sign(1), _binaryForm(false)
{
	int size = s.size();
	for (int i = size - 1; i >= 0; --i)
	{
		if (i == 0 && s[i] == '-')
			_sign = -1;
		else
			_digits.push_back(s[i] - '0');
	}
}

BigInt::BigInt(const char* s) : _sign(1), _binaryForm(false)
{
	int size = strlen(s);
	for (int i = size - 1; i >= 0; --i)
	{
		if (i == 0 && s[i] == '-')
			_sign = -1;
		else
			_digits.push_back(s[i] - '0');
	}
}

BigInt::BigInt(const BigInt& other) : _digits(other._digits), _sign(other._sign), _binaryForm(other._binaryForm)
{}

BigInt& BigInt::operator=(const BigInt& other)
{
	if (this == &other)
		return *this;

	_digits = other._digits;
	_sign = other._sign;
	_binaryForm = other._binaryForm;

	return *this;
}

void BigInt::_increment()
{
	int i = 0;
	int size = _digits.size();
	for (; i < size && _digits[i] == 9; ++i)
		_digits[i] = 0;
	if (i == size)
		_digits.push_back(1);
	else
		++_digits[i];
}

void BigInt::_decrement()
{
	int size = _digits.size();
	int i = 0;
	for (; i < size && _digits[i] == 0; ++i)
		_digits[i] = 9;
	--_digits[i];
	if (size > 1 && _digits[size - 1] == 0)
		_digits.pop_back();
}

void BigInt::_trim()
{
	int size = _digits.size();
	for (int i = size - 1; i > 0 && _digits[i] == 0; --i)
		_digits.pop_back();
	if (_digits.size() == 1 && _digits[0] == 0)
		_sign = 1;
}

BigInt BigInt::_toBinary() const
{
	if (_binaryForm)
		return *this;

	BigInt reminder(*this);
	reminder._sign = 1;
	BigInt result;
	BigInt powerOf2(1);
	while (powerOf2 < reminder)
		powerOf2 *= 2;
	powerOf2 /= 2;

	while (powerOf2 > 0)
	{
		bool isGreater = reminder >= powerOf2;
		result._digits.push_back(isGreater);
		if (isGreater)
			reminder -= powerOf2;
		powerOf2 /= 2;
	}

	size_t size = result._digits.size();
	for (size_t i = 0; i < size / 2; ++i)
	{
		char temp = result._digits[i];
		result._digits[i] = result._digits[size - i - 1];
		result._digits[size - i - 1] = temp;
	}
	result._sign = _sign;
	result._binaryForm = true;
	return result;
}

BigInt BigInt::_fromBinary() const
{
	if (!_binaryForm)
		return *this;

	BigInt result;
	BigInt powerOfTwo(1);
	BigInt current(*this);

	if (_sign == -1)
	{
		size_t size = current._digits.size();
		for (size_t i = 0; i < size; ++i)
			current._digits[i] = !current._digits[i];
	}

	int size = _digits.size();
	for (int i = 0; i < size; ++i)
	{
		if (current._digits[i] == 1)
			result += powerOfTwo;
		powerOfTwo *= 2;
	}
	result._sign = _sign;
	if (_sign == -1)
		result -= 1;
	result._binaryForm = false;
	return result;
}

BigInt BigInt::_inDecimal() const
{
	if (_binaryForm)
		return _fromBinary();
	return *this;
}

BigInt BigInt::_inBinary() const
{
	if (_binaryForm)
		return *this;
	return _toBinary();
}

BigInt& BigInt::operator++()
{
	*this = _inDecimal();
	if (_sign == -1)
	{
		if (_digits.size() == 1 && _digits[0] == 1)
		{
			_sign = 1;
			_digits[0] = 0;
			return *this;
		}
		_decrement();
	}
	else
		_increment();
	return *this;
}

BigInt BigInt::operator++(int)
{
	*this = _inDecimal();
	BigInt tmp(*this);
	++(*this);
	return tmp;
}

BigInt& BigInt::operator--()
{
	*this = _inDecimal();
	if (_sign == -1)
		_increment();
	else
	{
		if (_digits.size() == 1 && _digits[0] == 0)
		{
			_sign = -1;
			_digits[0] = 1;
			return *this;
		}
		_decrement();
	}
	return *this;
}

BigInt BigInt::operator--(int)
{
	*this = _inDecimal();
	BigInt tmp(*this);
	--(*this);
	return tmp;
}

BigInt& operator+=(BigInt& a, const BigInt& b)
{
	a = a._inDecimal();
	BigInt b1(b._inDecimal());
	if (a._sign == b1._sign)
	{
		int bSize = b1._digits.size();
		for (int i = 0, carry = 0; i < bSize || carry; ++i)
		{
			if (i == a._digits.size())
				a._digits.push_back(0);
			a._digits[i] += carry + (i < bSize ? b1._digits[i] : 0);
			carry = 0;
			if (a._digits[i] >= 10)
			{
				carry = 1;
				a._digits[i] -= 10;
			}
		}
	}
	else if (b1 != 0)
		a -= -b1;
	return a;
}

BigInt operator+(const BigInt& a, const BigInt& b)
{
	BigInt temp(a);
	temp += b;
	return temp;
}

BigInt& operator-=(BigInt& a, const BigInt& b)
 {
	a = a._inDecimal();
	BigInt b1(b._inDecimal());
	if (a._sign == b1._sign)
	{
		if (a._sign == 1 && a >= b1 || a._sign == -1 && a <= b1)
		{
			int aSize = a._digits.size();
			int bSize = b1._digits.size();
			for (int i = 0, carry = 0; i < bSize || carry; ++i)
			{
				a._digits[i] -= carry + (i < bSize ? b1._digits[i] : 0);
				carry = 0;
				if (a._digits[i] < 0)
				{
					carry = 1;
					a._digits[i] += 10;
				}
			}
			a._trim();
		}
		else
		{
			a = b1 - a;
			a._sign = -a._sign;
		}
	}
	else
		a += -b1;
	return a;
}

BigInt operator-(const BigInt& a, const BigInt& b)
{
	BigInt temp(a);
	temp -= b;
	return temp;
}

BigInt operator-(const BigInt& a)
{
	BigInt temp(a);

	if (!(a._digits.size() == 1 && a._digits[0] == 0))
		temp._sign = -a._sign;
	return temp;
}

BigInt& operator*=(BigInt& a, const BigInt& b)
{
	a = a * b;
	return a;
}

BigInt operator*(const BigInt& a, const BigInt& b)
{
	BigInt a1 = a._inDecimal();
	BigInt b1 = b._inDecimal();
	BigInt result;
	result._sign = a1._sign * b1._sign;
	int aSize = a1._digits.size();
	int bSize = b1._digits.size();
	result._digits.resize(aSize + bSize);
	for (int i = 0; i < aSize; ++i)
	{
		if (a1._digits[i])
		{
			for (int j = 0, carry = 0; j < bSize || carry; ++j)
			{
				int current = result._digits[i + j] + (int)a1._digits[i] * (j < b1._digits.size() ? b1._digits[j] : 0) + carry;
				carry = current / 10;
				result._digits[i + j] = current % 10;
			}
		}
	}
	result._trim();
	return result;
}

BigInt& operator/=(BigInt& a, const BigInt& b)
{
	a = a._inDecimal();
	BigInt b1 = b._inDecimal();
	a = divmod(a, b1).first;
	return a;
}

BigInt operator/(const BigInt& a, const BigInt& b)
{
	BigInt a1 = a._inDecimal();
	BigInt b1 = b._inDecimal();
	return divmod(a1, b1).first;
}

BigInt& operator%=(BigInt& a, const BigInt& b)
{
	a = a._inDecimal();
	BigInt b1 = b._inDecimal();
	a = divmod(a, b1).second;
	return a;
}

BigInt operator%(const BigInt& a, const BigInt& b)
{
	BigInt a1 = a._inDecimal();
	BigInt b1 = b._inDecimal();
	return divmod(a1, b1).second;
}

BigInt& operator/=(BigInt& a, int v)
{
	a = a._inDecimal();
	if (v < 0)
		a._sign = -a._sign;
	int aSize = a._digits.size();
	for (int i = aSize - 1, reminder = 0; i >= 0; --i)
	{
		long long current = a._digits[i] + reminder * 10ll;
		a._digits[i] = current / v;
		reminder = current % v;
	}
	a._trim();
	return a;
}

BigInt operator/(const BigInt& a, int v)
{
	BigInt result(a);
	result /= v;
	return result;
}

BigInt& operator%=(BigInt& a, int v)
{
	a = a % v;
	return a;
}

BigInt operator%(const BigInt& a, int v)
{
	BigInt a1 = a._inDecimal();
	if (v < 0)
		v = -v;
	int m = 0;
	int aSize = a1._digits.size();
	for (int i = aSize - 1; i >= 0; --i)
		m = (a1._digits[i] + m * 10) % v;
	return m * a1._sign;
}

BigInt& operator^=(BigInt& a, const BigInt& b)
{
	a = a ^ b;
	return a;
}

BigInt operator^(const BigInt& a, const BigInt& b)
{
	BigInt binaryA = a._inBinary();
	BigInt binaryB = b._inBinary();
	size_t aSize = binaryA._digits.size();
	size_t bSize = binaryB._digits.size();
	BigInt longerBinary = aSize > bSize ? binaryA : binaryB;
	BigInt shorterBinary = aSize > bSize ? binaryB : binaryA;
	size_t longerSize = longerBinary._digits.size();
	size_t shorterSize = shorterBinary._digits.size();

	for (size_t i = 0; i < longerSize; ++i)
		longerBinary._digits[i] ^= i < shorterSize ? shorterBinary._digits[i] : shorterBinary._sign == -1;

	longerBinary._sign = a._sign * b._sign;

	return longerBinary;
}

BigInt& operator|=(BigInt& a, const BigInt& b)
{
	a = a | b;
	return a;
}

BigInt operator|(const BigInt& a, const BigInt& b)
{
	BigInt binaryA = a._inBinary();
	BigInt binaryB = b._inBinary();
	size_t aSize = binaryA._digits.size();
	size_t bSize = binaryB._digits.size();
	BigInt longerBinary = aSize > bSize ? binaryA : binaryB;
	BigInt shorterBinary = aSize > bSize ? binaryB : binaryA;
	size_t longerSize = longerBinary._digits.size();
	size_t shorterSize = shorterBinary._digits.size();

	for (size_t i = 0; i < longerSize; ++i)
		longerBinary._digits[i] |= i < shorterSize ? shorterBinary._digits[i] : shorterBinary._sign == -1;

	longerBinary._sign = a._sign * b._sign;

	return longerBinary;
}

BigInt& operator&=(BigInt& a, const BigInt& b)
{
	a = a & b;
	return a;
}

BigInt operator&(const BigInt& a, const BigInt& b)
{
	BigInt binaryA = a._inBinary();
	BigInt binaryB = b._inBinary();
	size_t aSize = binaryA._digits.size();
	size_t bSize = binaryB._digits.size();
	BigInt longerBinary = aSize > bSize ? binaryA : binaryB;
	BigInt shorterBinary = aSize > bSize ? binaryB : binaryA;
	size_t longerSize = longerBinary._digits.size();
	size_t shorterSize = shorterBinary._digits.size();

	for (size_t i = 0; i < longerSize; ++i)
		longerBinary._digits[i] &= i < shorterSize ? shorterBinary._digits[i] : shorterBinary._sign == -1;

	longerBinary._sign = a._sign * b._sign;

	return longerBinary;
}

BigInt& operator>>=(BigInt& a, size_t shift)
{
	a = a >> shift;
	return a;
}

BigInt operator>>(const BigInt& a, size_t shift)
{
	BigInt binaryA = a._inBinary();
	int aSize = binaryA._digits.size();
	if (shift > aSize)
		return 0;

	for (size_t i = 0; i < aSize - shift; ++i)
		binaryA._digits[i] = binaryA._digits[i + shift];

	for (size_t i = aSize - shift; i < aSize; ++i)
		binaryA._digits[i] = 0;

	binaryA._trim();

	return binaryA;
}

BigInt& operator<<=(BigInt& a, size_t shift)
{
	a = a << shift;
	return a;
}

BigInt operator<<(const BigInt& a, size_t shift)
{
	if (a._digits.size() == 1 && a._digits[0] == 0)
		return 0;
	if (shift == 0)
		return a;
	BigInt binaryA = a._inBinary();

	int aSize = binaryA._digits.size() + shift;
	binaryA._digits.resize(aSize);
	for (int i = aSize - 1; i >= shift; --i)
		binaryA._digits[i] = binaryA._digits[i - shift];

	for (int i = shift; i >= 0; --i)
		binaryA._digits[i] = 0;

	return binaryA;
}

BigInt operator~(const BigInt& a)
{
	BigInt binaryA = a._inBinary();
	size_t aSize = binaryA._digits.size();

	for (size_t i = 0; i < aSize; ++i)
		binaryA._digits[i] = !binaryA._digits[i];

	return binaryA;
}

bool operator==(const BigInt& a, const BigInt& b)
{
	return a._digits == b._digits && a._sign == b._sign;
}

bool operator!=(const BigInt& a, const BigInt& b)
{
	return !(a == b);
}

bool operator<(const BigInt& a, const BigInt& b)
{
	BigInt a1 = a._inDecimal();
	BigInt b1 = b._inDecimal();
	if (a1._sign != b1._sign)
		return a1._sign < b1._sign;
	if (a1._digits.size() != b1._digits.size())
		return a1._digits.size() * a1._sign < b1._digits.size()* b1._sign;
	int bSize = b1._digits.size();
	for (int i = bSize - 1; i >= 0; --i)
	{
		if (a1._digits[i] != b1._digits[i])
			return a1._digits[i] * a1._sign < b1._digits[i] * b1._sign;
	}
	return false;
}

bool operator<=(const BigInt& a, const BigInt& b)
{
	return !(a > b);
}

bool operator>(const BigInt& a, const BigInt& b)
{
	return b < a;
}

bool operator>=(const BigInt& a, const BigInt& b)
{
	return !(a < b);
}

std::ostream& operator<<(std::ostream& out, const BigInt& bi)
{
	BigInt a1 = bi._inDecimal();
	int size = a1._digits.size();
	if (a1._sign == -1)
		out << '-';
	for (int i = size - 1; i >= 0; --i)
		out << (short)a1._digits[i];
	return out;
}

std::pair<BigInt, BigInt> divmod(const BigInt& a, const BigInt& b)
{
	int norm = 10 / (b._digits.back() + 1);
	BigInt a2(a * norm);
	a2._sign = 1;
	BigInt b2(b * norm);
	b2._sign = 1;
	BigInt q;
	BigInt r;
	q._digits.resize(a2._digits.size());

	int aSize = a2._digits.size();
	for (int i = aSize - 1; i >= 0; --i)
	{
		r *= 10;
		r += a2._digits[i];
		int s1 = b2._digits.size() < r._digits.size() ? r._digits[b2._digits.size()] : 0;
		int s2 = b2._digits.size() - 1 < r._digits.size() ? r._digits[b2._digits.size() - 1] : 0;
		int d = (s1 * 10 + s2) / b2._digits.back();
		r -= b2 * d;
		while (r < 0)
		{
			r += b2;
			--d;
		}
		q._digits[i] = d;
	}

	q._sign = a._sign * b._sign;
	r._sign = a._sign;
	q._trim();
	r._trim();
	return { q, r / norm };
}
