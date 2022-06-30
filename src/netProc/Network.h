#pragma once
#include <map>
#include <queue>
#include <iostream>

#include <quicr/quicr_client.h>

struct QuicrMessageProcessor {
    virtual void on_quicr_message(const std::string& name, quicr::bytes&& message, std::uint64_t object_id) = 0;
};

struct QuicrDelegate: public quicr::QuicRClient::Delegate {

    QuicrDelegate(std::shared_ptr<QuicrMessageProcessor> msg_processor_in);

    virtual void on_data_arrived(const std::string& name,
                               quicr::bytes&& data,
                               std::uint64_t group_id,
                               std::uint64_t object_id) override {

      std::lock_guard<std::mutex> lock(queue_mutex);
      received_byte_queues[name].push(data);
  }

    virtual void on_connection_close(const std::string& name) override{
        log(quicr::LogLevel::info, "[Delegate] Media Connection Closed: " + name);
    }

    virtual void log(quicr::LogLevel level, const std::string& message) override {
        std::clog <<  message << std::endl;
    }

private:
  std::shared_ptr<QuicrMessageProcessor> msg_processor;
  std::mutex queue_mutex;
  std::map<std::string, std::queue<quicr::bytes>> received_byte_queues;
};

struct QuicrName {
    static constexpr auto base = "quicr://example.com/cto/v1/";
    static constexpr auto membership = "membership/";
    static constexpr auto message = "message/";

    static std::string name_for_membership(const std::string& team_id) {
        return base  + team_id + std::string(membership);
    }

    static std::string name_for_message(const std::string& team_id) {
        return base  + team_id + std::string(message);
    }

    static std::string name_for_join(const std::string& team_id) {
        return name_for_membership(team_id) + std::string("join/");
    }

    static std::string name_for_commit(const std::string& team_id)
    {
        return name_for_membership(team_id) + std::string("commit/");
    }

    static std::string name_for_kp_hash(const std::string& team_id, const std::string& hash)
    {
        return name_for_join(team_id) + hash;
    }

    static std::string name_for_welcome(const std::string& team_id, const std::string& hash)
    {
        return name_for_join(team_id) + hash + "/welcome";
    }

    static std::string name_for_device(const std::string& team_id, const std::string& channel, const std::string& device_id ) {
        return name_for_message(team_id) + channel + "/" + device_id;
    }
};

struct TeamInfo {
    uint16_t device_id;
    uint32_t & team_id;
};

enum struct EventSource {
    SecProc = 0,
    Network = 1
};
// Main class holding logic to participate
// in the Qmsg Flow
struct Network : public QuicrMessageProcessor
{
  explicit Network(const std::string& server_ip, const uint16_t port);
  ~Network() = default;

  // public api
  void set_kphash_for_welcome(std::string&& team_id, std::string&& hash);

  // event handlers
  void handleDeviceInfo(const uint32_t team_id, const uint16_t device_id);
  void handleKeyPackageEvent(EventSource source, const uint32_t team_id, quicr::bytes& key_package);
  void handleMLSWelcomeEvent(const uint32_t team_id, quicr::bytes& bytes);
  void handleMLSCommitEvent(const uint32_t team_id, quicr::bytes& bytes);


  // quicr message processor
  virtual void on_quicr_message(const std::string& name, quicr::bytes&& message, std::uint64_t object_id) override;

private:

  std::string kp_hash;

  QuicrDelegate delegate;
  quicr::QuicRClient qr_client;
};