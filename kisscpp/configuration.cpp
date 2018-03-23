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
  Config* Config::instance(const std::string &app_id               /* = "kisscpp_application" */,
                           const std::string &app_instance         /* = "0" */,
                           const std::string &explicit_config_path /* = "" */)
  {
    if (!singleton_instance) {
      singleton_instance = new Config(app_id, app_instance, explicit_config_path);
    }

    return singleton_instance;
  }

  //--------------------------------------------------------------------------------
  void Config::initiate(const std::string &explicit_config_path /* = "" */)
  {
    kisscpp::LogStream log(__PRETTY_FUNCTION__);
    std::string        cfg_root_path = "/etc/kisscpp";
    char              *kcpp_cfg_root = std::getenv("KCPP_CFG_ROOT");
    char              *kcpp_exec_env = std::getenv("KCPP_EXEC_ENV");

    if(explicit_config_path.empty()) {
      if(kcpp_cfg_root) {
        cfg_root_path = kcpp_cfg_root;
      }
    } else {
      cfg_root_path   = explicit_config_path;
    }

    cfg_root_path    += "/" + application_id;

    if(kcpp_exec_env) {
      cfg_root_path  += "/" + std::string(kcpp_exec_env);
    }

    config_path_instance = cfg_root_path + '/' + application_id + '.' + application_instance + ".kcppcfg";
    config_path_common   = cfg_root_path + '/' + application_id + '.' + "common"             + ".kcppcfg";

    log << "About to load configuration for instance [" << config_path_instance
        << "] and common ["                             << config_path_common
        << kisscpp::manip::flush;

    loadConfig();
  }

  //--------------------------------------------------------------------------------
  bool Config::isAllowedIp(const std::string &ip_address)
  {
    bool retval = false;

    if(allow_all_ip_addrs) {
      retval = true;
    } else {
      retval = (comms_white_list_ip_addrs.find(ip_address) != comms_white_list_ip_addrs.end());
    }

    return retval;
  }

  //--------------------------------------------------------------------------------
  bool Config::isAllowedClient(const std::string &app_id, const std::string &app_instance)
  {
    bool retval = false;

    if(allow_all_applications) {
      retval = true;
    } else if (comms_white_list_applications.find(app_id) != comms_white_list_applications.end()) {
      retval = ((comms_white_list_applications[app_id]).isAllowedInstance(app_instance));
    }

    return retval;
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

    populateWhiteLists();
    populateDefaultDirs();
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

  //--------------------------------------------------------------------------------
  void Config::populateWhiteLists()
  {
    if(cfg_data.find("kcc-white-list") != cfg_data.not_found()) {

      allow_all_ip_addrs     = (cfg_data.get<std::string>("kcc-white-list.all-ip-addrs","false") == "true")?true:false;
      allow_all_applications = (cfg_data.get<std::string>("kcc-white-list.all-apps"    ,"false") == "true")?true:false;

      if(!allow_all_ip_addrs) {
        BOOST_FOREACH(boost::property_tree::ptree::value_type &v, cfg_data.get_child("kcc-white-list.ip-list")) {
          comms_white_list_ip_addrs.insert(v.second.data());
        }
      }

      if(!allow_all_applications) {
        BOOST_FOREACH(boost::property_tree::ptree::value_type &v, cfg_data.get_child("kcc-white-list.application-list")) {

          if(v.first == "application") {

            std::string app_id                                    = v.second.get<std::string>("id");
            (comms_white_list_applications[app_id]).all_instances = (v.second.get<std::string>("all-instances","false") == "true")?true:false;

            if(!(comms_white_list_applications[app_id]).all_instances) {
              BOOST_FOREACH(boost::property_tree::ptree::value_type &app_data, v.second.get_child("instance-list")) {
                (comms_white_list_applications[app_id]).addAppInstance(app_data.second.data());
              }
            }
          }

        }
      }

    } else {

      allow_all_ip_addrs     = true;
      allow_all_applications = true;

    }
  }

  //--------------------------------------------------------------------------------
  void Config::populateDefaultDirs()
  {
    std::string  cache_dir       = "/tmp";
    std::string  queue_dir       = "/tmp";
    char        *kcpp_cache_root = std::getenv("KCPP_CACHE_ROOT");
    char        *kcpp_queue_root = std::getenv("KCPP_QUE_ROOT");

    if(kcpp_cache_root && kcpp_queue_root) {
      cache_dir = kcpp_cache_root;
      queue_dir = kcpp_queue_root;
    } else if(kcpp_cache_root) {
      cache_dir = kcpp_cache_root;
      queue_dir = cache_dir;
    } else if(kcpp_queue_root) {
      queue_dir = kcpp_queue_root;
      cache_dir = queue_dir;
    }

    cache_dir += ("/" + application_id + "/" + application_instance);
    queue_dir += ("/" + application_id + "/" + application_instance);

    bfs::path cache_path(cache_dir);
    bfs::path queue_path(queue_dir);

    bfs::create_directories(cache_path);
    bfs::create_directories(queue_path);

    cfg_data.put("app-cache-dir", cache_dir);
    cfg_data.put("app-queue-dir", queue_dir);
  }
}

