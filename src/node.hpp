#pragma once
#include <memory>
#include <string>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>

namespace zclcpp
{
    template <typename MsgPubSubType>
    class Subscription;

    template <typename MsgPubSubType>
    class Publisher;

    class Node : public std::enable_shared_from_this<Node>
    {
    public:
        explicit Node(const std::string& name)
        {
            eprosima::fastdds::dds::DomainParticipantQos pqos;
            pqos.name(name);
            participant_ =
                eprosima::fastdds::dds::DomainParticipantFactory::get_instance()
                ->create_participant(0, pqos);
        }

        ~Node()
        {
            if (participant_)
                eprosima::fastdds::dds::DomainParticipantFactory::get_instance()
                ->delete_participant(participant_);
        }

        eprosima::fastdds::dds::DomainParticipant* participant() const
        {
            return participant_;
        }

        template <typename PubSubT>
        auto create_publisher(const std::string& topic_name, int qos_depth = 10)
        {
            return std::make_shared<Publisher<PubSubT>>(
                shared_from_this(), topic_name, qos_depth);
        }

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

