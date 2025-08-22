#pragma once

#include <cstdint>
#include <vector>

class ClientIdToServerIdMap {
public:
    ClientIdToServerIdMap() {
        vec.reserve(30000);
    }

    void emplace(uint16_t clientId, uint16_t serverId) {
        if (clientId >= vec.size()) {
            vec.resize(vec.size() * 2, 0);
        }

        if (vec[clientId] == 0) {
            vec[clientId] = serverId;
        }
    }

    uint16_t getServerId(uint16_t clientId) const {
        uint16_t serverId = 0;
        if (clientId < vec.size()) {
            serverId = vec[clientId];
        }
        return serverId;
    }

    void clear() {
        vec.clear();
    }

private:
    std::vector<uint16_t> vec;
};