SRC_DIR=../src
BUILD_DIR=../build
LINK_DIR=../build/linkers
BIN_DIR=../bin

CC=clang
CWARN=-W -Werror -Wall -Wno-incompatible-library-redeclaration -Wno-undefined-inline -Wno-unused-parameter -Wno-implicit-function-declaration -Wno-empty-body -Wno-address-of-packed-member
CFLAGS=-std=c17 -m32 -march=i386 -masm=intel -ffreestanding -fno-builtin -nostdinc -Os $(CWARN)

ASM_FILES=bootloader _FAT16 load_kernel #second third 
K_FILES!=find $(SRC_DIR)/kernel_loader/ -name *.c -exec basename -s ".c" {} \;
K_INCLUDES!=find $(SRC_DIR)/kernel_loader/includes/ -name *.c -exec basename -s ".c" {} \;
IMAGES!=find $(SRC_DIR)/binaries/ -name *.png -exec basename -s ".png" {} \;
LFLAGS = -m elf_i386 --oformat binary -M

.SILENT:

build: reset_filetable bootcode font kernel kernel_loader $(IMAGES)
	python3 $(BUILD_DIR)/add_file.py -package
	dd if=/dev/zero of=$(BIN_DIR)/image.bin bs=512 count=2880
	dd if=$(BIN_DIR)/temp.bin of=$(BIN_DIR)/image.bin conv=notrunc
	# find $(BIN_DIR)/ -type f -not -name "image.bin" -delete
	rm -f $(BUILD_DIR)/*.o
	rm -f $(BUILD_DIR)/clust

font: $(BIN_DIR)/font.bin
	python3 $(BUILD_DIR)/add_file.py font.bin

$(IMAGES):
	python3 $(BUILD_DIR)/convert_image.py $@.png
	python3 $(BUILD_DIR)/add_file.py $@.bin

$(ASM_FILES):
	nasm -f elf -O0 -o $(BUILD_DIR)/$@.o $(SRC_DIR)/bootcode/$@.asm

$(BIN_DIR)/font.bin:
	nasm -f bin -O0 -o $(BIN_DIR)/font.bin $(SRC_DIR)/binaries/font.asm
	# python3 $(BUILD_DIR)/add_file.py font.bin

bootcode: $(ASM_FILES)
	ld $(LFLAGS) -T$(LINK_DIR)/$@.ld $(ASM_FILES:%=$(BUILD_DIR)/%.o) -o $(BIN_DIR)/$@.bin
	python3 $(BUILD_DIR)/add_file.py -init

$(K_INCLUDES):
	$(CC) -c $(CFLAGS) $(K_INCLUDES:%=$(SRC_DIR)/kernel_loader/includes/%.c) -I$(SRC_DIR)/C/includes

kernel_loader: $(K_INCLUDES)
	$(CC) -c $(CFLAGS) -o $@.o $(SRC_DIR)/kernel_loader/$@.c -I$(SRC_DIR)/kernel_loader/includes
	ld $(LFLAGS) -T$(LINK_DIR)/$@.ld $@.o $(K_INCLUDES:%=%.o) -o $(BIN_DIR)/$@.bin
	# ld -m elf_i386 -T$(LINK_DIR)/$@.ld $@.o $(K_INCLUDES:%=%.o) -o $(BIN_DIR)/image.elf
	python3 $(BUILD_DIR)/add_file.py $@.bin

kernel: $(K_INCLUDES)
	$(CC) -c $(CFLAGS) -o $@.o $(SRC_DIR)/kernel/$@.c -I$(SRC_DIR)/kernel_loader/includes
	ld $(LFLAGS) -T$(LINK_DIR)/$@.ld $@.o $(K_INCLUDES:%=%.o) -o $(BIN_DIR)/$@.bin
	ld -m elf_i386 -T$(LINK_DIR)/$@.ld $@.o $(K_INCLUDES:%=%.o) -o $(BIN_DIR)/image.elf
	python3 $(BUILD_DIR)/add_file.py $@.bin

.PHONY: clean run reset_filetable

reset_filetable:
	rm -f $(BIN_DIR)/filetable.bin
	rm -f $(BIN_DIR)/clustertable.bin
	rm -f $(BUILD_DIR)/clust

run: build
	export DISPLAY=:0;\
	qemu-system-i386 \
	-display gtk \
	-m 512M \
	-drive format=raw,file=$(BIN_DIR)/image.bin,index=0,media=disk \
	-rtc base=localtime,clock=host,driftfix=slew

crun:
	export DISPLAY=:0;\
	qemu-system-i386 \
	-display gtk \
	-m 1G \
	-drive format=raw,file=$(BIN_DIR)/image.bin,index=0,media=disk \
	-rtc base=localtime,clock=host,driftfix=slew

debug: build
	export DISPLAY=:0;\
	qemu-system-i386 \
	-s -S \
	-display gtk \
	-m 1G \
	-drive format=raw,file=$(BIN_DIR)/image.bin,index=0,media=disk \
	-rtc base=localtime,clock=host,driftfix=slew

cdebug:
	export DISPLAY=:0;\
	qemu-system-i386 \
	-s -S \
	-display gtk \
	-m 1G \
	-drive format=raw,file=$(BIN_DIR)/image.bin,index=0,media=disk \
	-rtc base=localtime,clock=host,driftfix=slew

clean:
	rm -f $(BUILD_DIR)/*.o
	rm -f $(BIN_DIR)/*.bin