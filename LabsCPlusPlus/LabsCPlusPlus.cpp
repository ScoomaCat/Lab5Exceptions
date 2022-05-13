#include <iostream>
#include <stdio.h>
#include <windows.h>
#include <excpt.h>
#include <eh.h>
#include <exception>

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

//Floats won't generate exception without calling EnableFloatExeptions(). Will just return inf or NaN
//For more info about this topic read docs at https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/controlfp-s?view=msvc-170
void EnableFloatExeptions(unsigned int newControlWord)
{
	_clearfp();
	_controlfp_s(0, newControlWord, _MCW_EM);
}

void InitSecondTask() {
	std::cout << "[Second task start]" << std::endl;

	__try
	{
		unsigned int oldControlWord;
		_controlfp_s(&oldControlWord, 0, 0);
		EnableFloatExeptions(oldControlWord & ~_EM_ZERODIVIDE | _EM_OVERFLOW);

		double a = 1;
		double b = 0;
		double c = a / b; 
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		int code = GetExceptionCode();

		std::cout << "Exception code: " << code << std::endl;

		if (code == EXCEPTION_FLT_DIVIDE_BY_ZERO)
		{
			std::cout << "Caught the expected EXCEPTION_FLT_DIVIDE_BY_ZERO exception!" << std::endl;
		}
		else {
			std::cout << "Ooops. Something went wrong, that's a diff exception!" << std::endl;
		}
	}

	std::cout << "[Second task end]" << std::endl;
}

class SE_Exception : public std::exception
{
private:
	const unsigned int nSE;
public:
	SE_Exception() noexcept : SE_Exception{ 0 } {}
	SE_Exception(unsigned int n) noexcept : nSE{ n } {}
	unsigned int getSeNumber() const noexcept { return nSE; }
};

class Scoped_SE_Translator
{
private:
	const _se_translator_function old_SE_translator;
public:
	Scoped_SE_Translator(_se_translator_function new_SE_translator) noexcept
		: old_SE_translator{ _set_se_translator(new_SE_translator) } {}
	~Scoped_SE_Translator() noexcept { _set_se_translator(old_SE_translator); }
};

void trans_func(unsigned int u, EXCEPTION_POINTERS*)
{
	throw SE_Exception(u);
}

//[[[!!!!!WARNING!!!!!]]]] 
//Check your project properties before executing this code!
//Go to Project Properties -> Configuration Properties -> C/C++ and set Enable C++ Exceptions to /EHa
//This code will NOT work otherwise! 
//For more info about this topic read docs at https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/set-se-translator?view=msvc-170#example-catch-__try-exception-error
void InitThirdTask()
{
	Scoped_SE_Translator scoped_se_translator{ trans_func };
	std::cout << "[Third task start]" << std::endl;

	try
	{
		int* p = 0x00000000;
		*p = 13;
	}
	catch (const SE_Exception& e)
	{
		printf("Caught the expected exception, error %8.8x.\n", e.getSeNumber());
	}
	catch (...)
	{
		std::cout << "Unknown failure occurred." << std::endl;
	}

	std::cout << "[Third task end]" << std::endl;
}

int main()
{
	InitFirstTask();
	InitSecondTask();
	InitThirdTask();

	std::cout << "[All tasks failed successfully]" << std::endl;
}