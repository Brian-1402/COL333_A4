CC=g++
CXXFLAGS="-std=c++11 -O3 -funroll-loops -flto -march=native"

mkdir -p bin
$CC $CXXFLAGS main.cpp -o bin/main.out
# chmod 777 run.sh