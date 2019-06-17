// exception classes for various error types
#ifndef Xcept_
#define Xcept_

#include <string>

using namespace std;

// bad initializers
class BadInitializers
{
public:
	BadInitializers()
	{
	}
};

// insufficient memory
class NoMem
{
public:
	NoMem()
	{
	}
};

// improper array, find, insert, or delete index
// or deletion from empty structure
class OutOfBounds
{
public:
	OutOfBounds()
	{
	}
};

// use when operands should have matching size
class SizeMismatch
{
public:
	SizeMismatch()
	{
	}
};

// use when zero was expected
class MustBeZero
{
public:
	MustBeZero()
	{
	}
};

// use when zero was expected
class BadInput
{
public:
	BadInput()
	{
	}
};

class vendingMachineError
{
public:
	vendingMachineError(string const& s)
	{
		sE = s;
	}
	virtual ~vendingMachineError()
	{
	}
	string const& getErrorString() const
	{
		return sE;
	}
protected:
	string sE;
};

#endif
