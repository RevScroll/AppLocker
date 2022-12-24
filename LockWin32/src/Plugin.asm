IDC_LOG_BTN         equ   1
IDC_CANCEL_BTN      equ   2
ID_LABEL1          equ   3
ID_LABEL2          equ   4
IDC_ACC_EDIT           equ   5
IDC_PWD_EDIT           equ   6

_LxGetProcAddress typedef proto :dword,:dword      ;��������
_LxLoadLibraryA   typedef proto :dword
_LxCreateWindowExA typedef proto  :DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD
_LxDefWindowProcA typedef proto   :DWORD,:DWORD,:DWORD,:DWORD
_LxDestroyWindow typedef proto   :DWORD
_LxDispatchMessageA typedef proto   :DWORD
_LxGetDlgItemTextA typedef proto   :DWORD,:DWORD,:DWORD,:DWORD
_LxGetMessageA typedef proto   :DWORD,:DWORD,:DWORD,:DWORD
_LxMessageBoxA    typedef proto   :dword,:dword,:dword,:dword
_LxPostQuitMessage typedef proto   :DWORD
_LxRegisterClassExA typedef proto   :DWORD
_LxShowWindow typedef proto   :DWORD,:DWORD
_LxTranslateAcceleratorA typedef proto   :DWORD,:DWORD,:DWORD
_LxTranslateMessage typedef proto   :DWORD
_LxUpdateWindow typedef proto   :DWORD

_LxExitProcess typedef proto   :DWORD
_LxGetModuleHandleA typedef proto   :DWORD
_LxRtlZeroMemory typedef proto   :DWORD,:DWORD
_LxlstrcmpA typedef proto   :DWORD,:DWORD
_LxUnregisterClassA typedef proto   :DWORD,:DWORD


;user32.dll
_ApiGetProcAddress  typedef ptr _LxGetProcAddress  ;������������
_ApiLoadLibraryA       typedef ptr _LxLoadLibraryA
_ApiCreateWindowExA   typedef ptr _LxCreateWindowExA
_ApiDefWindowProcA   typedef ptr _LxDefWindowProcA
_ApiDestroyWindow   typedef ptr _LxDestroyWindow
_ApiDispatchMessageA   typedef ptr _LxDispatchMessageA
_ApiGetDlgItemTextA   typedef ptr _LxGetDlgItemTextA
_ApiGetMessageA   typedef ptr _LxGetMessageA
_ApiMessageBoxA   typedef ptr _LxMessageBoxA
_ApiPostQuitMessage   typedef ptr _LxPostQuitMessage
_ApiRegisterClassExA   typedef ptr _LxRegisterClassExA
_ApiShowWindow   typedef ptr _LxShowWindow
_ApiTranslateAcceleratorA   typedef ptr _LxTranslateAcceleratorA
_ApiTranslateMessage   typedef ptr _LxTranslateMessage
_ApiUpdateWindow   typedef ptr _LxUpdateWindow
;kernel32.dll
_ApiExitProcess        typedef ptr _LxExitProcess
_ApiGetModuleHandleA   typedef ptr _LxGetModuleHandleA
_ApiRtlZeroMemory   typedef ptr _LxRtlZeroMemory
_ApilstrcmpA   typedef ptr _LxlstrcmpA
_ApiUnregisterClassA   typedef ptr _LxUnregisterClassA


MB_ERROR				equ MB_OK or MB_ICONERROR
MB_TIP					equ MB_OK or MB_ICONINFORMATION

	.code
szGetProcAddress   db 'GetProcAddress',0
szLoadLibraryA     db 'LoadLibraryA',0
szCreateWindowExA  db 'CreateWindowExA',0
szDefWindowProcA   db 'DefWindowProcA',0
szDestroyWindow    db 'DestroyWindow',0
szDispatchMessageA db 'DispatchMessageA',0
szGetDlgItemTextA  db 'GetDlgItemTextA',0
szGetMessageA      db 'GetMessageA',0
szMessageBoxA      db 'MessageBoxA',0
szPostQuitMessage  db 'PostQuitMessage',0
szRegisterClassExA db 'RegisterClassExA',0
szShowWindow       db 'ShowWindow',0
szTranslateAcceleratorA  db 'TranslateAcceleratorA',0
szTranslateMessage db 'TranslateMessage',0
szUpdateWindow     db 'UpdateWindow',0
szUser32           db 'user32.dll',0,0
szExitProcess      db 'ExitProcess',0
szGetModuleHandleA db 'GetModuleHandleA',0
szRtlZeroMemory    db 'RtlZeroMemory',0
szlstrcmpA         db 'lstrcmpA',0
szUnregisterClassA db 'UnregisterClassA',0
szKernel32         db 'kernel32.dll',0

_GetProcAddress  	_ApiGetProcAddress ? ;������������
_LoadLibraryA       	_ApiLoadLibraryA ?
_CreateWindowExA   	_ApiCreateWindowExA  ?
_DefWindowProcA   	_ApiDefWindowProcA  ?
_DestroyWindow   	_ApiDestroyWindow  ?
_DispatchMessageA   	_ApiDispatchMessageA  ?
_GetDlgItemTextA   	_ApiGetDlgItemTextA  ?
_GetMessageA   		_ApiGetMessageA  ?
_MessageBoxA   		_ApiMessageBoxA  ?
_PostQuitMessage   	_ApiPostQuitMessage  ?
_RegisterClassExA   	_ApiRegisterClassExA  ?
_ShowWindow   		_ApiShowWindow  ?
_TranslateAcceleratorA  _ApiTranslateAcceleratorA  ?
_TranslateMessage   	_ApiTranslateMessage  ?
_UpdateWindow   	_ApiUpdateWindow  ?
;kernel32.dll
_ExitProcess        	_ApiExitProcess  ?
_GetModuleHandleA   	_ApiGetModuleHandleA  ?
_RtlZeroMemory   	_ApiRtlZeroMemory  ?
_lstrcmpA   		_ApilstrcmpA  ?
_UnregisterClassA   		_ApiUnregisterClassA  ?


_getKernelBase  proc _dwKernelRetAddress
	local @dwRet

	pushad

	mov @dwRet,0

	mov edi,_dwKernelRetAddress
	and edi,0ffff0000h  ;����ָ������ҳ�ı߽磬��1000h����

	.repeat
		.if word ptr [edi]==IMAGE_DOS_SIGNATURE  ;�ҵ�kernel32.dll��dosͷ
			mov esi,edi
			add esi,[esi+003ch]
			.if word ptr [esi]==IMAGE_NT_SIGNATURE ;�ҵ�kernel32.dll��PEͷ��ʶ
				mov @dwRet,edi
			.break
			.endif
		.endif
		sub edi,010000h
		.break .if edi<070000000h
	.until FALSE
	popad
	mov eax,@dwRet
	ret
_getKernelBase  endp   


_getApi proc _hModule,_lpApi
	local @ret
	local @dwLen

	pushad
	mov @ret,0
	;����API�ַ����ĳ��ȣ���������
	mov edi,_lpApi
	mov ecx,-1
	xor al,al
	cld
	repnz scasb
	mov ecx,edi
	sub ecx,_lpApi
	mov @dwLen,ecx

	;��pe�ļ�ͷ������Ŀ¼��ȡ�������ַ
	mov esi,_hModule
	add esi,[esi+3ch]
	assume esi:ptr IMAGE_NT_HEADERS
	mov esi,[esi].OptionalHeader.DataDirectory.VirtualAddress
	add esi,_hModule
	assume esi:ptr IMAGE_EXPORT_DIRECTORY

	;���ҷ������Ƶĵ���������
	mov ebx,[esi].AddressOfNames
	add ebx,_hModule
	xor edx,edx
	.repeat
		push esi
		mov edi,[ebx]
		add edi,_hModule
		mov esi,_lpApi
		mov ecx,@dwLen
		repz cmpsb
		.if ZERO?
			pop esi
			jmp @F
		.endif
		pop esi
		add ebx,4
		inc edx
	.until edx>=[esi].NumberOfNames
	jmp _ret
	@@:
	;ͨ��API����������ȡ��������ٻ�ȡ��ַ����
	sub ebx,[esi].AddressOfNames
	sub ebx,_hModule
	shr ebx,1
	add ebx,[esi].AddressOfNameOrdinals
	add ebx,_hModule
	movzx eax,word ptr [ebx]
	shl eax,2
	add eax,[esi].AddressOfFunctions
	add eax,_hModule

	;�ӵ�ַ��õ����������ĵ�ַ
	mov eax,[eax]
	add eax,_hModule
	mov @ret,eax

	_ret:
	assume esi:nothing
	popad
	mov eax,@ret
	ret
_getApi endp


_Quit  proc
	pushad
	call @F   ; ��ȥ�ض�λ
	@@:
	pop ebx
	sub ebx,offset @B   ;��λ����ַebx  

	push [ebx+offset hMainWin]
	mov edx,[ebx+offset _DestroyWindow]
	call edx
	
	push [ebx + offset hInstance]
	mov eax, offset szClassName
	add eax, edx
	push eax
	mov edx, [ebx+offset _UnregisterClassA]
	call edx
	
	push NULL
	mov ecx,[ebx+offset _PostQuitMessage]
	call ecx
	popad
	ret
_Quit  endp


_Exit proc
	call @F   ; ��ȥ�ض�λ
	@@:
	pop ebx
	sub ebx,offset @B   ;��λ����ַebx  

	push NULL
	mov edx,[ebx+offset _ExitProcess]
	call edx
_Exit endp


_Success proc
	pushad
	call @F   ; ��ȥ�ض�λ
	@@:
	pop ebx
	sub ebx,offset @B   ;��λ����ַebx
	push MB_TIP
	
	mov eax, offset szSuccessTitle
	add eax, ebx
	push eax
	
	mov eax, offset szSuccessTip
	add eax, ebx
	push eax
	
	mov eax, [ebx + offset hMainWin]
	push eax
	
	mov edx, [ebx + offset _MessageBoxA]
	call edx
	popad
	ret
_Success endp

_Falid proc	
	pushad
	call @F   ; ��ȥ�ض�λ
	@@:
	pop ebx
	sub ebx,offset @B   ;��λ����ַebx
	push MB_ERROR
	
	mov eax, offset szFalidTitle
	add eax, ebx
	push eax
	
	mov eax, offset szFalidTip
	add eax, ebx
	push eax
	
	mov eax, [ebx + offset hMainWin]
	push eax
	
	mov edx, [ebx + offset _MessageBoxA]
	call edx
	popad
	ret
_Falid endp

;-----------------
; ��ȡ���ж�̬���ӿ�Ļ���ַ
;-----------------
_getDllBase proc
	pushad
	call @F   ; ��ȥ�ض�λ
	@@:
	pop ebx
	sub ebx,offset @B

	nop

	;ʹ��LoadLibrary��ȡuser32.dll�Ļ���ַ
	mov eax,offset szUser32
	add eax,ebx

	mov edx,[ebx+offset _LoadLibraryA]

	push eax
	call edx
	mov [ebx+offset hUser32Base],eax

	popad
	ret
_getDllBase endp

;-----------------
; ��ȡ���к�������ڵ�ַ
;-----------------
_getFuns proc
	pushad
	call @F   ; ��ȥ�ض�λ
	@@:
	pop ebx
	sub ebx,offset @B

	;CreateWindowExA
	mov eax,offset szCreateWindowExA
	add eax,ebx
	push eax
	push [ebx+offset hUser32Base]
	mov edx,[ebx+offset _GetProcAddress]
	call edx
	mov [ebx+offset _CreateWindowExA],eax

	;DefWindowProcA
	mov eax,offset szDefWindowProcA
	add eax,ebx
	push eax
	push [ebx+offset hUser32Base]
	mov edx,[ebx+offset _GetProcAddress]
	call edx
	mov [ebx+offset _DefWindowProcA],eax

	;DestroyWindow
	mov eax,offset szDestroyWindow
	add eax,ebx
	push eax
	push [ebx+offset hUser32Base]
	mov edx,[ebx+offset _GetProcAddress]
	call edx
	mov [ebx+offset _DestroyWindow],eax

	;DispatchMessageA
	mov eax,offset szDispatchMessageA
	add eax,ebx
	push eax
	push [ebx+offset hUser32Base]
	mov edx,[ebx+offset _GetProcAddress]
	call edx
	mov [ebx+offset _DispatchMessageA],eax

	;GetDlgItemTextA
	mov eax,offset szGetDlgItemTextA
	add eax,ebx
	push eax
	push [ebx+offset hUser32Base]
	mov edx,[ebx+offset _GetProcAddress]
	call edx
	mov [ebx+offset _GetDlgItemTextA],eax

	;GetMessageA
	mov eax,offset szGetMessageA
	add eax,ebx
	push eax
	push [ebx+offset hUser32Base]
	mov edx,[ebx+offset _GetProcAddress]
	call edx
	mov [ebx+offset _GetMessageA],eax

	;MessageBoxA
	mov eax,offset szMessageBoxA
	add eax,ebx
	push eax
	push [ebx+offset hUser32Base]
	mov edx,[ebx+offset _GetProcAddress]
	call edx
	mov [ebx+offset _MessageBoxA],eax

	;PostQuitMessage
	mov eax,offset szPostQuitMessage
	add eax,ebx
	push eax
	push [ebx+offset hUser32Base]
	mov edx,[ebx+offset _GetProcAddress]
	call edx
	mov [ebx+offset _PostQuitMessage],eax

	;RegisterClassExA
	mov eax,offset szRegisterClassExA
	add eax,ebx
	push eax
	push [ebx+offset hUser32Base]
	mov edx,[ebx+offset _GetProcAddress]
	call edx
	mov [ebx+offset _RegisterClassExA],eax

	;ShowWindow
	mov eax,offset szShowWindow
	add eax,ebx
	push eax
	push [ebx+offset hUser32Base]
	mov edx,[ebx+offset _GetProcAddress]
	call edx
	mov [ebx+offset _ShowWindow],eax

	;TranslateAcceleratorA
	mov eax,offset szTranslateAcceleratorA
	add eax,ebx
	push eax
	push [ebx+offset hUser32Base]
	mov edx,[ebx+offset _GetProcAddress]
	call edx
	mov [ebx+offset _TranslateAcceleratorA],eax

	;TranslateMessage
	mov eax,offset szTranslateMessage
	add eax,ebx
	push eax
	push [ebx+offset hUser32Base]
	mov edx,[ebx+offset _GetProcAddress]
	call edx
	mov [ebx+offset _TranslateMessage],eax

	;UpdateWindow
	mov eax,offset szUpdateWindow
	add eax,ebx
	push eax
	push [ebx+offset hUser32Base]
	mov edx,[ebx+offset _GetProcAddress]
	call edx
	mov [ebx+offset _UpdateWindow],eax


	;ExitProcess
	mov eax,offset szExitProcess
	add eax,ebx
	push eax
	push [ebx+offset hKernel32Base]
	mov edx,[ebx+offset _GetProcAddress]
	call edx
	mov [ebx+offset _ExitProcess],eax

	;GetModuleHandleA
	mov eax,offset szGetModuleHandleA
	add eax,ebx
	push eax
	push [ebx+offset hKernel32Base]
	mov edx,[ebx+offset _GetProcAddress]
	call edx
	mov [ebx+offset _GetModuleHandleA],eax


	;RtlZeroMemory
	mov eax,offset szRtlZeroMemory
	add eax,ebx
	push eax
	push [ebx+offset hKernel32Base]
	mov edx,[ebx+offset _GetProcAddress]
	call edx
	mov [ebx+offset _RtlZeroMemory],eax


	;lstrcmpA
	mov eax,offset szlstrcmpA
	add eax,ebx
	push eax
	push [ebx+offset hKernel32Base]
	mov edx,[ebx+offset _GetProcAddress]
	call edx
	mov [ebx+offset _lstrcmpA],eax


	;UnregisterClassA
	mov eax,offset szUnregisterClassA
	add eax,ebx
	push eax
	push [ebx+offset hUser32Base]
	mov edx,[ebx+offset _GetProcAddress]
	call edx
	mov [ebx+offset _UnregisterClassA],eax
	
	popad
	ret
_getFuns endp