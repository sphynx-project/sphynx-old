MAKEFLAGS += --no-print-directory

DEPS := ovmf=https://retrage.github.io/edk2-nightly/bin/RELEASEX64_OVMF.fd sphynxboot=https://github.com/sphynxos/sphynxboot/archive/refs/heads/v1.x.x-alpha.zip flanterm=https://github.com/mintsuki/flanterm/archive/refs/heads/trunk.zip
DEPS_DIR := deps
TMP_DIR := $(shell mktemp -d)

BIN_DIR := bin
BOOT_DIR := $(DEPS_DIR)/sphynxboot
KERNEL_DIR := kernel

TARGET_TEST := $(KERNEL_DIR)/kernel.elf
TARGET_BOOT := $(BIN_DIR)/BOOTX64.efi

OVMF := $(DEPS_DIR)/ovmf/RELEASEX64_OVMF.fd
ROOT_DIR := $(shell pwd)

BOOT_CONF := $(KERNEL_DIR)/boot.conf

all: setup deps-download-sphynxboot kernel $(TARGET_BOOT)

.PHONY: cleanup_tmp
cleanup_tmp:
	@echo " + rm -rf $(TMP_DIR)"
	@rm -rf $(TMP_DIR)

define DOWNLOAD_template
deps-download-$(1):
	@if [ ! -d $(DEPS_DIR)/$(1) ]; then \
		echo " + mkdir -p $(DEPS_DIR)/$(1)"; \
		mkdir -p $(DEPS_DIR)/$(1); \
		if [ "$(2)" = ".zip" ]; then \
			echo " + curl -Ls $(3) -o $(TMP_DIR)/$(1).zip"; \
			curl -Ls $(3) -o $(TMP_DIR)/$(1).zip && \
			echo " + unzip -o -q $(TMP_DIR)/$(1).zip -d $(TMP_DIR)"; \
			unzip -o -q $(TMP_DIR)/$(1).zip -d $(TMP_DIR) && \
			echo " + mv $(TMP_DIR)/*/* $(DEPS_DIR)/$(1)"; \
			mv $(TMP_DIR)/*/* $(DEPS_DIR)/$(1) && \
			$(MAKE) cleanup_tmp; \
		else \
			echo " + curl -Ls $(3) -o $(DEPS_DIR)/$(1)/$(notdir $(3))"; \
			curl -Ls $(3) -o $(DEPS_DIR)/$(1)/$(notdir $(3)) || { echo "Error downloading $(3)"; exit 1; }; \
		fi; \
	else \
		echo " + $(DEPS_DIR)/$(1) already exists, skipping dep"; \
	fi
endef

$(foreach DEP,$(DEPS),$(eval $(call DOWNLOAD_template,$(word 1,$(subst =, ,$(DEP))),$(suffix $(word 2,$(subst =, ,$(DEP)))),$(word 2,$(subst =, ,$(DEP))))))

.PHONY: deps
deps: $(foreach DEP,$(DEPS),deps-download-$(word 1,$(subst =, ,$(DEP))))

$(TARGET_BOOT): $(BOOT_DIR)/bin/BOOTX64.efi | $(BIN_DIR)
	@echo " + cp $(BOOT_DIR)/bin/BOOTX64.efi $(TARGET_BOOT)"
	@cp $(BOOT_DIR)/bin/BOOTX64.efi $(TARGET_BOOT)

$(TARGET_TEST): kernel

$(OVMF): deps-download-ovmf

$(BOOT_DIR)/bin/BOOTX64.efi: bootloader

$(BIN_DIR):
	@echo " + mkdir -p $(BIN_DIR)"
	@mkdir -p $(BIN_DIR)

.PHONY: setup
setup: $(BIN_DIR)

.PHONY: kernel
kernel: deps-download-flanterm
	@echo " + $(MAKE) -C $(KERNEL_DIR)"
	@$(MAKE) -C $(KERNEL_DIR)

.PHONY: bootloader
bootloader: deps-download-sphynxboot
	@echo " + $(MAKE) -C $(BOOT_DIR)"
	@$(MAKE) -C $(BOOT_DIR)

.PHONY: gen-img
gen-img: all
	@if [ "$(shell uname -s)" = "Darwin" ]; then \
		echo " + dd if=/dev/zero of=boot.img bs=1m count=64"; \
	    dd if=/dev/zero of=boot.img bs=1m count=64; \
		echo " + mkfs.fat -F 32 -n EFI_SYSTEM boot.img"; \
	    mkfs.fat -F 32 -n EFI_SYSTEM boot.img; \
		echo " + mmd -i boot.img ::/EFI ::/EFI/BOOT"; \
	    mmd -i boot.img ::/EFI ::/EFI/BOOT; \
		echo " + mcopy -i boot.img $(TARGET_BOOT) ::/EFI/BOOT/BOOTX64.efi"; \
	    mcopy -i boot.img $(TARGET_BOOT) ::/EFI/BOOT/BOOTX64.efi; \
		echo " + mcopy -i $(TARGET_TEST) ::/sphynx/kernel.elf"; \
		mcopy -i $(TARGET_TEST) ::/sphynx/kernel.elf; \
		echo " + mcopy -i test.txt ::/sphynx/ramfs"; \
		mcopy -i test.txt ::/sphynx/ramfs; \
		echo " + mcopy -i $(BOOT_CONF) ::boot.conf"; \
		mcopy -i $(BOOT_CONF) ::boot.conf; \
	else \
	    echo " + dd if=/dev/zero of=boot.img bs=1M count=64"; \
	    dd if=/dev/zero of=boot.img bs=1M count=64; \
	    echo " + mkfs.fat -F 32 -n EFI_SYSTEM boot.img"; \
	    mkfs.fat -F 32 -n EFI_SYSTEM boot.img; \
	    echo " + sudo mount -o loop boot.img mnt"; \
	    mkdir -p mnt; \
	    sudo mount -o loop boot.img mnt; \
	    echo " + sudo mkdir -p mnt/EFI/BOOT"; \
	    sudo mkdir -p mnt/EFI/BOOT; \
	    echo " + sudo mkdir -p mnt/boot/kernel"; \
	    sudo mkdir -p mnt/boot/kernel; \
	    echo " + sudo mkdir -p mnt/sphynx"; \
	    sudo mkdir -p mnt/sphynx; \
	    echo " + sudo cp $(TARGET_BOOT) mnt/EFI/BOOT/BOOTX64.efi"; \
	    sudo cp $(TARGET_BOOT) mnt/EFI/BOOT/BOOTX64.efi; \
		echo " + sudo cp $(TARGET_TEST) mnt/sphynx/kernel.elf"; \
		sudo cp $(TARGET_TEST) mnt/sphynx/kernel.elf; \
		echo " + sudo cp test.txt mnt/sphynx/ramfs"; \
		sudo cp test.txt mnt/sphynx/ramfs; \
		echo " + sudo cp $(BOOT_CONF) mnt/boot.conf"; \
		sudo cp $(BOOT_CONF) mnt/boot.conf; \
	    echo " + sudo umount mnt"; \
	    sudo umount mnt; \
	    echo " + rm -rf mnt"; \
	    rm -rf mnt; \
	fi

.PHONY: run
run: gen-img $(OVMF)
	@echo " + qemu-system-x86_64 -m 2G -drive if=pflash,format=raw,readonly=on,file=$(OVMF) -drive if=ide,format=raw,file=boot.img -debugcon stdio"
	@qemu-system-x86_64 -m 2G -drive if=pflash,format=raw,readonly=on,file=$(OVMF) -drive if=ide,format=raw,file=boot.img -debugcon stdio

.PHONY: clean
clean:
	@echo " + $(MAKE) -C $(BOOT_DIR) clean"
	@$(MAKE) -C $(BOOT_DIR) clean
	@echo " + $(MAKE) -C $(KERNEL_DIR) clean"
	@$(MAKE) -C $(KERNEL_DIR) clean
	@echo " + Removing $(BIN_DIR) mnt boot.img $(DEPS_DIR)"
	@rm -rf $(BIN_DIR) mnt boot.img $(DEPS_DIR)