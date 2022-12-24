EXE = "LockWin32.exe"
OBJS = UnRelocalLoginWin.obj

LINK_FLAG = /subsystem:windows
ML_FLAG = /c /coff /Zi

$(EXE): $(OBJS) $(RES)
	LINK $(LINK_FLAG) /SECTION:.text,erw /OUT:$(EXE) $(OBJS)
	
UnRelocalLoginWin.obj:
	ml $(ML_FLAG) ./src/UnRelocalLoginWin.asm
	
clean:
	del *.obj