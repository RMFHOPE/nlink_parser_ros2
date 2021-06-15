#include "init_serial.h"
#include <yaml-cpp/yaml.h>
#include "rclcpp/rclcpp.hpp"

#include <string>
/*
void enumerate_ports() {
  auto devices_found = serial::list_ports();
  auto iter = devices_found.begin();
  while (iter != devices_found.end()) {
    serial::PortInfo device = *iter++;

    printf("(%s, %s, %s)\n", device.port.c_str(), device.description.c_str(),
           device.hardware_id.c_str());
  }
  std::string test;
  test.clear();
}
*/

bool initSerial(serial::Serial *serial, char *param_file_path)
{
  try
  {
    YAML::Node port_config = YAML::LoadFile(param_file_path)["port_config"];
    YAML::const_iterator it = port_config.begin();
    std::map<std::string, std::string> port_config_dict;
    for (; it != port_config.end(); ++it)
    {
        auto key = it->first.as<std::string>();
        port_config_dict[key] = it->second.as<std::string>();
    }
    // auto get_value = [&](const std::string& key)-> void{
    //   auto _value = port_config_dict.find(key);
    //   if(_value==port_config_dict.end()){
    //     std::cout<< "Failed to get param: "<< key<<", please check the config file and retry."<<std::endl;
    //     exit(EXIT_FAILURE);
    //   } else {
    //     return _value->second();
    //   }
    // };

    // auto port_name = "/dev/ttyUSB0";
    
    if(port_config_dict.find("port_name")==port_config_dict.end() || port_config_dict.find("baudrate")==port_config_dict.end()){
      std::cout<< "Failed to get a param please check the config file and retry."<<std::endl;
      exit(EXIT_FAILURE);
    }
    std::string port_name, baudrate;
    auto search = port_config_dict.find("port_name");
    if(search==port_config_dict.end()){
      port_name = "/dev/ttyUSB0";
    } else {
      port_name = search->second;
    }
    
    search = port_config_dict.find("baudrate");
    if(search==port_config_dict.end()){
      baudrate = "921600";
    } else {
      baudrate = search->second;    
    }
        
    // ros::param::param<int>("~baud_rate", 921600);
    serial->setPort(port_name);
    serial->setBaudrate(static_cast<uint32_t>(std::stoi(baudrate)));
    // RCLCPP_INFO("try to open serial port with %s,%d", port_name.data(), baud_rate);
    std::cout<< "try to open serial port with port: " << port_name << " baud rate: "<< baudrate << std::endl;
    auto timeout = serial::Timeout::simpleTimeout(10);
    // without setTimeout,serial can not write any data
    // https://stackoverflow.com/questions/52048670/can-read-but-cannot-write-serial-ports-on-ubuntu-16-04/52051660?noredirect=1#comment91056825_52051660
    serial->setTimeout(timeout);
    serial->open();

    if (serial->isOpen())
    {
      std::cout<< "Serial port opened successfully, waiting for data."<<std::endl;
    }
    else
    {
      // RCLCPP_ERROR("Failed to open serial port, please check and retry.");
      std::cout<< "Failed to open serial port, please check and retry."<<std::endl;
      exit(EXIT_FAILURE);
    }
      // std::cout<< "init_Serial_done"<<std::endl;
      return true;
  }
  catch (const std::exception &e)
  {
    // RCLCPP_ERROR("Unhandled Exception: %s", e.what());
    std::cout<< "Unhandled Exception"<<e.what()<<std::endl;
    exit(EXIT_FAILURE);
  }
}
