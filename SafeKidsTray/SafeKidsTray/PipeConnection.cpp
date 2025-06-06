#include "PipeConnection.h"
#include <windows.h>
#include <iostream>


PipeConnection::PipeConnection() {
	hPipe = INVALID_HANDLE_VALUE;
}

PipeConnection::~PipeConnection() {
	if (hPipe != INVALID_HANDLE_VALUE) {
		CloseHandle(hPipe);
	}
}

PipeConnection& PipeConnection::GetInstance()
{
	static PipeConnection instance;
	return instance;
}

bool PipeConnection::SendMessageToServer(const std::wstring& message) {
	if (hPipe == INVALID_HANDLE_VALUE) {
		return false; // Pipe not initialized
	}

	DWORD bytesWritten;
	BOOL success = WriteFile(
		hPipe,                   // handle to pipe 
		message.c_str(),        // buffer to write from 
		(DWORD)(message.size() * sizeof(wchar_t)), // size of buffer 
		&bytesWritten,          // number of bytes written 
		NULL);                  // not overlapped I/O 
	if (!success) {
		std::cout << "Failed to write to pipe. Error: " << GetLastError() << std::endl;
		return false; // Write failed
	}
	return success && (bytesWritten == message.size() * sizeof(wchar_t));
}

bool PipeConnection::InitPipe()
{
	__TRY:
	hPipe = CreateFileW(
		PIPE_NAME,                // pipe name 
		GENERIC_READ | GENERIC_WRITE, // read and write access 
		0,                          // no sharing 
		NULL,                       // default security attributes
		OPEN_EXISTING,              // opens existing pipe
		FILE_ATTRIBUTE_NORMAL,      // default attributes 
		NULL);                      // no template file

	if (hPipe == INVALID_HANDLE_VALUE)
	{
		std::cout << "Failed to connect to pipe. Error: " << GetLastError() << std::endl;
		Sleep(1000); // Wait a bit before retrying
		goto __TRY; // Retry connecting to the pipe
	}

	wchar_t response[256] = { 0 };
	DWORD bytesRead = 0;
	BOOL success;
	while (true) {
		DWORD availableBytes = 0;
		if (!PeekNamedPipe(hPipe, NULL, 0, NULL, &availableBytes, NULL)) {
			std::wcout << L"PeekNamedPipe failed. Error: " << GetLastError() << std::endl;
			break;
		}

		if (availableBytes == 0) {
			Sleep(100); 
			continue;
		}

		success = ReadFile(
			hPipe,
			response,
			sizeof(response) - sizeof(wchar_t),
			&bytesRead,
			NULL);

		if (!success || bytesRead == 0) {
			std::wcout << L"ReadFile failed or client disconnected. Error: " << GetLastError() << std::endl;
			break;
		}

		response[bytesRead / sizeof(wchar_t)] = L'\0';
		MessageBoxW(NULL, response, L"Pipe Message", MB_OK | MB_ICONINFORMATION);
	}


	CloseHandle(hPipe); // Close the pipe handle
}