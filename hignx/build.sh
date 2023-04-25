#!/bin/bash
# ------------------------------------------------------------------------------
# Usage
# ------------------------------------------------------------------------------
usage() {
  cat <<EOM
$0: [hs]
  -h display help
EOM
  exit 1
}
# ------------------------------------------------------------------------------
# Requirements to build...
# ------------------------------------------------------------------------------
check_req() {
  which gcc make || {
    echo "Failed to find required build packages. Please install with: sudo apt-get install make gcc liburing-dev"
    exit 1
  }
}
# ------------------------------------------------------------------------------
# Sync Submodules
# ------------------------------------------------------------------------------
submod_sync() {
  # This is necessary in scenarios where the URL of the remote for a given submodule has changed.
  git submodule sync || {
    echo "FAILED TO SYNC HIGNX LIB"
    exit 1
  }
  git submodule update -f --init || {
    echo "FAILED TO UPDATE TO LATEST HIGNX LIB"
    exit 1
  }
}
# ------------------------------------------------------------------------------
# Build fludd
# ------------------------------------------------------------------------------
build_hignx() {
  set -o errexit
  make -j$(nproc) 
  popd
}
# ------------------------------------------------------------------------------
# main
# ------------------------------------------------------------------------------
main() {
  b_type=$1
  check_req
  submod_sync
  build_hignx
}
while getopts "hs" opts; do
  case "${opts}" in
    h)
      usage
      ;;
    *)
      usage
      ;;
  esac
done
main "${build_type}"