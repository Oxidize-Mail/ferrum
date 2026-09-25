#include "config.h"

#include <pwd.h>
#include <unistd.h>

#include <charconv>
#include <cstdint>
#include <expected>
#include <filesystem>
#include <format>
#include <iostream>

namespace fs = std::filesystem;

template <typename T>
concept StringLike = std::convertible_to<T, std::string_view>;

auto get_env_variable(StringLike auto const& key) -> std::string {
  std::string_view key_view = key;
  const char* value = std::getenv(key_view.data());
  return value ? std::string(value) : std::string();
}

auto config::write_starter_config(const fs::path& path, std::string bot_token,
                                  std::string server_id)
    -> std::expected<void, ConfigError> {
  std::error_code ec;
  fs::create_directories(path.parent_path(), ec);
  if (ec) {
    return std::unexpected(
        ConfigError{ConfigError::Kind::create_failed,
                    std::format("Could not create {}: {}",
                                path.parent_path().string(), ec.message())});
  }

  std::ofstream config_file(path);
  if (!config_file.is_open()) {
    return std::unexpected(ConfigError{
        ConfigError::Kind::create_failed,
        std::format("Could not create the config file at {}", path.string())});
  }
  std::error_code perm_error;
  fs::perms skim_perms = fs::perms::owner_read | fs::perms::owner_write;
  fs::permissions(path, skim_perms, fs::perm_options::replace, perm_error);
  if (perm_error) {
    return std::unexpected(ConfigError{
        ConfigError::Kind::create_failed,
        std::format("Failed to set permissions on config file at: {}",
                    path.string())});
  }
  std::string error_msg;

  if (bot_token.empty()) {
    bot_token = "0";
    error_msg.append(
        std::format("\nBot token environment variable was missing please go "
                    "to {} and fill value in\n",
                    path.string()));
  }

  std::uint64_t n{};
  auto [ptr, err] =
      std::from_chars(server_id.data(), server_id.data() + server_id.size(), n);
  bool ok = err == std::errc{} && ptr == server_id.data() + server_id.size();
  if (server_id.empty() || !ok) {
    server_id = "0";
    error_msg.append(
        std::format("\nServer ID environment variable was missing or invalid "
                    "please go to {} and fill value in\n",
                    path.string()));
  }
  std::string tmp =
      "# Ferrum Configuration File\n"
      "# This file contains configuration settings for the Ferrum "
      "application.\n"
      "# Please edit the values below as needed.\n\n"
      "[bot]\n"
      "token = \"{}\"\n\n"
      "[server]\n"
      "id = {}\n";
  std::string starter_config =
      std::vformat(tmp, std::make_format_args(bot_token, server_id));

  config_file << starter_config;

  // Close explicitly so a failed flush is reported here rather than swallowed
  // by the destructor.
  config_file.close();
  if (!config_file) {
    return std::unexpected(ConfigError{
        ConfigError::Kind::create_failed,
        std::format("Could not write the config file at {}", path.string())});
  }
  if (!error_msg.empty()) {
    return std::unexpected(
        ConfigError{ConfigError::Kind::invalid_value, error_msg});
  }

  return {};
}

auto config::Config::path() -> std::string { return this->config_path; };

auto config::get_config(const fs::path& path)
    -> std::expected<Config, ConfigError> {
  std::error_code exists_error;
  if (!fs::exists(path, exists_error)) {
    if (exists_error) {
      return std::unexpected(
          ConfigError{ConfigError::Kind::create_failed,
                      std::format("Could not access {}: {}", path.string(),
                                  exists_error.message())});
    }
    const auto written = write_starter_config(
        path, get_env_variable("BOT_TOKEN"), get_env_variable("SERVER_ID"));
    if (!written) {
      return std::unexpected(written.error());
    }
  }
  fs::perms correct_perms = fs::perms::owner_read | fs::perms::owner_write;
  if (fs::status(path).permissions() != correct_perms) {
    return std::unexpected(
        ConfigError{ConfigError::Kind::create_failed,
                    std::format("Config file has wrong permissions set at : {}",
                                path.string())});
  }
  Config config(path);
  try {
    config.config = toml::parse_file(config.path());
  } catch (const toml::parse_error& err) {
    return std::unexpected(
        ConfigError{ConfigError::Kind::parse_failed,
                    std::format("Could not parse {} at line {} column {}: {}",
                                path.string(), err.source().begin.line,
                                err.source().begin.column, err.description())});
  }

  return config;
}

auto config::default_path() -> std::expected<fs::path, ConfigError> {
  // The XDG base directory spec says $XDG_CONFIG_HOME wins, and that a relative
  // value must be ignored. It already names the config directory itself.
  if (const std::string xdg = get_env_variable("XDG_CONFIG_HOME");
      !xdg.empty()) {
    if (const fs::path xdg_path(xdg); xdg_path.is_absolute()) {
      return xdg_path / "ferrum/ferrum.toml";
    }
  }

  // $HOME before the passwd entry, so users and containers can redirect it.
  if (const std::string home_env = get_env_variable("HOME");
      !home_env.empty()) {
    return fs::path(home_env) / ".config/ferrum/ferrum.toml";
  }

  if (const passwd* pw = getpwuid(getuid());
      pw != nullptr && pw->pw_dir != nullptr) {
    return fs::path(pw->pw_dir) / ".config/ferrum/ferrum.toml";
  }

  return std::unexpected(ConfigError{ConfigError::Kind::no_home_directory,
                                     "Could not determine a home directory: "
                                     "none of $XDG_CONFIG_HOME, $HOME or the "
                                     "passwd entry for this user were usable"});
}

auto config::Config::delete_config() -> void {
  std::filesystem::remove(config_path);
}

auto config::Config::get_token() -> std::string {
  if (config["bot"]["token"] == "0") {
    std::cerr << "You have not set the bot token, please go set it in the file "
                 "located at: \n  "
              << config_path << "\n";
  }
  return config["bot"]["token"].value_or("");
}

auto config::Config::get_server_id() -> long long {
  return config["server"]["id"].value<long long>().value_or(0);
}
