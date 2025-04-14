#!/bin/bash

function write_info() {
    echo -e "\033[36m[INFO] $1\033[0m"
}

function write_error() {
    echo -e "\033[31m[ERROR] $1\033[0m"
}

function test_last_exit() {
    if [ $? -ne 0 ]; then
        write_error "Last command failed with exit code $?"
        exit $?
    fi
}

function show_progress_bar() {
    local current=$1
    local total=$2
    local name=$3
    local width=40
    local filled=$((current * width / total))
    local empty=$((width - filled))
    local bar="["
    
    for ((i=0; i<filled; i++)); do
        bar+="="
    done
    
    for ((i=0; i<empty; i++)); do
        bar+=" "
    done
    
    bar+="]"
    
    echo -ne "\r\033[33m$name $bar ($current/$total)\033[0m"
}

function show_help() {
    echo -e "\033[32mVoiker Build Script Usage:\033[0m"
    echo "  -c    Clean build directory"
    echo "  -t    Build and run in QEMU"
    echo "  -h    Show this help message"
    exit 0
}

obinary="voiker.bin"

cflags="-std=gnu99 -ffreestanding -O2 -Wall -Wextra -I."
ldflags="-ffreestanding -O2 -nostdlib -lgcc"


sfiles=(
    "kernel/interupts/gdt.s"
    "kernel/boot.s"
)

cfiles=(
    "kernel/hardware/hardware.c"
    "kernel/hardware/vga.c"
    "kernel/interupts/gdt.c"
    "kernel/io/port.c"
    "kernel/io/serial.c"
    "kernel/main.c"
    "kernel/tty.c"
    "lib/io.c"
    "lib/string.c"
)

write_info "================ VOIBLD ================"
write_info "obinary = $obinary"
write_info "cflags  = $cflags"
write_info "ldflags = $ldflags"
write_info "sfiles  = ("
for file in "${sfiles[@]}"; do
    write_info "    $file"
done
write_info ")"
write_info "cfiles  = ("
for file in "${cfiles[@]}"; do
    write_info "    $file"
done
write_info ")"
write_info "========================================"

SECONDS=0

if [ "$1" == "-h" ]; then
    show_help
fi

if [ "$1" == "-c" ]; then
    write_info "Clearing build directory..."
    rm -rf build/*
    exit 0
fi

if ! command -v i686-elf-as &> /dev/null; then
    write_error "i686-elf-as not found in PATH"
    exit 1
fi

if ! command -v i686-elf-gcc &> /dev/null; then
    write_error "i686-elf-gcc not found in PATH"
    exit 1
fi

if [ ! -f "linker.ld" ]; then
    write_error "linker.ld not found"
    exit 1
fi

mkdir -p build

total_files=$((${#sfiles[@]} + ${#cfiles[@]}))
current_file=0

for sfile in "${sfiles[@]}"; do
    current_file=$((current_file + 1))
    show_progress_bar $current_file $total_files "Building"
    
    if [ ! -f "$sfile" ]; then
        echo ""
        write_error "Source file not found: $sfile"
        exit 1
    fi
    
    name=$(echo $sfile | tr '/' '_')
    i686-elf-as "$sfile" -o "build/${name}.o"
    test_last_exit
done


for cfile in "${cfiles[@]}"; do
    current_file=$((current_file + 1))
    show_progress_bar $current_file $total_files "Building"
    
    if [ ! -f "$cfile" ]; then
        echo ""
        write_error "Source file not found: $cfile"
        exit 1
    fi
    
    name=$(echo $cfile | tr '/' '_')
    i686-elf-gcc -c "$cfile" -o "build/${name}.o" $cflags
    test_last_exit
done
echo ""

write_info "Linking: $obinary"
objfiles=$(find build -name "*.o")
if [ -z "$objfiles" ]; then
    write_error "No object files found to link"
    exit 1
fi
i686-elf-gcc -T linker.ld -o "$obinary" build/*.o $ldflags
test_last_exit

write_info "================= DONE ================="
build_time=$SECONDS
write_info "Build completed in $(echo "scale=2; $build_time" | bc) seconds"

if [ "$1" == "-t" ]; then
    if ! command -v qemu-system-i386 &> /dev/null; then
        write_error "qemu-system-i386 not found in PATH"
        exit 1
    fi
    write_info "Running QEMU for target: $obinary"
    qemu-system-i386 -kernel "$obinary"
    test_last_exit
fi