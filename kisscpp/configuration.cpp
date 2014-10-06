// File  : configuration.cpp
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

#include "configuration.hpp"

namespace kisscpp
{
  Config                          * Config::singleton_instance;

  //--------------------------------------------------------------------------------
  Config* Config::instance(const std::string app_id               /* = "kisscpp_application" */,
                           const std::string app_instance         /* = "0" */,
                           const std::string explicit_config_path /* = "" */)
  {
    if (!singleton_instance) {
      singleton_instance = new Config(app_id, app_instance, explicit_config_path);
    }

    return singleton_instance;
  }

  //--------------------------------------------------------------------------------
  void Config::initiate(const std::string explicit_config_path /* = "" */)
  {
    std::string        cfg_root_path     = "/etc/kisscpp";
    std::string        execution_env     = "";
    char              *kcpp_cfg_root     = std::getenv("KCPP_CFG_ROOT");
    char              *kcpp_exec_env     = std::getenv("KCPP_EXEC_ENV");
    std::stringstream  t_config_path_instance;
    std::stringstream  t_config_path_common;

    if(explicit_config_path.empty()) {
      if(kcpp_cfg_root) {
        cfg_root_path = kcpp_cfg_root;
      }
    } else {
      cfg_root_path   = explicit_config_path;
    }

    if(kcpp_exec_env) {
      execution_env = kcpp_exec_env;
    }

    t_config_path_instance << cfg_root_path        << '/'
                           << execution_env        << '/'
                           << application_id       << '.'
                           << application_instance << ".kcppcfg";

    t_config_path_common   << cfg_root_path        << '/'
                           << execution_env        << '/'
                           << application_id       << '.'
                           << "common"             << ".kcppcfg";

    config_path_instance = t_config_path_instance.str();
    config_path_common   = t_config_path_common.str();

    loadConfig();
  }

  //--------------------------------------------------------------------------------
  void Config::loadConfig()
  {
    if(loadConfig(config_path_common, cfg_data)) {          // If the common configuration could be loaded.

      BoostPtree cfg_instance;

      if(loadConfig(config_path_instance, cfg_instance)) {  // Try to load the instance configuration into a local BoostPtree
        ptreeMerge(cfg_data, cfg_instance);                 // Merge the local BoostPtree into the configuration BoostPrtee
      }

    } else {                                                // If we could not load the common config.
      loadConfig(config_path_instance, cfg_data);           // Try to load the instance config.
    }
  }

  //--------------------------------------------------------------------------------
  bool Config::loadConfig(std::string &cfg_path, BoostPtree &pt)
  {
    LogStream log(__PRETTY_FUNCTION__);
    bool      retval = false;
    bfs::path cp(cfg_path);

    if(bfs::exists(cp) && bfs::is_regular_file(cp)) {       // if a file exists an attemtp will be made to read it.
      boost::property_tree::read_json(cfg_path, pt);        // if the attemtp fails the application will have to deal with
      retval = true;                                        // the resulting exception, as that is considered a fatal error.
    } else {                                                // However, if there is no file to begin with. It's entirally
      log << kisscpp::manip::info_high                      // possible that the application does not require configuration.
          << "Could not load Config: File ["                // i.e. No exception will be thrown.
          << cfg_path                                       // We do log it though, so devs will at least be aware of it.
          << "] does not exists or is not a regular file."
          << kisscpp::manip::flush;
    }

    return retval;
  }
}
