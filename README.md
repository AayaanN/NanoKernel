# NanoKernel

NanoKernel is a lightweight cooperative multithreading kernel designed for resource-constrained embedded systems. It features a round-robin scheduler, inter-task synchronization, and low-latency task switching.

## Features

- **Cooperative Round-Robin Scheduling** – Efficient task switching with minimal overhead.
- **Inter-Task Synchronization** – Supports basic synchronization primitives.
- **Low-Latency Execution** – Optimized for real-time performance on embedded platforms.
- **Bare-Metal Implementation** – Written in C with custom assembly for context switching.

## Getting Started

### Prerequisites

- ARM Cortex-M-based microcontroller (tested on STM32)
- GCC toolchain for ARM
- Make

### Installation & Usage

1. Clone the repository:
    ```bash
    git clone https://github.com/yourusername/NanoKernel.git
    cd NanoKernel
    ```

2. Build the project:
    ```bash
    make
    ```

3. Flash to your target board using OpenOCD or ST-Link:
    ```bash
    make flash
    ```

## Code Overview

- **k_task.c** – Core scheduler logic
- **svc_handler.sS** – Assembly routines for task switching
- **main.c** – Some example task implementations

