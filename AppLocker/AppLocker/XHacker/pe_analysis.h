#pragma once
#include "tools.h"

namespace lxutil
{
	const int READ_BUFFER_ERROR = 0;
	const int LOAD_STRUCT_ERROR = 1;

	template<class _NtHeaderT, class _OptHeaderT>
	class PEAnalyser
	{
	public:
		using _IMAGE_NT_HEADERS = _NtHeaderT;
		using _IMAGE_OPTIONAL_HEADER = _OptHeaderT;
		using _PIMAGE_NT_HEADERS = _NtHeaderT*;
		using _PIMAGE_OPTIONAL_HEADER = _OptHeaderT*;

		PEAnalyser() = delete;
		PEAnalyser(const PEAnalyser&) = delete;
		PEAnalyser& operator=(const PEAnalyser&) = delete;
		PEAnalyser& operator=(const PEAnalyser&&) = delete;

		inline PEAnalyser(PEAnalyser&& pe)
		{
			pe.__bAutoFree = false;
			_lpBuffer = pe._lpBuffer;
			_lpszPath = pe._lpszPath;
			_dwFileSize = pe._dwFileSize;
			__bAutoFree = true;
			__loadStructs();
		}
		inline PEAnalyser(LPCSTR _lpszPath) :
			__bAutoFree(true),
			_lpBuffer(nullptr)
		{
			this->_lpszPath = (LPSTR)malloc(strlen(_lpszPath) + 1);
			memcpy(this->_lpszPath, _lpszPath, strlen(_lpszPath) + 1);
			__readBuffer();
			__loadStructs();
		}
		inline PEAnalyser(LPVOID _lpBuffer, DWORD _dwBufSize) :
			__bAutoFree(true),
			_lpszPath(nullptr)
		{
			this->_lpBuffer = (LPSTR)_lpBuffer;
			_dwFileSize = _dwBufSize;
			__loadStructs();
		}
		inline ~PEAnalyser() { if (__bAutoFree) release(); }

		PIMAGE_DOS_HEADER lpDosHeader;
		_PIMAGE_NT_HEADERS lpNtHeader;
		PIMAGE_FILE_HEADER lpFileHeader;
		_PIMAGE_OPTIONAL_HEADER lpOptHeader;
		PIMAGE_SECTION_HEADER lpFirstScnHeader;
		PIMAGE_SECTION_HEADER lpLastScnHeader;

		inline LPSTR rdbuf() noexcept { return _lpBuffer; }
		inline LPCSTR rdbuf() const noexcept { return _lpBuffer; }
		inline DWORD fileSize() const noexcept { return _dwFileSize; }
		inline LPSTR filePath() const noexcept { return _lpszPath; }

		inline bool release()
		{
			if (_lpszPath)
			{
				free(_lpszPath);
				_lpszPath = nullptr;
			}
			if (_lpBuffer)
			{
				free(_lpBuffer);
				_lpBuffer = nullptr;
				return true;
			}
			return false;
		}

		LPSTR expandLastScn(DWORD _dwSize, LPDWORD _lpdwRealSize = nullptr)
		{
			if (static_cast<int>(_dwSize) <= 0)
			{
				if (_lpdwRealSize) *_lpdwRealSize = 0;
				LPSTR buf = (LPSTR)malloc(_dwFileSize);
				if (!buf) return nullptr;
				memcpy(buf, _lpBuffer, _dwFileSize);
				return buf;
			}
			DWORD realSize = AlignValue(_dwSize, lpOptHeader->FileAlignment);
			lpLastScnHeader->SizeOfRawData += realSize;
			lpOptHeader->SizeOfImage += AlignValue(_dwSize, lpOptHeader->SectionAlignment);
			LPSTR newBuffer = (LPSTR)malloc(_dwFileSize + realSize);
			if (!newBuffer) return nullptr;
			memset(newBuffer + _dwFileSize, 0, realSize);
			memcpy(newBuffer, _lpBuffer, _dwFileSize);
			if (_lpdwRealSize) *_lpdwRealSize = realSize;
			return newBuffer;
		}

		DWORD RVAToFOA(DWORD _dwRVA) const noexcept
		{
			for (size_t i = 0; i < lpFileHeader->NumberOfSections; i++)
			{
				auto cur_scn = lpFirstScnHeader + i;
				DWORD end_rva{};
				if (i != lpFileHeader->NumberOfSections - 1)
					end_rva = (cur_scn + 1)->VirtualAddress;
				else
					end_rva = cur_scn->VirtualAddress + cur_scn->SizeOfRawData;

				if (_dwRVA >= cur_scn->VirtualAddress &&
					_dwRVA < end_rva)
				{
					return _dwRVA - cur_scn->VirtualAddress + cur_scn->PointerToRawData;
				}
			}
			return _dwRVA;
		}

		DWORD FOAToRVA(DWORD _dwFOA) const noexcept
		{
			for (size_t i = 0; i < lpFileHeader->NumberOfSections; i++)
			{
				auto cur_scn = lpFirstScnHeader + i;
				DWORD end_rva{};
				if (i != lpFileHeader->NumberOfSections - 1)
					end_rva = (cur_scn + 1)->PointerToRawData;
				else
					end_rva = cur_scn->PointerToRawData + cur_scn->SizeOfRawData;

				if (_dwFOA >= cur_scn->PointerToRawData &&
					_dwFOA < end_rva)
				{
					return _dwFOA - cur_scn->PointerToRawData + cur_scn->VirtualAddress;
				}
			}
			return _dwFOA;
		}

		LPSTR fileBufToImageBuf()
		{
			LPSTR newBuffer = (LPSTR)malloc(lpOptHeader->SizeOfImage);
			memset(newBuffer, 0, lpOptHeader->SizeOfImage);
			memcpy(newBuffer, _lpBuffer, lpOptHeader->SizeOfHeaders);
			for (size_t i = 0; i < lpFileHeader->NumberOfSections; i++)
			{
				memcpy(
					newBuffer + (lpFirstScnHeader + i)->VirtualAddress,
					_lpBuffer + (lpFirstScnHeader + i)->PointerToRawData,
					(lpFirstScnHeader + i)->SizeOfRawData
				);
			}
			return newBuffer;
		}

		inline bool isExe() const noexcept
		{
			return IsContainFlags(lpFileHeader->Characteristics, IMAGE_FILE_EXECUTABLE_IMAGE);
		}

		inline bool is32() const noexcept
		{
			return lpOptHeader->Magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC;
		}

	protected:
		LPSTR _lpszPath;
		LPSTR _lpBuffer;
		DWORD _dwFileSize;

	private:
		int __bAutoFree;

		void __readBuffer()
		{
			_lpBuffer = ReadBuffer(_lpszPath, &_dwFileSize);
			if (!_lpBuffer) throw READ_BUFFER_ERROR;
		}

		void __loadStructs()
		{
			lpDosHeader = (PIMAGE_DOS_HEADER)_lpBuffer;
			if (lpDosHeader->e_magic != IMAGE_DOS_SIGNATURE) throw LOAD_STRUCT_ERROR;
			lpNtHeader = (_PIMAGE_NT_HEADERS)(_lpBuffer + lpDosHeader->e_lfanew);
			lpFileHeader = &lpNtHeader->FileHeader;
			lpOptHeader = &lpNtHeader->OptionalHeader;
			if (lpNtHeader->Signature != IMAGE_NT_SIGNATURE) throw LOAD_STRUCT_ERROR;
			lpFirstScnHeader = (PIMAGE_SECTION_HEADER)(_lpBuffer + lpDosHeader->e_lfanew + sizeof _IMAGE_NT_HEADERS);
			lpLastScnHeader = lpFirstScnHeader + lpFileHeader->NumberOfSections - 1;
		}
	};


	using PEAnalyser32 = PEAnalyser<IMAGE_NT_HEADERS32, IMAGE_OPTIONAL_HEADER32>;
	using PEAnalyser64 = PEAnalyser<IMAGE_NT_HEADERS64, IMAGE_OPTIONAL_HEADER64>;
}