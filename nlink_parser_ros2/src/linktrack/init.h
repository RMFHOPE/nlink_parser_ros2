#ifndef LINKTRACKINIT_H
#define LINKTRACKINIT_H

// #include <ros/ros.h>
#include <rclcpp/rclcpp.hpp>
#include <serial/serial.h>

#include <unordered_map>

#include "std_msgs/msg/string.hpp"
#include <nlink_parser_ros2_interfaces/msg/linktrack_anchorframe0.hpp>
#include <nlink_parser_ros2_interfaces/msg/linktrack_nodeframe0.hpp>
#include <nlink_parser_ros2_interfaces/msg/linktrack_nodeframe1.hpp>
#include <nlink_parser_ros2_interfaces/msg/linktrack_nodeframe2.hpp>
#include <nlink_parser_ros2_interfaces/msg/linktrack_nodeframe3.hpp>
#include <nlink_parser_ros2_interfaces/msg/linktrack_nodeframe5.hpp>
#include <nlink_parser_ros2_interfaces/msg/linktrack_nodeframe6.hpp>
#include <nlink_parser_ros2_interfaces/msg/linktrack_tagframe0.hpp>

#include "nlink_unpack/nlink_utils.h"
#include "protocol_extracter/nprotocol_extracter.h"

class NProtocolExtracter;
namespace linktrack
{
  class Init  : public rclcpp::Node 
  {
  public:
    explicit Init(NProtocolExtracter *protocol_extraction,
                  serial::Serial *serial);
  serial::Serial *serial_;

  private:
    NProtocolExtracter* protocol_extraction_;

    void initDataTransmission();
    void serialReadTimer();
    void initAnchorFrame0(NProtocolExtracter *protocol_extraction);
    void initTagFrame0(NProtocolExtracter *protocol_extraction);
    void initNodeFrame0(NProtocolExtracter *protocol_extraction);
    void initNodeFrame1(NProtocolExtracter *protocol_extraction);
    void initNodeFrame2(NProtocolExtracter *protocol_extraction);
    void initNodeFrame3(NProtocolExtracter *protocol_extraction);
    void initNodeFrame5(NProtocolExtracter *protocol_extraction);
    void initNodeFrame6(NProtocolExtracter *protocol_extraction);

    std::unordered_map<NProtocolBase *, rclcpp::Publisher<nlink_parser_ros2_interfaces::msg::LinktrackNodeframe2>::SharedPtr> publishers_;
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr dt_sub_;
    rclcpp::TimerBase::SharedPtr serial_read_timer_;

    rclcpp::Publisher<nlink_parser_ros2_interfaces::msg::LinktrackAnchorframe0>::SharedPtr pub_anchor_frame0_;
    rclcpp::Publisher<nlink_parser_ros2_interfaces::msg::LinktrackTagframe0>::SharedPtr pub_tag_frame0_;
    rclcpp::Publisher<nlink_parser_ros2_interfaces::msg::LinktrackNodeframe0>::SharedPtr pub_node_frame0_;
    rclcpp::Publisher<nlink_parser_ros2_interfaces::msg::LinktrackNodeframe1>::SharedPtr pub_node_frame1_;
    rclcpp::Publisher<nlink_parser_ros2_interfaces::msg::LinktrackNodeframe2>::SharedPtr pub_node_frame2_;
    rclcpp::Publisher<nlink_parser_ros2_interfaces::msg::LinktrackNodeframe3>::SharedPtr pub_node_frame3_;
    rclcpp::Publisher<nlink_parser_ros2_interfaces::msg::LinktrackNodeframe5>::SharedPtr pub_node_frame5_;
    rclcpp::Publisher<nlink_parser_ros2_interfaces::msg::LinktrackNodeframe6>::SharedPtr pub_node_frame6_;
    // ros::NodeHandle nh_;
    // ros::Subscriber dt_sub_;
  };
} // namespace linktrack

#endif // LINKTRACKINIT_H
