set internal_libs=E:\SVN\Internal_Libs_new
set issue=REFNET-7301

set mk_source=FormatCheck\bin\release_vc17
set buildserver=\\orio-release\FeatureTest
set MKbranch=docliner\MKbranch

set start_directory=%CD%
set client_mk_destination=%buildserver%\%issue%\%MKbranch%\Client
set mk_destination=%buildserver%\%issue%\%MKbranch%\MK
set mk_root_source=%internal_libs%\%mk_source%
set DocEngine_source=%mk_root_source%\DocEngine
set Renamer_source=%mk_root_source%\Renamer
set mk_sub_root_source=%mk_root_source%\FormatCheck
set mk_for_build=%start_directory%\..\..\Client\ReferentNet\ReferentNet\FormatCheck
set mk_for_build_tests=%start_directory%\..\..\Client\Referents\AutoTests\UnitTests\CheckAndSaveTests

set install=xcopy /z /y /i /f /r
set install_recursive=xcopy /z /e /y /i /f /r
set uninstall=rmdir /S /Q

(svn update %internal_libs% && (%uninstall% %buildserver%\%issue%\%MKbranch% || echo Nothing to uninstall from MKbranch. It's OK.) && mkdir %mk_destination% && mkdir %client_mk_destination% && git pull && %install% %mk_root_source%\FormatCheck*.dll %mk_for_build% && %install% %mk_root_source%\FormatCheck.dll %mk_for_build_tests% && git commit -m "Updating MK" && git push && %install% %mk_root_source%\*.* %mk_destination% && %install_recursive% %mk_sub_root_source% %mk_destination%\FormatCheck && %install_recursive% %DocEngine_source% %client_mk_destination%\DocEngine && %install_recursive% %Renamer_source% %client_mk_destination%\Renamer && echo All processed successfuly.) || echo Error occured.

pause