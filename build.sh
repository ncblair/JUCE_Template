python src/matrix/create_parameters.py
python src/matrix/create_modulators.py
cd build
cmake -DCMAKE_PREFIX_PATH="~/CPP/JUCE/install" -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" -DCMAKE_BUILD_TYPE=Release ..
cd ..
cmake --build build
# /Applications/Ableton\ Live\ 11\ Suite.app/Contents/MacOS/Live Peripheral/TemplateTest\ Project/TemplateTest.als