#pragma once

namespace sbd::constants {
    static constexpr auto INDEX_FILE = "index";
    static constexpr auto DATA_FILE_NAME = "data";
    static constexpr auto TEMP_INDEX_FILE_NAME = "temp_index";
    static constexpr auto TEMP_DATA_FILE_NAME = "temp_data";

    static constexpr auto PAGE_SIZE = 60ull;
	static constexpr auto DATA_SIZE = 7ull;
    static constexpr auto HEADER_SIZE = 8ull;
    static constexpr auto INITIAL_PAGES_COUNT = 3ull;
    static constexpr auto OVERFLOW_RATIO = 0.2;
    static constexpr auto DATA_RECORD_SIZE = HEADER_SIZE + DATA_SIZE;
    static constexpr size_t DATA_RECORD_PER_PAGE = PAGE_SIZE / DATA_RECORD_SIZE;
    static constexpr auto INDEX_BLOCKING_FACTOR = PAGE_SIZE/HEADER_SIZE;
    static constexpr auto INCORRECT_RECORD_KEY = 0xFFFFFFFFull;
    static constexpr auto MAX_RECORD_KEY = 0x0FFFFFFFull;
    static constexpr auto REORGANISATION_ALPHA = 0.5;
}
