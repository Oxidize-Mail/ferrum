#ifndef FERRUM_CONFIG

#define FERRUM_CONFIG
#include <expected>
#include <filesystem>
#include <string>

#include <toml++/toml.hpp>

namespace config
{
    /*
     * @brief why loading or creating the configuration failed
     */
    struct ConfigError
    {
        enum class Kind
        {
            no_home_directory, // could not work out where the user's config should live
            create_failed, // config was absent and could not be written
            parse_failed, // config exists but is not valid TOML
            invalid_value // Environment variable wasn't set or was invalid
        };

        Kind kind;
        std::string message; // human readable, ready to print
    };

    /*
     * @brief holds configuration for bot
     */
    class Config
    {
    private:
        std::filesystem::path config_path;

    public:
        // Returns system path to config file
        auto path() -> std::string;

        // Returns server id to be used for command registration to a specific server
        auto get_server_id() -> long long;

        // Returns bot token to be used for authentication with discord
        auto get_token() -> std::string;

        // Deletes config file
        auto delete_config() -> void;

        toml::table config;

        explicit Config(std::filesystem::path path) : config_path(std::move(path))
        {
        };
        Config() = default;
    };

    //static std::string get_bot_token();

    // Loads config from disk, if config isn't present it writes the config to disk,
    // filing in server id and bot token from environment variables if they are loaded in to shell.
    // If the environment variables aren't set it uses 0 for placeholder and notifies user to go update the config file.
    auto get_config(const std::filesystem::path& path) -> std::expected<Config, ConfigError>;

    // Resolves where the config file should live, preferring $XDG_CONFIG_HOME, then
    // $HOME, then the passwd entry for the current user.
    auto default_path() -> std::expected<std::filesystem::path, ConfigError>;


    /* Helper function for get_config, used if config file is not present
     *
     * @param path: path to config file on system
     * @param bot_token: api key for bot registered with discord
     * @param server_id: id of server to register specific commands with
     *
     */
    auto write_starter_config(const std::filesystem::path& path, std::string bot_token,
                              std::string server_id) -> std::expected<void, ConfigError>;
}

#endif
