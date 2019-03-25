set deployUpdater=0
set deployClient=0
set deploySingleClient=1
set deployFirebirdServer=0
set deployServer=1
set deployAdmin=0
set deployRuntime=1
set deployDiff=1
set deployOnlyNew=0

set MainSource=c:\SVN\ReferentNet
set Client=%MainSource%
set SingleClient=c:\SVN\ReferentNet_8807
set Server=%MainSource%
set Admin=%MainSource%
set Updater=%MainSource%

set Deploy=E:\Taxcom\Docliner Single
set InternalLibs=c:\SVN\internal_libs
set ExternalLibs=c:\SVN\external_libs
set Diff=%Deploy%\Diff
set MinimalRuntime=%Deploy%\MinimalRuntimeDiff
set Runtime=%Deploy%\RuntimeDiff
set FullRuntime=%Deploy%\FullRuntimeDiff

set install=xcopy /z /y /i /f /r
set install_recursive=xcopy /z /e /y /i /f /r

if "%VCToolsRedistDir%" equ "" (
echo Error: Environment variable VCToolsRedistDir isn't defined. You should use MSVC Developer console.
pause
exit /b 1
)

if %deployOnlyNew% neq 0 (
set install_only_new=%install% /m
set install_recursive_only_new=%install_recursive% /m
) else (
set install_only_new=%install%
set install_recursive_only_new=%install_recursive%
)

title "%CD%\%0 du%deployUpdater% ds%deployServer% dsc%deploySingleClient% dc%deployClient% da%deployAdmin% dfs%deployFirebirdServer% dd%deployDiff% dr%deployRuntime% don%deployOnlyNew%"

if %deployRuntime% NEQ 0 (
rmdir "%MinimalRuntime%"
rmdir "%Runtime%"
rmdir "%FullRuntime%"

mkdir "%MinimalRuntime%"
mkdir "%Runtime%"
mkdir "%FullRuntime%"
)

if %deployDiff% NEQ 0 (
mkdir "%Diff%"
)

if %deployServer% NEQ 0 (
mkdir "%Deploy%\RefNetServer\RefNetUpdater"
mkdir "%Deploy%\RefNetServer\FirebirdServer"
mkdir "%Deploy%\RefNetServer\FirebirdServerProxy"
)
if %deployClient% NEQ 0 (
mkdir "%Deploy%\Client\RefNetUpdater"
)
if %deploySingleClient% NEQ 0 (
mkdir "%Deploy%\Client\RefNetUpdater"
)
if %deployAdmin% NEQ 0 (
mkdir "%Deploy%\Admin\RefNetUpdater"
)

if %deployDiff% NEQ 0 (
%install% "%ExternalLibs%\Firebird\Src\output_Win32\*.dll" "%Diff%"
%install% "%ExternalLibs%\Firebird\Src\output_Win32\*.conf" "%Diff%"
%install% "%ExternalLibs%\Firebird\Src\output_Win32\*.msg" "%Diff%"
%install% "%ExternalLibs%\Firebird\Src\output_Win32\*.dat" "%Diff%"
%install% "%ExternalLibs%\Firebird\Src\output_Win32\udf\*.*" "%Diff%\udf"
%install% "%ExternalLibs%\Firebird\Src\output_Win32\plugins\*.*" "%Diff%\plugins"
%install% "%ExternalLibs%\Firebird\Src\output_Win32\intl\*.*" "%Diff%\intl"
%install% "%ExternalLibs%\Firebird\Src\output_Win32\system32\*.*" "%Diff%\system32"
%install_only_new% "%ExternalLibs%\zlib-1.2.8\builds\Release\*.dll" "%Diff%"
%install_only_new% "%ExternalLibs%\libxml2-2.7.8\build_x86_release_md\lib\*.dll" "%Diff%"
%install_only_new% "%ExternalLibs%\casablanca_141_xp\Binaries\Win32\Release\*.dll" "%Diff%"
%install_only_new% "%ExternalLibs%\poco-1.4.6p2-all\bin\141\*.dll" "%Diff%"
%install_only_new% "%ExternalLibs%\xerces-c-3.1.1\Build\Win32\v141_xp\Release_2017\*.dll" "%Diff%"
%install_only_new% "%ExternalLibs%\ZipArchive_4.6.1\ZipArchive\lib\v141_xp\*.dll" "%Diff%"
%install_only_new% "%InternalLibs%\TxFirebird\UDF\XML\fbXml\Release_2017\*.dll" "%Diff%\udf"
%install_only_new% "%InternalLibs%\TxFirebird\UDF\XML\fbXml\SQL\*.sql" "%Diff%\udf"
%install_only_new% "%InternalLibs%\ControledTransactionsCutter\Release_2017\*.dll" "%Diff%"
%install_only_new% "%InternalLibs%\CPCryptoSt\lib\Release_2017\v141_xp\*.dll" "%Diff%"
%install_only_new% "%InternalLibs%\TaxcomExchange\Release_2017\*.dll" "%Diff%"
%install_only_new% "%InternalLibs%\UnifiedFormat\Release_2017\*.dll" "%Diff%"
)
if %deployRuntime% NEQ 0 (
%install% "%VCToolsRedistDir%\x86\Microsoft.VC141.CRT\*.dll" "%MinimalRuntime%"
%install_recursive% "%MinimalRuntime%" "%Runtime%"
%install% "%VCToolsRedistDir%\x86\Microsoft.VC141.MFC\*.dll" "%Runtime%"
%install% "%VCToolsRedistDir%\x86\Microsoft.VC141.MFCLOC\*enu.dll" "%Runtime%"
%install% "%VCToolsRedistDir%\x86\Microsoft.VC141.MFCLOC\*rus.dll" "%Runtime%"
%install_recursive% "%Runtime%" "%FullRuntime%"
%install_recursive_only_new% "%ExternalLibs%\Runtime\Internal_MSCRT_141_xp" "%FullRuntime%"
)

if %deploySingleClient% NEQ 0 (
%install_only_new% "%SingleClient%\Client\Referents\SingleUserReferent\SingleUserReferent\bin\x86\Single_Release\*.config" "%Deploy%\Client"
%install_only_new% "%SingleClient%\Client\Referents\SingleUserReferent\SingleUserReferent\bin\x86\Single_Release\*.dll" "%Deploy%\Client"
%install_only_new% "%SingleClient%\Client\Referents\SingleUserReferent\SingleUserReferent\bin\x86\Single_Release\*.exe" "%Deploy%\Client"
for /d %%d in ("%SingleClient%\Client\Referents\SingleUserReferent\SingleUserReferent\bin\x86\Single_Release\*") do (
%install_only_new% "%%d" "%Deploy%\Client\%%~nd"
)

%install_only_new% "%SingleClient%\Client\Referents\Single_Release\*.config" "%Deploy%\Client"
%install_only_new% "%SingleClient%\Client\Referents\Single_Release\*.dll" "%Deploy%\Client"
%install_only_new% "%SingleClient%\Client\Referents\Single_Release\*.exe" "%Deploy%\Client"
for /d %%d in ("%SingleClient%\Client\Referents\Single_Release\*") do (
%install_only_new% "%%d" "%Deploy%\Client\%%~nd"
)

%install_recursive% "%InternalLibs%\FormatCheck\bin\release_vc17" "%Deploy%\Client"
%install_recursive% "%Diff%" "%Deploy%\Client"
%install_recursive% "%FullRuntime%" "%Deploy%\Client"

if %deployUpdater% NEQ 0 (
%install_only_new% "%Updater%\Updater\Updater\Release\*.config" "%Deploy%\Client\RefNetUpdater"
%install_only_new% "%Updater%\Updater\Updater\Release\*.dll" "%Deploy%\Client\RefNetUpdater"
%install_only_new% "%Updater%\Updater\Updater\Release\*.exe" "%Deploy%\Client\RefNetUpdater"
for /d %%d in ("%Updater%\Updater\Updater\Release\*") do (
%install_only_new% "%%d" "%Deploy%\Client\RefNetUpdater\%%~nd"
)

%install_recursive% "%Diff%" "%Deploy%\Client\RefNetUpdater"
%install_recursive% "%FullRuntime%" "%Deploy%\Client\RefNetUpdater"
)
)

if %deployClient% NEQ 0 (
%install_only_new% "%Client%\Client\Referents\MultiUserReferent\MultiUserReferent\bin\x86\Release\*.config" "%Deploy%\Client"
%install_only_new% "%Client%\Client\Referents\MultiUserReferent\MultiUserReferent\bin\x86\Release\*.dll" "%Deploy%\Client"
%install_only_new% "%Client%\Client\Referents\MultiUserReferent\MultiUserReferent\bin\x86\Release\*.exe" "%Deploy%\Client"
for /d %%d in ("%Client%\Client\Referents\MultiUserReferent\MultiUserReferent\bin\x86\Release\*") do (
%install_only_new% "%%d" "%Deploy%\Client\%%~nd"
)

%install_only_new% "%Client%\Client\Referents\Release\*.config" "%Deploy%\Client"
%install_only_new% "%Client%\Client\Referents\Release\*.dll" "%Deploy%\Client"
%install_only_new% "%Client%\Client\Referents\Release\*.exe" "%Deploy%\Client"
for /d %%d in ("%Client%\Client\Referents\Release\*") do (
%install_only_new% "%%d" "%Deploy%\Client\%%~nd"
)

%install_recursive% "%InternalLibs%\FormatCheck\bin\release_vc17" "%Deploy%\Client"
%install_recursive% "%Diff%" "%Deploy%\Client"
%install_recursive% "%FullRuntime%" "%Deploy%\Client"

if %deployUpdater% NEQ 0 (
%install_only_new% "%Updater%\Updater\Updater\Release\*.config" "%Deploy%\Client\RefNetUpdater"
%install_only_new% "%Updater%\Updater\Updater\Release\*.dll" "%Deploy%\Client\RefNetUpdater"
%install_only_new% "%Updater%\Updater\Updater\Release\*.exe" "%Deploy%\Client\RefNetUpdater"
for /d %%d in ("%Updater%\Updater\Updater\Release\*") do (
%install_only_new% "%%d" "%Deploy%\Client\RefNetUpdater\%%~nd"
)

%install_recursive% "%Diff%" "%Deploy%\Client\RefNetUpdater"
%install_recursive% "%FullRuntime%" "%Deploy%\Client\RefNetUpdater"
)
)

if %deployAdmin% NEQ 0 (
%install_only_new% "%Admin%\Administration\AdminNet\bin\x86\Release\*.config" "%Deploy%\Admin"
%install_only_new% "%Admin%\Administration\AdminNet\bin\x86\Release\*.dll" "%Deploy%\Admin"
%install_only_new% "%Admin%\Administration\AdminNet\bin\x86\Release\*.exe" "%Deploy%\Admin"
for /d %%d in ("%Admin%\Administration\AdminNet\bin\x86\Release\*") do (
%install_only_new% "%%d" "%Deploy%\Admin\%%~nd"
)

%install_only_new% "%Admin%\Administration\Release\*.config" "%Deploy%\Admin"
%install_only_new% "%Admin%\Administration\Release\*.dll" "%Deploy%\Admin"
%install_only_new% "%Admin%\Administration\Release\*.exe" "%Deploy%\Admin"
for /d %%d in ("%Admin%\Administration\Release\*") do (
%install_only_new% "%%d" "%Deploy%\Admin\%%~nd"
)

%install_recursive% "%Diff%" "%Deploy%\Admin"
%install_recursive% "%FullRuntime%" "%Deploy%\Admin"

if %deployUpdater% NEQ 0 (
%install_only_new% "%Updater%\Updater\Updater\Release\*.config" "%Deploy%\Admin\RefNetUpdater"
%install_only_new% "%Updater%\Updater\Updater\Release\*.dll" "%Deploy%\Admin\RefNetUpdater"
%install_only_new% "%Updater%\Updater\Updater\Release\*.exe" "%Deploy%\Admin\RefNetUpdater"
for /d %%d in ("%Updater%\Updater\Updater\Release\*") do (
%install_only_new% "%%d" "%Deploy%\Admin\RefNetUpdater\%%~nd"
)

%install_recursive% "%Diff%" "%Deploy%\Admin\RefNetUpdater"
%install_recursive% "%FullRuntime%" "%Deploy%\Admin\RefNetUpdater"
)
)

if %deployServer% NEQ 0 (
%install_only_new% "%Server%\Server\RefNetServer\Release\*.config" "%Deploy%\RefNetServer"
%install_only_new% "%Server%\Server\RefNetServer\Release\*.dll" "%Deploy%\RefNetServer"
%install_only_new% "%Server%\Server\RefNetServer\Release\*.exe" "%Deploy%\RefNetServer"
for /d %%d in ("%Server%\Server\RefNetServer\Release\*") do (
%install_only_new% "%%d" "%Deploy%\RefNetServer\%%~nd"
)

%install_recursive% "%InternalLibs%\FormatCheck\bin\release_vc17" "%Deploy%\RefNetServer"
%install_recursive% "%Diff%" "%Deploy%\RefNetServer"
%install_recursive% "%FullRuntime%" "%Deploy%\RefNetServer"

if %deployUpdater% NEQ 0 (
%install_only_new% "%Updater%\Updater\Updater\Release\*.config" "%Deploy%\RefNetServer\RefNetUpdater"
%install_only_new% "%Updater%\Updater\Updater\Release\*.dll" "%Deploy%\RefNetServer\RefNetUpdater"
%install_only_new% "%Updater%\Updater\Updater\Release\*.exe" "%Deploy%\RefNetServer\RefNetUpdater"
for /d %%d in ("%Updater%\Updater\Updater\Release\*") do (
%install_only_new% "%%d" "%Deploy%\RefNetServer\RefNetUpdater\%%~nd"
)

%install_recursive% "%Diff%" "%Deploy%\RefNetServer\RefNetUpdater"
%install_recursive% "%FullRuntime%" "%Deploy%\RefNetServer\RefNetUpdater"
)
)

if %deployFirebirdServer% NEQ 0 (
%install_recursive% "%ExternalLibs%\Firebird\Src\output_Win32" "%Deploy%\RefNetServer\FirebirdServerProxy"
%install_recursive% "%ExternalLibs%\Firebird\Src\output_Win32" "%Deploy%\RefNetServer\FirebirdServer"
%install% "%InternalLibs%\TxFirebird\UDF\XML\fbXml\Release_2017\*.dll" "%Deploy%\RefNetServer\FirebirdServerProxy\udf"
%install% "%InternalLibs%\TxFirebird\UDF\XML\fbXml\SQL\*.sql" Deploy%\RefNetServer\FirebirdServerProxy\udf"
%install% "%InternalLibs%\TxFirebird\UDF\XML\fbXml\Release_2017\*.dll" "%Deploy%\RefNetServer\FirebirdServer\udf"
%install% "%InternalLibs%\TxFirebird\UDF\XML\fbXml\SQL\*.sql" Deploy%\RefNetServer\FirebirdServer\udf"
%install_recursive% "%FullRuntime%" "%Deploy%\RefNetServer\FirebirdServerProxy"
%install_recursive% "%FullRuntime%" "%Deploy%\RefNetServer\FirebirdServer"
)