#pragma once

#include "MetadataWriter.h"
#include "MetadataWriter6.h"
#include "../Loaders/DatLoader.h"
#include "../Items.h"

class DatCompiler {
    // 10.98 .dat compiler
    public:
        void compile(DatLoader& datLoader, Items& items);
};