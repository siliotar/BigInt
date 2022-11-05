#include "BigInt.hpp"
#include <ostream>

void BigInt::_fromNumber(long long n)
{
	if (n < 0)
		_sign = -1;
	do
	{
		_digits.push_back(n % 2 * _sign);
		n /= 2;
	} while (n);

	if (_sign == -1)
	{
		_sign = 1;
		*this = ~*this;
		++(*this);
		_trim();
	}
}

static void _multiplyBase10by2(std::string& s)
{
	size_t size = s.size();
	bool reminder = false;
	for (size_t i = 0; i < size; ++i)
	{
		s[i] += s[i] + reminder;
		reminder = 0;
		if (s[i] >= 10)
		{
			reminder = 1;
			s[i] -= 10;
		}
	}
	if (reminder)
		s.push_back(1);
}

static bool _base10Greater(const std::string& s1, const std::string& s2)
{
	int size1 = s1.size();
	int size2 = s2.size();
	if (size1 != size2)
		return size1 > size2;
	for (int i = size1 - 1; i >= 0; --i)
		if (s1[i] != s2[i])
			return s1[i] > s2[i];
	return false;
}

static void _getMaxPowerOfTwo(const std::string& s, std::vector<std::string>& result)
{
	std::string temp("\1");
	while (!_base10Greater(temp, s))
	{
		result.push_back(temp);
		_multiplyBase10by2(temp);
	}
}

static void _trimBase10(std::string& s)
{
	size_t size = s.size();
	for (size_t i = size - 1; i > 0 && s[i] == 0; --i)
		s.pop_back();
}

static void _minusBase10(std::string& s1, const std::string& s2)
{
	size_t size = s2.size();
	bool reminder = false;
	for (size_t i = 0; i < size || reminder; ++i)
	{
		s1[i] -= reminder + (i < s2.size() ? s2[i] : 0);
		reminder = 0;
		if (s1[i] < 0)
		{
			reminder = 1;
			s1[i] += 10;
		}
	}
	_trimBase10(s1);
}

void BigInt::_fromString(std::string& s)
{
	std::vector<std::string> powersOf2;
	_getMaxPowerOfTwo(s, powersOf2);
	int size = powersOf2.size();

	for (int i = size - 1; i >= 0; --i)
	{
		if (!_base10Greater(powersOf2[i], s))
		{
			_digits.push_back(1);
			_minusBase10(s, powersOf2[i]);
		}
		else
			_digits.push_back(0);
	}

	size = _digits.size();
	for (int i = 0; i < size / 2; ++i)
	{
		char temp = _digits[i];
		_digits[i] = _digits[size - i - 1];
		_digits[size - i - 1] = temp;
	}
	if (_sign == -1)
	{
		_sign = 1;
		*this = ~*this;
		++(*this);
		_trim();
	}
}

BigInt::BigInt(int n) : _sign(1)
{
	_fromNumber(n);
}

BigInt::BigInt(long n) : _sign(1)
{
	_fromNumber(n);
}

BigInt::BigInt(long long n) : _sign(1)
{
	_fromNumber(n);
}

BigInt::BigInt(const std::string& s) : _sign(1)
{
	bool negative = s[0] == '-';
	std::string s1(negative ? s.substr(1) : s);
	if (negative)
		_sign = -1;
	size_t size = s1.size();
	for (size_t i = 0; i < size / 2; ++i)
	{
		char temp = s1[i] - '0';
		s1[i] = s1[size - i - 1] - '0';
		s1[size - i - 1] = temp;
	}
	if (size % 2)
		s1[size / 2] -= '0';
	_fromString(s1);
}

BigInt::BigInt(const char* s) : _sign(1)
{
	bool negative = s[0] == '-';
	std::string s1(negative ? s + 1 : s);
	if (negative)
		_sign = -1;
	size_t size = s1.size();
	for (size_t i = 0; i < size / 2; ++i)
	{
		char temp = s1[i] - '0';
		s1[i] = s1[size - i - 1] - '0';
		s1[size - i - 1] = temp;
	}
	if (size % 2)
		s1[size / 2] -= '0';
	_fromString(s1);
}

BigInt::BigInt(const BigInt& other) : _digits(other._digits), _sign(other._sign)
{}

BigInt& BigInt::operator=(const BigInt& other)
{
	if (this == &other)
		return *this;

	_digits = other._digits;
	_sign = other._sign;
	_trim();

	return *this;
}

void BigInt::_increment()
{
	int i = 0;
	int size = _digits.size();
	for (; i < size && _digits[i] == 1; ++i)
		_digits[i] = 0;
	if (i == size)
		_digits.push_back(1);
	else
		_digits[i] = 1;
}

void BigInt::_decrement()
{
	int size = _digits.size();
	int i = 0;
	for (; i < size && _digits[i] == 0; ++i)
		_digits[i] = 1;
	_digits[i] = 0;
	if (size > 1 && _digits[size - 1] == 0)
	{
		if (_sign == 1)
			_digits.pop_back();
		else
			_digits.push_back(1);
	}
}

void BigInt::_trim()
{
	int size = _digits.size();
	size_t newSize = size;
	bool signIsPos = _sign == 1;
	bool signIsNeg = _sign == -1;
	for (int i = size - 1; i > 0 && (signIsPos && _digits[i] == 0 || signIsNeg && _digits[i] == 1 && _digits[i - 1] == 1); --i)
		--newSize;
	_digits.resize(newSize);
	if (_digits.size() == 1 && _digits[0] == 0)
		_sign = 1;
}

BigInt& BigInt::operator++()
{
	if (_sign == -1 && _digits.size() == 1 && _digits[0] == 1)
	{
		_sign = 1;
		_digits.resize(1);
		_digits[0] = 0;
		return *this;
	}
	_increment();
	return *this;
}

BigInt BigInt::operator++(int)
{
	BigInt tmp(*this);
	++(*this);
	return tmp;
}

BigInt& BigInt::operator--()
{
	if (_digits.size() == 1 && _digits[0] == 0)
	{
		_sign = -1;
		_digits[0] = 1;
		return *this;
	}
	_decrement();
	return *this;
}

BigInt BigInt::operator--(int)
{
	BigInt tmp(*this);
	--(*this);
	return tmp;
}

BigInt& operator+=(BigInt& a, const BigInt& b)
{
	if (a._sign == b._sign)
	{
		int aSize = a._digits.size();
		int bSize = b._digits.size();
		int maxSize = aSize > bSize ? aSize : bSize;
		a._digits.resize(maxSize + 2, a._sign == -1);
		for (int i = 0, carry = 0; i < maxSize || carry; ++i)
		{
			int summ = a._digits[i] + carry + (i < bSize ? b._digits[i] : b._sign == -1);
			carry = summ > 1;
			a._digits[i] = summ % 2;
			if (i >= maxSize)
			{
				int nextSumm = (a._sign == -1) + (carry + (b._sign == -1));
				if (summ == nextSumm)
				{
					a._sign = a._digits[i] == 1 ? -1 : 1;
					break;
				}
			}
		}
		a._trim();
	}
	else if (b != 0)
		a -= -b;
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
	if (a == b)
		a = 0;
	else if (a._sign == b._sign)
	{
		int aSize = a._digits.size();
		int bSize = b._digits.size();
		if (aSize >= bSize)
		{
			int maxSize = aSize > bSize ? aSize : bSize;
			for (int i = 0, carry = 0; i < maxSize || carry; ++i)
			{
				if (i == a._digits.size())
					a._digits.push_back(a._sign == -1);
				int summ = a._digits[i] - (carry + (i < bSize ? b._digits[i] : b._sign == -1));
				carry = summ < 0;
				a._digits[i] = summ % 2;
				if (i >= maxSize)
				{
					int nextSumm = (a._sign == -1) - (carry + (b._sign == -1));
					if (summ == nextSumm)
					{
						a._sign = a._digits[i] == 1 ? -1 : 1;
						break;
					}
				}
			}
		}
		else
		{
			a = b - a;
			a._sign = -a._sign;
		}
	}
	else
		a += -b;
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
	{
		temp = ~temp;
		++temp;
		temp._trim();
	}
	return temp;
}

BigInt& operator*=(BigInt& a, const BigInt& b)
{
	a = a * b;
	return a;
}

BigInt operator*(const BigInt& a, const BigInt& b)
{
	BigInt a1 = a._sign == 1 ? a : -a;
	BigInt b1 = b._sign == 1 ? b : -b;
	BigInt result;
	int aSize = a1._digits.size();
	int bSize = b1._digits.size();
	result._digits.resize(aSize + bSize);
	for (int i = 0; i < aSize; ++i)
	{
		if (a1._digits[i])
		{
			for (int j = 0, carry = 0; j < bSize || carry; ++j)
			{
				if (result._digits.size() == i + j)
					result._digits.push_back(0);
				int current = result._digits[i + j] + (int)a1._digits[i] * (j < bSize ? b1._digits[j] : 0) + carry;
				carry = current / 2;
				result._digits[i + j] = current % 2;
			}
		}
	}
	result._trim();
	return a._sign * b._sign == 1 ? result : -result;
}

BigInt& operator/=(BigInt& a, const BigInt& b)
{
	BigInt b1 = b;
	a = divmod(a, b1).first;
	return a;
}

BigInt operator/(const BigInt& a, const BigInt& b)
{
	BigInt a1 = a;
	BigInt b1 = b;
	return divmod(a1, b1).first;
}

BigInt& operator%=(BigInt& a, const BigInt& b)
{
	BigInt b1 = b;
	a = divmod(a, b1).second;
	return a;
}

BigInt operator%(const BigInt& a, const BigInt& b)
{
	BigInt a1 = a;
	BigInt b1 = b;
	return divmod(a1, b1).second;
}

BigInt& operator/=(BigInt& a, int v)
{
	int sign = a._sign;
	if (sign == -1)
		a = -a;
	int aSize = a._digits.size();
	for (int i = aSize - 1, reminder = 0; i >= 0; --i)
	{
		int current = a._digits[i] + reminder * 2;
		a._digits[i] = current / v;
		reminder = current % v;
	}
	a._trim();
	if (v * sign < 0)
		a = -a;
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
	BigInt a1 = a._sign == 1 ? a : -a;
	if (v < 0)
		v = -v;
	int m = 0;
	int aSize = a1._digits.size();
	for (int i = aSize - 1; i >= 0; --i)
		m = (a1._digits[i] + m * 2) % v;
	return m * a._sign;
}

BigInt& operator^=(BigInt& a, const BigInt& b)
{
	a = a ^ b;
	return a;
}

BigInt operator^(const BigInt& a, const BigInt& b)
{
	size_t aSize = a._digits.size();
	size_t bSize = b._digits.size();
	BigInt longerBinary = aSize > bSize ? a : b;
	const BigInt& shorterBinary = aSize > bSize ? b : a;
	size_t longerSize = longerBinary._digits.size();
	size_t shorterSize = shorterBinary._digits.size();

	for (size_t i = 0; i < shorterSize; ++i)
		longerBinary._digits[i] = longerBinary._digits[i] ^ shorterBinary._digits[i];

	const char filler = shorterBinary._sign == -1;

	for (size_t i = shorterSize; i < longerSize; ++i)
		longerBinary._digits[i] = longerBinary._digits[i] ^ filler;

	longerBinary._sign = a._sign * b._sign;

	return longerBinary;
}

BigInt& operator|=(BigInt& a, const BigInt& b)
{
	size_t aSize = a._digits.size();
	size_t bSize = b._digits.size();
	size_t shorterSize = aSize < bSize ? aSize : bSize;

	for (size_t i = 0; i < shorterSize; ++i)
		a._digits[i] |= b._digits[i];

	if (aSize < bSize)
	{
		a._digits.resize(bSize, a._sign == -1);
		for (size_t i = aSize; i < bSize; ++i)
			a._digits[i] |= b._digits[i];
	}
	else
	{
		const char filler = b._sign == -1;
		for (size_t i = bSize; i < aSize; ++i)
			a._digits[i] |= filler;
	}

	a._sign = a._sign == -1 || b._sign == -1 ? -1 : 1;

	return a;
}

BigInt operator|(const BigInt& a, const BigInt& b)
{
	BigInt a1 = a;
	a1 |= b;
	return a1;
}

BigInt& operator&=(BigInt& a, const BigInt& b)
{
	size_t aSize = a._digits.size();
	size_t bSize = b._digits.size();
	size_t shorterSize = aSize < bSize ? aSize : bSize;

	for (size_t i = 0; i < shorterSize; ++i)
		a._digits[i] &= b._digits[i];

	if (aSize < bSize)
	{
		a._digits.resize(bSize, a._sign == -1);
		for (size_t i = aSize; i < bSize; ++i)
			a._digits[i] &= b._digits[i];
	}
	else
	{
		const char filler = b._sign == -1;
		for (size_t i = bSize; i < aSize; ++i)
			a._digits[i] &= filler;
	}

	a._sign = a._sign == -1 && b._sign == -1 ? -1 : 1;

	return a;
}

BigInt operator&(const BigInt& a, const BigInt& b)
{
	BigInt a1 = a;
	a1 &= b;
	return a1;
}

BigInt& operator>>=(BigInt& a, size_t shift)
{
	int aSize = a._digits.size();
	if (shift > aSize)
		return a = 0;

	for (size_t i = 0; i < aSize - shift; ++i)
		a._digits[i] = a._digits[i + shift];

	for (size_t i = aSize - shift; i < aSize; ++i)
		a._digits[i] = a._sign == -1;

	a._trim();

	return a;
}

BigInt operator>>(const BigInt& a, size_t shift)
{
	BigInt temp = a;
	temp >>= shift;
	return temp;
}

BigInt& operator<<=(BigInt& a, size_t shift)
{
	if (a._digits.size() == 1 && a._digits[0] == 0)
		return a = 0;
	if (shift == 0)
		return a;

	size_t aSize = a._digits.size() + shift;
	a._digits.resize(aSize);
	for (size_t i = aSize - 1; i >= shift; --i)
		a._digits[i] = a._digits[i - shift];

	for (size_t i = 0; i < shift; ++i)
		a._digits[i] = 0;

	return a;
}

BigInt operator<<(const BigInt& a, size_t shift)
{
	BigInt temp = a;
	temp <<= shift;
	return temp;
}

BigInt operator~(const BigInt& a)
{
	BigInt a1(a);
	if (a1._sign == 1 && (a1._digits.size() != 1 || a1._digits[0] != 0))
		a1._digits.push_back(0);
	size_t aSize = a1._digits.size();

	for (size_t i = 0; i < aSize; ++i)
		a1._digits[i] = !a1._digits[i];

	a1._sign = -a1._sign;

	a1._trim();
	return a1;
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
	if (a._sign != b._sign)
		return a._sign < b._sign;

	int bSize = b._digits.size();
	if (a._digits.size() != bSize)
		return a._digits.size() * a._sign < b._digits.size() * b._sign;

	for (int i = bSize - 1; i >= 0; --i)
		if (a._digits[i] != b._digits[i])
			return a._digits[i] < b._digits[i];

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
	out << to_string(bi);
	return out;
}

std::pair<BigInt, BigInt> divmod(const BigInt& a, const BigInt& b)
{
	int norm = 2 / (b._digits.back() + 1);
	BigInt a1((a ._sign == 1 ? a : -a) * norm);
	BigInt b1((b._sign == 1 ? b : -b) * norm);
	BigInt q;
	BigInt r;
	q._digits.resize(a1._digits.size());

	int aSize = a1._digits.size();
	for (int i = aSize - 1; i >= 0; --i)
	{
		r *= 2;
		r += (int)a1._digits[i];
		int s1 = b1._digits.size() < r._digits.size() ? r._digits[b1._digits.size()] : 0;
		int s2 = b1._digits.size() - 1 < r._digits.size() ? r._digits[b1._digits.size() - 1] : 0;
		int d = (s1 * 2 + s2) / b1._digits.back();
		r -= b1 * d;
		while (r._sign == -1)
		{
			r += b1;
			--d;
		}
		q._digits[i] = d;
	}

	if (a._sign * b._sign == -1)
		q = -q;
	if (a._sign == -1)
		r = -r;
	q._trim();
	r._trim();
	return { q, r / norm };
}

static void _plusBase10(std::string& s1, const std::string& s2)
{
	size_t size = s2.size();
	bool reminder = false;
	for (size_t i = 0; i < size || reminder; ++i)
	{
		if (s1.size() <= i)
			s1.push_back(0);
		s1[i] += reminder + (i < s2.size() ? s2[i] : 0);
		reminder = 0;
		if (s1[i] >= 10)
		{
			reminder = 1;
			s1[i] -= 10;
		}
	}
}

std::string to_string(const BigInt& a)
{
	std::string temp("\1");
	std::string result;
	result.push_back(0);
	BigInt a1 = a._sign == 1 ? a : ++(~a);
	size_t size = a1._digits.size();

	for (size_t i = 0; i < size; ++i)
	{
		if (a1._digits[i])
			_plusBase10(result, temp);
		_multiplyBase10by2(temp);
	}

	size = result.size();
	for (size_t i = 0; i < size / 2; ++i)
	{
		char temp = result[i] + '0';
		result[i] = result[size - i - 1] + '0';
		result[size - i - 1] = temp;
	}
	if (size % 2)
		result[size / 2] += '0';

	if (a._sign == -1)
		result = "-" + result;

	return result;
}

long long BigInt::getLL() const
{
	long long result = 0;
	BigInt a = _sign == 1 ? *this : -(*this);
	int size = a._digits.size();

	for (int i = size - 1; i >= 0; --i)
		result = result * 2 + a._digits[i];

	return result * _sign;
}
