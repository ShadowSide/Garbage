set curdir=%CD%
title "%CD%\%0"

set InternalLibs=c:\SVN\internal_libs
set ExternalLibs=c:\SVN\external_libs
set BuildTmp=C:\SVN\Temp

set MainSource=C:\SVN\ReferentNet
set Client=%MainSource%
set SingleClient=C:\SVN\ReferentNet_8807
set Server=E:\SVN\ReferentNet
set Admin=%MainSource%
set Updater=%MainSource%

c:
mkdir "%BuildTmp%"
cd "%BuildTmp%"
del "%BuildTmp%\cleanGeneral.log"

::clean zlib.lib
::clean iconv_a.lib
::clean libxml2_a.lib
::clean boost
::clean firebird_server
::clean "C:\SVN\External_Libs\cpprestsdk-2.10.1\cpprestsdk141.sln"

echo Starting
echo Started

::echo cpprestsdk141 >> "%BuildTmp%\cleanGeneral.log"
::MSBuild /t:Clean "%ExternalLibs%\cpprestsdk-2.10.1\cpprestsdk141.sln" /p:Configuration="Release" /p:Platform="x86"
::IF %ERRORLEVEL% NEQ 0 goto err

echo xerces-all >> "%BuildTmp%\cleanGeneral.log"
MSBuild /t:Clean "%ExternalLibs%\xerces-c-3.1.1\projects\Win32\VC12\xerces-all\xerces-all.sln" /p:Configuration="Release_2017" /p:Platform="Win32"
IF %ERRORLEVEL% NEQ 0 goto err

echo ZipArchive >> "%BuildTmp%\cleanGeneral.log"
MSBuild /t:Clean "%ExternalLibs%\ZipArchive_4.6.1/ZipArchive/ZipArchive.sln" /p:Configuration="Release STL MD DLL_2017" /p:Platform="Win32"
IF %ERRORLEVEL% NEQ 0 goto err

echo ControledTransactionsCutter_vc12 >> "%BuildTmp%\cleanGeneral.log"
MSBuild /t:Clean "%InternalLibs%\ControledTransactionsCutter\ControledTransactionsCutter_vc12.sln" /p:Configuration="Release_2017" /p:Platform="Win32"
IF %ERRORLEVEL% NEQ 0 goto err

echo StackWalker_120 >> "%BuildTmp%\cleanGeneral.log"
MSBuild /t:Clean "%InternalLibs%\StackWalker\StackWalker_120.sln" /p:Configuration="Release_2017" /p:Platform="Win32"
IF %ERRORLEVEL% NEQ 0 goto err

echo Foundation_vs120 >> "%BuildTmp%\cleanGeneral.log"
MSBuild /t:Clean "%ExternalLibs%\poco-1.4.6p2-all\Foundation\Foundation_vs120.sln" /p:Configuration="release_shared_2017" /p:Platform="Win32"
IF %ERRORLEVEL% NEQ 0 goto err

echo Net_vs120 >> "%BuildTmp%\cleanGeneral.log"
MSBuild /t:Clean "%ExternalLibs%\poco-1.4.6p2-all\Net\Net_vs120.sln" /p:Configuration="release_shared_2017" /p:Platform="Win32"
IF %ERRORLEVEL% NEQ 0 goto err

echo Logger >> "%BuildTmp%\cleanGeneral.log"
MSBuild /t:Clean "%InternalLibs%\Logger\Logger.sln" /p:Configuration="Release_2017" /p:Platform="Win32"
IF %ERRORLEVEL% NEQ 0 goto err

echo fbXml >> "%BuildTmp%\cleanGeneral.log"
MSBuild /t:Clean "%InternalLibs%\TxFirebird\UDF\XML\fbXml\fbXml.sln" /p:Configuration="Release_2017" /p:Platform="Win32"
IF %ERRORLEVEL% NEQ 0 goto err

echo TaxcomExchange >> "%BuildTmp%\cleanGeneral.log"
MSBuild /t:Clean "%InternalLibs%\TaxcomExchange\TaxcomExchange.sln" /p:Configuration="Release_2017" /p:Platform="Win32"
IF %ERRORLEVEL% NEQ 0 goto err

echo CPCrypto_vc12 >> "%BuildTmp%\cleanGeneral.log"
MSBuild /t:Clean "%InternalLibs%\CPCryptoSt\CPCrypto_vc12.sln" /p:Configuration="Release_2017" /p:Platform="Win32"
IF %ERRORLEVEL% NEQ 0 goto err

echo UnifiedFormat_vc12 >> "%BuildTmp%\cleanGeneral.log"
MSBuild /t:Clean "%InternalLibs%\UnifiedFormat\UnifiedFormat_vc12.sln" /p:Configuration="Release_2017" /p:Platform="Win32"
IF %ERRORLEVEL% NEQ 0 goto err

echo Admin >> "%BuildTmp%\cleanGeneral.log"
MSBuild /t:Clean "%Admin%\Administration\Admin.sln" /p:Configuration="Release" /p:Platform="x86"
IF %ERRORLEVEL% NEQ 0 goto err

echo M Referents >> "%BuildTmp%\cleanGeneral.log"
MSBuild /t:Clean "%Client%\Client\Referents\Referents.sln" /p:Configuration="Release" /p:Platform="x86"
IF %ERRORLEVEL% NEQ 0 goto err

echo Updater >> "%BuildTmp%\cleanGeneral.log"
MSBuild /t:Clean "%Updater%\Updater\Updater\Updater.sln" /p:Configuration="Release" /p:Platform="x86"
IF %ERRORLEVEL% NEQ 0 goto err

echo RefNetServer >> "%BuildTmp%\cleanGeneral.log"
MSBuild /t:Clean "%Server%\Server\RefNetServer\RefNetServer.sln" /p:Configuration="Release" /p:Platform="x86"
IF %ERRORLEVEL% NEQ 0 goto err

echo S Referents >> "%BuildTmp%\cleanGeneral.log"
MSBuild /t:Clean "%SingleClient%\Client\Referents\Referents.sln" /p:Configuration="Single_Release" /p:Platform="x86"
IF %ERRORLEVEL% NEQ 0 goto err


cd "%curdir%"
echo !!!OK!!!
exit /b 0
:err
cd "%curdir%"
echo !!!error!!!
exit /b 1