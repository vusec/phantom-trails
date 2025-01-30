set -e

cd riscv-isa-sim
git apply ../processor.patch

COMMON_FLAGS="-O3 -static-libstdc++ -static-libgcc -fPIC"

mkdir -p build
cd build
../configure CC=/usr/bin/gcc CXX=/usr/bin/g++ LD=/usr/bin/d LDFLAGS="" CFLAGS="$COMMON_FLAGS" CXXFLAGS="$COMMON_FLAGS" CPPFLAGS="$COMMON_FLAGS"
AFL_PASSTHROUGH=1 CC=/usr/bin/gcc CXX=/usr/bin/g++ LD=/usr/bin/d LDFLAGS="" CFLAGS="$COMMON_FLAGS" CXXFLAGS="$COMMON_FLAGS" CPPFLAGS="$COMMON_FLAGS" make -j`nproc`

cd ../..

LIBS=$(find riscv-isa-sim/build/ -name "*.a")
FINAL_FLAGS="${COMMON_FLAGS} ${LIBS} -fuse-ld=lld"

/usr/bin/g++ -std=c++17 -g main.cc $FINAL_FLAGS -o main \
             -I./riscv-isa-sim/riscv/ -I./riscv-isa-sim/build/ \
             -L./riscv-isa-sim/build/

/usr/bin/g++ -std=c++17 -g SpikeWrapper.cpp $FINAL_FLAGS -o SpikeWrapper.so -shared \
             -I./riscv-isa-sim/riscv/ -I./riscv-isa-sim/build/ \
             -L./riscv-isa-sim/build/

cp SpikeWrapper.so /usr/lib
