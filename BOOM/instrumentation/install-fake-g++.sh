CONDA_BIN="/chipyard/.conda-env/bin"
CONDA_BIN_PREFIX="/chipyard/.conda-env/bin/x86_64-conda-linux-gnu"

# Install fake compiler in the conda environment.
ln -sf /fake-compiler/g++ ${CONDA_BIN_PREFIX}-g++
ln -sf /fake-compiler/g++ ${CONDA_BIN_PREFIX}-c++
ln -sf /fake-compiler/g++ ${CONDA_BIN}/g++
ln -sf /fake-compiler/gcc ${CONDA_BIN_PREFIX}-gcc
ln -sf /fake-compiler/gcc ${CONDA_BIN}/gcc
ln -sf /usr/bin/ld.lld ${CONDA_BIN_PREFIX}-ld
ln -sf /usr/bin/ld.lld ${CONDA_BIN}/ld
