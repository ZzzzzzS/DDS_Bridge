/**
 * @file publisher.hpp
 * @author zishun.zhou (zhouzishun@mail.zzshub.cn)
 * @brief this file defines the Publisher class for Fast DDS integration.
 * @version 1.0
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/topic/Topic.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>


namespace zclcpp
{
    class Node;

    /**
     * @brief Represents a DDS publisher.
     *
     * @tparam MsgPubSubType The message type used for publishing.
     */
    template <typename MsgPubSubType>
    class Publisher
    {
    public:
        /// @brief Message type for Publisher.
        using MsgT = typename MsgPubSubType::type;

        /// @brief Shared pointer type for Publisher.
        using SharedPtr = std::shared_ptr<Publisher<MsgPubSubType>>;

        /**
         * @brief Construct a new Publisher object, it is recommended to use the create_publisher() method of the Node class instead.
         *
         * @param node node pointer
         * @param topic_name The name of the topic
         * @param qos_depth The QoS depth
         */
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

        /**
         * @brief Publish a message.
         *
         * @param msg The message to publish.
         */
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