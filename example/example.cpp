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
        //由于std::shared_ptr的限制，无法直接在构造函数中create_publisher，需要使用init进行二次初始化
    }

    void init()
    {
        this->string_publisher_ = this->create_publisher<std_msgs::StringPubSubType>("my_string_topic", 10);
        this->string_subscription_ = this->create_subscription<std_msgs::StringPubSubType>(
            "my_string_topic", 10,
            [this](const std_msgs::String& msg) {
                std::cout << "Received message: " << msg.data() << std::endl;
            });
    }

    void run()
    {
        // Simulate some work, e.g., publishing messages
        std_msgs::String msg;
        msg.data("Hello from My Node Fast DDS!");
        string_publisher_->publish(msg);
    }

private:
    std::shared_ptr<zclcpp::Publisher<std_msgs::StringPubSubType>> string_publisher_;
    std::shared_ptr<zclcpp::Subscription<std_msgs::StringPubSubType>> string_subscription_;

};

int main()
{
    std::cout << "Hello, Fast DDS!" << std::endl;

    auto my_node = std::make_shared<MyNode>("MyFastDDSNode"); //可以继承node，并在里面实现自己的逻辑
    my_node->init(); //调用init进行二次初始化


    auto std_node = std::make_shared<zclcpp::Node>("StdFastDDSNode"); //也可以直接使用node类
    auto std_publisher = std_node->create_publisher<std_msgs::StringPubSubType>("my_string_topic", 10);

    while (true)
    {
        my_node->run();
        std_msgs::String msg;
        msg.data("Hello from std node Fast DDS!");
        std_publisher->publish(msg);

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }




}