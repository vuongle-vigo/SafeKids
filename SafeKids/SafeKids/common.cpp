#include <Windows.h>
#include <iostream>

#define PRINT_API_ERR(API_NAME) \
	std::cout << API_NAME << ": " << GetLastError() << std::endl;