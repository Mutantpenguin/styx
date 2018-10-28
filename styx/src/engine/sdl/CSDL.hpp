#pragma once

#include <exception>

class CSDL final
{
public:
	CSDL();
	~CSDL();

	class Exception: public std::exception
	{
	public:
		explicit Exception( void ) {}

		virtual ~Exception() throw() {}
	};
};
