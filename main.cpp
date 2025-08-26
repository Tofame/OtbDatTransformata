#include <iostream>
#include <filesystem>
#include "Items.h"
#include "Loaders/OtbLoader.h"
#include "Loaders/DatLoader.h"
#include "Compilers/DatCompiler.h"

int main() {
    auto items = Items();

    auto otbLoader = OtbLoader();
    otbLoader.loadFromOtb("data/items/items.otb", items);

    auto datLoader = DatLoader();
    datLoader.loadFromDat("data/items/Tibia.dat", items);

    // Variables
    auto& datItems = items.getItemTypesDat();
    auto& otbItems = items.getItemTypes();

    // Crystal Coins (serverId 2160) proof of correct loading:
    std::cout << otbItems.at(2160).clientId << "\n";
    std::cout << items.getCidToSidMap().getServerId(otbItems.at(2160).clientId) << "\n";
    std::cout << "\n";

    // Let's check if .dat == .otb
    std::cout << "Checking if .dat == .otb, by pickupable flag:\n";
    bool datOtbMatch = true;
    for (auto& otbItem : otbItems) {
        try {
            auto& datItem = datItems.at(otbItem.clientId);

            if(datItem.pickupable) {
                if(!datItem.pickupable) {
                    datOtbMatch = false;
                    std::cerr << "Cid " << otbItem.clientId << " " << datItem.clientId << " Sid " << otbItem.id << " are not matching pickupable (.dat side) (ERROR)!\n";
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "Error processing item (client ID: " << otbItem.clientId << "): "
                      << e.what() << std::endl;
            continue;
        }
    }

    if(!datOtbMatch) {
        std::cerr << "Result: The files .dat and .otb are NOT matching!\n";
    } else {
        std::cout << "Result: .dat and .otb are matching, properly loaded too!\n";
    }

    std::cout << "==== Duplicate Check =====\n";
    //items.checkDuplicatedClientIds();

    std::cout << "==== Check if there is any otb it that doesnt point to dat item =====\n";
    for(auto& _it : otbItems) {
        try {
            auto &datItem = datItems.at(_it.clientId);
        } catch (const std::exception& e) {
            std::cout << "Found otb with clientId " << _it.clientId
                      << " that has no dat item\n";
            break;
        }
    }
    std::cout << "Check complete. If any messages above appeared, some OTB items have no matching DAT item.\n";

    std::cout << "\n=====Start compiling\n";
    DatCompiler datCompiler;
    datCompiler.compile(datLoader, items);
    return 0;
}
