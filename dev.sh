#!/bin/bash
# 🗓️ Daily Reminder - Development & Build Script
# Usage: ./dev.sh [dev|build|clean|install|run]

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
MAGENTA='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Project root directory (where this script is located)
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BACKEND_DIR="$PROJECT_ROOT/backend"
FRONTEND_DIR="$PROJECT_ROOT/frontend"

# Build directories
BACKEND_BUILD_DIR="$BACKEND_DIR/build"

# PID tracking
BACKEND_PID=""

# Print colored message
print_msg() {
    local color=$1
    local msg=$2
    echo -e "${color}${msg}${NC}"
}

# Print section header
print_header() {
    echo ""
    print_msg "$CYAN" "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    print_msg "$CYAN" "  $1"
    print_msg "$CYAN" "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
}

# Check if command exists
check_command() {
    if ! command -v $1 &> /dev/null; then
        print_msg "$RED" "❌ Error: $1 is not installed"
        exit 1
    fi
}

# Check prerequisites
check_prerequisites() {
    print_header "Checking Prerequisites"
    
    print_msg "$YELLOW" "Checking required tools..."
    check_command cmake
    check_command node
    check_command npm
    
    print_msg "$GREEN" "✅ All prerequisites satisfied"
}

# Clean build artifacts
clean_build() {
    print_header "Cleaning Build Artifacts"
    
    # Clean backend
    if [ -d "$BACKEND_BUILD_DIR" ]; then
        print_msg "$YELLOW" "🧹 Cleaning backend build..."
        rm -rf "$BACKEND_BUILD_DIR"
    fi
    
    # Clean frontend
    if [ -d "$FRONTEND_DIR/.next" ]; then
        print_msg "$YELLOW" "🧹 Cleaning frontend build..."
        cd "$FRONTEND_DIR"
        rm -rf .next out node_modules/.cache
    fi
    
    print_msg "$GREEN" "✅ Clean complete"
}

# Install all dependencies
install_dependencies() {
    print_header "Installing Dependencies"
    
    # Install frontend dependencies from project root (using workspace)
    print_msg "$YELLOW" "📦 Installing frontend dependencies..."
    cd "$PROJECT_ROOT"
    npm install --workspace=frontend
    
    print_msg "$GREEN" "✅ All dependencies installed"
}

# Build frontend (Next.js)
build_frontend() {
    local mode=$1
    print_header "Building Frontend (Next.js)"
    
    cd "$FRONTEND_DIR"
    
    # Install dependencies if needed (check frontend node_modules)
    if [ ! -d "$FRONTEND_DIR/node_modules" ]; then
        print_msg "$YELLOW" "📦 Installing npm dependencies..."
        cd "$PROJECT_ROOT"
        npm install --workspace=frontend
        cd "$FRONTEND_DIR"
    fi
    
    if [ "$mode" = "production" ]; then
        print_msg "$YELLOW" "🔨 Building frontend for production..."
        npm run build
    else
        print_msg "$YELLOW" "🔨 Building frontend for development..."
        npm run build
    fi
    
    print_msg "$GREEN" "✅ Frontend build complete"
    cd "$PROJECT_ROOT"
}

# Build backend (Qt)
build_backend() {
    local mode=$1
    print_header "Building Backend (Qt + QHttpServer)"
    
    cd "$BACKEND_DIR"
    
    # Create build directory
    mkdir -p "$BACKEND_BUILD_DIR"
    cd "$BACKEND_BUILD_DIR"
    
    if [ "$mode" = "Debug" ]; then
        print_msg "$YELLOW" "🔨 Configuring CMake for development..."
        cmake -DCMAKE_BUILD_TYPE=Debug ..
        
        print_msg "$YELLOW" "🔨 Building Qt application (Debug)..."
        cmake --build . --config Debug
    else
        print_msg "$YELLOW" "🔨 Configuring CMake for production..."
        cmake -DCMAKE_BUILD_TYPE=Release ..
        
        print_msg "$YELLOW" "🔨 Building Qt application (Release)..."
        cmake --build . --config Release
    fi
    
    print_msg "$GREEN" "✅ Backend build complete"
}

# Cleanup function for graceful shutdown
cleanup() {
    print_msg "$YELLOW" "🧹 Shutting down services..."
    
    # Kill Next.js dev server if running
    if [ ! -z "$FRONTEND_PID" ] && ps -p $FRONTEND_PID > /dev/null 2>&1; then
        print_msg "$YELLOW" "Stopping Next.js dev server (PID: $FRONTEND_PID)..."
        kill $FRONTEND_PID 2>/dev/null || true
    fi
    
    # Kill backend if running
    if [ ! -z "$BACKEND_PID" ] && ps -p $BACKEND_PID > /dev/null 2>&1; then
        print_msg "$YELLOW" "Stopping backend (PID: $BACKEND_PID)..."
        kill $BACKEND_PID 2>/dev/null || true
    fi
    
    # Kill any remaining processes from this script
    pkill -P $$ 2>/dev/null || true
    
    print_msg "$GREEN" "✅ Shutdown complete"
    exit 0
}

# Set trap for cleanup
trap cleanup EXIT INT TERM

# Run development mode
run_dev() {
    print_header "Starting Development Environment"
    
    print_msg "$MAGENTA" "🚀 Starting application in development mode..."
    print_msg "$CYAN" "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    
    # Check if frontend dependencies are installed
    if [ ! -d "$FRONTEND_DIR/node_modules" ]; then
        print_msg "$YELLOW" "📦 Installing frontend dependencies..."
        cd "$PROJECT_ROOT"
        npm install --workspace=frontend
    fi
    
    # Build backend
    build_backend "Debug"
    
    # Start Next.js dev server in background
    print_msg "$YELLOW" "💻 Starting Next.js dev server..."
    cd "$FRONTEND_DIR"
    npm run dev > /dev/null 2>&1 &
    FRONTEND_PID=$!
    cd "$PROJECT_ROOT"
    
    # Wait for Next.js to be ready
    print_msg "$YELLOW" "⏳ Waiting for Next.js dev server..."
    local max_attempts=30
    local attempt=0
    while [ $attempt -lt $max_attempts ]; do
        if curl -s http://localhost:3000 > /dev/null 2>&1; then
            print_msg "$GREEN" "✅ Next.js dev server ready at http://localhost:3000"
            break
        fi
        sleep 1
        attempt=$((attempt + 1))
        if [ $attempt -eq $max_attempts ]; then
            print_msg "$RED" "❌ Next.js dev server failed to start"
            cleanup
            exit 1
        fi
    done
    
    # Start the Qt application (it will serve everything)
    print_msg "$CYAN" "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    print_msg "$GREEN" "🎉 Application ready!"
    print_msg "$MAGENTA" "   Backend API: http://localhost:8080"
    print_msg "$MAGENTA" "   Frontend:    http://localhost:3000 (Next.js dev server)"
    print_msg "$MAGENTA" "   Qt Window:   Loading frontend..."
    print_msg "$CYAN" "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    echo ""
    print_msg "$YELLOW" "💡 Press Ctrl+C to stop all services"
    echo ""
    
    # Run the Qt application (this will block until app closes)
    cd "$BACKEND_BUILD_DIR"
    if [ -f "backend" ]; then
        # Set fontconfig to suppress warnings
        export FONTCONFIG_FILE=/etc/fonts/fonts.conf
        export FONTCONFIG_PATH=/etc/fonts
        ./backend
    else
        print_msg "$RED" "❌ Error: backend executable not found"
        print_msg "$YELLOW" "Expected location: $BACKEND_BUILD_DIR/backend"
        cleanup
        exit 1
    fi
}

# Build production
build_production() {
    print_header "Building Production Release"
    
    build_frontend "production"
    build_backend "Release"
    
    print_msg "$GREEN" "🎉 Production build complete!"
    print_msg "$BLUE" "📦 Executable location:"
    print_msg "$BLUE" "   $BACKEND_BUILD_DIR/backend"
    echo ""
    print_msg "$YELLOW" "To run: cd $BACKEND_BUILD_DIR && ./backend"
}

# Just run the app (assumes already built)
run_app() {
    print_header "Running Application"
    
    # Check if backend is built
    if [ ! -f "$BACKEND_BUILD_DIR/backend" ]; then
        print_msg "$YELLOW" "⚠️  Backend not built yet. Building now..."
        build_frontend "development"
        build_backend "Debug"
    fi
    
    print_msg "$GREEN" "🚀 Starting application..."
    echo ""
    print_msg "$YELLOW" "💡 Press Ctrl+C to stop"
    echo ""
    
    # Run Qt app with fontconfig settings
    cd "$BACKEND_BUILD_DIR"
    export FONTCONFIG_FILE=/etc/fonts/fonts.conf
    export FONTCONFIG_PATH=/etc/fonts
    ./backend
}

# Show usage
show_usage() {
    echo "Usage: ./dev.sh [COMMAND]"
    echo ""
    echo "Commands:"
    echo "  dev      - Build and run in development mode (default) ⭐"
    echo "  run      - Just run (assumes already built)"
    echo "  install  - Install all dependencies"
    echo "  build    - Build production release"
    echo "  clean    - Clean all build artifacts"
    echo "  help     - Show this help message"
    echo ""
    echo "Examples:"
    echo "  ./dev.sh          # Build and run dev (RECOMMENDED!)"
    echo "  ./dev.sh install  # Install dependencies"
    echo "  ./dev.sh run      # Just run (if already built)"
    echo "  ./dev.sh build    # Build production"
    echo "  ./dev.sh clean    # Clean builds"
    echo ""
    echo "NPM shortcuts:"
    echo "  npm run dev       # Same as ./dev.sh dev"
    echo "  npm run build     # Same as ./dev.sh build"
    echo "  npm run clean     # Same as ./dev.sh clean"
    echo ""
    echo "⭐ Quick start: npm run dev (does everything!)"
}

# Main script logic
main() {
    local command=${1:-dev}
    
    print_msg "$CYAN" "🗓️  Daily Reminder - Build System"
    print_msg "$CYAN" "📁 Working from: $PROJECT_ROOT"
    echo ""
    
    case $command in
        dev)
            check_prerequisites
            run_dev
            ;;
        run)
            check_prerequisites
            run_app
            ;;
        install)
            check_prerequisites
            install_dependencies
            ;;
        build|prod|production)
            check_prerequisites
            build_production
            ;;
        clean)
            clean_build
            ;;
        help|--help|-h)
            show_usage
            ;;
        *)
            print_msg "$RED" "❌ Unknown command: $command"
            echo ""
            show_usage
            exit 1
            ;;
    esac
}

# Run main function
main "$@"
