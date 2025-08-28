#include <iostream>
#include <filesystem>
#include "Items.h"
#include "Loaders/OtbLoader.h"
#include "Loaders/DatLoader.h"
#include "Compilers/DatCompiler.h"

int main() {
    auto items = Items();

    auto otbLoader = OtbLoader();
    auto datLoader = DatLoader();
    try {
        otbLoader.loadFromOtb("data/items/items.otb", items);
        datLoader.loadFromDat("data/items/Tibia.dat", items);
    } catch (const std::exception& e) {
        std::cerr << "[Error] " << e.what() << std::endl;
        std::cout << "\nPress ENTER to exit...";
        std::cin.get();
        return 1;
    }

    // Variables
    auto& datItems = items.getItemTypesDat();
    auto& otbItems = items.getItemTypes();

    // Tests of .otb/.dat, for stage 1
    bool _checkCidSidMap = false;
    bool _checkDatOtbMatch = false;
    bool _printDuplicatedClientIds = false; // it's always gonna happen, it's whole reason .otb and .dat exist
    // because one server id itemtype can point to several client ids.
    bool _checkMissingClientItems = false; // that situation shouldn't be possible, if it is, it means something
    // went VERY wrong. There is always more sids than cids, and OTB is based on DAT, so a SID item should ALWAYS
    // point to atleast one CID item.

    // Crystal Coins (serverId 2160) proof of correct loading:
    if(_checkCidSidMap) {
        std::cout << "==== Check cid/sid mapping by using Crystal Coin ====\n";
        std::cout << otbItems.at(2160).clientId << "\n";
        std::cout << items.getCidToSidMap().getServerId(otbItems.at(2160).clientId) << "\n";
        std::cout << "\n";
    }

    // Let's check if .dat == .otb
    if(_checkDatOtbMatch) {
    std::cout << "==== Check if .dat == .otb, by pickupable flag ====\n";
    bool datOtbMatch = true;
    for (auto& otbItem : otbItems) {
        try {
            auto& datItem = datItems.at(otbItem.clientId);

            if(otbItem.pickupable) {
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
        std::cerr << "Result: The files .dat and .otb are NOT matching, update .otb first!\n";
        return 0;
    } else {
        std::cout << "Result: .dat and .otb are matching, properly loaded too!\n";
    }
    }

    if(_printDuplicatedClientIds) {
        std::cout << "\n==== Duplicate Check =====\n";
        items.checkDuplicatedClientIds();
    }

    if(_checkMissingClientItems) {
        std::cout << "\n==== Check if there is any otb item that doesnt point to dat item =====\n";
        for (auto &_it: otbItems) {
            try {
                auto &datItem = datItems.at(_it.clientId);
            } catch (const std::exception &e) {
                std::cout << "Found otb with clientId " << _it.clientId
                          << " that has no dat item\n";
                break;
            }
        }
        std::cout << "Check complete. If any messages above appeared, some OTB items have no matching DAT item.\n";
    }

    // BlackTek little quiver adjustment
    if(Settings::getInstance().fixBlackTekQuiver) {
        auto &_qdat = datItems.at(11469);
        _qdat.isContainer = true;
        auto &_qotb = otbItems.at(12425);
        _qotb.isContainer = true;
    }

    // Prepare substitute itemtype for e.g. deprecateds
    items.substituteItemType = ItemType();

    std::cout << "\n=====Start compiling\n";
    DatCompiler datCompiler;
    datCompiler.compile(datLoader, items);

    std::cout << "\nPress ENTER to exit...";
    std::cin.get();
    return 0;
}
