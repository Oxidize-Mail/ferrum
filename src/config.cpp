#include "config.h"
#include <iostream>
#include <filesystem>
#include <format>
#include <unistd.h>
#include<pwd.h>

#include "toml++/toml.h"

namespace fs = std::filesystem;

static auto get_bot_token() -> std::string
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

static auto get_server_id_env() -> std::string
{
    const char* value = std::getenv("SERVER_ID");
    return value ? std::string(value) : std::string();
}

auto config::write_starter_config(const fs::path& path, std::string bot_token,
                                  std::string server_id) -> std::expected<void, ConfigError>
{
    std::error_code ec;
    fs::create_directories(path.parent_path(), ec);
    if (ec)
    {
        return std::unexpected(ConfigError{
            ConfigError::Kind::create_failed,
            std::format("Could not create {}: {}", path.parent_path().string(), ec.message())
        });
    }

    std::ofstream config_file(path);
    if (!config_file.is_open())
    {
        return std::unexpected(ConfigError{
            ConfigError::Kind::create_failed,
            std::format("Could not create the config file at {}", path.string())
        });
    }
    if (bot_token.empty())
    {
        bot_token = "0";
        std::cerr << "Bot token environment variable was missing please go to " << path <<
            " and fill value in\n";
    }
    if (server_id.empty())
    {
        server_id = "0";
        std::cerr << "Server ID environment variable was missing please go to " << path <<
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

    config_file << starter_config;

    // Close explicitly so a failed flush is reported here rather than swallowed by
    // the destructor.
    config_file.close();
    if (!config_file)
    {
        return std::unexpected(ConfigError{
            ConfigError::Kind::create_failed,
            std::format("Could not write the config file at {}", path.string())
        });
    }
    return {};
}

auto config::Config::path() -> std::string
{
    return this->config_path;
};

auto config::get_config(const fs::path& path) -> std::expected<Config, ConfigError>
{
    if (!fs::exists(path))
    {
        const auto written = write_starter_config(path, get_bot_token(), get_server_id_env());
        if (!written)
        {
            return std::unexpected(written.error());
        }
    }

    Config config(path);
    try
    {
        config.config = toml::parse_file(config.path());
    }
    catch (const toml::parse_error& err)
    {
        return std::unexpected(ConfigError{
            ConfigError::Kind::parse_failed,
            std::format("Could not parse {} at line {} column {}: {}", path.string(),
                        err.source().begin.line, err.source().begin.column, err.description())
        });
    }
    return config;
}

auto config::default_path() -> std::expected<fs::path, ConfigError>
{
    // The XDG base directory spec says $XDG_CONFIG_HOME wins, and that a relative
    // value must be ignored. It already names the config directory itself.
    if (const char* xdg = std::getenv("XDG_CONFIG_HOME"); xdg != nullptr && *xdg != '\0')
    {
        if (const fs::path xdg_path(xdg); xdg_path.is_absolute())
        {
            return xdg_path / "ferrum/ferrum.toml";
        }
    }

    // $HOME before the passwd entry, so users and containers can redirect it.
    if (const char* home_env = std::getenv("HOME"); home_env != nullptr && *home_env != '\0')
    {
        return fs::path(home_env) / ".config/ferrum/ferrum.toml";
    }

    if (const passwd* pw = getpwuid(getuid()); pw != nullptr && pw->pw_dir != nullptr)
    {
        return fs::path(pw->pw_dir) / ".config/ferrum/ferrum.toml";
    }

    return std::unexpected(ConfigError{
        ConfigError::Kind::no_home_directory,
        "Could not determine a home directory: none of $XDG_CONFIG_HOME, $HOME or the "
        "passwd entry for this user were usable"
    });
}

auto config::Config::delete_config() -> void
{
    std::filesystem::remove(config_path);
}

auto config::Config::get_token() -> std::string
{
    if (config["bot"]["token"] == "0")
    {
        std::cerr << "You have not set the bot token, please go set it in the file located at: \n  " << config_path <<
            "\n";
    }
    return config["bot"]["token"].value_or("");
}

auto config::Config::get_server_id() -> long long
{
    return config["server"]["id"].value<long long>().value_or(0);
}
