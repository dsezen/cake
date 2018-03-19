REM build dependencies
pushd libs

REM -----------------------------------
REM BULLET 3
REM -----------------------------------
cd bullet3
cmake -DBUILD_PYBULLET=ON -DUSE_DOUBLE_PRECISION=ON -DCMAKE_BUILD_TYPE=Release -DPYTHON_INCLUDE_DIR="C:\Python\Python36-32\include" -DPYTHON_LIBRARY="C:\Python\Python36-32\libs\python36.lib" -DPYTHON_DEBUG_LIBRARY="C:\Python\Python36-32\libs\python36_d.lib" -G "Visual Studio 15" .

REM back to src_main
popd

REM -----------------------------------
REM generate projects for main game.
REM -----------------------------------
del /s /q build
mkdir build
cd build
cmake -G "Visual Studio 15" ..
pause