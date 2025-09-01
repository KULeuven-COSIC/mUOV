# mUOV 
This repository contains the artifact for the paper "mUOV: Masking the Unbalanced Oil and Vinegar Digital Signature Scheme at First- and Higher-Order", accepted at ACM CCS 2025 [[ePrint]](https://eprint.iacr.org/2024/1875). 

## :file_folder: Contents
- [maskverif](maskverif): scripts for formal verification in maskVerif (see [README](maskverif/README.md))
- [src](src): masked UOV source code
- [common](common): STM32F4 board files, utils, ...

## :round_pushpin: Selecting target
Our code can be compiled for and run on:
- Arm Cortex-M4: reduced parameter set, side-channel leakage removed (assembly routines). 
- CPU/laptop: mainly for benchmarking (full parameter set), no assembly.

> [!IMPORTANT]  
> The code in this repository targets the [STM32F4 Discovery board](https://www.st.com/en/evaluation-tools/stm32f4discovery.html), containing an Arm Cortex-M4 microcontroller.

## :clipboard: Requirements

### [pqov](https://github.com/pqov/pqov) and [libopencm3](https://github.com/libopencm3/libopencm3)
After cloning this repository, initialize the reference (pqov) code and the libopencm3 library (for execution on Arm Cortex-M4).
```shell
git submodule update --init --recursive
```
> [!NOTE]  
> If targeting the Arm Cortex-M4 board, also install the following requirements. Otherwise, go to "Running and Benchmarking" section.

### [Arm toolchain](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)
Install the `arm-none-eabi-gcc` (or `gcc-arm-none-eabi`) toolchain/package.

### [stlink](https://github.com/texane/stlink)
Depending on your operating system, stlink may be available in your package manager -- if not, please refer to the stlink Github page for instructions on how to compile it from source (in that case, be careful to use libusb-1.0.0-dev, not libusb-0.1).

### [pyserial](https://github.com/pyserial/pyserial)
(Requires Python >= 3.8). Your package repository might offer `python3-serial` (Debian, Ubuntu) or `python-pyserial` (Arch) or `python3-pyserial` (Fedora, openSUSE) or `pyserial` (Slack, CentOS, Gentoo) or `py3-pyserial` (Alpine) directly. If you do not have pip3 installed yet, you can typically find it as `python3-pip` (Debian, Ubuntu) or `python-pip` (Arch) using your package manager.

Make sure the board is connected to the host machine using the mini-USB port (USB to TLL adapter). Connect the `TX`/`TXD` pin of the USB connector to the `PA3` pin on the board and connect `RX`/`RXD` pin to `PA2`. Also connect the `GND` pins.

## :hammer_and_wrench: Running and Benchmarking
Select a pre-defined parameter set (`UOV-I`, `UOV-III`, `UOV-V`) or define a custom one in [UOV_params.h](src/UOV_params.h).

Choose the target platform in [settings.h](src/settings.h):
- `M4_TARGET`
- `CPU_TARGET`

Enable `RUN_BENCHMARK` for detailed benchmark numbers in [settings.h](src/settings.h).

For running on M4, first compile libopencm:
Compile by executing:
```shell
make all
```

For reading the output over USART (`/dev/ttyUSB0`), open the listener:
```shell
python3 host.py
```

Compile mUOV by moving to src folder and making:
```shell
cd src/
```
```shell
make all -f MakefileM4.mk
```
or
```shell
make all -f MakefileCPU.mk
```

Flash `mUOV_m4.bin` to the target board by executing:
```shell
./deploy.sh 
```
or execute binary (and read results) on CPU:
```shell
./muov-benchmark >output_performance.txt
```

After done:
```shell
make clean -f MakefileM4.mk
```
or
```shell
make clean -f MakefileCPU.mk
```
and in main/root folder:
```shell
make clean
```
