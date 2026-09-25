#include <commands.h>

namespace commands {

auto ping() -> std::string { return "JKBoyo shush your dirty mouth!"; }

auto praise() -> std::string {
  return "Bow to our glorious LEADER Brinhasavlin";
}

auto high_praise() -> std::string {
  return "Bow to our glorious LEADER Brinhasavlin!! Creator of the "
         "mighty Potato. long may he reign!!!";
}

auto wiz() -> std::string { return "Hey Merlin, you are a mighty wizard "; }
auto make_a_difference() -> std::string {
  return "If you ever wanted to make a difference in compilers of the world, "
         "then all you need to do is join up and become a SoftOmni "
         "contributor";
}

auto CommandRegistry::register_command(Command cmd) -> void {
  handlers_[cmd.name] = cmd.handler;
  definitions_.push_back(std::move(cmd));
}

auto CommandRegistry::find(std::string_view name) const
    -> const std::function<std::string()>* {
  auto it = handlers_.find(std::string(name));
  return (it != handlers_.end()) ? &it->second : nullptr;
}

auto CommandRegistry::get_all_commands() const -> const std::vector<Command>& {
  return definitions_;
}

}  // namespace commands
