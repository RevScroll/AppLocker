include Plugin.asm


_WndProc proc uses ebx edi esi,hWnd,uMsg,wParam,lParam
	local @stPos:POINT
	local @hLabel1:dword
	local @hLabel2:dword
	local @hLogEdit:dword
	local @hCancelEdit:dword
	local @hLogBtn:dword
	local @hCancelBtn:dword

	call @F   ; 免去重定位
	@@:
	pop ebx
	sub ebx,offset @B   ;求定位基地址ebx

	mov eax,uMsg

	.if eax==WM_CREATE
		mov eax,hWnd
		mov [ebx+offset hMainWin],eax

		;标签
		mov eax,offset szLabelClass
		add eax,ebx
		mov ecx,offset szAccLab
		add ecx,ebx

		mov edx,[ebx+offset hInstance]

		push ebx

		push NULL
		push edx
		push ID_LABEL1
		push hWnd
		push 22
		push 90
		push 10
		push 10
		push WS_CHILD or WS_VISIBLE or SS_CENTERIMAGE
		push ecx
		push eax
		push NULL
		mov edx,[ebx+offset _CreateWindowExA]
		call edx


		mov @hLabel1,eax
		pop ebx

		mov eax,offset szLabelClass
		add eax,ebx
		mov ecx,offset szPwdLab
		add ecx,ebx

		mov edx,[ebx+offset hInstance]

		push ebx


		push NULL
		push edx
		push ID_LABEL2
		push hWnd
		push 22
		push 90
		push 40
		push 10
		push WS_CHILD or WS_VISIBLE or SS_CENTERIMAGE
		push ecx
		push eax
		push NULL
		mov edx,[ebx+offset _CreateWindowExA]
		call edx


		mov @hLabel2,eax
		pop ebx

		;文本框
		mov eax,offset szEditClass
		add eax,ebx

		mov edx,[ebx+offset hInstance]

		push ebx


		push NULL
		push edx
		push IDC_ACC_EDIT
		push hWnd
		push 22
		push 175
		push 10
		push 105
		push WS_CHILD or WS_VISIBLE \
		or WS_BORDER
		push NULL
		push eax
		push WS_EX_TOPMOST
		mov edx,[ebx+offset _CreateWindowExA]
		call edx


		mov @hLogEdit,eax
		pop ebx

		mov eax,offset szEditClass
		add eax,ebx

		mov edx,[ebx+offset hInstance]
		push ebx

		push NULL
		push edx
		push IDC_PWD_EDIT
		push hWnd
		push 22
		push 175
		push 40
		push 105
		push WS_CHILD or WS_VISIBLE \
		or WS_BORDER or ES_PASSWORD
		push NULL
		push eax
		push WS_EX_TOPMOST
		mov edx,[ebx+offset _CreateWindowExA]
		call edx

		mov @hCancelEdit,eax
		pop ebx

		;按钮
		mov eax,offset szButtonClass
		add eax,ebx
		mov ecx,offset szLogBtn
		add ecx,ebx

		mov edx,[ebx+offset hInstance]
		push ebx

		push NULL
		push edx
		push IDC_LOG_BTN
		push hWnd
		push 25
		push 60
		push 70
		push 60
		push WS_CHILD or WS_VISIBLE or BS_DEFPUSHBUTTON or BS_PUSHBUTTON
		push ecx
		push eax
		push NULL
		mov edx,[ebx+offset _CreateWindowExA]
		call edx


		mov @hLogBtn,eax
		pop ebx

		mov eax,offset szButtonClass
		add eax,ebx
		mov ecx,offset szCancelBtn
		add ecx,ebx

		mov edx,[ebx+offset hInstance]
		push ebx 

		push NULL
		push edx
		push IDC_CANCEL_BTN
		push hWnd
		push 25
		push 60
		push 70
		push 170
		push WS_CHILD or WS_VISIBLE or BS_DEFPUSHBUTTON or BS_PUSHBUTTON
		push ecx
		push eax
		push NULL
		mov edx,[ebx+offset _CreateWindowExA]
		call edx

		mov @hCancelBtn,eax
		pop ebx
	.elseif eax==WM_CLOSE
		call _Exit
	.elseif eax==WM_COMMAND  ;处理菜单及加速键消息
		mov eax,wParam
		movzx eax,ax
		.if eax==IDC_LOG_BTN

			push ebx

			mov eax, sizeof szBuffer
			dec eax
			push eax 
			mov eax,offset szBuffer
			add eax,ebx
			push eax
			push IDC_ACC_EDIT
			push hWnd
			mov edx,[ebx+offset _GetDlgItemTextA]
			call edx

			pop ebx
			push ebx

			mov eax, sizeof szBuffer2
			dec eax
			push eax 
			mov eax,offset szBuffer2
			add eax,ebx
			push eax
			push IDC_PWD_EDIT
			push hWnd
			mov edx,[ebx+offset _GetDlgItemTextA]
			call edx

			pop ebx
			push ebx

			mov eax,offset szBuffer
			add eax,ebx
			mov ecx,offset lpszAcc
			add ecx,ebx

			push ecx
			push eax
			mov edx,[ebx+offset _lstrcmpA]
			call edx

			pop ebx
			.if eax
				call _Falid
				jmp finish
			.endif
			push ebx

			mov eax,offset szBuffer2
			add eax,ebx
			mov ecx,offset lpszPass
			add ecx,ebx

			push ecx
			push eax
			mov edx,[ebx+offset _lstrcmpA]
			call edx

			pop ebx
			.if eax
				call _Falid
				jmp finish
			.endif
			call _Success
			call _Quit
		.elseif eax==IDC_CANCEL_BTN
			call _Exit
		.endif
	.else
		push lParam
		push wParam
		push uMsg
		push hWnd
		mov edx,[ebx+offset _DefWindowProcA]
		call edx
		ret
	.endif

finish:
	xor eax,eax
	ret
_WndProc endp