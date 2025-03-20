#include "Utils.hpp"

int main(void)
{
  std::vector<std::string> result = Utils::split("Hello,World,How,Are,You", ',');

  for (std::string str : result)
    std::cout << str << std::endl;

  return 0;
}
