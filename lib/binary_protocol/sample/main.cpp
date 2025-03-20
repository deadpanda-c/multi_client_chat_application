#include "BinaryProtocol.hpp"

int main(void)
{
  std::string data = "Hello World!";

  std::string encoded = BinaryProtocol::encode(data);
  std::string decoded = BinaryProtocol::decode(encoded);

  std::cout << "Encoded: " << encoded << std::endl;
  std::cout << "Decoded: " << decoded << std::endl;

  return 0;
}
