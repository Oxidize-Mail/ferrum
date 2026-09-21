#include <commands.h>

namespace commands {

std::string ping() { return "JKBoyo shush your dirty mouth!"; }

std::string praise() { return "Bow to our glorious LEADER Brinhasavlin"; }

std::string high_praise() {
  return "Bow to our glorious LEADER Brinhasavlin!! Creator of the "
         "mighty Potato. long may he reign!!!";
}

std::string wiz() { return "Hey Merlin, you are a mighty wizard "; }
std::string make_a_difference() {
  return "If you ever wanted to make a difference in compilers of the world, "
         "then all you need to do is join up and become a SoftOmni "
         "contributor";
}

void CommandRegistry::register_command(Command cmd) {
  handlers_[cmd.name] = cmd.handler;
  definitions_.push_back(std::move(cmd));
}

const std::function<std::string()>* CommandRegistry::find(
    std::string_view name) const {
  auto it = handlers_.find(std::string(name));
  return (it != handlers_.end()) ? &it->second : nullptr;
}

const std::vector<Command>& CommandRegistry::get_all_commands() const {
  return definitions_;
}

}  // namespace commands
