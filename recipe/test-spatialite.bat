@echo on
set "EXPECT_RTTOPO=0"
if "%1"=="gpl" set "EXPECT_RTTOPO=1"
call %CC% /nologo /MD /I"%LIBRARY_INC%" test-spatialite.c /link /LIBPATH:"%LIBRARY_LIB%" spatialite_i.lib sqlite3.lib /OUT:test-spatialite.exe
if errorlevel 1 exit /b 1
test-spatialite.exe linked %EXPECT_RTTOPO%
if errorlevel 1 exit /b 1
test-spatialite.exe module %EXPECT_RTTOPO% "%LIBRARY_BIN%\mod_spatialite.dll"
if errorlevel 1 exit /b 1
