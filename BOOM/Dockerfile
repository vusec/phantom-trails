FROM ubuntu:23.04 AS base

# Ubuntu23 is EOL :rip:
RUN sed -i "s/archive/old-releases/g" /etc/apt/sources.list
RUN sed -i "s/security\./old-releases\./g" /etc/apt/sources.list

# Prerequisites.
# Also fix messed up default LLDB installation.
# psmisc -> `killall` command (used by  the fuzzer to stop itself)/
COPY docker/install-rust.sh /tmp/
RUN apt-get update && \
    apt-get install -y make cmake ninja-build git python3 wget lld gcc g++ \
    nano fish gdb lldb python3-pip device-tree-compiler bc liblldb-dev curl \
    psmisc htop && \
    /tmp/install-rust.sh && rm /tmp/install-rust.sh && \
    ln -s /usr/lib/llvm-14/lib/python3.10/dist-packages/lldb/* /usr/lib/python3/dist-packages/lldb/

ARG NPROCS=32
ARG COVERAGE=Taint
ARG BOOM_BUILD_FLAGS="--debug"
ARG SANITIZER=MSAN

# ------- Setup chipyard.
WORKDIR /

# Install conda.
ENV MINIFORGE_NAME=Miniforge3 \
    MINIFORGE_VERSION=22.11.1-4 \
    LANG=C.UTF-8 LC_ALL=C.UTF-8 \
    PATH=/opt/conda/bin:${PATH}
RUN wget --no-hsts --quiet https://github.com/conda-forge/miniforge/releases/download/${MINIFORGE_VERSION}/${MINIFORGE_NAME}-${MINIFORGE_VERSION}-Linux-$(uname -m).sh -O /tmp/miniforge.sh && \
    /bin/bash /tmp/miniforge.sh -b -p /opt/conda  && \
    conda init bash
# Clone chipyard.
RUN git clone https://github.com/ucb-bar/chipyard.git
WORKDIR /chipyard
RUN git checkout 1.8.1
# Install chipyard requirements through conda.
RUN conda install -n base conda-lock
RUN conda run -n base /bin/bash -c "./build-setup.sh  --skip-toolchain-extra riscv-tools"

# Install verilator in the conda env.
# RUN wget http://archive.ubuntu.com/ubuntu/pool/universe/v/verilator/verilator_4.038-1_amd64.deb && \
#     apt-get install -y ./verilator_4.038-1_amd64.deb

RUN apt-get update && apt-get install -y verilator
RUN mv /chipyard/.conda-env/bin/verilator /chipyard/.conda-env/bin/old-verilator && \
    ln -s /usr/bin/verilator/ /chipyard/.conda-env/bin/verilator

WORKDIR /chipyard
COPY patches /patches
ENV PATCHES_PATH=/patches
# Patch some horrible bugs across chipyard's build system.
RUN git apply /patches/riscv-build-script.patch

# Now finally we can build riscv-tools.
SHELL ["conda", "run", "--no-capture-output", "-p",  "/chipyard/.conda-env", "/bin/bash", "-c"]
RUN ./scripts/build-toolchain-extra.sh riscv-tools

# ------- Build stock BOOM (before instrumentation).
# WORKDIR /chipyard/sims/verilator
# RUN apt install hyperfine
# RUN make -j${NPROCS} VERILATOR="verilator --cc --exe --no-timing" CONFIG=MediumBoomConfig
# RUN make -j${NPROCS} VERILATOR="verilator --cc --exe --no-timing" CONFIG=LargeBoomConfig

# ------- Setup instrumentation.
SHELL ["/bin/bash", "-c"]

# Build Clang with BFSAN.
WORKDIR /clang
COPY llvm /clang/llvm
RUN mkdir -p build && cd build && \
    cmake -DLLVM_ENABLE_PROJECTS="clang;" -GNinja ../llvm/llvm \
    -DLLVM_ENABLE_RUNTIMES="libcxx;libcxxabi;compiler-rt" \
    -DCMAKE_BUILD_TYPE=Release \
    -DLLVM_USE_LINKER=lld \
    -DLLVM_PARALLEL_LINK_JOBS=1 \
    -DCMAKE_INSTALL_PREFIX=/clang/inst \
    -DLIBCXX_CXX_FLAGS="-fsanitize=memory" \
    -DLLVM_TARGETS_TO_BUILD="X86" \
    -DCLANG_ENABLE_STATIC_ANALYZER=OFF \
    -DCLANG_ENABLE_ARCMT=OFF
RUN cd build && ninja -j${NPROCS} -l${NPROCS} && ninja install -j${NPROCS} -l${NPROCS}
# Compile libc++ with BFSan.
RUN mkdir -p libcxx
RUN mkdir libcxx-build && cd libcxx-build && \
    cmake -GNinja -S ../llvm/runtimes \
    -DCMAKE_C_COMPILER=/clang/inst/bin/clang \
    -DCMAKE_CXX_COMPILER=/clang/inst/bin/clang++ \
    -DLLVM_USE_SANITIZER="Memory" \
    -DCMAKE_INSTALL_PREFIX=/clang/libcxx \
    -DLLVM_ENABLE_RUNTIMES="libcxx;libcxxabi"
RUN cd libcxx-build && ninja -j${NPROCS} -l${NPROCS} cxx cxxabi && ninja install -j${NPROCS} -l${NPROCS}
# Install our clang.
ENV PATH="/clang/inst/bin/:${PATH}"

# Build AFL++.
WORKDIR /fuzzer
COPY AFL /fuzzer/afl
# The md5sum is just so the build fails (afl does `exit 0` at the end of the build...)
RUN cd afl && make clean && \
    LD=/usr/bin/ld.lld CFLAGS="-fuse-ld=lld" CXXFLAGS="-fuse-ld=lld" make -j${NPROCS} all && \
    md5sum /fuzzer/afl/SanitizerCoveragePCGUARD.so

# Set build env flags.
# ENV DFSAN_FLAGS="-fsanitize=dataflow  -mllvm -dfsan-track-select-control-flow=false -mllvm -dfsan-combine-offset-labels-on-gep=false -mllvm -dfsan-combine-pointer-labels-on-load=false -fsanitize-blacklist=/fake-compiler/dfsanlist.txt"
ENV DFSAN_FLAGS="-fsanitize=memory -fsanitize-blacklist=/fake-compiler/dfsanlist.txt -Wno-unused-command-line-argument -Wno-macro-redefined"
ENV CFLAGS="${DFSAN_FLAGS}" \
    CXXFLAGS="${DFSAN_FLAGS} -nostdinc++ -nostdlib++ -isystem /clang/libcxx/include/c++/v1" \
    LDFLAGS="${DFSAN_FLAGS} -L /clang/libcxx/lib  -Wl,-rpath,/clang/libcxx/lib -lc++ -fuse-ld=lld" \
    CC="/fuzzer/afl/afl-clang-fast" \
    CXX="/fuzzer/afl/afl-clang-fast++" \
    LD="ld.lld" \
    MAKE="make" \
    AR="llvm-ar" \
    RANLIB="llvm-ranlib"
# Set AFL environment vars.
ENV AFL_I_DONT_CARE_ABOUT_MISSING_CRASHES=1 \
    AFL_SKIP_CPUFREQ=1
# Define what kind of coverage we're using.
ENV HWFUZZ_COVERAGE=$COVERAGE

# Create fake compiler.
COPY instrumentation /fake-compiler
WORKDIR /fake-compiler
RUN ./build-fake-g++.sh
# Install fake compiler.
ENV PATH="/fake-compiler/:${PATH}"
WORKDIR /fake-compiler
RUN ./install-fake-g++.sh

# ------- Install Fuzzer

# Install fuzzer.
COPY Fuzzer /driver/Fuzzer/
COPY docker/install-fuzzer.sh /driver/Fuzzer
RUN cd /driver/Fuzzer ; bash install-fuzzer.sh
ENV PATH="/driver/Fuzzer/target/release/:$PATH"

# Tell the fuzzer which bugs it needs to find.
COPY expected_findings /chipyard/sims/verilator/expected_findings
ENV FUZZING_EXPECTED_LIST /chipyard/sims/verilator/expected_findings

# ------- Build Spike.

# Build the architectural simulator.
COPY ArchSim /ArchSim/
WORKDIR /ArchSim
RUN ./build-instrumented.sh

# ------- Build Samples.
SHELL ["conda", "run", "--no-capture-output", "-p",  "/chipyard/.conda-env", "/bin/bash", "-c"]

# Copy over handy scripts.
COPY scripts /scripts
ENV PATH="/scripts/:${PATH}"

# Build RISC-V binaries.
COPY Samples /Samples
RUN cd /Samples && make clean && make
RUN ln -sf /chipyard/sims/verilator/programs  /Samples

# ------- Build custom BOOM wrapper.

# Add our custom BOOM wrapper to chipyard.
COPY boom-wrapper /boom-wrapper
RUN ln -s /boom-wrapper /chipyard/generators/fuzz && \
    cat /boom-wrapper/build.sbt.addendum >> /chipyard/build.sbt
# Use our own Makefile to generate the simulation.
RUN mv /chipyard/sims/verilator/Makefile /chipyard/sims/verilator/old-Makefile && \
    ln -sf /boom-wrapper/simulation-makefile  /chipyard/sims/verilator/Makefile

# Add options to enable MDS vuln and disable BIM initialization
RUN cd /chipyard/generators/boom && git apply /patches/boom-all.patch

# Build bootrom and init code.
RUN cd /boom-wrapper/boot && make
RUN cd /boom-wrapper/init && make

# Build the simulation.
WORKDIR /chipyard/sims/verilator
RUN phantom-trails build -j${NPROCS} --sanitizer=$SANITIZER $(echo $BOOM_BUILD_FLAGS | sed "s/'//g")
