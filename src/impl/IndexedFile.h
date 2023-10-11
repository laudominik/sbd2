#pragma once

#include "Record.h"
#include <File.h>

namespace sbd::impl {
    class IndexedFile {
        generic::File<IndexRecord> index;
        generic::File<DataRecord> data;
    };
}


