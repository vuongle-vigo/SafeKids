#include "ComputerInfo.h"
#include "common.cpp"

ComputerInfo::ComputerInfo() {
	if (!GetSerialNumber()) {}
}

ComputerInfo::~ComputerInfo() {

}

bool ComputerInfo::GetSerialNumber() {
	HANDLE hFile = CreateFileW(L"\\\\.\\PhysicalDrive0", 0, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
	if (hFile == INVALID_HANDLE_VALUE) {
		PRINT_API_ERR("CreateFileW");
		return false;
	} 

	STORAGE_PROPERTY_QUERY storagePropertyQuery = {};
	storagePropertyQuery.PropertyId = StorageDeviceProperty;
	storagePropertyQuery.QueryType = PropertyStandardQuery;
	STORAGE_DESCRIPTOR_HEADER storageDescriptorHeader = {};
	DWORD dwBytesReturned = 0;
	if (!DeviceIoControl(hFile, IOCTL_STORAGE_QUERY_PROPERTY, &storagePropertyQuery, sizeof(STORAGE_PROPERTY_QUERY),
		&storageDescriptorHeader, sizeof(STORAGE_DESCRIPTOR_HEADER), &dwBytesReturned, nullptr)) {
		PRINT_API_ERR("DeviceIoControl");
		CloseHandle(hFile);
		return false;
	}

	const DWORD dwOutBufferSize = storageDescriptorHeader.Size;
	if (!dwOutBufferSize) {
		return false;
	}

	BYTE* pbOutBuffer = new BYTE[dwOutBufferSize];
	if (!DeviceIoControl(hFile, IOCTL_STORAGE_QUERY_PROPERTY, &storagePropertyQuery, sizeof(STORAGE_PROPERTY_QUERY),
		pbOutBuffer, dwOutBufferSize, &dwBytesReturned, nullptr)) {
		if (pbOutBuffer) {
			delete[] pbOutBuffer;
		}

		CloseHandle(hFile);
		return false;
	}

	PSTORAGE_DEVICE_DESCRIPTOR pStorageDeviceDescriptor = (PSTORAGE_DEVICE_DESCRIPTOR)pbOutBuffer;
	const DWORD dwSerialNumberOffset = pStorageDeviceDescriptor->SerialNumberOffset;
	if (!dwSerialNumberOffset) {
		if (pbOutBuffer) {
			delete[] pbOutBuffer;
		}

		CloseHandle(hFile);
		return false;
	}

	const char* serialNumber = (char*)pbOutBuffer + dwSerialNumberOffset;
	m_szSerial = serialNumber;
	m_szSerial.erase(std::remove(m_szSerial.begin(), m_szSerial.end(), ' '), m_szSerial.end());

	return true;
}