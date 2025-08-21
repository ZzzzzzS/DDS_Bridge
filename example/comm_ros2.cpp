#include "iostream"
#include "node.hpp"
#include "subscriber.hpp"
#include "publisher.hpp"
#include "std_msgs.hpp"
#include "std_msgsPubSubTypes.hpp"
#include "geometry_msgs/msg/TwistPubSubTypes.hpp"
#include <thread>

int main()
{
    std::cout << "Hello, Fast DDS!" << std::endl;


    auto std_node = std::make_shared<zclcpp::Node>("ros2_comm_node"); //也可以直接使用node类
    //使用ros2 topic echo /ros2_send 查看话题
    auto std_publisher = std_node->create_publisher<std_msgs::msg::StringPubSubType>("rt/ros2_send", 10);

    //使用ros2 topic pub /ros2_receive std_msgs/msg/String "{data: 'Hello, ROS 2!'}"  发布话题 
    auto std_subscription = std_node->create_subscription<std_msgs::msg::StringPubSubType>(
        "rt/ros2_receive", 10,
        [](const std_msgs::msg::String& msg) {
            std::cout << "Received message: " << msg.data() << std::endl;
        });

    //使用ros2 topic echo /send_cmd  查看话题
    auto twist_sub = std_node->create_publisher<geometry_msgs::msg::TwistPubSubType>("rt/send_cmd", 10);

    geometry_msgs::msg::Twist twist_msg;
    std_msgs::msg::String msg;
    msg.data("Hello from std node Fast DDS!");
    while (true)
    {
        std_publisher->publish(msg);


        twist_msg.linear().x(twist_msg.linear().x() + 1.0);
        twist_msg.linear().y(0.0);
        twist_msg.linear().z(0.0);
        twist_sub->publish(twist_msg);

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}