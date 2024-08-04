CC = clang++
C = clang
ASM = nasm
LD = ld.lld

CFLAGS = -O0 -Wall -Werror -g -ffreestanding -Wno-instantiation-after-specialization -Wno-int-conversion -Wno-infinite-recursion \
         -fno-stack-protector -fno-stack-check -fno-omit-frame-pointer -fno-lto -m64 \
         -march=x86-64 -mabi=sysv -mno-80387 -mno-mmx -mno-red-zone -fno-pie --target=x86_64-pc-none-elf \
         -fno-builtin -mno-sse -mno-sse2 -mno-avx
CFLAGS += -I../deps/sphynxboot -I../deps -Iinclude -I.
CXXFLAGS = $(CFLAGS)

LDFLAGS = -nostdlib -static -m elf_x86_64 -z max-page-size=0x1000 -T linker.ld

TARGET = kernel.elf

SOURCES_CPP = $(shell find src -name '*.cpp') $(shell find ../deps/flanterm -name '*.cpp')
SOURCES_C = $(shell find src -name '*.c') $(shell find ../deps/flanterm -name '*.c')
SOURCES_ASM = $(shell find src -name '*.asm')
SOURCES = $(SOURCES_CPP) $(SOURCES_C) $(SOURCES_ASM)

OBJECTS_CPP = $(SOURCES_CPP:.cpp=.o)
OBJECTS_C = $(SOURCES_C:.c=.o)
OBJECTS_ASM = $(SOURCES_ASM:.asm=.o)
OBJECTS = $(OBJECTS_CPP) $(OBJECTS_C) $(OBJECTS_ASM)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@echo " + $(LD) $(LDFLAGS) $(OBJECTS) -o $@"
	@$(LD) $(LDFLAGS) $(OBJECTS) -o $@

%.o: %.cpp $(HEADERS)
	@echo " + $(CC) $(CXXFLAGS) -c $< -o $@"
	@$(CC) $(CXXFLAGS) -c $< -o $@

%.o: %.c $(HEADERS)
	@echo " + $(C) $(CFLAGS) -c $< -o $@"
	@$(C) $(CFLAGS) -c $< -o $@

%.o: %.asm
	@echo " + $(ASM) -f elf64 $< -o $@"
	@$(ASM) -f elf64 $< -o $@

clean:
	@echo " + rm -f $(OBJECTS) $(TARGET)"
	@rm -f $(OBJECTS) $(TARGET)

.PHONY: clean all
