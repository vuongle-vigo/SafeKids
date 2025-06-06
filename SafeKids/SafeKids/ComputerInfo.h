#pragma once
#include <atlstr.h>
#include <windows.h>
#include <memory>
#include <string>
#include <iphlpapi.h>
#include <stdio.h>
#include <atlstr.h>
#include <comdef.h>
#include <Wbemidl.h>
#include <MMSystem.h>

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "IPHLPAPI.lib")

class ComputerInfo {
public:
	ComputerInfo();
	~ComputerInfo();
	static ComputerInfo& GetInstance();
	bool SetMachineGUID();
	std::string GetMachineGUID();
	std::string GetDesktopName();
private:
	std::string m_szMachineGUID;
};