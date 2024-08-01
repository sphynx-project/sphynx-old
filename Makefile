MAKEFLAGS += --no-print-directory

# Dependencies
DEPS := ovmf=https://retrage.github.io/edk2-nightly/bin/RELEASEX64_OVMF.fd sphynxboot=https://github.com/sphynxos/sphynxboot/archive/refs/heads/main.zip
DEPS_DIR := deps
TMP_DIR := /tmp

BIN_DIR := bin
BOOT_DIR := $(DEPS_DIR)/sphynxboot
KERNEL_DIR := kernel

TARGET_TEST := $(KERNEL_DIR)/kernel.elf
TARGET_BOOT := $(BIN_DIR)/BOOTX64.elf

OVMF := $(DEPS_DIR)/ovmf/RELEASEX64_OVMF.fd
ROOT_DIR := $(shell pwd)

.PHONY: all
all: setup deps kernel $(TARGET_BOOT)

define DOWNLOAD_template
$(DEPS_DIR)/$(1):
	@echo " + mkdir -p $(TMP_DIR)/$(1)"
	@mkdir -p $(TMP_DIR)/$(1)
	@echo " + mkdir -p $(DEPS_DIR)/$(1)"
	@mkdir -p $(DEPS_DIR)/$(1)
ifeq ($(2),.zip)
	@echo " + curl -Ls $(3) -o $(TMP_DIR)/$(1)/$(1).zip"
	@curl -Ls $(3) -o $(TMP_DIR)/$(1)/$(1).zip
	@echo " + unzip -q $(TMP_DIR)/$(1)/$(1).zip -d $(TMP_DIR)/$(1)"
	@unzip -o -q $(TMP_DIR)/$(1)/$(1).zip -d $(TMP_DIR)/$(1)
	@echo " + mv $(TMP_DIR)/$(1)/*/* $(DEPS_DIR)/$(1)"
	@mv $(TMP_DIR)/$(1)/*/* $(DEPS_DIR)/$(1)
	@echo " + rm -rf $(TMP_DIR)/$(1)"
	@rm -rf $(TMP_DIR)/$(1)
else
	@echo " + curl -Ls $(3) -o $(DEPS_DIR)/$(1)/$(notdir $(3))"
	@curl -Ls $(3) -o $(DEPS_DIR)/$(1)/$(notdir $(3))
endif

.PHONY: $(1)
$(1): $(DEPS_DIR)/$(1)
endef

$(foreach DEP,$(DEPS),$(eval $(call DOWNLOAD_template,$(word 1,$(subst =, ,$(DEP))),$(suffix $(word 2,$(subst =, ,$(DEP)))),$(word 2,$(subst =, ,$(DEP))))))

.PHONY: deps
deps: $(foreach DEP,$(DEPS),$(word 1,$(subst =, ,$(DEP))))

$(TARGET_BOOT): $(BOOT_DIR)/sphynxboot.efi | $(BIN_DIR)
	@cp $(BOOT_DIR)/bin/BOOTX64.efi $(TARGET_BOOT)

$(TARGET_TEST): kernel

$(BOOT_DIR)/sphynxboot.efi: $(BOOT_DIR)
	@$(MAKE) -C $(BOOT_DIR)

$(BIN_DIR):
	@echo " + mkdir -p $(BIN_DIR)"
	@mkdir -p $(BIN_DIR)

.PHONY: setup
setup: $(BIN_DIR)

.PHONY: kernel
kernel:
	@$(MAKE) -C $(KERNEL_DIR)

.PHONY: bootloader
bootloader:
	@$(MAKE) -C $(BOOT_DIR)

.PHONY: test
test: all
	@if [ "$(shell uname -s)" = "Darwin" ]; then \
	    dd if=/dev/zero of=boot.img bs=1m count=64; \
	    mkfs.fat -F 32 -n EFI_SYSTEM boot.img; \
	    mmd -i boot.img ::/EFI ::/EFI/BOOT; \
	    mcopy -i boot.img $(TARGET_BOOT) ::/EFI/BOOT/BOOTX64.efi; \
		mcopy -i $(TARGET_TEST) ::kernel.elf; \
	else \
	    dd if=/dev/zero of=boot.img bs=1M count=64; \
	    mkfs.fat -F 32 -n EFI_SYSTEM boot.img; \
	    mkdir -p mnt; \
	    sudo mount -o loop boot.img mnt; \
	    sudo mkdir -p mnt/EFI/BOOT; \
	    sudo cp $(TARGET_BOOT) mnt/EFI/BOOT/BOOTX64.efi; \
		sudo cp $(TARGET_TEST) mnt/kernel.elf; \
	    sudo umount mnt; \
	    rm -rf mnt; \
	fi
	@qemu-system-x86_64 -m 2G -drive if=pflash,format=raw,readonly=on,file=$(OVMF) -drive if=ide,format=raw,file=boot.img -debugcon stdio

.PHONY: clean
clean:
	@$(MAKE) -C $(BOOT_DIR) clean
	@$(MAKE) -C $(KERNEL_DIR) clean
	@echo " + rm -rf $(BIN_DIR) mnt boot.img $(DEPS_DIR)"
	@rm -rf $(BIN_DIR) mnt boot.img $(DEPS_DIR)
