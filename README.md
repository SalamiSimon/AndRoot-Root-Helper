# AndRoot-Root-Helper

Automated ADB command to help root your phone

---

## Introduction

AndRoot-Root-Helper is a command-line tool designed to simplify the process of rooting your Android device using ADB (Android Debug Bridge) commands. While it may not be a professionally developed application and is still a work in progress, it aims to provide a convenient way for users to execute the necessary ADB commands to unlock the bootloader, manipulate image files, and flash/boot patched files on their Android devices.

---

## Prerequisites

Before using AndRoot-Root-Helper, ensure that you have met the following prerequisites:

1. **Unlocked Bootloader**: You must have already unlocked your device's bootloader. Note that unlocking the bootloader may void your device's warranty and erase all data, so make sure to back up your data before proceeding.

2. **Backed Up Data**: As mentioned above, unlocking the bootloader can erase your device's data. Ensure that you have backed up all important data from your device before using this tool.

3. **USB Debugging Enabled**: USB debugging must be enabled on your Android device. You can enable this option in the Developer Options of your device's settings.

4. **Working USB Cable**: A reliable USB cable is essential. Sometimes, ADB can detect your device while Fastboot cannot due to cable issues. Always verify that both ADB and Fastboot can detect your device before performing any actions like booting a new image file.

---

## Usage

AndRoot-Root-Helper provides a set of commands to perform various tasks related to rooting your device. Here are some of the common commands:

- `adb-bootloader`: Reboots your device into bootloader/fastboot mode.
- `adb-pull <source> <destination>`: Pulls a file or directory from your device to your computer.
- `adb-push <source> <destination>`: Pushes a file or directory from your computer to your device.
- `adb-flash <file>`: Flashes a specified file to your device.
- `adb-boot <file>`: Boots a patched file on your device.

Please note that the functionality of AndRoot-Root-Helper is a work in progress, and not all features may be fully functional at this time.

---

## Installation

To use AndRoot-Root-Helper, you can simply clone this repository to your local machine:

```bash
git clone https://github.com/your-username/AndRoot-Root-Helper.git
```

Ensure that you have ADB and Fastboot set up on your computer, as this tool relies on these utilities.

---

## Contributing

This project is open to contributions. If you'd like to contribute to the development of AndRoot-Root-Helper, please fork this repository, make your changes, and submit a pull request. Be sure to follow best practices for coding and documentation.

---

## Disclaimer

Use AndRoot-Root-Helper at your own risk. Unlocking the bootloader and making modifications to your Android device can lead to data loss and may void warranties. The developers and maintainers of AndRoot-Root-Helper are not responsible for any damage or issues that may arise from using this tool. Always proceed with caution and ensure you have backups before making any changes to your device.

---

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details.

---

## Contact

If you have any questions, issues, or suggestions regarding AndRoot-Root-Helper, feel free to reach out to us via [email](mailto:your-email@example.com) or by creating an issue in the [GitHub repository](https://github.com/your-username/AndRoot-Root-Helper/issues).

Thank you for using AndRoot-Root-Helper!

---
*Please note that this project is a work in progress, and its functionality may be limited or unstable. Use with caution and at your own discretion.*
