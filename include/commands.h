#pragma once

#include <dpp/dpp.h>

#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace commands {

// Pure logic functions
auto ping() -> std::string;
auto praise() -> std::string;
auto high_praise() -> std::string;
auto wiz() -> std::string;
auto make_a_difference() -> std::string;

struct Command {
  std::string name;
  std::string description;
  std::optional<dpp::snowflake> guild_id;
  std::function<std::string()> handler;
};

class CommandRegistry {
 public:
  auto register_command(Command cmd) -> void;
  auto find(std::string_view name) const -> const std::function<std::string()>*;
  [[nodiscard]] auto get_all_commands() const -> const std::vector<Command>&;

 private:
  std::unordered_map<std::string, std::function<std::string()>> handlers_;
  std::vector<Command> definitions_;
};

}  // namespace commands
