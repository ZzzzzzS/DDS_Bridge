/**
 * @file node.hpp
 * @author zishun.zhou (zhouzishun@mail.zzshub.cn)
 * @brief this file defines the Node class for Fast DDS integration.
 * @version 1.0
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once
#include <memory>
#include <string>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>

 /// @brief Namespace for zzs communication library.
namespace zclcpp
{
    template <typename MsgPubSubType>
    class Subscription;

    template <typename MsgPubSubType>
    class Publisher;

    /**
     * @brief Represents a DDS node.
     *
     * This class is responsible for creating and managing DDS publishers and subscribers.
     */
    class Node : public std::enable_shared_from_this<Node>
    {
    public:
        explicit Node(const std::string& name, eprosima::fastdds::dds::DomainId_t domain_id = 0)
        {
            eprosima::fastdds::dds::DomainParticipantQos pqos;
            pqos.name(name);
            participant_ =
                eprosima::fastdds::dds::DomainParticipantFactory::get_instance()
                ->create_participant(domain_id, pqos);
        }

        ~Node()
        {
            if (participant_)
                eprosima::fastdds::dds::DomainParticipantFactory::get_instance()
                ->delete_participant(participant_);
        }


        /**
         * @brief Get the DDS DomainParticipant.
         *
         * @return eprosima::fastdds::dds::DomainParticipant*
         */
        eprosima::fastdds::dds::DomainParticipant* participant() const
        {
            return participant_;
        }

        /**
         * @brief Create a publisher object
         *
         * @tparam PubSubT publisher pubsub type
         * @param topic_name The name of the topic
         * @param qos_depth The QoS depth
         * @return auto the created publisher
         */
        template <typename PubSubT>
        auto create_publisher(const std::string& topic_name, int qos_depth = 10)
        {
            return std::make_shared<Publisher<PubSubT>>(
                shared_from_this(), topic_name, qos_depth);
        }

        /**
         * @brief Create a subscription object
         *
         * @tparam PubSubT subscription pubsub type
         * @param topic_name The name of the topic
         * @param qos_depth The QoS depth
         * @param cb The callback to be called when a message is received
         * @return auto the created subscription
         */
        template <typename PubSubT>
        auto create_subscription(const std::string& topic_name,
            int qos_depth,
            typename Subscription<PubSubT>::Callback cb)
        {
            return std::make_shared<Subscription<PubSubT>>(
                shared_from_this(), topic_name, qos_depth, std::move(cb));
        }


    private:
        eprosima::fastdds::dds::DomainParticipant* participant_{ nullptr };
    };
}

