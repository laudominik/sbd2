#pragma once

namespace sbd::constants {
    static constexpr auto INDEX_FILE = "index";
    static constexpr auto PAGE_SIZE = 60ull;
	static constexpr auto DATA_SIZE = 7ull;
    static constexpr auto HEADER_SIZE = 8ull;
    static constexpr auto INITIAL_PAGES_COUNT = 2ull;
    static constexpr auto DATA_RECORD_SIZE = HEADER_SIZE + DATA_SIZE;
    static constexpr auto INDEX_BLOCKING_FACTOR = PAGE_SIZE/HEADER_SIZE;
	static constexpr auto DATA_FILE_NAME = "data";
    static constexpr auto INCORRECT_RECORD_KEY = 0xFFFFFFFFull;
}
