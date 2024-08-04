# Sphynx - Boot Process

*Note: This document is a work in progress.*

## Table of Contents

1. [The Bootloader](#1-the-bootloader)
   - [1.1 Introduction](#11-introduction)
   - [1.2 Loading the Kernel](#12-loading-the-kernel)
   - [1.3 Configuration](#13-configuration)
2. [Kernel Initialization](#2-kernel-initialization)
   - [2.1 Setup](#21-setup)

## 1. The Bootloader

### 1.1 Introduction

Sphynx uses its proprietary bootloader, `sphynxboot` (available [here](https://github.com/sphynxos/sphynxboot)). The bootloader's main role is to load the kernel and pass essential information to it.

### 1.2 Loading the Kernel

The bootloader does not automatically detect the kernel's location on the drive. Therefore, a configuration file must be created for the bootloader (see [1.3 Configuration](#13-configuration)).

The bootloader is designed for the x86_64 architecture and supports only ELF-formatted kernels. It loads the ELF file, retrieves the entry point address of the kernel, and collects various system parameters, including the framebuffer, memory map, and RSDT. This information is then organized and passed to the kernel.

### 1.3 Configuration

The bootloader requires a configuration file named `/boot.conf`. This file should be simple, with each setting specified in the format `key=value`. Values should not contain spaces, and the file must end with a newline character (`\n`).

**Example Configuration:**

`/boot.conf`:
```ini
kernel=/sphynx/kernel.elf
```

**Available Keys:**

- **kernel:** *Specifies the path to the kernel file.*
   - ***<path>***: *Path to the kernel file.*
- **ramfs:** *Specifies the path to the RAM filesystem.*
   - ***<path>***: *Path to the RAM filesystem.*
- **instant_boot:** *Indicates whether to boot the kernel immediately or wait for a user event.*
   - ***yes, true, on***: *Enables immediate boot (any other value will be treated as no and ignored).*

## 2. Kernel Initialization

### 2.1 Setup

*To be completed*