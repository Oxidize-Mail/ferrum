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
std::string ping();
std::string praise();
std::string high_praise();
std::string wiz();
std::string make_a_difference();

struct Command {
  std::string name;
  std::string description;
  std::optional<dpp::snowflake> guild_id;
  std::function<std::string()> handler;
};

class CommandRegistry {
 public:
  void register_command(Command cmd);
  const std::function<std::string()>* find(std::string_view name) const;
  [[nodiscard]] const std::vector<Command>& get_all_commands() const;

 private:
  std::unordered_map<std::string, std::function<std::string()>> handlers_;
  std::vector<Command> definitions_;
};

}  // namespace commands
