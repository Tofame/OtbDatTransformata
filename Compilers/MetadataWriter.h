#pragma once

#include "../ItemType.h"
#include "../Loaders/fileLoader.h"

class MetadataWriter {
protected:
    virtual bool writeProperties(const ItemType& type, PropWriteStream& stream);
    virtual bool writeItemProperties(const ItemType& type, PropWriteStream& stream);
};