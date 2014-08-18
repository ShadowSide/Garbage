@echo off
ECHO Begin build debug and release library BOOST for MS VS 2008.

ECHO Setting VC90 environment variables.
CALL "%VS90COMNTOOLS%vsvars32.bat"

ECHO make sure bjam.exe is available (build if this not the case)
IF NOT EXIST "bjam.exe" (
   CALL bootstrap.bat
)

ECHO Start building complete libraries.
bjam --toolset=msvc-9.0 --with-regex --build-type=complete
REM TODO: Quit if errors

ECHO End build complete library BOOST for MS VS 2008.
pause
