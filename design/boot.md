Certainly! Here's how you can format the Table of Contents using a list:

---

# sphynx - Boot Process

*Note: This document is not finished yet*

## Table of Contents

1. [The Bootloader](#10-the-bootloader)
   - [1.1 Introduction](#11-introduction)
   - [1.2 Loading the Kernel](#12-loading-the-kernel)
   - [1.3 Configuration](#13-configuration)
2. [Kernel Initialization](#20-kernel-initialization)
   - [2.1 Setup](#21-setup)

## 1.0 The Bootloader

### 1.1 Introduction

Sphynx employs its own bootloader, known as `sphynxboot` (available [here](https://github.com/sphynxos/sphynxboot)). The bootloader's primary function is to load the kernel and transfer necessary information to it.

### 1.2 Loading the Kernel

By default, the bootloader does not automatically know the location of the kernel on the drive. Therefore, it is essential to create a configuration file for the bootloader (see [1.3 Configuration](#13-configuration)).

The bootloader expects the kernel to be compiled for the x86_64 architecture, as it supports only x86_64. The kernel must be in ELF format. The bootloader loads the ELF file, retrieves a pointer to the kernel entry point, and collects various system information, such as the framebuffer, memory map, and RSDT. This information is then organized into a structured format and passed to the kernel.

### 1.3 Configuration

The bootloader requires a configuration file located at `/boot.conf`. This file is straightforward, with each setting defined in the format `key=value`. Note that values cannot contain spaces, and the configuration file must end with a newline character (`\n`). 

**Example Configuration:**

`/boot.conf`:
```ini
kernel=/sphynx/kernel.elf
```

**Available Keys:**

- **kernel:** *Specifies the path to the kernel file.*

## 2.0 Kernel Initialization

### 2.1 Setup

*todo*