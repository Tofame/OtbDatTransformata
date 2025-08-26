#pragma once

#include <vector>
#include <iostream>
#include <unordered_map>
#include "ItemType.h"
#include "Loaders/ClientIdToServerIdMap.h"

class Items
{
    public:
        Items();

        // non-copyable
        Items(const Items&) = delete;
        Items& operator=(const Items&) = delete;

        std::vector<ItemType>& getItemTypes() {
            return itemTypes;
        };
        ClientIdToServerIdMap& getCidToSidMap() {
            return clientIdToServerIdMap;
        }

        // Temp, just for proof of concept
        std::vector<ItemType>& getItemTypesDat() {
            return itemTypesDat;
        };

        void checkDuplicatedClientIds() {
            // Map: clientId -> list of server IDs that use it
            std::unordered_map<uint16_t, std::vector<uint16_t>> duplicates;

            for (uint16_t serverId = 0; serverId < itemTypes.size(); ++serverId) {
                uint16_t clientId = itemTypes[serverId].clientId;

                if (clientId == 0) continue; // skip empty items

                duplicates[clientId].push_back(serverId);
            }

            std::cout << "Duplicated client IDs:\n";
            for (auto& [clientId, servers] : duplicates) {
                if (servers.size() > 1) {
                    std::cout << "Client ID " << clientId << " used by server IDs: ";
                    for (auto id : servers)
                        std::cout << id << " ";
                    std::cout << "\n";
                }
            }
        }
    private:
        std::vector<ItemType> itemTypes;
        ClientIdToServerIdMap clientIdToServerIdMap;

        // Temp, just for proof of concept
        std::vector<ItemType> itemTypesDat;
};