// File  : main.cpp
// Author: Dirk J. Botha <bothadj@gmail.com>
//
// This file is part of kisscpp library.
//
// The kisscpp library is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// The kisscpp library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with the kisscpp library. If not, see <http://www.gnu.org/licenses/>.

#include "boost/program_options.hpp"

#include "kcsrt.hpp"

namespace bpo = boost::program_options;

int main(int argc, char* argv[])
{
  try {

    bpo::options_description desc("Options");

    desc.add_options()
      ("help,h"         , "Print help messages")
      ("console-mode,C" , "Start the application in console-mode")
      ("instance,I"     , bpo::value<std::string>()->required(), "Instance id for the process you wish to start up.")
      ("out-port,o"     , bpo::value<std::string>()->required(), "Port for outgoing communications");

    bpo::variables_map vm;

    try {
      bpo::store(bpo::command_line_parser(argc, argv).options(desc).run(), vm); // throws on error

      if(vm.count("help")) {
        std::cout << "Add some usage detail here." << std::endl;
        return 0;
      }

      bpo::notify(vm); // throws on error, so do after help in case there are any problems
    } catch(boost::program_options::required_option& e) {
      std::cerr << "ERROR: " << e.what() << std::endl;
      return 1;
    } catch(boost::program_options::error& e) {
      std::cerr << "ERROR: " << e.what() << std::endl;
      return 1;
    }

    kcsrt app(vm["instance"].as<std::string>(),
              vm["out-port"].as<std::string>(),
              !vm.count("console-mode"));

    app.run();

  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}

