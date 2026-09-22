#include "config.h"
#include <iostream>
#include <filesystem>
#include <format>
#include <unistd.h>
#include<pwd.h>

#include "toml++/toml.h"

namespace fs = std::filesystem;

std::string get_bot_token()
{
#ifdef _MSC_VER
    char* value = nullptr;
    size_t len = 0;
    _dupenv_s(&value, &len, "BOT_TOKEN");
    std::unique_ptr<char, decltype(&free)> guard(value, free);
    return value ? std::string(value) : std::string();
#else
    const char* value = std::getenv("BOT_TOKEN");
    return value ? std::string(value) : std::string();
#endif
}
std::string get_server_id_env()
{
    const char* value = std::getenv("SERVER_ID");
    return value ? std::string(value) : std::string();
}

void config::write_starter_config(const fs::path& config_path, std::string bot_token, std::string server_id)
{
    fs::create_directories(config_path.parent_path());
    std::ofstream my_file(config_path);
    if (!my_file.is_open())
    {
        std::cerr << "Error creating the config file!\n";
        exit(1);
    }
    if (bot_token.empty())
    {
        bot_token = "0";
        std::cerr << "Bot token environment variable was missing please go to " << config_path <<
            " and fill value in\n";
    }
    if (server_id.empty())
    {
        server_id = "0";
        std::cerr << "Server ID environment variable was missing please go to " << config_path <<
            " and fill value in\n";
    }
    std::string tmp = "# Ferrum Configuration File\n"
        "# This file contains configuration settings for the Ferrum application.\n"
        "# Please edit the values below as needed.\n\n"
        "[bot]\n"
        "token = \"{}\"\n\n"
        "[server]\n"
        "id = {}\n";
    std::string starter_config = std::vformat(tmp, std::make_format_args(bot_token, server_id));

    my_file << starter_config;
}

std::string config::Config::path()
{
    return this->config_path;
};

config::Config config::get_config()

{
    toml::table tbl;
        auto bot_token = get_bot_token();
        auto server_id = get_server_id_env();

    fs::path root_path;
    try
    {
        uid_t uid = getuid();
        struct passwd* pw = getpwuid(uid);
        if (pw && pw->pw_dir)
        {
            root_path = fs::path(pw->pw_dir);
        }
        else if (const char* home_env = std::getenv("HOME"); home_env != nullptr)
        {
            root_path = fs::path(home_env);
        }


        fs::path config_path = root_path / ".config/ferrum/ferrum.toml";
        if (!fs::exists(config_path))
        {
            write_starter_config(config_path, bot_token, server_id);
        }
        Config config(config_path);
        tbl = toml::parse_file(config.path());

        config.config = tbl;
        return config;
    }
    catch (const toml::parse_error&
        err)
    {
        std::cerr << "Parsing failed:\n" << err << "\n";
        exit(1);
    };
}

std::string config::Config::get_token()
{
    if (config["bot"]["token"] == "0")
    {
        std::cerr << "You have not set the bot token, please go set it in the file located at: \n  " << config_path <<
            "/n";
    }
    return config["bot"]["token"].value_or("");
}

long long config::Config::get_server_id()
{
    return config["server"]["id"].value<long long>().value_or(0);
}
