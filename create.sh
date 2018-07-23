rm -rf build
mkdir build
cd build
cmake .. \
 -DCMAKE_TOOLCHAIN_FILE=toolchain \
 -DCMAKE_INCLUDE_PATH="/workspaces/blevinsk/git/libmetal/build_r5/usr/local/include;/workspaces/blevinsk/xsdk_workspace/18.3/standalone_r5_0/psu_cortexr5_0/include" \
 -DCMAKE_LIBRARY_PATH="/workspaces/blevinsk/git/libmetal/build_r5/usr/local/lib/;/workspaces/blevinsk/xsdk_workspace/18.3/standalone_r5_0/psu_cortexr5_0/lib"  -DWITH_APPS=on # -DWITH_PROXY_APPS=on -DWITH_PROXY=on
make DESTDIR=$(pwd) install VERBOSE=1

