
#include "Network.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <thread>

#include "names.h"

///
///  Utility
///
static std::string
to_hex(const quicr::bytes& data)
{
    std::stringstream hex(std::ios_base::out);
    hex.flags(std::ios::hex);
    for (const auto& byte : data) {
        hex << std::setw(2) << std::setfill('0') << int(byte);
    }
    return hex.str();
}

Network::Network(const std::string& server_ip, const uint16_t port)
  : qr_client(delegate, server_ip, port)
{
    while(!qr_client.is_transport_ready()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cout << "Transport is ready" << std::endl;
}

void Network::check_network_messages(std::vector<QuicrMessageInfo>& messages_out)
{
    return delegate.get_queued_messages(messages_out);
}

void Network::publish(uint32_t team_id, uint32_t channel_id, uint16_t device_id, quicr::bytes&& data)
{
    auto qname = QuicrName::name_for_device(std::to_string(team_id),
                                         std::to_string(channel_id),
                                         std::to_string(device_id));
    std::cout << "[Network]: Publishing for the device: " << qname << std::endl;
    publish(std::move(qname), std::move(data));
}

void Network::subscribe_to_devices(uint32_t team_id, uint32_t channel_id, std::vector<uint16_t>&& devices)
{
    auto qnames = std::vector<std::string>{};
    std::transform(std::begin(devices),
                   std::end(devices),
                   std::back_inserter(qnames),
                   [team_id, channel_id](const auto& device) {
                        return QuicrName::name_for_device(std::to_string(team_id),
                                                          std::to_string(channel_id),
                                                          std::to_string(device));
                    });

    for(auto& name: qnames) {
        std::cout << "[Network] Subscribing to Device " << name << std::endl;
    }

    subscribe(std::move(qnames));
}

void Network::unsubscribe_from_device(uint32_t team_id, uint32_t channel_id, uint16_t device_id)
{}

void Network::subscribe_for_keypackage(uint32_t team_id, quicr::bytes&& kp_hash)
{
    auto kp_hash_str = to_hex(kp_hash);
    std::cout << "[Network]:subscribe_for_keypackage: hash: " << kp_hash_str << std::endl;
    if(keypackage_hashes.count(team_id) && keypackage_hashes[team_id] == kp_hash_str) {
        std::cout << "[Network]:subscribe_for_keypackage: matching hash no-op " << kp_hash_str << std::endl;
        return;
    }

    auto qname = QuicrName::name_for_kp_hash(std::to_string(team_id), to_hex(kp_hash));
    std::cout << "[Network]:subscribe_for_keypackage: QName " << qname << std::endl;

    subscribe({qname});

    keypackage_hashes[team_id] = kp_hash_str;
}

// event handlers


void Network::handleKeyPackageEvent(EventSource source, const uint32_t team_id, quicr::bytes&& key_package,
                                    quicr::bytes&& key_package_hash)
{
    if(source == EventSource::SecProc) {
        std::cout << "[Network]:handleKeyPackageEvent: team " << team_id  << std::endl;
        // subscribe for welcome and commit
        subscribe({QuicrName::name_for_welcome(std::to_string(team_id), to_hex(key_package_hash)),
                   QuicrName::name_for_commit(std::to_string(team_id))});

        // publish the keypackage to the leader
        auto qname = QuicrName::name_for_kp_hash(std::to_string(team_id), to_hex(key_package_hash));
        std::cout << "[Network]:handleKeyPackageEvent: Publishing to QName: " << qname << std::endl;
        publish(std::move(qname), std::move(key_package));
    }
}

void Network::handleMLSWelcomeEvent(EventSource source, const uint32_t team_id, quicr::bytes&& welcome)
{
    if(source == EventSource::SecProc) {
        // publish the event
        if(!keypackage_hashes.count(team_id)) {
            // log error
            return;
        }

        auto kp_hash = keypackage_hashes[team_id];
        auto qname = QuicrName::name_for_welcome(std::to_string(team_id), kp_hash);
        publish(std::move(qname), std::move(welcome));
    }
}

void Network::handleMLSCommitEvent(EventSource source, const uint32_t team_id, quicr::bytes&& commit)
{
    if(source == EventSource::SecProc) {

        auto qname = QuicrName::name_for_commit(std::to_string(team_id));
        publish(std::move(qname), std::move(commit));
    }
}


///
/// Private API
///

void Network::publish(std::string&& name, quicr::bytes&& data)
{
    if(!publisher_registration_status.count(name)) {
        qr_client.register_names({name}, true);
        publisher_registration_status[name] = true;
    }

    std::cout << "publishing :" << to_hex(data) << std::endl;
    qr_client.publish_named_data(name, std::move(data), 1, 0);
}

void Network::subscribe(std::vector<std::string>&& names)
{

    // todo: apply set_intersection??
    auto qnames = std::move(names);
    for (auto it = qnames.begin(); it != qnames.end(); it++)
    {
        if (subscribers.count(*it))
        {
            qnames.erase(it--);
        }
    }

    // send subscribes to all the remaining names
    qr_client.subscribe(qnames, true, true);
}
