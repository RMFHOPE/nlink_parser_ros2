#include "init.h"

#include "nutils.h"
#include "protocols.h"

#define ARRAY_ASSIGN(DEST, SRC)                                                \
  for (size_t _CNT = 0; _CNT < sizeof(SRC) / sizeof(SRC[0]); ++_CNT)           \
  {                                                                            \
    DEST[_CNT] = SRC[_CNT];                                                    \
  }

namespace linktrack
{
  nlink_parser_ros2_interfaces::msg::LinktrackAnchorframe0 g_msg_anchorframe0;
  nlink_parser_ros2_interfaces::msg::LinktrackTagframe0 g_msg_tagframe0;
  nlink_parser_ros2_interfaces::msg::LinktrackNodeframe0 g_msg_nodeframe0;
  nlink_parser_ros2_interfaces::msg::LinktrackNodeframe1 g_msg_nodeframe1;
  nlink_parser_ros2_interfaces::msg::LinktrackNodeframe2 g_msg_nodeframe2;
  nlink_parser_ros2_interfaces::msg::LinktrackNodeframe3 g_msg_nodeframe3;
  nlink_parser_ros2_interfaces::msg::LinktrackNodeframe5 g_msg_nodeframe5;
  nlink_parser_ros2_interfaces::msg::LinktrackNodeframe6 g_msg_nodeframe6;


  Init::Init(NProtocolExtracter *protocol_extraction, serial::Serial *serial) : Node("linktrack_ros2")
  {
    serial_ = serial;
    protocol_extraction_ = protocol_extraction;
    initDataTransmission();
    initAnchorFrame0(protocol_extraction);
    initTagFrame0(protocol_extraction);
    initNodeFrame0(protocol_extraction);
    initNodeFrame1(protocol_extraction);
    initNodeFrame2(protocol_extraction);
    initNodeFrame3(protocol_extraction);
    initNodeFrame5(protocol_extraction);
    initNodeFrame6(protocol_extraction);

    rclcpp::QoS qos(rclcpp::KeepLast(200));
    pub_anchor_frame0_= create_publisher<nlink_parser_ros2_interfaces::msg::LinktrackAnchorframe0>("nlink_linktrack_anchorframe0", qos);
    pub_tag_frame0_= create_publisher<nlink_parser_ros2_interfaces::msg::LinktrackTagframe0>("nlink_linktrack_tagframe0", qos);
    pub_node_frame0_= create_publisher<nlink_parser_ros2_interfaces::msg::LinktrackNodeframe0>("nlink_linktrack_nodeframe0", qos);
    pub_node_frame1_= create_publisher<nlink_parser_ros2_interfaces::msg::LinktrackNodeframe1>("nlink_linktrack_nodeframe1", qos);
    pub_node_frame2_= create_publisher<nlink_parser_ros2_interfaces::msg::LinktrackNodeframe2>("nlink_linktrack_nodeframe2", qos);
    pub_node_frame3_= create_publisher<nlink_parser_ros2_interfaces::msg::LinktrackNodeframe3>("nlink_linktrack_nodeframe3", qos);
    pub_node_frame5_= create_publisher<nlink_parser_ros2_interfaces::msg::LinktrackNodeframe5>("nlink_linktrack_nodeframe5", qos);
    pub_node_frame6_= create_publisher<nlink_parser_ros2_interfaces::msg::LinktrackNodeframe6>("nlink_linktrack_nodeframe6", qos);

    serial_read_timer_ =  this->create_wall_timer(std::chrono::milliseconds(1000), std::bind(&Init::serialReadTimer, this));
    RCLCPP_INFO(this->get_logger(),"Initialized linktrack");
  }

  void Init::serialReadTimer(){
    auto available_bytes = this->serial_->available();
    std::string str_received;
    if (available_bytes)
    {
      this->serial_->read(str_received, available_bytes);
      this->protocol_extraction_->AddNewData(str_received);
    }
  }

  void Init::initDataTransmission()
  {
    auto callback = [this](const std_msgs::msg::String::SharedPtr msg) -> void {
    if (this->serial_)
      this->serial_->write(msg->data);
    };
    dt_sub_ =
        create_subscription<std_msgs::msg::String>("nlink_linktrack_data_transmission", 1000, callback);
  }

  void Init::initAnchorFrame0(NProtocolExtracter *protocol_extraction)
  {
    auto protocol = new NLT_ProtocolAnchorFrame0;
    protocol_extraction->AddProtocol(protocol);
    protocol->SetHandleDataCallback([=] {
      // if (!publishers_[protocol])
      // {
      //   auto topic = "nlink_linktrack_anchorframe0";
      //   rclcpp::QoS qos(rclcpp::KeepLast(200));
      //   publishers_[protocol] =
      //       nh_.advertise<nlink_parser::LinktrackAnchorframe0>(topic, 200);
      //       create_publisher<nlink_parser_ros2_interfaces::msg::LinktrackAnchorframe0>(topic, qos);
      //   TopicAdvertisedTip(topic);
      // }
      auto data = nlt_anchorframe0_.result;
      g_msg_anchorframe0.role = data.role;
      g_msg_anchorframe0.id = data.id;
      g_msg_anchorframe0.voltage = data.voltage;
      g_msg_anchorframe0.local_time = data.local_time;
      g_msg_anchorframe0.system_time = data.system_time;
      auto &msg_nodes = g_msg_anchorframe0.nodes;
      msg_nodes.clear();
      decltype(g_msg_anchorframe0.nodes)::value_type msg_node;
      for (size_t i = 0, icount = data.valid_node_count; i < icount; ++i)
      {
        auto node = data.nodes[i];
        msg_node.role = node->role;
        msg_node.id = node->id;
        ARRAY_ASSIGN(msg_node.pos_3d, node->pos_3d)
        ARRAY_ASSIGN(msg_node.dis_arr, node->dis_arr)
        msg_nodes.push_back(msg_node);
      }
      // publishers_.at(protocol)->publish(g_msg_anchorframe0);
      pub_anchor_frame0_->publish(g_msg_anchorframe0);
    });
  }

  void Init::initTagFrame0(NProtocolExtracter *protocol_extraction)
  {
    auto protocol = new NLT_ProtocolTagFrame0;
    protocol_extraction->AddProtocol(protocol);
    protocol->SetHandleDataCallback([=] {
      // if (!publishers_[protocol])
      // {
      //   auto topic = "nlink_linktrack_tagframe0";
      //   rclcpp::QoS qos(rclcpp::KeepLast(200));
      //   publishers_[protocol] =
      //     create_publisher<nlink_parser_ros2_interfaces::msg::LinktrackTagframe0>(topic, qos);
      //   TopicAdvertisedTip(topic);
      // }

      const auto &data = g_nlt_tagframe0.result;
      auto &msg_data = g_msg_tagframe0;

      msg_data.role = data.role;
      msg_data.id = data.id;
      msg_data.local_time = data.local_time;
      msg_data.system_time = data.system_time;
      msg_data.voltage = data.voltage;
      ARRAY_ASSIGN(msg_data.pos_3d, data.pos_3d)
      ARRAY_ASSIGN(msg_data.eop_3d, data.eop_3d)
      ARRAY_ASSIGN(msg_data.vel_3d, data.vel_3d)
      ARRAY_ASSIGN(msg_data.dis_arr, data.dis_arr)
      ARRAY_ASSIGN(msg_data.imu_gyro_3d, data.imu_gyro_3d)
      ARRAY_ASSIGN(msg_data.imu_acc_3d, data.imu_acc_3d)
      ARRAY_ASSIGN(msg_data.angle_3d, data.angle_3d)
      ARRAY_ASSIGN(msg_data.quaternion, data.quaternion)

      // publishers_.at(protocol)->publish(msg_data);
      pub_tag_frame0_->publish(msg_data);
    });
  }

  void Init::initNodeFrame0(NProtocolExtracter *protocol_extraction)
  {
    auto protocol = new NLT_ProtocolNodeFrame0;
    protocol_extraction->AddProtocol(protocol);
    protocol->SetHandleDataCallback([=] {
      // if (!publishers_[protocol])
      // {
      //   auto topic = "nlink_linktrack_nodeframe0";
      //   rclcpp::QoS qos(rclcpp::KeepLast(200));
      //   publishers_[protocol] =
      //       create_publisher<nlink_parser_ros2_interfaces::msg::LinktrackNodeframe0>(topic, qos);
      //   TopicAdvertisedTip(topic);
      //   ;
      // }
      const auto &data = g_nlt_nodeframe0.result;
      auto &msg_data = g_msg_nodeframe0;
      auto &msg_nodes = msg_data.nodes;

      msg_data.role = data.role;
      msg_data.id = data.id;

      msg_nodes.resize(data.valid_node_count);
      for (size_t i = 0; i < data.valid_node_count; ++i)
      {
        auto &msg_node = msg_nodes[i];
        auto node = data.nodes[i];
        msg_node.id = node->id;
        msg_node.role = node->role;
        msg_node.data.resize(node->data_length);
        memcpy(msg_node.data.data(), node->data, node->data_length);
      }

      // publishers_.at(protocol)->publish(msg_data);
      pub_node_frame0_->publish(msg_data);
    });
  }

  void Init::initNodeFrame1(NProtocolExtracter *protocol_extraction)
  {
    auto protocol = new NLT_ProtocolNodeFrame1;
    protocol_extraction->AddProtocol(protocol);
    protocol->SetHandleDataCallback([=] {
      // if (!publishers_[protocol])
      // {
      //   auto topic = "nlink_linktrack_nodeframe1";
      //   rclcpp::QoS qos(rclcpp::KeepLast(200));
      //   publishers_[protocol] =
      //       create_publisher<nlink_parser_ros2_interfaces::msg::LinktrackNodeframe1>(topic, qos);
      //   TopicAdvertisedTip(topic);
      // }
      const auto &data = g_nlt_nodeframe1.result;
      auto &msg_data = g_msg_nodeframe1;
      auto &msg_nodes = msg_data.nodes;

      msg_data.role = data.role;
      msg_data.id = data.id;
      msg_data.local_time = data.local_time;
      msg_data.system_time = data.system_time;
      msg_data.voltage = data.voltage;

      msg_nodes.resize(data.valid_node_count);
      for (size_t i = 0; i < data.valid_node_count; ++i)
      {
        auto &msg_node = msg_nodes[i];
        auto node = data.nodes[i];
        msg_node.id = node->id;
        msg_node.role = node->role;
        ARRAY_ASSIGN(msg_node.pos_3d, node->pos_3d)
      }

      // publishers_.at(protocol)->publish(msg_data);
      pub_node_frame1_->publish(msg_data);
    });
  }

  void Init::initNodeFrame2(NProtocolExtracter *protocol_extraction)
  {
    auto protocol = new NLT_ProtocolNodeFrame2;
    protocol_extraction->AddProtocol(protocol);
    protocol->SetHandleDataCallback([=] {
      // if (!publishers_[protocol])
      // {
      //   auto topic = "nlink_linktrack_nodeframe2";
      //   rclcpp::QoS qos(rclcpp::KeepLast(200));
      //   publishers_[protocol] =
      //       create_publisher<nlink_parser_ros2_interfaces::msg::LinktrackNodeframe2>(topic, qos);
      //   TopicAdvertisedTip(topic);
      // }
      const auto &data = g_nlt_nodeframe2.result;
      auto &msg_data = g_msg_nodeframe2;
      auto &msg_nodes = msg_data.nodes;

      msg_data.role = data.role;
      msg_data.id = data.id;
      msg_data.local_time = data.local_time;
      msg_data.system_time = data.system_time;
      msg_data.voltage = data.voltage;
      ARRAY_ASSIGN(msg_data.pos_3d, data.pos_3d)
      ARRAY_ASSIGN(msg_data.eop_3d, data.eop_3d)
      ARRAY_ASSIGN(msg_data.vel_3d, data.vel_3d)
      ARRAY_ASSIGN(msg_data.imu_gyro_3d, data.imu_gyro_3d)
      ARRAY_ASSIGN(msg_data.imu_acc_3d, data.imu_acc_3d)
      ARRAY_ASSIGN(msg_data.angle_3d, data.angle_3d)
      ARRAY_ASSIGN(msg_data.quaternion, data.quaternion)

      msg_nodes.resize(data.valid_node_count);
      for (size_t i = 0; i < data.valid_node_count; ++i)
      {
        auto &msg_node = msg_nodes[i];
        auto node = data.nodes[i];
        msg_node.id = node->id;
        msg_node.role = node->role;
        msg_node.dis = node->dis;
        msg_node.fp_rssi = node->fp_rssi;
        msg_node.rx_rssi = node->rx_rssi;
      }

      // publishers_.at(protocol)->publish(msg_data);
      pub_node_frame2_->publish(msg_data);
    });
  }

  void Init::initNodeFrame3(NProtocolExtracter *protocol_extraction)
  {
    auto protocol = new NLT_ProtocolNodeFrame3;
    protocol_extraction->AddProtocol(protocol);
    protocol->SetHandleDataCallback([=] {
      // if (!publishers_[protocol])
      // {
      //   auto topic = "nlink_linktrack_nodeframe3";
      //   rclcpp::QoS qos(rclcpp::KeepLast(200));
      //   publishers_[protocol] =
      //          create_publisher<nlink_parser_ros2_interfaces::msg::LinktrackNodeframe3>(topic, qos);
      //   TopicAdvertisedTip(topic);
      // }
      const auto &data = g_nlt_nodeframe3.result;
      auto &msg_data = g_msg_nodeframe3;
      auto &msg_nodes = msg_data.nodes;

      msg_data.role = data.role;
      msg_data.id = data.id;
      msg_data.local_time = data.local_time;
      msg_data.system_time = data.system_time;
      msg_data.voltage = data.voltage;

      msg_nodes.resize(data.valid_node_count);
      for (size_t i = 0; i < data.valid_node_count; ++i)
      {
        auto &msg_node = msg_nodes[i];
        auto node = data.nodes[i];
        msg_node.id = node->id;
        msg_node.role = node->role;
        msg_node.dis = node->dis;
        msg_node.fp_rssi = node->fp_rssi;
        msg_node.rx_rssi = node->rx_rssi;
      }

      // publishers_.at(protocol)->publish(msg_data);
      pub_node_frame3_->publish(msg_data);
    });
  }

  void Init::initNodeFrame5(NProtocolExtracter *protocol_extraction)
  {
    auto protocol = new NLT_ProtocolNodeFrame5;
    protocol_extraction->AddProtocol(protocol);
    protocol->SetHandleDataCallback([=] {
      // if (!publishers_[protocol])
      // {
      //   auto topic = "nlink_linktrack_nodeframe5";
      //      rclcpp::QoS qos(rclcpp::KeepLast(200));
      //   publishers_[protocol] =
      //       create_publisher<nlink_parser_ros2_interfaces::msg::LinktrackNodeframe5>(topic, qos);
      //   TopicAdvertisedTip(topic);
      // }
      const auto &data = g_nlt_nodeframe5.result;
      auto &msg_data = g_msg_nodeframe5;
      auto &msg_nodes = msg_data.nodes;

      msg_data.role = data.role;
      msg_data.id = data.id;
      msg_data.local_time = data.local_time;
      msg_data.system_time = data.system_time;
      msg_data.voltage = data.voltage;

      msg_nodes.resize(data.valid_node_count);
      for (size_t i = 0; i < data.valid_node_count; ++i)
      {
        auto &msg_node = msg_nodes[i];
        auto node = data.nodes[i];
        msg_node.id = node->id;
        msg_node.role = node->role;
        msg_node.dis = node->dis;
        msg_node.fp_rssi = node->fp_rssi;
        msg_node.rx_rssi = node->rx_rssi;
      }

      // publishers_.at(protocol)->publish(msg_data);
      pub_node_frame5_->publish(msg_data);
    });
  }

  void Init::initNodeFrame6(NProtocolExtracter *protocol_extraction)
  {
    auto protocol = new NLT_ProtocolNodeFrame6;
    protocol_extraction->AddProtocol(protocol);
    protocol->SetHandleDataCallback([=] {
      // if (!publishers_[protocol])
      // {
      //   auto topic = "nlink_linktrack_nodeframe6";
      //   rclcpp::QoS qos(rclcpp::KeepLast(200));
      //   publishers_[protocol] =
      //       create_publisher<nlink_parser_ros2_interfaces::msg::LinktrackNodeframe6>(topic, qos);
      //   TopicAdvertisedTip(topic);
      //   ;
      // }
      const auto &data = g_nlt_nodeframe6.result;
      auto &msg_data = g_msg_nodeframe6;
      auto &msg_nodes = msg_data.nodes;

      msg_data.role = data.role;
      msg_data.id = data.id;

      msg_nodes.resize(data.valid_node_count);
      for (size_t i = 0; i < data.valid_node_count; ++i)
      {
        auto &msg_node = msg_nodes[i];
        auto node = data.nodes[i];
        msg_node.id = node->id;
        msg_node.role = node->role;
        msg_node.data.resize(node->data_length);
        memcpy(msg_node.data.data(), node->data, node->data_length);
      }

      // publishers_.at(protocol)->publish(msg_data);
      pub_node_frame6_->publish(msg_data);
    });
  }

} // namespace linktrack