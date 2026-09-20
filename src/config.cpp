#include "config.h"

std::string get_bot_token() {
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
