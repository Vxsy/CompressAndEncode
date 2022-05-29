#include <iostream>
#include "stdafx.h"

#pragma comment(lib, "ws2_32")
#pragma comment(lib, "crypt32.lib") 
#pragma comment(lib, "Cabinet.lib")

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <Windows.h>
#include <string>
#include <compressapi.h>
#include <wincrypt.h>
#include <vector>
#include <fstream>
#include <cstddef>

using namespace std;


LPWSTR compressAndBase64Encode(LPCWSTR lpEncFile) 
{
	COMPRESSOR_HANDLE compressor = NULL;
	PBYTE fileBuffer = NULL;
	PBYTE compressedBuffer = NULL;
	PBYTE encodedBuffer = NULL;
	SIZE_T fileBufferSize, fileDataSize, compressedBufferSize, compressedDataSize;
	DWORD BytesRead;
	BOOL bErrorFlag = FALSE;
	LPWSTR outputBuffer = NULL;

	//File handle
	HANDLE encFileHandle = CreateFile(lpEncFile,
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (encFileHandle == INVALID_HANDLE_VALUE) {
		wprintf(L"File handle error! GLE: % d\n", GetLastError());
		return FALSE;
	}

	//Allocate Memory 
	fileDataSize = GetFileSize(encFileHandle, NULL);
	fileBuffer = new BYTE[fileDataSize + 1];

	bErrorFlag = ReadFile(encFileHandle, fileBuffer, fileDataSize, &BytesRead, NULL);
	if (FALSE == bErrorFlag) {
		CloseHandle(encFileHandle);
		return FALSE;
	}
	if (!CreateCompressor(
		COMPRESS_ALGORITHM_LZMS,
		NULL,
		&compressor
	)) {
		wprintf(L"Compressor creation error!");
		return FALSE;
	}

	if (!Compress(
		compressor,
		fileBuffer,
		fileDataSize,
		NULL,
		0,
		&compressedBufferSize)) {
		DWORD ErrorCode = GetLastError();

		if (ErrorCode != ERROR_INSUFFICIENT_BUFFER) {
			wprintf(L"Compression buffer determination error!");
			return FALSE;
		}

		compressedBuffer = (PBYTE)malloc(compressedBufferSize);
		if (!compressedBuffer) {
			wprintf(L"Memory allocation error!");
			return FALSE;
		}
	}

	if (!Compress(
		compressor,
		fileBuffer,
		fileDataSize,
		compressedBuffer,
		compressedBufferSize,
		&compressedDataSize)) {
		wprintf(L"Compression process error!");
		return FALSE;
	}

	// Base64 encode the compressed buffer
	DWORD dwSize = 0;
	if (!CryptBinaryToString(compressedBuffer, compressedDataSize, CRYPT_STRING_BASE64, NULL, &dwSize)) {
		wprintf(L"Determining the size of the outputBuffer error!");
		return FALSE;
	}
	dwSize++;
	outputBuffer = new TCHAR[dwSize];
	if (!CryptBinaryToString(compressedBuffer, compressedDataSize, CRYPT_STRING_BASE64, outputBuffer, &dwSize)) {
		wprintf(L"Base64 encoding process error!");
		return FALSE;
	}

	CloseHandle(encFileHandle);
	return outputBuffer;
}

void main()
{
	string input;
	ifstream infile;

	std::wstring filename(L"FULL PATH HERE");
	std::wcout << compressAndBase64Encode(filename.c_str());
}