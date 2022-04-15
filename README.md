Karplus Strong in JUCE


build
mkdir build
cd build
cmake -DCMAKE_PREFIX_PATH="~/CPP/JUCE/install" -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" ..
cmake --build .