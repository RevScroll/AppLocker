.386
.model flat,stdcall
option casemap:none

include    windows.inc
include    user32.inc
includelib user32.lib
include    kernel32.inc
includelib kernel32.lib


FLOWER macro
	;jmp @F
	;db 0EBh
	;@@: call _Rubbish
	;mov esi, dword ptr[esp+20]
endm


.code
jmp start
jmpBack:
	db 0E9h, 0, 0, 0, 0
db 0, 0

szSuccessTip       db  64 dup(0)
szSuccessTitle	   db  64 dup(0)
szFalidTitle	   db  64 dup(0)
szFalidTip		   db  64 dup(0)
szCaptionMain      db  16 dup(0)
szLogBtn      	   db  16 dup(0)
szCancelBtn        db  16 dup(0)
szAccLab           db  16 dup(0)
szPwdLab           db  16 dup(0)
lpszAcc            db  64 dup(0)
lpszPass           db  64 dup(0)
dwWinX			   dd  0
dwWinY			   dd  0

szButtonClass      db  'button',0
szEditClass        db  'edit',0
szLabelClass       db  'static',0
szClassName        db  '__LxLWV',0

szBuffer           db  16 dup(0)
szBuffer2          db  16 dup(0)

hInstance          dd  0
hMainWin           dd  0

hUser32Base        dd  0
hKernel32Base      dd  0


_Rubbish proc
	ret
_Rubbish endp

include WndProc.asm

;----------------------
; 主窗口程序
;----------------------
_WinMain  proc _base

	local @stWndClass:WNDCLASSEX
	local @stMsg:MSG
	local @hAccelerator

	mov ebx,_base
	push ebx

	push NULL
	mov edx,[ebx+offset _GetModuleHandleA]
	call edx


	pop ebx
	mov [ebx+offset hInstance],eax

	push ebx
	;注册窗口类


	push sizeof @stWndClass
	lea eax,@stWndClass
	push eax
	mov edx,[ebx+offset _RtlZeroMemory]
	call edx       


	mov @stWndClass.hIcon,NULL
	mov @stWndClass.hIconSm,NULL

	mov @stWndClass.hCursor,NULL

	pop ebx

	mov edx,offset _WndProc
	add edx,ebx
	mov ecx,offset szClassName
	add ecx,ebx

	push [ebx+offset hInstance]
	pop @stWndClass.hInstance
	mov @stWndClass.cbSize,sizeof WNDCLASSEX
	mov @stWndClass.style,CS_HREDRAW or CS_VREDRAW
	mov @stWndClass.lpfnWndProc,edx
	mov @stWndClass.hbrBackground,COLOR_WINDOW
	mov @stWndClass.lpszClassName,ecx
	push ebx

	lea eax,@stWndClass
	push eax
	mov edx,[ebx+offset _RegisterClassExA]
	call edx

	pop ebx

	mov edx,offset szClassName
	add edx,ebx
	mov ecx,offset szCaptionMain
	add ecx,ebx

	mov eax,offset hInstance
	add eax,ebx
	push ebx


	;建立并显示窗口
	push NULL
	push [eax]
	push NULL
	push NULL
	push 140
	push 300
	push [ebx + offset dwWinY]
	push [ebx + offset dwWinX]
	push DS_MODALFRAME or DS_FIXEDSYS or WS_MINIMIZEBOX or WS_POPUP or WS_CAPTION or WS_SYSMENU
	push ecx
	push edx
	push WS_EX_CLIENTEDGE
	mov edx,[ebx+offset _CreateWindowExA]
	call edx


	pop ebx
	mov [ebx+offset hMainWin],eax

	mov edx,offset hMainWin
	add edx,ebx

	push ebx
	push edx

	push SW_SHOWNORMAL
	push [edx]
	mov ecx,[ebx+offset _ShowWindow]
	call ecx

	pop edx

	push [edx]
	mov ecx,[ebx+offset _UpdateWindow]
	call ecx

	pop ebx

	;消息循环
	.while TRUE
		push ebx

		push 0
		push 0
		push NULL
		lea eax,@stMsg
		push eax
		mov edx,[ebx+offset _GetMessageA]
		call edx

		pop ebx
		.break .if eax==0
		mov ecx,offset hMainWin
		add ecx,ebx

		push ebx

		lea eax,@stMsg
		push eax
		push @hAccelerator
		push [ecx]
		mov edx,[ebx+offset _TranslateAcceleratorA]
		call edx

		pop ebx
		.if eax==0
			lea eax,@stMsg
			push eax
			mov edx,[ebx+offset _TranslateMessage]
			call edx

			lea eax,@stMsg
			push eax
			mov edx,[ebx+offset _DispatchMessageA]
			call edx
		.endif
	.endw
	ret
_WinMain endp


start:
	FLOWER
	;取当前函数的堆栈栈顶值
	mov eax,dword ptr [esp]
	push eax
	
	call @F   ; 免去重定位
	@@:
	pop ebx
	sub ebx,offset @B
	
	pop eax
	FLOWER
	;获取kernel32.dll的基地址
	invoke _getKernelBase,eax
	mov [ebx+offset hKernel32Base],eax

	;从基地址出发搜索GetProcAddress函数的首址
	FLOWER
	mov eax,offset szGetProcAddress
	add eax,ebx
	mov ecx,[ebx+offset hKernel32Base]
	FLOWER
	invoke _getApi,ecx,eax
	mov [ebx+offset _GetProcAddress],eax   ;为函数引用赋值 GetProcAddress

	FLOWER
	;使用GetProcAddress函数的首址，传入两个参数调用GetProcAddress函数，获得LoadLibraryA的首址
	mov eax,offset szLoadLibraryA
	add eax,ebx
	
	push eax
	push [ebx+offset hKernel32Base]
	mov edx,[ebx+offset _GetProcAddress]
	call edx
	mov [ebx+offset _LoadLibraryA],eax

	FLOWER
	invoke _getDllBase      ;获取所有用到的dll的基地址，kernel32除外
	invoke _getFuns         ;获取所有用到的函数的入口地址，GetProcAddress和LoadLibraryA除外
	invoke _WinMain,ebx
	
	FLOWER
	jmp jmpBack
	ret
	end start
