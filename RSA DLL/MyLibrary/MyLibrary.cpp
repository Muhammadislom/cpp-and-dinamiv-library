// MyLibrary.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#define LIBOLL extern "C" __declspec(dllexport)

LIBOLL float sum(int a, int b) 
{
	return a + b;
}

LIBOLL float mult(int a, int b)
{
	return a - b;
}
LIBOLL float multiplication(int a, int b)
{
	return a * b;
}
LIBOLL float division(int a, int b)
{
	return a / b;
}

LIBOLL void setMessage()
{
	int msgboxID = MessageBox(
		NULL,
		(LPCWSTR)L"DLL from Python",
		(LPCWSTR)L"Python tutorials",
		MB_ICONWARNING | MB_CANCELTRYCONTINUE | MB_DEFBUTTON2
		);
}

