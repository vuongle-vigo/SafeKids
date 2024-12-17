#include "ComputerInfo.h"
#include "common.cpp"

ComputerInfo::ComputerInfo() {
	if (!SetMachineGUID()) {}
}

ComputerInfo::~ComputerInfo() {

}

ComputerInfo& ComputerInfo::GetInstance() {
	static ComputerInfo instance;
	return instance;
}

bool ComputerInfo::SetMachineGUID() {
	HKEY hKey;
	const char* subkey = "SOFTWARE\\Microsoft\\Cryptography";
	const char* value = "MachineGuid";

	if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, subkey, 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
		return false;
	}

	char buffer[128];
	DWORD bufferSize = sizeof(buffer);
	if (RegQueryValueExA(hKey, value, NULL, NULL, (LPBYTE)buffer, &bufferSize) != ERROR_SUCCESS) {
		RegCloseKey(hKey);
		return false;
	}

	RegCloseKey(hKey);
	m_szMachineGUID = std::string(buffer);

	return true;
}

std::string ComputerInfo::GetMachineGUID() {
	return m_szMachineGUID;
}