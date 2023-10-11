#pragma once

namespace sbd::constants {
	static constexpr auto PAGE_SIZE = 60ull;
	static constexpr auto DATA_SIZE = 7ull;
    static constexpr auto HEADER_SIZE = 8ull;
    static constexpr auto DATA_RECORD_SIZE = HEADER_SIZE + DATA_SIZE;
	static constexpr auto DATA_FILE_NAME = "data";
    static constexpr auto INDEX_FILE_NAME = "index";
}
