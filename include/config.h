#ifndef FERRUM_CONFIG

#define FERRUM_CONFIG
#include <filesystem>
#include <string>

#include "toml++/toml.h"

namespace config
{
    class Config
    {
    private:
        std::filesystem::path config_path;

    public:
        Config(std::filesystem::path path) : config_path(std::move(path))
        {
        };
        long long get_server_id();
        std::string get_token();
        toml::table config;
        std::string path();
    };

    //static std::string get_bot_token();
    Config get_config();
    void write_starter_config(const std::filesystem::path&, std::string, std::string);
}

#endif
