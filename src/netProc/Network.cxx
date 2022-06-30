
#include "Network.h"
#include <string>

void Network::set_kphash_for_welcome(std::string&& team_id, std::string&& hash)
{
    if (!kp_hash.empty() && kp_hash == hash) {
        // todo unsubscribe
        return;
    }

    kp_hash = hash;
    // subscribe to receive the key_package
    auto name = QuicrName::name_for_kp_hash(team_id, kp_hash)
    qr_client.subscribe({name}, false);
}