set idlfile=%1.idl
if NOT "%2"=="" set idlfile=%2.idl
midl /nologo /Oicf /D "_DEBUG" /win32 /tlb %1.tlb %idlfile% 
if %errorlevel% NEQ 0 goto end
tlb2h %1.tlb %3 %4 %5 %6 %7
:end
