#pragma once
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/topic/Topic.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>


namespace zclcpp
{
    class Node;

    template <typename MsgPubSubType>
    class Publisher
    {
    public:
        using MsgT = typename MsgPubSubType::type;
        using SharedPtr = std::shared_ptr<Publisher<MsgPubSubType>>;

        Publisher(std::shared_ptr<Node> node,
            const std::string& topic_name,
            int qos_depth)
            : node_(node)
        {
            // 1. 注册类型
            type_.reset(new MsgPubSubType());
            type_.register_type(node_->participant());

            // 2. Topic
            //check if topic already exists
            topic_ = node_->participant()->find_topic(topic_name, eprosima::fastdds::dds::Duration_t(1));
            if (topic_ == nullptr)
            {
                topic_ = node_->participant()->create_topic(
                    topic_name, type_.get_type_name(),
                    eprosima::fastdds::dds::TOPIC_QOS_DEFAULT);
            }

            // 3. Publisher / DataWriter
            pub_ = node_->participant()->create_publisher(
                eprosima::fastdds::dds::PUBLISHER_QOS_DEFAULT, nullptr);

            eprosima::fastdds::dds::DataWriterQos wqos;
            wqos.history().kind = eprosima::fastdds::dds::KEEP_LAST_HISTORY_QOS;
            wqos.history().depth = qos_depth;
            writer_ = pub_->create_datawriter(topic_, wqos);
        }

        void publish(const typename MsgPubSubType::type& msg)
        {
            writer_->write(const_cast<MsgT*>(&msg));
        }

    private:
        std::shared_ptr<Node> node_;
        eprosima::fastdds::dds::TypeSupport type_;
        eprosima::fastdds::dds::Topic* topic_{ nullptr };
        eprosima::fastdds::dds::Publisher* pub_{ nullptr };
        eprosima::fastdds::dds::DataWriter* writer_{ nullptr };
    };
};