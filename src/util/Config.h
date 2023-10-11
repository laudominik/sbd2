#pragma once


namespace sbd::util {
    class Config {
    public:
        void switchDebugMode() {
            debugMode = !debugMode;
        }
        [[nodiscard]] bool isDebugModeEnabled() const{
            return debugMode;
        }

        static Config& instance(){
            static Config config;
            return config;
        }
    private:
        bool debugMode{false};
    };
}

