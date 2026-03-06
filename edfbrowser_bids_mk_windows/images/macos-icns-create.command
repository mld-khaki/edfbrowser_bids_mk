#!/usr/bin/env bash

# check requirements
for cmd in sips iconutil
do
  command -v ${cmd} >/dev/null 2>&1 || { echo >&2 "This script requires '${cmd}' but it's not available. Aborting."; exit 1; }
done

base_dir="$(dirname "$0")"
dest_dir="${base_dir}"
[ "$@" ] && [ -d "$@" ] && dest_dir="$@"

# make sure we are in the correct directory when we double-click a .command file
pushd "${dest_dir}"

base_image="${base_dir}/edf.png"
iconset_dir="edf.iconset"
icns="$destdir/edf.icns"

mkdir "$iconset_dir"

# create iconset images
sips "$base_image" \
  --out "$iconset_dir/icon_512x512@2x.png" \
  --setProperty format png \
  -Z 1024
sips "$base_image" \
  --out "$iconset_dir/icon_512x512.png" \
  --setProperty format png \
  -Z 512
sips "$base_image" \
  --out "$iconset_dir/icon_256x256@2x.png" \
  --setProperty format png \
  -Z 512
sips "$base_image" \
  --out "$iconset_dir/icon_256x256.png" \
  --setProperty format png \
  -Z 256
sips "$base_image" \
  --out "$iconset_dir/icon_128x128@2x.png" \
  --setProperty format png \
  -Z 256
sips "$base_image" \
  --out "$iconset_dir/icon_128x128.png" \
  --setProperty format png \
  -Z 128
sips "$base_image" \
  --out "$iconset_dir/icon_32x32@2x.png" \
  --setProperty format png \
  -Z 64
sips "$base_image" \
  --out "$iconset_dir/icon_32x32.png" \
  --setProperty format png \
  -Z 32
sips "$base_image" \
  --out "$iconset_dir/icon_16x16@2x.png" \
  --setProperty format png \
  -Z 32
sips "$base_image" \
  --out "$iconset_dir/icon_16x16.png" \
  --setProperty format png \
  -Z 16
  
# create icns
iconutil \
  --convert icns \
  "$iconset_dir"
  
# clean up
rm -r "$iconset_dir"
  
popd