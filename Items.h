#pragma once

#include <vector>
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
    private:
        std::vector<ItemType> itemTypes;
        ClientIdToServerIdMap clientIdToServerIdMap;

        // Temp, just for proof of concept
        std::vector<ItemType> itemTypesDat;
};