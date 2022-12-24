#pragma once

#include "pe_analysis.h"

#define XHACKER_INJECTEDINFO_SIGNATURE 0x4c584e42

namespace lxutil
{
	const int NO_INJECTED_CODE_ERROR = 1000;
	const int LAST_SCN_CHANGED_ERROR = 1001;

	typedef struct _InjectedInfo
	{
		DWORD Signature;
		DWORD OrigiPTST;
		DWORD CodeFOA;
		DWORD OrigiOEP;
		DWORD ExpandedSpace;
		DWORD OrigiSizeOfImage;
		DWORD CodeSize;
		IMAGE_SECTION_HEADER OrigiLastScn;
	}InjectedInfo, * PInjectedInfo;

	template<class _NtHeaderT, class _OptHeaderT>
	class XHacker : public PEAnalyser<_NtHeaderT, _OptHeaderT>
	{
	public:

		inline XHacker(LPCSTR _lpszPath) : PEAnalyser<_NtHeaderT, _OptHeaderT>(_lpszPath)
		{
		}
		inline XHacker(LPSTR _lpBuffer, DWORD _dwBufSize) : PEAnalyser<_NtHeaderT, _OptHeaderT>(_lpBuffer, _dwBufSize)
		{
		}
		inline ~XHacker() {  }


		PEAnalyser<_NtHeaderT, _OptHeaderT> injectEntryCode(
			LPVOID _lpCodeBuf,
			DWORD _dwCodeSize,
			DWORD _dwJmpBackCmdOffset,
			DWORD _InjectedCodeOEP)
		{
			InjectedInfo info{};
			info.Signature = XHACKER_INJECTEDINFO_SIGNATURE;
			info.CodeFOA = this->lpLastScnHeader->PointerToRawData + this->lpLastScnHeader->Misc.VirtualSize;
			info.CodeSize = _dwCodeSize;
			info.OrigiOEP = this->lpOptHeader->AddressOfEntryPoint;
			info.OrigiLastScn = *this->lpLastScnHeader;
			info.OrigiPTST = this->lpFileHeader->PointerToSymbolTable;
			info.OrigiSizeOfImage = this->lpOptHeader->SizeOfImage;

			this->lpLastScnHeader->Characteristics |= IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE | IMAGE_SCN_MEM_EXECUTE;
			DWORD origiFreeSize = this->lpLastScnHeader->SizeOfRawData - this->lpLastScnHeader->Misc.VirtualSize;
			auto newBuffer = this->expandLastScn(_dwCodeSize + sizeof InjectedInfo - origiFreeSize, &info.ExpandedSpace);


			PEAnalyser<_NtHeaderT, _OptHeaderT> pe{ newBuffer, this->_dwFileSize + info.ExpandedSpace };
			LPSTR injectPos = pe.rdbuf() + pe.lpLastScnHeader->Misc.VirtualSize + pe.lpLastScnHeader->PointerToRawData;
			DWORD offset1 = injectPos + _dwJmpBackCmdOffset + 5 - pe.rdbuf();
			offset1 = this->FOAToRVA(offset1);
			DWORD offset2 = pe.lpOptHeader->AddressOfEntryPoint - offset1;
			memcpy((LPSTR)_lpCodeBuf + _dwJmpBackCmdOffset + 1, &offset2, 4);
			pe.lpOptHeader->AddressOfEntryPoint = pe.lpLastScnHeader->VirtualAddress + pe.lpLastScnHeader->Misc.VirtualSize + _InjectedCodeOEP;
			pe.lpFileHeader->PointerToSymbolTable = pe.lpLastScnHeader->Misc.VirtualSize + _dwCodeSize + pe.lpLastScnHeader->PointerToRawData;
			memcpy(injectPos, _lpCodeBuf, _dwCodeSize);
			memcpy(injectPos + _dwCodeSize, &info, sizeof InjectedInfo);
			pe.lpLastScnHeader->Misc.VirtualSize += _dwCodeSize + sizeof InjectedInfo;
			return pe;
		}


		PEAnalyser<_NtHeaderT, _OptHeaderT> UninjectEntryCode()
		{
			if (!this->lpFileHeader->PointerToSymbolTable)
				throw NO_INJECTED_CODE_ERROR;
			PInjectedInfo info = (PInjectedInfo)(this->_lpBuffer + this->lpFileHeader->PointerToSymbolTable);
			if (info->Signature != XHACKER_INJECTEDINFO_SIGNATURE)
				throw NO_INJECTED_CODE_ERROR;
			if (strcmp((LPSTR)info->OrigiLastScn.Name, (LPSTR)this->lpLastScnHeader->Name))
				throw LAST_SCN_CHANGED_ERROR;

			DWORD size = this->_dwFileSize - info->ExpandedSpace;
			LPSTR newBuffer = (LPSTR)malloc(size);
			if (!newBuffer)
				throw;
			memset(newBuffer, 0, size);
			memcpy(newBuffer, this->_lpBuffer, info->OrigiLastScn.PointerToRawData + info->OrigiLastScn.Misc.VirtualSize);
			lxutil::PEAnalyser<_NtHeaderT, _OptHeaderT> pe{ newBuffer, size };
			*pe.lpLastScnHeader = info->OrigiLastScn;
			pe.lpFileHeader->PointerToSymbolTable = info->OrigiPTST;
			pe.lpOptHeader->SizeOfImage = info->OrigiSizeOfImage;
			pe.lpOptHeader->AddressOfEntryPoint = info->OrigiOEP;
			return pe;
		}
	};

	using XHacker32 = XHacker<IMAGE_NT_HEADERS32, IMAGE_OPTIONAL_HEADER32>;
	using XHacker64 = XHacker<IMAGE_NT_HEADERS64, IMAGE_OPTIONAL_HEADER64>;
}