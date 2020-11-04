Compile instruction for Linux

go to nurbs++-3.0.11
export CXXFLAGS="-O2 -fpermissive -fPIC"
./configure --prefix=<SOURCE_ROOT_PATH>/compiled

make
make install

cd ..
mkdir build
cd build
cmake ..
make
