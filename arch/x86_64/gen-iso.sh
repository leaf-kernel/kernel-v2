#!/bin/bash
mkdir -p modules
cd ../../
tar -cvf arch/x86_64/modules/ramfs ramfs/* > /dev/null 2>&1
cd arch/x86_64
mkdir -p ../../release
mkdir -p iso
mkdir -p iso/boot
cp -v root/* iso > /dev/null 2>&1
cp -v ../../build-x86_64/kernel/Leaf-x86_64-raw.bin iso/boot/leaf > /dev/null 2>&1
make -C limine > /dev/null 2>&1
cp -v limine/limine-bios.sys limine/limine-bios-cd.bin limine/limine-uefi-cd.bin iso > /dev/null 2>&1
mkdir -p iso/EFI/BOOT
cp -v limine/BOOTX64.EFI iso/EFI/BOOT/ > /dev/null 2>&1
cp -v limine/BOOTIA32.EFI iso/EFI/BOOT/ > /dev/null 2>&1
cp -v modules/* iso > /dev/null 2>&1
xorriso -as mkisofs -b limine-bios-cd.bin -no-emul-boot -boot-load-size 4 -boot-info-table --efi-boot limine-uefi-cd.bin -efi-boot-part --efi-boot-image --protective-msdos-label iso -o ../../release/Leaf-x86_64-$(date +%B-%Y).iso > /dev/null 2>&1
limine/limine bios-install ../../release/Leaf-x86_64-$(date +%B-%Y).iso > /dev/null 2>&1
rm -rf iso
