@setlocal
@call egcc64.bat
@gcc cmd-here.cc %CCFLAGS2% %LFLAGS% -o cmd-here.exe
@copy /Y cmd-here.exe %PROGRAMS%\bin
