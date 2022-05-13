#include <iostream>
#include <stdio.h>
#include <windows.h>
#include <excpt.h>

void InitFirstTask() {
	std::cout << "[First task start]" << std::endl;

	__try
	{
		int* p = 0x00000000;   // pointer to NULL
		*p = 13;    // causes an access violation exception;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		int code = GetExceptionCode();

		std::cout << "Exception code: " << code << std::endl;

		if (code == EXCEPTION_ACCESS_VIOLATION)
		{
			std::cout << "Caught the expected EXCEPTION_ACCESS_VIOLATION exception!" << std::endl;
		}
		else {
			std::cout << "Ooops. Something went wrong, that's a diff exception!" << std::endl;
		}
	}

	std::cout << "[First task end]\n";
}

void InitSecondTask() {
	std::cout << "[Second task start]" << std::endl;

	__try
	{
		int a = 1; //Doubles won't generate exception! Will just return inf or NaN
		int b = 0;
		double c = a / b;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		int code = GetExceptionCode();

		std::cout << "Exception code: " << code << std::endl;

		if (code == EXCEPTION_INT_DIVIDE_BY_ZERO)
		{
			std::cout << "Caught the expected EXCEPTION_INT_DIVIDE_BY_ZERO exception!" << std::endl;
		}
		else {
			std::cout << "Ooops. Something went wrong, that's a diff exception!" << std::endl;
		}
	}

	std::cout << "[Second task end]" << std::endl;
}


void InitThirdTask() {
	std::cout << "[Third task start]" << std::endl;

	try
	{
		int* p = 0x00000000;   // pointer to NULL
		*p = 13;    // causes an access violation exception;
	}
	catch (const std::runtime_error& re)
	{
		// speciffic handling for runtime_error
		std::cout << "Runtime error: " << re.what() << std::endl;
	}
	catch (const std::exception& ex)
	{
		// speciffic handling for all exceptions extending std::exception, except
		// std::runtime_error which is handled explicitly
		std::cout << "Error occurred: " << ex.what() << std::endl;
	}
	catch (...)
	{
		// catch any other errors (that we have no information about)
		std::cout << "Unknown failure occurred. Possible memory corruption" << std::endl;
	}


	std::cout << "[Third task end]" << std::endl;
}

int main()
{
	InitFirstTask();
	InitSecondTask();

	__try
	{
		InitThirdTask();
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		int code = GetExceptionCode();

		std::cout << "Exception code: " << code << std::endl;
		std::cout << "This exception was not caught by try-catch block, the program still fails! That's why we have SEH :D" << std::endl;
	}

	std::cout << "[All tasks failed successfully]" << std::endl;
}