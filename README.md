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

Coming soon, see important notice.

## 🏗️ Building Demos

> [!NOTE]
> ON the device that is to run this code, a computer that is the same OS and
> architecture, or inside the provided docker container.

To build demos, start at the root of the repo and execute the following command:

```bash
conan build demos -pr macosx
```

## 💾 Executing Programs

To execute the program simply run the following:

```bash
./build/.../app
```

## Contributing

See [`CONTRIBUTING.md`](CONTRIBUTING.md) for details.

## License

Apache 2.0; see [`LICENSE`](LICENSE) for details.
