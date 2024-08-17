#!/bin/bash

# MIT License

# Copyright (c) 2024 Kevin Alavik

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
MAGENTA='\033[0;35m'
CYAN='\033[0;36m'
WHITE='\033[0;37m'
NC='\033[0m'

OK=1
ERROR=2
WARN=3
INFO=4
DEBUG=5
TRACE=6

LOG_LEVEL=$WARN

CACHE_FILE=".fenix-cache"

function log {
    local level="$1"
    shift
    local message="$@"
    local timestamp=$(date "+%Y-%m-%d %H:%M:%S")

    if [[ $level -le $LOG_LEVEL ]]; then
        case $level in
            $ERROR) color=$RED; prefix="🛑 ERROR:" ;;
            $WARN)  color=$YELLOW; prefix="⚠️  WARN:" ;;
            $INFO)  color=$BLUE; prefix="ℹ️  INFO:" ;;
            $DEBUG) color=$MAGENTA; prefix="🔍 DEBUG:" ;;
            $TRACE) color=$CYAN; prefix="🔎 TRACE:" ;;
            $OK)    color=$GREEN; prefix="✅ OK:" ;;
            *)      color=$NC; prefix="LOG:" ;;
        esac
        while IFS= read -r line; do
            echo -e "${color}${timestamp} ${prefix} ${line}${NC}"
        done <<< "$message"
    fi
}

function usage {
    log $INFO "Usage: $0 <dir> [<target>]"
    log $INFO "    <dir>      Directory containing the 'build' recipe (required)"
    log $INFO "    <target>   Target to build or 'clean' to remove build artifacts (optional)"
}

function load_build_script {
    local dir=$1
    local build_script="$dir/build"

    if [[ -f "$build_script" && -x "$build_script" ]]; then
        log $INFO "Loading build script from \"$build_script\""
        source "$build_script"
    else
        log $ERROR "'build' script not found or not executable in $dir."
        exit 1
    fi
}

function get_value {
    local var_name="$1"
    if [[ -z "${!var_name+x}" ]]; then
        echo "@null"
    else
        echo "${!var_name}"
    fi
}

function check_value {
    local var_name="$1"
    local result=$(get_value "$var_name")
    log $TRACE "Checking $var_name, result=${result[@]}"
    
    if [[ "$result" == "@null" ]]; then
        log $ERROR "Didn't find \"$var_name\""
        exit 1
    fi
}

function check_default_target {
    log $TRACE "Checking default target presence in targets array."
    
    if [[ ! ${#targets[@]} -gt 1 ]]; then
        IFS=',' read -r -a targets <<< "${targets[0]}"
    fi

    local found=0
    for target in "${targets[@]}"; do
        if [[ "$target" == "$default_target" ]]; then
            found=1
            break
        fi
    done

    if [[ "$default_target" == "clean" ]]; then
        found=1
    fi

    if [[ $found -eq 0 ]]; then
        log $ERROR "Target \"$default_target\" not found."
        exit 1
    fi
}

function set_dir {
    log $TRACE "Changing directory to $1"
    pushd "$1" > /dev/null || { log $ERROR "Failed to change directory to $1"; exit 1; }
}

function revert_dir {
    log $TRACE "Reverting directory"
    popd > /dev/null || { log $ERROR "Failed to revert directory"; exit 1; }
}

function check_file {
    local file="$1"

    if ! [[ -f "$file" ]]; then
        log $ERROR "File '$file' does not exist or is not a regular file."
        exit 1
    fi
}

function check_function {
    local func_name="$1"

    if ! declare -f "$func_name" > /dev/null; then
        log $WARN "Function '$func_name' does not exist."
    fi
}

function spawn_command {
    local command="$1"
    shift
    local args=("$@")
    
    log $TRACE "Spawning command: $command ${args[@]}"
    echo "+ $command ${args[@]}" 
    "$command" "${args[@]}" &
    local pid=$!
    wait $pid
    
    if [[ $? -ne 0 ]]; then
        log $ERROR "Command \"$command ${args[@]}\" failed with exit code $?"
    else
        log $TRACE "Command \"$command ${args[@]}\" completed successfully."
    fi
}

function spawn_commands_parallel {
    local commands=("$@")
    
    local pids=()
    
    for cmd in "${commands[@]}"; do
        log $TRACE "Spawning command: $cmd"
        echo "+ $cmd"
        eval "$cmd" &
        pids+=($!)
    done
    
    for pid in "${pids[@]}"; do
        wait $pid
        if [[ $? -ne 0 ]]; then
            log $ERROR "Command with PID $pid failed."
        fi
    done
    
    log $TRACE "All parallel commands completed successfully."
}

function expand_wildcards {
    local pattern="$1"
    local result=()

    while IFS= read -r file; do
        result+=("$file")
    done < <(find . -type f -name "$(basename "$pattern")")

    echo "${result[@]}"
}

function check_exec {
    local exec="$1"

    if ! command -v "$exec" > /dev/null; then
        log $ERROR "Command '$exec' not found."
        exit 1
    fi
}

function update_cache {
    log $TRACE "Updating cache"
    > "$CACHE_FILE"
    for src_file in "${src_files_decoded[@]}"; do
        local file_hash=$(sha256sum "$src_file" | awk '{ print $1 }')
        echo "$src_file $file_hash" >> "$CACHE_FILE"
    done
}

function cache_is_valid {
    local valid=1
    
    if [[ -f "$CACHE_FILE" ]]; then
        declare -A cache_file_data
        
        while IFS= read -r line; do
            local file hash
            read -r file hash <<< "$line"
            cache_file_data["$file"]="$hash"
        done < "$CACHE_FILE"
        
        for src_file in "${src_files_decoded[@]}"; do
            if [[ ! -f "$src_file" ]]; then
                log $WARN "Cached source file '$src_file' is missing."
                valid=0
                break
            fi
            local current_hash
            current_hash=$(sha256sum "$src_file" | awk '{ print $1 }')
            if [[ "${cache_file_data[$src_file]}" != "$current_hash" ]]; then
                log $WARN "Source file '$src_file' has changed."
                valid=0
                break
            fi
        done
    else
        log $WARN "Cache file '$CACHE_FILE' does not exist. Creating new cache."
        valid=0
    fi
    
    if [[ $valid -eq 0 ]]; then
        update_cache
    fi
    
    return $valid
}


function clean {
    log $INFO "Cleaning build artifacts."

    clean_targets=$(get_value clean)
    log $TRACE "Clean targets: ${clean_targets[@]}"
    
    for target_file in "${clean_targets[@]}"; do
        log $TRACE "Loading target \"$target_file.target\""
        if [[ -f "$target_file.target" ]]; then
            source "$target_file.target"
        else
            log $ERROR "Clean script $target_file not found or not executable."
            exit 1
        fi
    done

    if [[ -d "$build_dir" ]]; then
        log $INFO "Removing build directory $build_dir"
        rm -rf "$build_dir"
    else
        log $WARN "Build directory $build_dir does not exist."
    fi

    if [[ -d "$bin_dir" ]]; then
        log $INFO "Removing output directory $bin_dir"
        rm -rf "$bin_dir"
    else
        log $WARN "Output directory $bin_dir does not exist."
    fi

    if [[ -f "$CACHE_FILE" ]]; then
        log $INFO "Removing cache file $CACHE_FILE"
        rm "$CACHE_FILE"
    fi

    log $OK "Clean operation completed successfully."
}

function build_c_simple {
    if [[ $cleaning == 1 ]]; then
        log $DEBUG "Cleaning targets"
        clean
        exit 0
    fi

    check_value src_files

    src_files_decoded=()

    for pattern in "${src_files[@]}"; do
        if [[ "$pattern" == *\** ]]; then
            files=($(expand_wildcards "$pattern"))
        else
            files=($(eval echo "$pattern"))
        fi

        if [[ ${#files[@]} -eq 0 ]]; then
            log $WARN "No files matched the pattern '$pattern'."
        else
            src_files_decoded+=("${files[@]}")
        fi
    done

    log $TRACE "Source files: ${src_files_decoded[@]}"

    update_cache

    if cache_is_valid; then
        log $INFO "Cache is valid. Skipping build."
        exit 0
    fi

    cc=$(get_value compiler)
    [[ "$cc" == "@null" ]] && cc="gcc"
    check_exec "$cc"
    log $DEBUG "Using compiler: $cc"

    ld=$(get_value linker)
    [[ "$ld" == "@null" || -z "$ld" ]] && ld="$cc"
    check_exec "$ld"
    log $DEBUG "Using linker: $ld"

    ccflags=$(get_value cflags)
    ldflags=$(get_value lflags)

    obj_dir=$(get_value build_dir)
    [[ "$obj_dir" == "@null" ]] && obj_dir="obj"
    mkdir -p "$obj_dir"
    log $DEBUG "Using build directory: $obj_dir"

    out_dir=$(get_value bin_dir)
    [[ "$out_dir" == "@null" ]] && out_dir="bin"
    mkdir -p "$out_dir"
    log $DEBUG "Using bin directory: $out_dir"

    object_files=()
    compile_commands=()
    for src_file in "${src_files_decoded[@]}"; do
        base_name=$(basename "$src_file")
        obj_file="$obj_dir/${base_name%.*}.o"

        if [[ ! -f "$obj_file" || "$src_file" -nt "$obj_file" ]]; then
            compile_command="$cc $ccflags -c $src_file -o $obj_file"
            compile_commands+=("$compile_command")
            object_files+=("$obj_file")
            log $TRACE "Object file $obj_file needs to be rebuilt."
        else
            log $TRACE "Object file $obj_file is up-to-date."
        fi
    done

    if [[ ${#compile_commands[@]} -gt 0 ]]; then
        log $TRACE "Compiling object files in parallel"
        spawn_commands_parallel "${compile_commands[@]}"
    else
        log $INFO "No source files need recompilation."
    fi

    object_files=($(find "$obj_dir" -name '*.o'))

    if [[ ${#object_files[@]} -eq 0 ]]; then
        log $ERROR "No object files found in $obj_dir."
        exit 1
    fi

    output_file="$out_dir/$(basename "${src_files_decoded[0]}" .c)"
    link_command="$ld $ldflags -o \"$output_file\" ${object_files[@]}"
    log $TRACE "Linking object files into $output_file"
    spawn_command "$ld" $ldflags -o "$output_file" "${object_files[@]}"
    
    if [[ $? -ne 0 ]]; then
        log $ERROR "Linking failed."
        exit 1
    fi

    log $OK "Build successful. Executable is located at $(pwd)/$output_file"
}

function build {
    log $TRACE "Checking for target file $default_target.target"
    check_file "$default_target.target"
    source "$default_target.target"
    
    local func_name="${default_target}_pre"
    log $TRACE "Checking if function $func_name exists."
    check_function "$func_name"
    if [[ $(declare -f "$func_name") ]]; then
        log $DEBUG "Executing function $func_name"
        $func_name
    else
        log $DEBUG "Function $func_name does not exist, skipping."
    fi

    handle_project_type $project_type

    case $project_type in
        C-Simple)
            log $DEBUG "Identified C-Simple project"
            build_c_simple
            ;;
        *)
            log $ERROR "Unhandled project type: $project_type"
            ;;
    esac
}

function pre_build {
    log $TRACE "Checking build values"
    check_value project_kind
    check_value targets
    check_value default_target

    check_default_target

    if [[ "$default_target" == "clean" ]]; then
        cleaning=true
    fi

    log $DEBUG "Project Kind: $project_kind"
    log $DEBUG "Targets: ${targets[@]}"
    log $DEBUG "Default Target: $default_target"
    
    set_dir "$dir"
    build
    revert_dir
}

function handle_project_type {
    local project_type=$1
    log $TRACE "Checking project type $project_type"
    case $project_type in
        C-Simple)
            log $DEBUG "Valid project type"
            ;;
        *)
            log $ERROR "Unknown project type: $project_type"
            exit 1
            ;;
    esac
}

if [[ $# -lt 1 ]]; then
    log $ERROR "Directory argument is required."
    usage
    exit 1
fi

dir=$1
target=${2:-}
project_type=''

cleaning=false

if [[ -d "$dir" ]]; then
    log $INFO "Loading build script from directory $dir"
    load_build_script "$dir"
    if [[ "$target" == "clean" ]]; then
        default_target="clean"
        cleaning=true
    elif [[ -n "$target" ]]; then
        default_target=$target
        log $INFO "Using specified target: $default_target"
    fi
    
    project_type=${project_kind:-default}

    pre_build
    
else
    log $ERROR "$dir is not a valid directory."
    usage
    exit 1
fi