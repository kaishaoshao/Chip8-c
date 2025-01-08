#!/bin/bash

# 脚本名称
SCRIPT_NAME=$(basename "$0")

# 获取脚本所在目录的绝对路径
SCRIPT_DIR=$(dirname "$(readlink -f "$0")")

# 构建目录
BUILD_DIR="$SCRIPT_DIR/build"

# 帮助信息
function show_help() {
    echo "Usage: $SCRIPT_NAME [options]"
    echo "Options:"
    echo "  -c, --clean     Clean the build directory"
    echo "  -b, --build     Build the project"
    echo "  -r, --run       Run the executable after building"
    echo "  -h, --help      Show this help message"
}

# 清理构建目录
function clean_build() {
    if [ -d "$BUILD_DIR" ]; then
        echo "Cleaning build directory..."
        rm -rf "$BUILD_DIR"
    else
        echo "Build directory does not exist. Nothing to clean."
    fi
}

# 配置和编译项目
function build_project() {
    if [ ! -d "$BUILD_DIR" ]; then
        echo "Creating build directory..."
        mkdir -p "$BUILD_DIR"
    fi

    cd "$BUILD_DIR" || exit 1

    echo "Configuring project with CMake..."
    cmake "$SCRIPT_DIR"

    if [ $? -eq 0 ]; then
        echo "Building project with make..."
        make
    else
        echo "CMake configuration failed."
        exit 1
    fi

    cd "$SCRIPT_DIR" || exit 1
}

# 运行可执行文件
function run_executable() {
    if [ -f "$BUILD_DIR/src/Chip8" ]; then
        echo "Running executable..."
        "$BUILD_DIR/src/Chip8"
    else
        echo "Executable not found. Please build the project first."
        exit 1
    fi
}

# 解析命令行参数
if [ $# -eq 0 ]; then
    show_help
    exit 0
fi

while [[ $# -gt 0 ]]; do
    case "$1" in
        -c|--clean)
            clean_build
            shift
            ;;
        -b|--build)
            build_project
            shift
            ;;
        -r|--run)
            run_executable
            shift
            ;;
        -h|--help)
            show_help
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            show_help
            exit 1
            ;;
    esac
done