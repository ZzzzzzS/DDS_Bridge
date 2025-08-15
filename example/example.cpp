#include "iostream"
#include "node.hpp"
#include "subscriber.hpp"
#include "publisher.hpp"
#include "std_msgs.hpp"
#include "std_msgsPubSubTypes.hpp"
#include "geometry_msgs.hpp"
#include "geometry_msgsPubSubTypes.hpp"
#include <thread>


class MyNode : public zclcpp::Node
{
public:
    explicit MyNode(const std::string& name) : Node(name)
    {

    }

    void init()
    {
        // Node initialization logic can go here
        this->string_publisher_ = this->create_publisher<std_msgs::StringPubSubType>("my_string_topic", 10);
        this->string_subscription_ = this->create_subscription<std_msgs::StringPubSubType>(
            "my_string_topic", 10,
            [this](const std_msgs::String& msg) {
                std::cout << "Received message: " << msg.data() << std::endl;
            });
    }

private:
    std::shared_ptr<zclcpp::Publisher<std_msgs::StringPubSubType>>string_publisher_;
    std::shared_ptr<zclcpp::Subscription<std_msgs::StringPubSubType>> string_subscription_;

};

int main()
{
    std::cout << "Hello, Fast DDS!" << std::endl;

    auto my_node = std::make_shared<MyNode>("MyFastDDSNode");
    my_node->init();
    while (true)
    {
        // Simulate some work, e.g., publishing messages
        std_msgs::String msg;
        msg.data("Hello from Fast DDS!");
        my_node->create_publisher<std_msgs::StringPubSubType>("my_string_topic", 10)->publish(msg);

        // Sleep or wait for incoming messages
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}