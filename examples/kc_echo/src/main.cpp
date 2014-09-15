// kc_echo : An example application for the KISSCPP library.
// Author : Dirk J. Botha <bothadj@gmail.com>

#include "kc_echo.hpp"

int main(int argc, char* argv[])
{
  try {
    kc_echo app;
    app.run();
  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}

