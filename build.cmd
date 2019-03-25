set curdir=%CD%
pushd
title "%CD%\%0"

set InternalLibs=c:\SVN\internal_libs
set ExternalLibs=c:\SVN\external_libs
set BuildTmp=C:\SVN\Temp

set MainSource=C:\SVN\ReferentNet
set Client=%MainSource%
set SingleClient=C:\SVN\ReferentNet_8807
set Server=%MainSource%
set Admin=%MainSource%
set Updater=%MainSource%

set MainBuildMode=Clean,ReBuild
set ClientBuildMode=%MainBuildMode%
set SingleClientBuildMode=%MainBuildMode%
set ServerBuildMode=%MainBuildMode%
set AdminBuildMode=%MainBuildMode%
set UpdaterBuildMode=%MainBuildMode%

c:
mkdir "%BuildTmp%"
cd "%BuildTmp%"
del "%BuildTmp%\buildGeneral.log"

::build zlib.lib
::build iconv_a.lib
::build libxml2_a.lib
::build boost
::build firebird_server
::build "%ExternalLibs%\cpprestsdk-2.10.1\cpprestsdk141.sln"

echo Starting
echo Started

::echo cpprestsdk141 >> "%BuildTmp%\buildGeneral.log"
::MSBuild /t:Build "%ExternalLibs%\cpprestsdk-2.10.1\cpprestsdk141.sln" /p:Configuration="Release" /p:Platform="x86"
::IF %ERRORLEVEL% NEQ 0 goto err

echo xerces-all >> "%BuildTmp%\buildGeneral.log"
MSBuild /t:Build "%ExternalLibs%\xerces-c-3.1.1\projects\Win32\VC12\xerces-all\xerces-all.sln" /p:Configuration="Release_2017" /p:Platform="Win32"
IF %ERRORLEVEL% NEQ 0 goto err

echo ZipArchive >> "%BuildTmp%\buildGeneral.log"
MSBuild /t:Build "%ExternalLibs%\ZipArchive_4.6.1/ZipArchive/ZipArchive.sln" /p:Configuration="Release STL MD DLL_2017" /p:Platform="Win32"
IF %ERRORLEVEL% NEQ 0 goto err

echo ControledTransactionsCutter_vc12 >> "%BuildTmp%\buildGeneral.log"
MSBuild /t:Build "%InternalLibs%\ControledTransactionsCutter\ControledTransactionsCutter_vc12.sln" /p:Configuration="Release_2017" /p:Platform="Win32"
IF %ERRORLEVEL% NEQ 0 goto err

echo StackWalker_120 >> "%BuildTmp%\buildGeneral.log"
MSBuild /t:Build "%InternalLibs%\StackWalker\StackWalker_120.sln" /p:Configuration="Release_2017" /p:Platform="Win32"
IF %ERRORLEVEL% NEQ 0 goto err

echo Foundation_vs120 >> "%BuildTmp%\buildGeneral.log"
MSBuild /t:Build "%ExternalLibs%\poco-1.4.6p2-all\Foundation\Foundation_vs120.sln" /p:Configuration="release_shared_2017" /p:Platform="Win32"
IF %ERRORLEVEL% NEQ 0 goto err

echo Net_vs120 >> "%BuildTmp%\buildGeneral.log"
MSBuild /t:Build "%ExternalLibs%\poco-1.4.6p2-all\Net\Net_vs120.sln" /p:Configuration="release_shared_2017" /p:Platform="Win32"
IF %ERRORLEVEL% NEQ 0 goto err

echo Logger >> "%BuildTmp%\buildGeneral.log"
MSBuild /t:Build "%InternalLibs%\Logger\Logger.sln" /p:Configuration="Release_2017" /p:Platform="Win32"
IF %ERRORLEVEL% NEQ 0 goto err

echo fbXml >> "%BuildTmp%\buildGeneral.log"
MSBuild /t:Build "%InternalLibs%\TxFirebird\UDF\XML\fbXml\fbXml.sln" /p:Configuration="Release_2017" /p:Platform="Win32"
IF %ERRORLEVEL% NEQ 0 goto err

echo TaxcomExchange >> "%BuildTmp%\buildGeneral.log"
MSBuild /t:Build "%InternalLibs%\TaxcomExchange\TaxcomExchange.sln" /p:Configuration="Release_2017" /p:Platform="Win32"
IF %ERRORLEVEL% NEQ 0 goto err

echo CPCrypto_vc12 >> "%BuildTmp%\buildGeneral.log"
MSBuild /t:Build "%InternalLibs%\CPCryptoSt\CPCrypto_vc12.sln" /p:Configuration="Release_2017" /p:Platform="Win32"
IF %ERRORLEVEL% NEQ 0 goto err

echo UnifiedFormat_vc12 >> "%BuildTmp%\buildGeneral.log"
MSBuild /t:Build "%InternalLibs%\UnifiedFormat\UnifiedFormat_vc12.sln" /p:Configuration="Release_2017" /p:Platform="Win32"
IF %ERRORLEVEL% NEQ 0 goto err

::echo Admin >> "%BuildTmp%\buildGeneral.log"
::MSBuild /t:%AdminBuildMode% "%Admin%\Administration\Admin.sln" /p:Configuration="Release" /p:Platform="x86"
::IF %ERRORLEVEL% NEQ 0 goto err

::echo M Referents >> "%BuildTmp%\buildGeneral.log"
::MSBuild /t:%ClientBuildMode% "%Client%\Client\Referents\Referents.sln" /p:Configuration="Release" /p:Platform="x86"
::IF %ERRORLEVEL% NEQ 0 goto err

::echo Updater >> "%BuildTmp%\buildGeneral.log"
::MSBuild /t:%UpdaterBuildMode% "%Updater%\Updater\Updater\Updater.sln" /p:Configuration="Release" /p:Platform="x86"
::IF %ERRORLEVEL% NEQ 0 goto err

echo RefNetServer >> "%BuildTmp%\buildGeneral.log"
MSBuild /t:%ServerBuildMode% "%Server%\Server\RefNetServer\RefNetServer.sln" /p:Configuration="Release" /p:Platform="x86"
IF %ERRORLEVEL% NEQ 0 goto err

echo S Referents >> "%BuildTmp%\buildGeneral.log"
MSBuild /t:%SingleClientBuildMode% "%SingleClient%\Client\Referents\Referents.sln" /p:Configuration="Single_Release" /p:Platform="x86"
IF %ERRORLEVEL% NEQ 0 goto err


cd "%curdir%"
popd
echo !!!OK!!!
exit /b 0
:err
cd "%curdir%"
popd
echo !!!error!!!
exit /b 1