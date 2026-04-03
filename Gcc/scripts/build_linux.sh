#!/usr/bin/env bash

set -euo pipefail

ACTION="${1:-all}"
if [[ $# -gt 0 ]]; then
    shift
fi

MAKE_ARGS=("$@")

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
GCC_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
PROJECT_ROOT="$(cd "${GCC_DIR}/.." && pwd)"
BUILD_DIR="${GCC_DIR}/build"
AUTO_MK="${BUILD_DIR}/auto_sources.mk"
MAKEFILE="${GCC_DIR}/Makefile"
LOG_FILE="${BUILD_DIR}/build.log"

log() {
    printf '[%s] %s\n' "$(date '+%Y-%m-%d %H:%M:%S')" "$*"
}

is_allowed_path() {
    local rel="$1"

    if [[ "${rel}" == Gcc/build/* ]]; then
        return 1
    fi

    if [[ "${rel}" == FreeRTOS/Source/portable/* ]]; then
        [[ "${rel}" == "FreeRTOS/Source/portable/GCC/ARM_CM3/port.c" ]] && return 0
        [[ "${rel}" == "FreeRTOS/Source/portable/GCC/ARM_CM3/portmacro.h" ]] && return 0
        [[ "${rel}" == "FreeRTOS/Source/portable/MemMang/heap_4.c" ]] && return 0
        return 1
    fi

    return 0
}

write_auto_manifest() {
    local c_sources=()
    local asm_sources=()
    local include_dirs=()
    local rel
    local dir

    while IFS= read -r -d '' file; do
        rel="${file#${PROJECT_ROOT}/}"
        rel="${rel//\\//}"

        if ! is_allowed_path "${rel}"; then
            continue
        fi

        case "${rel}" in
            *.c)
                c_sources+=("${rel}")
                ;;
            *.s|*.S)
                asm_sources+=("${rel}")
                ;;
            *.h)
                dir="$(dirname "${rel}")"
                [[ "${dir}" != "." ]] && include_dirs+=("${dir}")
                ;;
        esac
    done < <(find "${PROJECT_ROOT}" -type f \( -name '*.c' -o -name '*.s' -o -name '*.S' -o -name '*.h' \) -print0)

    mkdir -p "${BUILD_DIR}"

    mapfile -t c_sources < <(printf '%s\n' "${c_sources[@]}" | LC_ALL=C sort -u)
    mapfile -t asm_sources < <(printf '%s\n' "${asm_sources[@]}" | LC_ALL=C sort -u)
    mapfile -t include_dirs < <(printf '%s\n' "${include_dirs[@]}" | LC_ALL=C sort -u)

    {
        printf 'AUTO_SRC_C := %s\n' "${c_sources[*]:-}"
        printf 'AUTO_SRC_ASM := %s\n' "${asm_sources[*]:-}"
        printf 'AUTO_INC_DIRS := %s\n' "${include_dirs[*]:-}"
    } > "${AUTO_MK}"

    MANIFEST_C_SOURCES=("${c_sources[@]}")
    MANIFEST_ASM_SOURCES=("${asm_sources[@]}")
    MANIFEST_INCLUDE_DIRS=("${include_dirs[@]}")
}

get_source_module_name() {
    local path="$1"
    if [[ "${path}" == BSP/* ]];              then echo "Board Support Package (BSP)"
    elif [[ "${path}" == Driver/* ]];         then echo "Peripheral device drivers"
    elif [[ "${path}" == FreeRTOS/Source/* ]]; then echo "FreeRTOS kernel"
    elif [[ "${path}" == FreeRTOS/App/* ]];   then echo "FreeRTOS application"
    elif [[ "${path}" == Libraries/* ]];      then echo "STM32 standard peripheral library"
    else echo "Other"
    fi
}

write_manifest_summary() {
    local total_source_count=$(( ${#MANIFEST_C_SOURCES[@]} + ${#MANIFEST_ASM_SOURCES[@]} ))
    local asm_word="files"; [[ ${#MANIFEST_ASM_SOURCES[@]} -eq 1 ]] && asm_word="file"

    local -a module_prefixes=("BSP/" "Driver/" "FreeRTOS/Source/" "FreeRTOS/App/" "Libraries/")
    local -a module_names=("Board Support Package (BSP)" "Peripheral device drivers" "FreeRTOS kernel" "FreeRTOS application" "STM32 standard peripheral library")

    # Count active modules
    local group_count=0
    local i
    for (( i=0; i<${#module_prefixes[@]}; i++ )); do
        local prefix="${module_prefixes[$i]}"
        local has=0
        for f in "${MANIFEST_C_SOURCES[@]:-}";   do [[ "${f}" == ${prefix}* ]] && has=1 && break; done
        for f in "${MANIFEST_ASM_SOURCES[@]:-}"; do [[ "${f}" == ${prefix}* ]] && has=1 && break; done
        [[ ${has} -eq 1 ]] && group_count=$(( group_count + 1 ))
    done
    local mod_word="modules"; [[ ${group_count} -eq 1 ]] && mod_word="module"

    log "This build will compile ${total_source_count} source files in total (${#MANIFEST_C_SOURCES[@]} C files, ${#MANIFEST_ASM_SOURCES[@]} assembly ${asm_word}), organized across ${group_count} ${mod_word}."

    for (( i=0; i<${#module_prefixes[@]}; i++ )); do
        local prefix="${module_prefixes[$i]}"
        local name="${module_names[$i]}"
        local -a c_in_group=() asm_in_group=()

        for f in "${MANIFEST_C_SOURCES[@]:-}";   do [[ "${f}" == ${prefix}* ]] && c_in_group+=("${f}");   done
        for f in "${MANIFEST_ASM_SOURCES[@]:-}"; do [[ "${f}" == ${prefix}* ]] && asm_in_group+=("${f}"); done

        [[ ${#c_in_group[@]} -eq 0 && ${#asm_in_group[@]} -eq 0 ]] && continue

        local parts=""
        if [[ ${#c_in_group[@]} -gt 0 ]]; then
            local c_word="files"; [[ ${#c_in_group[@]} -eq 1 ]] && c_word="file"
            parts="${#c_in_group[@]} C ${c_word}"
        fi
        if [[ ${#asm_in_group[@]} -gt 0 ]]; then
            local a_word="files"; [[ ${#asm_in_group[@]} -eq 1 ]] && a_word="file"
            [[ -n "${parts}" ]] && parts="${parts}, "
            parts="${parts}${#asm_in_group[@]} assembly ${a_word}"
        fi

        log "  ${name} -- ${parts}:"
        for f in "${c_in_group[@]}";   do log "    ${f}"; done
        for f in "${asm_in_group[@]}"; do log "    ${f}  [assembly]"; done
    done

    local dir_word="directories"; [[ ${#MANIFEST_INCLUDE_DIRS[@]} -eq 1 ]] && dir_word="directory"
    log "The compiler will search for headers in ${#MANIFEST_INCLUDE_DIRS[@]} include ${dir_word}:"
    for dir in "${MANIFEST_INCLUDE_DIRS[@]:-}"; do log "    ${dir}"; done
}

write_auto_manifest

mkdir -p "${BUILD_DIR}"
: > "${LOG_FILE}"

exec > >(tee -a "${LOG_FILE}") 2>&1

log "Build script started"
log "Action=${ACTION}"
log "ProjectRoot=${PROJECT_ROOT}"
log "GccDir=${GCC_DIR}"
log "BuildDir=${BUILD_DIR}"
log "Makefile=${MAKEFILE}"
log "AutoManifest=${AUTO_MK}"
log "Log file=${LOG_FILE}"
log "ExtraMakeArgs=${MAKE_ARGS[*]:-}"

write_manifest_summary

cd "${GCC_DIR}"

log "Running make print-vars"
make -f "${MAKEFILE}" HOST_OS=linux AUTO_MK="build/auto_sources.mk" "${MAKE_ARGS[@]}" print-vars

case "${ACTION}" in
    rebuild)
        log "Running make clean"
        make -f "${MAKEFILE}" HOST_OS=linux AUTO_MK="build/auto_sources.mk" "${MAKE_ARGS[@]}" clean
        log "Running make all"
        make -f "${MAKEFILE}" HOST_OS=linux AUTO_MK="build/auto_sources.mk" "${MAKE_ARGS[@]}" all
        ;;
    all|elf|hex|bin|size|clean|print-vars)
        log "Running make ${ACTION}"
        make -f "${MAKEFILE}" HOST_OS=linux AUTO_MK="build/auto_sources.mk" "${MAKE_ARGS[@]}" "${ACTION}"
        ;;
    *)
        echo "Unsupported action: ${ACTION}" >&2
        echo "Usage: $(basename "$0") [all|elf|hex|bin|size|clean|print-vars|rebuild]" >&2
        exit 1
        ;;
esac

log "Build finished successfully"