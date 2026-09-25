# Ferrum

A discord bot to drive engagement and foster a positive community in the Oxidize server

Ferrum supports **Linux and macOS**. Windows is not supported: the config code
relies on POSIX APIs (`pwd.h`, `getpwuid`) and Unix file permissions.

# Contributing

## Setup

We are using CMake to compile the project with VCPKG to handle install the required CPP libraries needed to build it.

### Prerequisites

| Tool         | Version | Notes                                                 |
| ------------ | ------- | ----------------------------------------------------- |
| clang-format | 22.0.0+ | Optional, but used to format code with `clang-format` |
| CMake        | 4.0.0+  | Required by `CMakeLists.txt`                          |
| Ninja        | any     | The `vcpkg-preset` uses the Ninja generator           |
| Compiler     | C++23   | GCC 13+ or Clang 16+                                  |
| Git          | any     | Needed to clone VCPKG                                 |

On Linux, most of this comes from your package manager:

```sh
# Arch
sudo pacman -S cmake ninja git base-devel

# Debian/Ubuntu
sudo apt install cmake ninja-build git build-essential curl zip unzip tar pkg-config
```

On macOS, install the Xcode Command Line Tools (`xcode-select --install`) and
the rest from Homebrew:

```sh
brew install cmake ninja pkg-config
```

### VCPKG SETUP

If you already have VCPKG installed and `VCPKG_ROOT` exported, skip to [Build](#build).

1. Clone the VCPKG repo somewhere permanent (not inside this project):

   ```sh
   git clone https://github.com/microsoft/vcpkg.git ~/vcpkg
   ```

2. Run the bootstrap script to build the `vcpkg` executable:

   ```sh
   ~/vcpkg/bootstrap-vcpkg.sh
   ```

3. Export `VCPKG_ROOT` and add VCPKG to your `PATH`. The CMake preset reads
   `VCPKG_ROOT` to find the toolchain file, so this step is not optional.

   ```sh
   # ~/.bashrc, ~/.zshrc, or your shell's rc file
   export VCPKG_ROOT="$HOME/vcpkg"
   export PATH="$VCPKG_ROOT:$PATH"
   ```

4. Open a new shell and confirm it took:

   ```sh
   echo $VCPKG_ROOT
   vcpkg --version
   ```

We use VCPKG in manifest mode, so you do **not** need to install D++ by hand.
`vcpkg.json` pins the dependency and CMake installs it into `vcpkg_installed/`
on the first configure.

### Third-party headers

Two header-only libraries are vendored under `third_party/`, each with its
license, so there is nothing to download or place by hand:

- `dotenv/dotenv.hpp` loads a `.env` file into the environment.
- `toml++/toml.hpp` parses and writes the TOML config file.

Both are resolved from the `third_party/` include directory (see
`target_include_directories` in `CMakeLists.txt`).

### Build

From the project root:

```sh
# Configure. The first run also installs D++ via VCPKG, which takes a while.
cmake --preset vcpkg-preset

# Compile
cmake --build build
```

The preset writes everything to `build/`, and the resulting binary is
`build/ferrum`. `build/` is the canonical build directory. CLion creates its own
`cmake-build-*` directories, which are gitignored and not used by CI.

Note that the project builds with `-Wall -Wextra -Werror -Wpedantic` —
warnings are errors, so a warning in your patch will
fail the build.

### Test

The tests use GoogleTest, which VCPKG installs alongside D++. After building:

```sh
ctest --test-dir build --output-on-failure
```

CI runs the same command on every pull request to `main`.

### Configuration

The bot reads its settings from a TOML config file:

| Condition                   | Config file location                    |
| --------------------------- | --------------------------------------- |
| `$XDG_CONFIG_HOME` is set   | `$XDG_CONFIG_HOME/ferrum/ferrum.toml`   |
| Otherwise                   | `~/.config/ferrum/ferrum.toml`          |

The file looks like this:

```toml
[bot]
token = "your-token-here"

[server]
id = 123456789012345678
```

- `bot.token` is the bot token from the Discord Developer Portal.
- `server.id` is the ID of the server that guild-only commands are registered
  to. Turn on **Developer Mode** in Discord (Settings > Advanced), then
  right-click the server and choose **Copy Server ID**.

The first time the bot runs, it creates the config file and fills it in from
the `BOT_TOKEN` and `SERVER_ID` environment variables. At startup it also loads
a `.env` file from the current working directory, so you can put the values
there instead of exporting them. The file is created with owner-only
permissions (`0600`), because it holds the token.

1. Create a Discord application at the
   [Discord Developer Portal](https://discord.com/developers/applications),
   open the **Bot** tab, and use **Reset Token** to generate a token.

2. Either copy the example env file and fill it in:

   ```sh
   cp .example_env .env
   ```

   ```
   BOT_TOKEN="your-token-here"
   SERVER_ID=123456789012345678
   ```

   or skip this step and edit the config file directly after the first run.

   `.env` is gitignored. Never commit it or the config file, and reset the token
   in the portal if it ever leaks.

3. Invite the bot to a test server from the **OAuth2 > URL Generator** tab,
   selecting the `bot` and `applications.commands` scopes.

The environment variables are only used to create the file. Once it exists,
the bot reads only the config file, so later changes to `.env` have no effect.
To start over, delete the config file and run the bot again.

If a variable is missing when the file is created, the bot writes `0` as a
placeholder and exits, printing the path of the file to edit.

### Run

Run the binary from the project root so it can find `.env`:

```sh
./build/ferrum
```

You should see the shard-ready log line on startup. The bot registers its slash
commands on first run. Global commands can take up to an hour to propagate to
every server. Guild commands are registered only to the server set in
`server.id` and appear immediately.

Currently registered commands:

| Command              | Scope  | Description       |
| -------------------- | ------ | ----------------- |
| `/ping`              | Global | ping JKBoyo       |
| `/praise`            | Global | Get potatoes      |
| `/high-praise`       | Guild  | give high praise  |
| `/wiz`               | Guild  | wizard            |
| `/make-a-difference` | Guild  | make a difference |
