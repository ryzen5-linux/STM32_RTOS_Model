@echo off
setlocal EnableExtensions EnableDelayedExpansion

set "ACTION=%~1"
if "%ACTION%"=="" set "ACTION=all"
if not "%~1"=="" shift
set "MAKE_ARGS="
:collect_make_args
if "%~1"=="" goto :collect_make_args_done
set "MAKE_ARGS=%MAKE_ARGS% "%~1""
shift
goto :collect_make_args

:collect_make_args_done

set "ACTION_OK=0"
for %%A in (all elf hex bin size clean print-vars rebuild) do (
    if /i "%ACTION%"=="%%A" set "ACTION_OK=1"
)
if "%ACTION_OK%"=="0" goto :usage

set "CANDIDATE_DIR=%CD%"
:find_project_root
if exist "%CANDIDATE_DIR%\Gcc\Makefile" goto :project_root_found
for %%I in ("%CANDIDATE_DIR%\..") do set "PARENT_DIR=%%~fI"
if /i "%PARENT_DIR%"=="%CANDIDATE_DIR%" (
    echo [ERROR] Project root not found. Run this script from the project tree.
    exit /b 1
)
set "CANDIDATE_DIR=%PARENT_DIR%"
goto :find_project_root

:project_root_found
set "PROJECT_ROOT=%CANDIDATE_DIR%"
set "GCC_DIR=%PROJECT_ROOT%\Gcc"
set "SCRIPT_DIR=%GCC_DIR%\scripts"
for %%I in ("%PROJECT_ROOT%\..") do set "PROJECT_PARENT=%%~fI"

set "BUILD_DIR=%GCC_DIR%\build"
set "MAKEFILE=%GCC_DIR%\Makefile"
set "AUTO_MK=%BUILD_DIR%\auto_sources.mk"
set "LOG_FILE=%BUILD_DIR%\build.log"

if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
break > "%LOG_FILE%"

call :find_make
if errorlevel 1 (
    call :log "ERROR: No make tool found in PATH. Install mingw32-make or make, then retry."
    exit /b 1
)

call :find_toolchain
if errorlevel 1 (
    call :log "ERROR: Unable to locate GNU Arm Embedded Toolchain under %PROJECT_PARENT%\GNU Arm Embedded Toolchain"
    exit /b 1
)

call :write_auto_manifest
if errorlevel 1 (
    call :log "ERROR: Failed to generate auto source manifest."
    exit /b 1
)

call :cleanup_stale_intermediates

call :log "Build script started"
call :log "Action=%ACTION%"
call :log "ProjectRoot=%PROJECT_ROOT%"
call :log "GccDir=%GCC_DIR%"
call :log "BuildDir=%BUILD_DIR%"
call :log "Makefile=%MAKEFILE%"
call :log "AutoManifest=%AUTO_MK%"
call :log "ToolchainBin=%TOOLCHAIN_BIN%"
call :log "Make=%MAKE_EXE%"
call :log "ExtraMakeArgs=%MAKE_ARGS%"
call :log "Log file=%LOG_FILE%"

call :write_manifest_summary

pushd "%GCC_DIR%"

if /i "%ACTION%"=="rebuild" (
    call :log "Running make clean"
    call :run_make clean
    set "RC=%ERRORLEVEL%"
    if not "%RC%"=="0" (
        call :log "Build failed with exit code %RC%"
        popd
        exit /b %RC%
    )

    call :log "Running make all"
    call :run_make all
    set "RC=%ERRORLEVEL%"
    call :log "Build finished with exit code %RC%"
    call :cleanup_stale_intermediates
    popd
    exit /b %RC%
)

call :log "Running make %ACTION%"
call :run_make %ACTION%
set "RC=%ERRORLEVEL%"
call :log "Build finished with exit code %RC%"
call :cleanup_stale_intermediates
popd
exit /b %RC%

:usage
echo Usage: %~nx0 [all^|elf^|hex^|bin^|size^|clean^|print-vars^|rebuild] [extra make args...]
exit /b 1

:find_make
set "MAKE_EXE="
for %%M in (mingw32-make.exe make.exe gmake.exe) do (
    for /f "delims=" %%P in ('where %%M 2^>nul') do (
        if not defined MAKE_EXE set "MAKE_EXE=%%P"
    )
)
if defined MAKE_EXE exit /b 0
exit /b 1

:find_toolchain
set "TOOLCHAIN_ROOT=%PROJECT_PARENT%\GNU Arm Embedded Toolchain"
if not exist "%TOOLCHAIN_ROOT%" exit /b 1

if exist "%TOOLCHAIN_ROOT%\bin\arm-none-eabi-gcc.exe" (
    set "TOOLCHAIN_BIN=%TOOLCHAIN_ROOT%\bin"
    call :to_forward_slash TOOLCHAIN_BIN
    exit /b 0
)

for /f "delims=" %%D in ('dir /b /ad /o-n "%TOOLCHAIN_ROOT%" 2^>nul') do (
    if exist "%TOOLCHAIN_ROOT%\%%D\bin\arm-none-eabi-gcc.exe" (
        set "TOOLCHAIN_BIN=%TOOLCHAIN_ROOT%\%%D\bin"
        call :to_forward_slash TOOLCHAIN_BIN
        exit /b 0
    )
)

exit /b 1

:to_forward_slash
set "%~1=!%~1:\=/!"
exit /b 0

:trim_leading_space
for /f "tokens=* delims= " %%T in ("!%~1!") do set "%~1=%%T"
exit /b 0

:is_allowed_path
set "REL=%~1"
if /i "!REL:~0,10!"=="Gcc/build/" exit /b 1

if /i "!REL:~0,24!"=="FreeRTOS/Source/portable" (
    if /i "!REL!"=="FreeRTOS/Source/portable/GCC/ARM_CM3/port.c" exit /b 0
    if /i "!REL!"=="FreeRTOS/Source/portable/GCC/ARM_CM3/portmacro.h" exit /b 0
    if /i "!REL!"=="FreeRTOS/Source/portable/MemMang/heap_4.c" exit /b 0
    exit /b 1
)

exit /b 0

:inc_module_count
set "MODULE=%~1"
set "TYPE=%~2"
set /a %MODULE%_%TYPE%+=1
exit /b 0

:accumulate_module
set "PATH_REL=%~1"
set "PATH_TYPE=%~2"

if /i "!PATH_REL:~0,4!"=="BSP/" (
    call :inc_module_count BSP %PATH_TYPE%
    exit /b 0
)
if /i "!PATH_REL:~0,7!"=="Driver/" (
    call :inc_module_count DRIVER %PATH_TYPE%
    exit /b 0
)
if /i "!PATH_REL:~0,16!"=="FreeRTOS/Source/" (
    call :inc_module_count FREERTOS_KERNEL %PATH_TYPE%
    exit /b 0
)
if /i "!PATH_REL:~0,13!"=="FreeRTOS/App/" (
    call :inc_module_count FREERTOS_APP %PATH_TYPE%
    exit /b 0
)
if /i "!PATH_REL:~0,10!"=="Libraries/" (
    call :inc_module_count LIB %PATH_TYPE%
    exit /b 0
)

call :inc_module_count OTHER %PATH_TYPE%
exit /b 0

:write_auto_manifest
set "TMP_C=%BUILD_DIR%\manifest_c_%RANDOM%%RANDOM%.tmp"
set "TMP_ASM=%BUILD_DIR%\manifest_asm_%RANDOM%%RANDOM%.tmp"
set "TMP_INC=%BUILD_DIR%\manifest_inc_%RANDOM%%RANDOM%.tmp"
set "TMP_C_U=%BUILD_DIR%\manifest_c_u_%RANDOM%%RANDOM%.tmp"
set "TMP_ASM_U=%BUILD_DIR%\manifest_asm_u_%RANDOM%%RANDOM%.tmp"
set "TMP_INC_U=%BUILD_DIR%\manifest_inc_u_%RANDOM%%RANDOM%.tmp"

break > "!TMP_C!"
break > "!TMP_ASM!"
break > "!TMP_INC!"

set /a SRC_C_COUNT=0
set /a SRC_ASM_COUNT=0
set /a INC_DIR_COUNT=0
set "SRC_C_LIST="
set "SRC_ASM_LIST="
set "INC_DIR_LIST="

set /a BSP_C=0
set /a BSP_ASM=0
set /a DRIVER_C=0
set /a DRIVER_ASM=0
set /a FREERTOS_KERNEL_C=0
set /a FREERTOS_KERNEL_ASM=0
set /a FREERTOS_APP_C=0
set /a FREERTOS_APP_ASM=0
set /a LIB_C=0
set /a LIB_ASM=0
set /a OTHER_C=0
set /a OTHER_ASM=0

for /r "%PROJECT_ROOT%" %%F in (*.c *.s *.h) do (
    set "FULL=%%~fF"
    set "REL=!FULL:%PROJECT_ROOT%\=!"
    set "REL=!REL:\=/!"

    call :is_allowed_path "!REL!"
    if !ERRORLEVEL! EQU 0 (
        set "EXT=%%~xF"
        if /i "!EXT!"==".c" (
            >> "!TMP_C!" echo !REL!
        )
        if /i "!EXT!"==".s" (
            >> "!TMP_ASM!" echo !REL!
        )
        if /i "!EXT!"==".h" (
            set "DIR_REL=%%~dpF"
            set "DIR_REL=!DIR_REL:%PROJECT_ROOT%\=!"
            set "DIR_REL=!DIR_REL:\=/!"
            if "!DIR_REL:~-1!"=="/" set "DIR_REL=!DIR_REL:~0,-1!"
            if not "!DIR_REL!"=="." (
                >> "!TMP_INC!" echo !DIR_REL!
            )
        )
    )
)

sort /unique "!TMP_C!" > "!TMP_C_U!"
sort /unique "!TMP_ASM!" > "!TMP_ASM_U!"
sort /unique "!TMP_INC!" > "!TMP_INC_U!"

for /f "usebackq delims=" %%L in ("!TMP_C_U!") do (
    if not "%%L"=="" (
        set /a SRC_C_COUNT+=1
        set "SRC_C_LIST=!SRC_C_LIST! %%L"
        call :accumulate_module "%%L" C
    )
)

for /f "usebackq delims=" %%L in ("!TMP_ASM_U!") do (
    if not "%%L"=="" (
        set /a SRC_ASM_COUNT+=1
        set "SRC_ASM_LIST=!SRC_ASM_LIST! %%L"
        call :accumulate_module "%%L" ASM
    )
)

for /f "usebackq delims=" %%L in ("!TMP_INC_U!") do (
    if not "%%L"=="" (
        set /a INC_DIR_COUNT+=1
        set "INC_DIR_LIST=!INC_DIR_LIST! %%L"
    )
)

del /f /q "!TMP_C!" "!TMP_ASM!" "!TMP_INC!" "!TMP_C_U!" "!TMP_ASM_U!" "!TMP_INC_U!" >nul 2>&1

call :trim_leading_space SRC_C_LIST
call :trim_leading_space SRC_ASM_LIST
call :trim_leading_space INC_DIR_LIST

(
    echo AUTO_SRC_C := !SRC_C_LIST!
    echo AUTO_SRC_ASM := !SRC_ASM_LIST!
    echo AUTO_INC_DIRS := !INC_DIR_LIST!
) > "%AUTO_MK%"

exit /b 0

:write_manifest_summary
set /a SRC_TOTAL_COUNT=SRC_C_COUNT+SRC_ASM_COUNT
set "ASM_WORD=files"
if "%SRC_ASM_COUNT%"=="1" set "ASM_WORD=file"

set /a ACTIVE_MODULES=0
set /a MOD_SUM=BSP_C+BSP_ASM
if not "%MOD_SUM%"=="0" set /a ACTIVE_MODULES+=1
set /a MOD_SUM=DRIVER_C+DRIVER_ASM
if not "%MOD_SUM%"=="0" set /a ACTIVE_MODULES+=1
set /a MOD_SUM=FREERTOS_KERNEL_C+FREERTOS_KERNEL_ASM
if not "%MOD_SUM%"=="0" set /a ACTIVE_MODULES+=1
set /a MOD_SUM=FREERTOS_APP_C+FREERTOS_APP_ASM
if not "%MOD_SUM%"=="0" set /a ACTIVE_MODULES+=1
set /a MOD_SUM=LIB_C+LIB_ASM
if not "%MOD_SUM%"=="0" set /a ACTIVE_MODULES+=1
set /a MOD_SUM=OTHER_C+OTHER_ASM
if not "%MOD_SUM%"=="0" set /a ACTIVE_MODULES+=1

set "MOD_WORD=modules"
if "%ACTIVE_MODULES%"=="1" set "MOD_WORD=module"

call :log "This build will compile %SRC_TOTAL_COUNT% source files in total (%SRC_C_COUNT% C files, %SRC_ASM_COUNT% assembly %ASM_WORD%), organized across %ACTIVE_MODULES% %MOD_WORD%."

set /a MOD_SUM=BSP_C+BSP_ASM
if not "%MOD_SUM%"=="0" call :log "  Board Support Package (BSP) -- %BSP_C% C files, %BSP_ASM% assembly files"
set /a MOD_SUM=DRIVER_C+DRIVER_ASM
if not "%MOD_SUM%"=="0" call :log "  Peripheral device drivers -- %DRIVER_C% C files, %DRIVER_ASM% assembly files"
set /a MOD_SUM=FREERTOS_KERNEL_C+FREERTOS_KERNEL_ASM
if not "%MOD_SUM%"=="0" call :log "  FreeRTOS kernel -- %FREERTOS_KERNEL_C% C files, %FREERTOS_KERNEL_ASM% assembly files"
set /a MOD_SUM=FREERTOS_APP_C+FREERTOS_APP_ASM
if not "%MOD_SUM%"=="0" call :log "  FreeRTOS application -- %FREERTOS_APP_C% C files, %FREERTOS_APP_ASM% assembly files"
set /a MOD_SUM=LIB_C+LIB_ASM
if not "%MOD_SUM%"=="0" call :log "  STM32 standard peripheral library -- %LIB_C% C files, %LIB_ASM% assembly files"
set /a MOD_SUM=OTHER_C+OTHER_ASM
if not "%MOD_SUM%"=="0" call :log "  Other -- %OTHER_C% C files, %OTHER_ASM% assembly files"

set "DIR_WORD=directories"
if "%INC_DIR_COUNT%"=="1" set "DIR_WORD=directory"
call :log "The compiler will search for headers in %INC_DIR_COUNT% include %DIR_WORD%."
if not "%SRC_C_LIST%"=="" call :log "C source files: %SRC_C_LIST%"
if not "%SRC_ASM_LIST%"=="" call :log "Assembly source files: %SRC_ASM_LIST%"
if not "%INC_DIR_LIST%"=="" call :log "Header search directories: %INC_DIR_LIST%"

exit /b 0

:run_make
setlocal
set "MAKE_GOAL=%~1"
set "TMP_OUT=%BUILD_DIR%\make_%RANDOM%%RANDOM%.tmp"

"%MAKE_EXE%" -f "%MAKEFILE%" HOST_OS=windows BUILD_DIR=build "TOOLCHAIN_BIN=%TOOLCHAIN_BIN%" AUTO_MK=build/auto_sources.mk %MAKE_ARGS% %MAKE_GOAL% > "%TMP_OUT%" 2>&1
set "MAKE_RC=%ERRORLEVEL%"

if exist "%TMP_OUT%" (
    type "%TMP_OUT%"
    type "%TMP_OUT%" >> "%LOG_FILE%"
    del /f /q "%TMP_OUT%" >nul 2>&1
)

set "RC=%MAKE_RC%"
endlocal & exit /b %RC%

:cleanup_stale_intermediates
del /f /q "%BUILD_DIR%\print-vars.*" >nul 2>&1
del /f /q "%BUILD_DIR%\all.*" >nul 2>&1
if exist "%GCC_DIR%\0" rd /s /q "%GCC_DIR%\0" >nul 2>&1
if exist "%PROJECT_ROOT%\0" rd /s /q "%PROJECT_ROOT%\0" >nul 2>&1
exit /b 0

:log
set "TS_DATE=%date:~0,10%"
set "TS=%TS_DATE% %time:~0,8%"
set "MSG=%~1"
echo [%TS%] %MSG%
>> "%LOG_FILE%" echo [%TS%] %MSG%
exit /b 0
