# Create fake gcc and g++ using the env flags.
echo "${CC} ${CFLAGS} \"\$@\"" > gcc && chmod +x gcc
echo "${CXX} ${CXXFLAGS} ${LDFLAGS}  \"\$@\"" > g++ && chmod +x g++

set -e
echo "int main(int argc, char **argv) { if (argc) **argv = 4; }" > /tmp/test.cpp
bash -x ./g++ /tmp/test.cpp -o /tmp/test_out -v
echo "Running test binary a few times"
/tmp/test_out
/tmp/test_out
/tmp/test_out
/tmp/test_out

# Test fake compiler.
# ./g++ tests/test1.cpp -o test1 && ./test1
# ./g++ tests/test2.cpp -o test2 && ./test2
