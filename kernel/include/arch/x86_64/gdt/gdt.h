#ifndef __GDT_H__
#define __GDT_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
	uint16_t size;
	uint32_t offset;
} __attribute__((packed)) gdt_pointer;

typedef struct {
	uint32_t reserved0;
	uint64_t rsp0;
	uint64_t rsp1;
	uint64_t rsp2;
	uint64_t reserved1;
	uint64_t ist1;
	uint64_t ist2;
	uint64_t ist3;
	uint64_t ist4;
	uint64_t ist5;
	uint64_t ist6;
	uint64_t ist7;
	uint64_t reserved2;
	uint16_t reserved3;
	uint16_t iomap_base;
} __attribute__((packed)) tss_entry_t;

typedef struct {
	uint16_t limit0;
	uint16_t base0;
	uint8_t base1;
	uint8_t access;
	uint8_t limit1_flags;
	uint8_t base2;
} __attribute__((packed)) gdt_entry;

void init_gdt(uint64_t *rsp0);
void set_kernel_stack(uint64_t *stack);

#endif	// __GDT_H__