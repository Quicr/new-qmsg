#pragma once

#include <string>

///
/// Helper to generate common names
///
struct QuicrName {
    static constexpr auto base = "quicr://example.com/cto/v1/";
    static constexpr auto membership = "membership/";
    static constexpr auto message = "message/";


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

private:

  static std::string name_for_membership(const std::string& team_id) {
      return base  + team_id + "/" + std::string(membership);
  }

  static std::string name_for_join(const std::string& team_id) {
      return name_for_membership(team_id) + std::string("join/");
  }

  static std::string name_for_message(const std::string& team_id) {
      return base  + team_id + "/" +  std::string(message);
  }

};