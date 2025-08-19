/**
 * @file subscriber.hpp
 * @author zishun.zhou (zhouzishun@mail.zzshub.cn)
 * @brief This file defines the Subscription class for Fast DDS integration.
 * @version 1.0
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once
#include <functional>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>
#include <fastdds/dds/subscriber/SampleInfo.hpp>

namespace zclcpp
{
    class Node;

    /**
     * @brief Represents a DDS subscription.
     *
     * @tparam MsgPubSubType The message type used for subscribing.
     */
    template <typename MsgPubSubType>
    class Subscription : public eprosima::fastdds::dds::DataReaderListener
    {
    public:
        /// @brief Message type for Subscription.
        using MsgT = typename MsgPubSubType::type;

        /// @brief Shared pointer type for Subscription.
        using SharedPtr = std::shared_ptr<Subscription<MsgPubSubType>>;

        /// @brief Callback type for message reception.
        using Callback = std::function<void(const typename MsgPubSubType::type&)>;

        /**
         * @brief Construct a new Subscription object, it is recommended to use the create_subscription() method of the Node class instead.
         *
         * @param node node pointer
         * @param topic_name The name of the topic
         * @param qos_depth The QoS depth
         * @param cb The callback function
         */
        Subscription(std::shared_ptr<Node> node,
            const std::string& topic_name,
            int qos_depth,
            Callback cb)
            : node_(node), cb_(cb)
        {
            type_.reset(new MsgPubSubType());
            type_.register_type(node_->participant());

            topic_ = node_->participant()->find_topic(topic_name, eprosima::fastdds::dds::Duration_t(1));
            if (topic_ == nullptr)
            {
                topic_ = node_->participant()->create_topic(
                    topic_name, type_.get_type_name(),
                    eprosima::fastdds::dds::TOPIC_QOS_DEFAULT);
            }

            sub_ = node_->participant()->create_subscriber(
                eprosima::fastdds::dds::SUBSCRIBER_QOS_DEFAULT, nullptr);

            eprosima::fastdds::dds::DataReaderQos rqos;
            rqos.history().kind = eprosima::fastdds::dds::KEEP_LAST_HISTORY_QOS;
            rqos.history().depth = qos_depth;
            reader_ = sub_->create_datareader(topic_, rqos, this);
        }

        /// @brief internal method to handle data availability, user should NOT call this directly.
        /// @param reader user should NOT call this directly.
        void on_data_available(eprosima::fastdds::dds::DataReader* reader) override
        {
            MsgT msg;
            eprosima::fastdds::dds::SampleInfo info;
            while (reader->take_next_sample(&msg, &info) == eprosima::fastdds::dds::RETCODE_OK)
            {
                if (info.valid_data && cb_)
                    cb_(msg);
            }
        }

    private:
        std::shared_ptr<Node> node_;
        Callback cb_;
        eprosima::fastdds::dds::TypeSupport type_;
        eprosima::fastdds::dds::Topic* topic_{ nullptr };
        eprosima::fastdds::dds::Subscriber* sub_{ nullptr };
        eprosima::fastdds::dds::DataReader* reader_{ nullptr };
    };
};

