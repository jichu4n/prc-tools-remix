#!/bin/bash

# Build script for macOS ARM using Rosetta 2 (x86_64 emulation)
# This allows building the ancient GCC 2.95.3 and 3.3.1 without modifications
# while maintaining 100% code compatibility with existing Palm OS projects.
#
# Author: github/savaughn
##

cd "$(dirname "$0")/.."
base_dir="$PWD"

# Parse command line arguments
skip_check=false
dest_dir="$base_dir/dist"

for arg in "$@"; do
  case $arg in
    --skip-check)
      skip_check=true
      ;;
    *)
      dest_dir="$arg"
      ;;
  esac
done

echo "=== Building prc-tools-remix for macOS using Rosetta 2 ==="
echo ""

# Check if running on Apple Silicon
if [[ $(uname -m) == "arm64" ]]; then
  echo "Detected Apple Silicon (ARM64)"
  echo "This build will use Rosetta 2 for x86_64 compatibility"
  echo ""
  
  # Check if Rosetta is installed
  if ! /usr/bin/pgrep oahd >/dev/null 2>&1; then
    echo "ERROR: Rosetta 2 is not installed"
    echo ""
    echo "To install Rosetta 2, run:"
    echo "  softwareupdate --install-rosetta"
    echo ""
    exit 1
  fi
  
  echo "✓ Rosetta 2 is installed"
  echo ""
  
  # Re-execute this script under Rosetta if not already running under it
  if [[ $(sysctl -n sysctl.proc_translated 2>/dev/null || echo 0) != "1" ]]; then
    echo "Re-launching under Rosetta 2..."
    echo ""
    exec arch -x86_64 "$0" "$@"
  fi
fi

# At this point, we're running under x86_64 (either native or via Rosetta)
echo "Running under architecture: $(uname -m)"
echo ""

function check_dependencies() {
  echo "Checking dependencies..."
  
  local missing_deps=()
  
  # Add Homebrew paths for x86_64
  if [ -d "/usr/local/bin" ]; then
    export PATH="/usr/local/bin:$PATH"
  fi
  
  # Check for required tools
  command -v gcc >/dev/null 2>&1 || missing_deps+=("gcc (Xcode Command Line Tools)")
  command -v make >/dev/null 2>&1 || missing_deps+=("make")
  command -v flex >/dev/null 2>&1 || missing_deps+=("flex")
  command -v bison >/dev/null 2>&1 || missing_deps+=("bison")
  command -v autoconf >/dev/null 2>&1 || missing_deps+=("autoconf")
  command -v automake >/dev/null 2>&1 || missing_deps+=("automake")
  command -v gperf >/dev/null 2>&1 || missing_deps+=("gperf")
  
  # Check for texinfo/makeinfo
  if ! command -v makeinfo >/dev/null 2>&1; then
    # Try to find it in Homebrew x86_64
    if [ -d "/usr/local/opt/texinfo/bin" ]; then
      export PATH="/usr/local/opt/texinfo/bin:$PATH"
    fi
  fi
  
  command -v makeinfo >/dev/null 2>&1 || missing_deps+=("texinfo")
  
  if [ ${#missing_deps[@]} -ne 0 ]; then
    echo "ERROR: Missing dependencies:"
    printf '  - %s\n' "${missing_deps[@]}"
    echo ""
    echo "To install x86_64 Homebrew and dependencies:"
    echo "  arch -x86_64 /bin/bash -c \"\$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)\""
    echo "  arch -x86_64 /usr/local/bin/brew install texinfo autoconf automake gperf"
    echo ""
    echo "For Xcode Command Line Tools:"
    echo "  xcode-select --install"
    echo ""
    exit 1
  fi
  
  echo "All dependencies found."
}

function build() {
  cd "$base_dir"
  rm -rf build && mkdir -p build && cd build
  
  # Create doc directory to prevent make errors
  mkdir -p doc

  # Configure - use i386-apple-darwin which is recognized by old GCC
  # Even though we're running x86_64, the GCC doesn't know about it
  # Force both --build and --host to avoid config.guess detecting x86_64
  # Note: Removed --disable-generic to enable building tools like build-prc and obj-res
  ../prc-tools-2.3/configure \
    --build=i386-apple-darwin \
    --host=i386-apple-darwin \
    --enable-targets=m68k-palmos,arm-palmos \
    --enable-languages=c,c++ \
    --disable-nls \
    --prefix=/usr/local \
    --infodir=/usr/local/share/info \
    --mandir=/usr/local/share/man \
    --with-palmdev-prefix=/usr/local/palmdev

  # Build with conservative flags for old code compatibility
  # Use -Wno-error=implicit-function-declaration to handle old C code
  # Use -j1 to avoid race conditions in the Makefiles
  CC="gcc -Wno-error=implicit-function-declaration" \
  CFLAGS="-w -O1 -fcommon -fno-strict-aliasing" \
  CXXFLAGS="-w -O1 -fcommon" \
  make -j1 || {
    # Sometimes there are race conditions even with -j1
    # If build fails, try again - usually succeeds on second attempt
    echo ""
    echo "Build encountered an error, retrying..."
    echo ""
    cd "$base_dir/build"
    CC="gcc -Wno-error=implicit-function-declaration" \
    CFLAGS="-w -O1 -fcommon -fno-strict-aliasing" \
    CXXFLAGS="-w -O1 -fcommon" \
    make -j1
  }
}

function install() {
  cd "$base_dir/build"
  
  # Install to the destination directory
  make MAKEINFO=true DESTDIR="$dest_dir" install || {
    echo ""
    echo "ERROR: Installation failed"
    echo "The build may not have completed successfully."
    exit 1
  }

  cd "$dest_dir"
  
  # Clean up
  if [ -d ./usr/local/include ]; then
    rmdir ./usr/local/include 2>/dev/null || true
  fi
  
  if [ -d ./usr/local/lib ]; then
    rm -f ./usr/local/lib/*.a
  fi
  
  if [ -d ./usr/local/share/info ]; then
    rm -f ./usr/local/share/info/*
  fi
  
  if [ -d ./usr/local/share/man/man7 ]; then
    rm -rf ./usr/local/share/man/man7
  fi
  
  if [ -d ./usr/local/share/man/man1 ]; then
    (
      cd ./usr/local/share/man/man1 &&
      for f in *; do
        if [[ "$f" != arm-palmos-* ]] && [[ "$f" != m68k-palmos-* ]]; then
          rm -f "$f"
        fi
      done
    )
  fi
  
  # Reinstall prc-tools's own documentation if the doc directory exists
  if [ -d "$base_dir/build/doc" ]; then
    make -C "$base_dir/build/doc" DESTDIR="$dest_dir" install
  fi
}

set -e

if [ "$skip_check" = false ]; then
  check_dependencies
fi

echo ""
echo "=== Starting build ==="
build

echo ""
echo "=== Installing to $dest_dir ==="
install

echo ""
echo "=== Build completed successfully! ==="
echo ""
echo "Installation directory: $dest_dir"
echo ""
echo "Note: The binaries are x86_64 and will run under Rosetta 2 on Apple Silicon."
echo ""
echo "To install to your system:"
echo "  sudo cp -R $dest_dir/usr/local/* /usr/local/"
echo ""
