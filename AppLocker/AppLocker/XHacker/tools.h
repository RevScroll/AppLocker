#pragma once
#include <Windows.h>
#include <fstream>
#include <string>

namespace lxutil
{
	inline DWORD AlignValue(DWORD _dwValue, DWORD _dwAlign) noexcept
	{
		if (_dwValue % _dwAlign == 0) return _dwValue;
		DWORD temp = _dwValue + _dwAlign;
		return temp - (temp % _dwAlign);
	}

	inline bool IsContainFlags(DWORD a, DWORD b) noexcept { return (a & b) == b; }

	inline LPSTR ReadBuffer(LPCSTR _lpszPath, LPDWORD _lpdwBufSize)
	{
		std::ifstream in(_lpszPath, std::ios::binary | std::ios::in);
		if (!in) return nullptr;
		in.seekg(0, std::ios::end);
		DWORD size = static_cast<DWORD>(in.tellg());
		if (size == 0) return nullptr;
		in.seekg(0, std::ios::beg);
		LPSTR buffer = (LPSTR)malloc(size);
		if (!buffer) return nullptr;
		in.read(buffer, size);
		*_lpdwBufSize = size;
		return (LPSTR)buffer;
	}

	inline bool SaveBuffer(LPCSTR _lpszPath, LPCVOID _lpBuffer, DWORD _dwBufSize)
	{
		std::ofstream out(_lpszPath, std::ios::out | std::ios::binary);
		if (!out) return false;
		out.write((LPCSTR)_lpBuffer, _dwBufSize);
		return true;
	}

	inline PIMAGE_DOS_HEADER GetDosHeader(LPCVOID _lpBuffer) noexcept
	{
		if (!_lpBuffer) return nullptr;
		auto lpDos = (PIMAGE_DOS_HEADER)_lpBuffer;
		return lpDos->e_magic == IMAGE_DOS_SIGNATURE ? lpDos : nullptr;
	}

	inline PIMAGE_NT_HEADERS32 GetNtHeader32(LPCVOID _lpBuffer) noexcept
	{
		auto lpDos = GetDosHeader(_lpBuffer);
		if (!lpDos) return nullptr;
		auto lpNt = (PIMAGE_NT_HEADERS32)((intptr_t)_lpBuffer + lpDos->e_lfanew);
		return lpNt->Signature == IMAGE_NT_SIGNATURE ? lpNt : nullptr;
	}

	inline PIMAGE_NT_HEADERS64 GetNtHeader64(LPCVOID _lpBuffer) noexcept
	{
		auto lpDos = GetDosHeader(_lpBuffer);
		if (!lpDos) return nullptr;
		auto lpNt = (PIMAGE_NT_HEADERS64)((intptr_t)_lpBuffer + lpDos->e_lfanew);
		return lpNt->Signature == IMAGE_NT_SIGNATURE ? lpNt : nullptr;
	}

	inline WORD GetMagic(LPCVOID _lpBuffer) noexcept
	{
		
		auto lpNt = GetNtHeader32(_lpBuffer);
		if (!lpNt) return -1;
		return lpNt->OptionalHeader.Magic;
	}

	inline bool Is32BitSoftware(LPCVOID _lpBuffer) noexcept
	{
		WORD magic = GetMagic(_lpBuffer);
		if (magic == -1) return false;
		return magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC;
	}

	inline bool IsExecutable(LPCVOID _lpBuffer) noexcept
	{
		auto lpNtHeader = GetNtHeader32(_lpBuffer);
		if (!lpNtHeader) return false;
		return IsContainFlags(lpNtHeader->FileHeader.Characteristics, IMAGE_FILE_EXECUTABLE_IMAGE);
	}
}