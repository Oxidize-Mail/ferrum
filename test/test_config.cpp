//
// Created by ghostvox on 9/22/26.
//

#include <unistd.h>

#include <cstdlib>
#include <filesystem>
#include <format>
#include <fstream>
#include <optional>
#include <string>

#include "config.h"
#include "gtest/gtest.h"

namespace fs = std::filesystem;

class TestConfig : public testing::Test {
 protected:
  // Config file under test. Its parent directory deliberately does not exist
  // yet, so get_config has to create it the way it would on a fresh install.
  fs::path config_path;

  void SetUp() override {
    setenv("BOT_TOKEN", "123", 1);
    setenv("SERVER_ID", "123", 1);

    // Test name keeps the tests apart; pid keeps concurrent runs of the binary,
    // and other users on a shared machine, out of each other's way.
    const testing::TestInfo* info =
        testing::UnitTest::GetInstance()->current_test_info();
    config_path = fs::path(testing::TempDir()) /
                  std::format("ferrum-{}-{}", info->name(), getpid()) /
                  "ferrum.toml";

    // A test that dies never reaches TearDown, so clean on the way in as well.
    fs::remove_all(config_path.parent_path());
    ASSERT_FALSE(fs::exists(config_path.parent_path()));
  };

  void TearDown() override { fs::remove_all(config_path.parent_path()); };
};

TEST_F(TestConfig, ReadToken) {
  auto cfg = config::get_config(config_path);
  ASSERT_TRUE(cfg.has_value()) << cfg.error().message;
  EXPECT_EQ("123", cfg->get_token());
}

TEST_F(TestConfig, ReadServerId) {
  auto cfg = config::get_config(config_path);
  ASSERT_TRUE(cfg.has_value()) << cfg.error().message;
  EXPECT_EQ(123, cfg->get_server_id());
}

TEST_F(TestConfig, MalformedConfigIsReportedNotFatal) {
  fs::create_directories(config_path.parent_path());
  fs::perms skim_perms =fs::perms::owner_read | fs::perms::owner_write;
  std::ofstream(config_path) << "[bot\ntoken = \n";
  fs::permissions(config_path, skim_perms, fs::perm_options::replace);

  const auto cfg = config::get_config(config_path);
  ASSERT_FALSE(cfg.has_value());
  EXPECT_EQ(config::ConfigError::Kind::parse_failed, cfg.error().kind);
  EXPECT_NE(std::string::npos, cfg.error().message.find(config_path.string()));
}

TEST_F(TestConfig, UnwritableLocationIsReportedNotFatal) {
  // Make the parent a regular file, so the directory can never be created.
  fs::create_directories(config_path.parent_path().parent_path());
  std::ofstream(config_path.parent_path()) << "not a directory\n";

  const auto cfg = config::get_config(config_path);
  ASSERT_FALSE(cfg.has_value());
  EXPECT_EQ(config::ConfigError::Kind::create_failed, cfg.error().kind);
}

// Saves and restores the environment variables default_path() reads, so these
// tests cannot leak into each other or into the rest of the suite.
class TestDefaultPath : public testing::Test {
 protected:
  void SetUp() override {
    saved_xdg = capture("XDG_CONFIG_HOME");
    saved_home = capture("HOME");
  };

  void TearDown() override {
    restore("XDG_CONFIG_HOME", saved_xdg);
    restore("HOME", saved_home);
  };

 private:
  static auto capture(const char* name) -> std::optional<std::string> {
    const char* value = std::getenv(name);
    return value != nullptr ? std::optional<std::string>(value) : std::nullopt;
  }

  static auto restore(const char* name, const std::optional<std::string>& value)
      -> void {
    if (value.has_value()) {
      setenv(name, value->c_str(), 1);
    } else {
      unsetenv(name);
    }
  }

  std::optional<std::string> saved_xdg;
  std::optional<std::string> saved_home;
};

TEST_F(TestDefaultPath, PrefersXdgConfigHome) {
  setenv("XDG_CONFIG_HOME", "/xdg-config", 1);
  setenv("HOME", "/home/ignored", 1);

  const auto path = config::default_path();
  ASSERT_TRUE(path.has_value()) << path.error().message;
  EXPECT_EQ(fs::path("/xdg-config/ferrum/ferrum.toml"), *path);
}

TEST_F(TestDefaultPath, FallsBackToHome) {
  unsetenv("XDG_CONFIG_HOME");
  setenv("HOME", "/home/fallback", 1);

  const auto path = config::default_path();
  ASSERT_TRUE(path.has_value()) << path.error().message;
  EXPECT_EQ(fs::path("/home/fallback/.config/ferrum/ferrum.toml"), *path);
}

TEST_F(TestDefaultPath, IgnoresRelativeXdgConfigHome) {
  // The XDG spec says a relative $XDG_CONFIG_HOME must be ignored.
  setenv("XDG_CONFIG_HOME", "relative/path", 1);
  setenv("HOME", "/home/fallback", 1);

  const auto path = config::default_path();
  ASSERT_TRUE(path.has_value()) << path.error().message;
  EXPECT_EQ(fs::path("/home/fallback/.config/ferrum/ferrum.toml"), *path);
}
