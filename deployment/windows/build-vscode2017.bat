export QT5_DIR="C:\Qt\5.12.2\msvc2017_64"
mkdir build
cd build
cmake -G "Visual Studio 15" -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release