#pragma once

#include <iostream>

class Client
{
  public:
      Client();
      ~Client();

      void setName(const std::string &name);
      std::string getName() const;

  private:
      std::string _name;
};
