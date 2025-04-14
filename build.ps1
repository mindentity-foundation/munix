# Remaked by BehindThatTeam with love
# This script is used to build Voiker

$ErrorActionPreference = "Stop"
$global:HasErrors = $false

function Write-Info($message) {
    Write-Host "[INFO] $message" -ForegroundColor Cyan
}

function Write-Error($message) {
    Write-Host "[ERROR] $message" -ForegroundColor Red
}

function Test-LastExitCode {
    if ($LASTEXITCODE -ne 0) {
        Write-Error "Last command failed with exit code $LASTEXITCODE"
        exit $LASTEXITCODE
    }
}

function Show-ProgressBar($current, $total, $name) {
    $width = 40
    $filled = [math]::Floor(($current / $total) * $width)
    $empty = $width - $filled
    $bar = "[" + ("=" * $filled) + (" " * $empty) + "]"
    Write-Host -NoNewline "`r$name $bar ($current/$total)" -ForegroundColor Yellow
}

function Show-Help {
    Write-Host "Voiker Build Script Usage:" -ForegroundColor Green
    Write-Host "  -c    Clean build directory"
    Write-Host "  -t    Build and run in QEMU"
    Write-Host "  -h    Show this help message"
    exit 0
}

$obinary = "voiker.bin"

$cflags = @("-std=gnu99", "-ffreestanding", "-O2", "-Wall", "-Wextra", "-I.")
$ldflags = @("-ffreestanding", "-O2", "-nostdlib", "-lgcc")

$sfiles = @(
    "kernel\interupts\gdt.s",
    "kernel\boot.s"
)

$cfiles = @(
    "kernel\drivers\keyboard.c",
    "kernel\hardware\hardware.c",
    "kernel\hardware\vga.c",
    "kernel\interupts\gdt.c",
    "kernel\io\io.c",
    "kernel\io\port.c",
    "kernel\io\serial.c",
    "kernel\main.c",
    "kernel\tty.c",
    "lib\io.c",
    "lib\string.c"
)

Write-Info "================ VOIBLD ================"
Write-Info "obinary = $obinary"
Write-Info "cflags  = $cflags"
Write-Info "ldflags = $ldflags"
Write-Info "sfiles  = ("
$sfiles | ForEach-Object { Write-Info "    $_" }
Write-Info ")"
Write-Info "cfiles  = ("
$cfiles | ForEach-Object { Write-Info "    $_" }
Write-Info ")"
Write-Info "========================================"

$stopwatch = [System.Diagnostics.Stopwatch]::StartNew()

if ($args[0] -eq "-h") {
    Show-Help
}

if ($args[0] -eq "-c") {
    Write-Info "Clearing build directory..."
    Remove-Item -Path "build\*" -Force -Recurse -ErrorAction SilentlyContinue
    exit
}

if (-not (Get-Command "i686-elf-as" -ErrorAction SilentlyContinue)) {
    Write-Error "i686-elf-as not found in PATH"
    exit 1
}
if (-not (Get-Command "i686-elf-gcc" -ErrorAction SilentlyContinue)) {
    Write-Error "i686-elf-gcc not found in PATH"
    exit 1
}

if (-not (Test-Path "linker.ld")) {
    Write-Error "linker.ld not found"
    exit 1
}

New-Item -ItemType Directory -Force -Path "build" | Out-Null

$totalFiles = $sfiles.Count + $cfiles.Count
$currentFile = 0

foreach ($sfile in $sfiles) {
    $currentFile++
    Show-ProgressBar $currentFile $totalFiles "Building"
    if (-not (Test-Path $sfile)) {
        Write-Host "`n"
        Write-Error "Source file not found: $sfile"
        exit 1
    }
    $name = $sfile -replace '\\', '_'
    i686-elf-as $sfile -o "build\$name.o"
    Test-LastExitCode
}

foreach ($cfile in $cfiles) {
    $currentFile++
    Show-ProgressBar $currentFile $totalFiles "Building"
    if (-not (Test-Path $cfile)) {
        Write-Host "`n"
        Write-Error "Source file not found: $cfile"
        exit 1
    }
    $name = $cfile -replace '\\', '_'
    i686-elf-gcc -c $cfile -o "build\$name.o" @cflags
    Test-LastExitCode
}
Write-Host "`n"

Write-Info "Linking: $obinary"
$objFiles = Get-ChildItem -Path "build" -Filter "*.o" | Select-Object -ExpandProperty FullName
if ($objFiles.Count -eq 0) {
    Write-Error "No object files found to link"
    exit 1
}
i686-elf-gcc -T linker.ld -o $obinary $objFiles @ldflags
Test-LastExitCode

Write-Info "================= DONE ================="
$stopwatch.Stop()
Write-Info ("Build completed in " + [math]::Round($stopwatch.Elapsed.TotalSeconds, 2) + " seconds")

if ($args[0] -eq "-t") {
    if (-not (Get-Command "qemu-system-i386" -ErrorAction SilentlyContinue)) {
        Write-Error "qemu-system-i386 not found in PATH"
        exit 1
    }
    Write-Info "Running QEMU for target: $obinary"
    qemu-system-i386 -kernel $obinary
    Test-LastExitCode
}