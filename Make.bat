@setlocal
@call egcc64.bat
@gcc cmd-here.cc %CCFLAGS2% %LFLAGS% -municode -o cmd-here.exe
@copy /Y cmd-here.exe %PROGRAMS%\bin
