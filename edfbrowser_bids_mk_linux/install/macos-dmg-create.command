#!/usr/bin/env bash

# Based on the example of script to package up and create a DMG for a macOS app
# https://gist.github.com/asmaloney/55d96a8c3558b2f92cb3

# check requirements
for cmd in sed egrep awk bc du hdiutil osascript otool
do
  command -v ${cmd} >/dev/null 2>&1 || \
    { echo >&2 "This script requires '${cmd}' but it's not available. Aborting."; exit 1; }
done

base_dir="$(dirname "$0")/.."
dest_dir="${base_dir}"
[ "$@" ] && [ -d "$@" ] && dest_dir="$@"

# make sure we are in the correct directory when we double-click a .command file
pushd "${dest_dir}"

# set up your app name, version number and background image file name
APP_NAME="EDFbrowser"
VERSION="$(sed -n 's,.*PROGRAM_VERSION "\([^"]*\).*,\1,p' ${base_dir}/global.h)"
DMG_BACKGROUND_IMG="macos-dmg-background.png"

# you should not need to change these
APP_EXE="${APP_NAME}.app/Contents/MacOS/EDFbrowser"

VOL_NAME="${APP_NAME} ${VERSION}"
DMG_TEMP="${APP_NAME}-${VERSION}-temp.dmg"
DMG_FINAL="${APP_NAME}-${VERSION}.dmg"

# use macdeployqt to add related Qt libraries to the app bundle as private frameworks (if the app is compiled dynamically)
if otool -L "${APP_EXE}" | grep Qt >/dev/null 2>&1
then
  cmd="macdeployqt"
  if command -v ${cmd} >/dev/null 2>&1
  then
    ${cmd} "${APP_NAME}.app" -no-strip
  else
    command -v brew >/dev/null 2>&1 && \
      cmd="$(brew --prefix qt 2>/dev/null)/bin/${cmd}" && \
      test -x "${cmd}" && \
      ${cmd} "${APP_NAME}.app" -no-strip || \
      echo >&2 "App deploy skipped: 'macdeployqt' not found. Try to install with 'brew install qt'."     
  fi
fi

# strip the executable (if strip command is available)
command -v strip >/dev/null 2>&1 && \
  strip -r -u "${APP_EXE}" || \
  echo >&2 "Strip of the executable skipped: 'strip' command has to be installed. Try to install with 'xcode-select --install'." 

# compress the executable (if upx command is available)
command -v upx >/dev/null 2>&1 && \
  upx -9 "${APP_EXE}" || \
  echo >&2 "Compress of the executable skipped: 'upx' command has to be installed. Try to install with 'brew install upx'."

# figure out how big our DMG needs to be
SIZE=`du -sh "${APP_NAME}.app" | sed 's/\([0-9\.]*\)M\(.*\)/\1/'` 
SIZE=`echo "${SIZE} + 1.0" | bc | awk '{print int($1+0.5)}'`

# create the temp DMG file
hdiutil create \
  -srcfolder "${APP_NAME}.app" \
  -volname "${VOL_NAME}" \
  -fs "HFS+" \
  -fsargs "-c c=64,a=16,e=16" \
  -format "UDRW" \
  -size ${SIZE}M \
  "${DMG_TEMP}"
  
# mount it and save the device
DEVICE=$(hdiutil attach \
  -readwrite \
  -noverify \
  "${DMG_TEMP}" | \
    egrep '^/dev/' | \
      sed 1q | \
        awk '{print $1}')

sleep 2

# add a link to the Applications dir
pushd "/Volumes/${VOL_NAME}"
ln -s "/Applications"
popd

# add a background image
mkdir "/Volumes/${VOL_NAME}/.background"
cp "${base_dir}/install/${DMG_BACKGROUND_IMG}" "/Volumes/${VOL_NAME}/.background/"

# resize the window, set the background, change the icon size and place the icons in the right position
echo '
  tell application "Finder"
    tell disk "'${VOL_NAME}'"
      open
      set current view of container window to icon view
      set toolbar visible of container window to false
      set statusbar visible of container window to false
      set the bounds of container window to {200, 120, 694, 362}
      set viewOptions to the icon view options of container window
      set arrangement of viewOptions to not arranged
      set icon size of viewOptions to 80
      set background picture of viewOptions to file ".background:'${DMG_BACKGROUND_IMG}'"
      set position of item "'${APP_NAME}'.app" of container window to {120, 104}
      set position of item "Applications" of container window to {377, 104}
      close
      open
      update without registering applications
      delay 2
    end tell
  end tell
' | osascript

sync

# unmount it
hdiutil detach "${DEVICE}"

# now make the final image a compressed disk image
echo "Creating compressed image"
hdiutil convert \
  "${DMG_TEMP}" \
  -format "UDZO" \
  -imagekey zlib-level=9 \
  -o "${DMG_FINAL}"

# clean up
rm "${DMG_TEMP}"

popd
