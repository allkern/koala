# git fetch --all --tags

# $VERSION_TAG = git describe --always --tags --abbrev=0
# $COMMIT_HASH = git rev-parse --short HEAD
# $OS_INFO = (Get-WMIObject win32_operatingsystem).caption + " " + (Get-WMIObject win32_operatingsystem).version + " " + (Get-WMIObject win32_operatingsystem).OSArchitecture

$KOALA_DIR = ".\src"

mkdir -Force -Path bin > $null

c++ -I"`"$($KOALA_DIR)`"" `
    "main.cpp" `
    "src\lexer\*.cpp" `
    "src\parser\*.cpp" `
    "src\parser\statements\*.cpp" `
    -o "bin\koala.exe" -std=c++20 -Ofast -g