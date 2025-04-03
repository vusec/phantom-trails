#include <cstdint>
#include <fstream>
#include <iostream>

int main(int argc, char** argv) {
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << " <HEXFILE> <BINFILE>";
    exit(-1);
  }

  const char* inName = argv[1];
  const char* outName = argv[2];

  std::string line;
  std::ifstream in(inName);
  std::ofstream wf(outName, std::ios::out | std::ios::binary);

  if (!in.is_open()) {
    std::cerr << "Couldn't open file " << inName << "\n";
    exit(-1);
  }

  if (!wf.is_open()) {
    std::cerr << "Couldn't open file " << outName << "\n";
    exit(-1);
  }

  while (std::getline(in, line)) {
#define parse_nibble(c) ((c) >= 'a' ? (c) - 'a' + 10 : (c) - '0')
    for (ssize_t i = line.length() - 2, j = 0; i >= 0; i -= 2, j++) {
      uint8_t byte = (parse_nibble(line[i]) << 4) | parse_nibble(line[i + 1]);
      wf << byte;
    }
  }

  return 0;
}
