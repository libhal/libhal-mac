# libhal-mac

[![✅ Checks](https://github.com/libhal/libhal-mac/actions/workflows/ci.yml/badge.svg)](https://github.com/libhal/libhal-mac/actions/workflows/ci.yml)
[![GitHub stars](https://img.shields.io/github/stars/libhal/libhal-mac.svg)](https://github.com/libhal/libhal-mac/stargazers)
[![GitHub forks](https://img.shields.io/github/forks/libhal/libhal-mac.svg)](https://github.com/libhal/libhal-mac/network)
[![GitHub issues](https://img.shields.io/github/issues/libhal/libhal-mac.svg)](https://github.com/libhal/libhal-mac/issues)

User space driver libraries for the Mac OSX operating system.

## 📚 Software APIs & Usage

To learn about available kernel APIs, documentation will be be made available soon.

To see some example code on how various APIs are used, see the programs inside the `demos` folder.

## 🧰 Setup

Following the
[🚀 Getting Started](https://libhal.github.io/latest/getting_started/)
instructions.

## 📡 Installing Profiles

> [!INFO]
> Make sure you have installed the default conan profile for your mac OS before
> proceeding.

Profiles define which platform you mean to build your project against. These
profiles are needed by the conan files to determine which files to build for demos and for libraries. In order to build binaries using libhal for the mac OS, run the following command.

```bash
conan config install -sf conan/profiles/v1 -tf profiles https://github.com/libhal/libhal-mac.git
```

The `mac-hal` profile will be added to your conan `profiles` directory. Now
that you have that installed, you can build demos and libraries.

## 🏗️ Building Demos

To build demos, start at the root of the repo and execute the following command:

```bash
conan build demos -pr mac-hal
```

If you'd like to make a minimum sized binary or a debug binary use the following commands:

```bash
# For minimum sized binaries (optimized for small code size)
conan build demos -pr mac-hal -s build_type=MinSizeRel
```

```bash
# For debug binaries (low optimization)
conan build demos -pr mac-hal -s build_type=Debug
```

We default to `Release` builds when building for Mac OSX.
By default the binaries will be built in the `./demos/build/mac/Release`
directory. There you can find your executables and build artifacts.
For `MinSizeRel` the binaries will be in `./demos/build/mac/MinSizeRel`.
For `Debug` the binaries will be in `./demos/build/mac/Debug`.

## 💾 Executing Programs

To execute the program simply run the following:

For `Release` builds:

```bash
./demos/build/mac/Release/mac_demos_serial
```

For `MinSizeRel` builds:

```bash
./demos/build/mac/MinSizeRel/mac_demos_serial
```

For `Debug` builds:

```bash
./demos/build/mac/Debug/mac_demos_serial
```

## Contributing

See [`CONTRIBUTING.md`](CONTRIBUTING.md) for details.

## License

Apache 2.0; see [`LICENSE`](LICENSE) for details.
