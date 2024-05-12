#include <arch/x86_64/apic/apic.h>
#include <arch/x86_64/cpu/cpu.h>
#include <arch/x86_64/idt/idt.h>
#include <libc/stdio/printf.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/boot.h>
#include <sys/error.h>
#include <tools/logger.h>
#include <tools/panic.h>

#define IDT_ENTRIES 256

idt_entry_t idt[IDT_ENTRIES];
idt_pointer_t idt_p;
void *irq_handlers[16];
extern void *last_rbp;
int g_irq_count;

extern uint64_t isr_tbl[];

static const char *exception_strings[32] = {"Division By Zero",
											"Debug",
											"Nonmaskable Interrupt",
											"Breakpoint",
											"Overflow",
											"Bound Range Exceeded",
											"Invalid Opcode",
											"Device Not Available",
											"Double Fault",
											"Coprocessor Segment Overrun",
											"Invalid TSS",
											"Segment Not Present",
											"Stack Segment Fault",
											"General Protection Fault",
											"Page Fault",
											"Reserved",
											"x87 FPU Error"
											"Alignment Check",
											"Machine Check",
											"Simd Exception",
											"Virtualization Exception",
											"Control Protection Exception",
											"Reserved",
											"Reserved",
											"Reserved",
											"Reserved",
											"Reserved",
											"Reserved",
											"Hypervisor Injection Exception",
											"VMM Communication Exception",
											"Security Exception",
											"Reserved"};
extern void load_idt(uint64_t);

void set_idt_gate(int num, uint64_t base, uint16_t sel, uint8_t flags) {
	idt[num].offset_low = (base & 0xFFFF);
	idt[num].offset_middle = (base >> 16) & 0xFFFF;
	idt[num].offset_high = (base >> 32) & 0xFFFFFFFF;
	idt[num].selector = sel;
	idt[num].ist = 0;
	idt[num].flags = flags;
	idt[num].zero = 0;
}

void init_idt() {
	idt_p.limit = sizeof(idt_entry_t) * IDT_ENTRIES - 1;
	idt_p.base = (uint64_t)&idt;

	asm("sti");

	for(int i = 0; i < IDT_ENTRIES; ++i) {
		set_idt_gate(i, isr_tbl[i], 0x28, 0x8E);
	}

	for(size_t i = 0; i < 16; i++) {
		irq_handlers[i] = NULL;
	}

	g_irq_count = 0;

	load_idt((uint64_t)&idt_p);
	asm("cli");
}

void excp_handler(int_frame_t frame) {
	if(frame.vector == 0xff)
		return;

	if(frame.vector < 0x20) {
		panic(exception_strings[frame.vector], &frame);
		hcf();
	} else if(frame.vector >= 32 && frame.vector <= 47) {
		int irq = frame.vector - 0x20;
		typedef void (*handler_func_t)(int_frame_t *);

		handler_func_t handler = irq_handlers[frame.vector];

		if(handler != NULL) {
			handler(&frame);
		}
	} else if(frame.vector == 0x80) {
		// TODO: System calls
	}
}

void irq_register(uint8_t irq, void *handler) {
	irq_handlers[irq] = handler;
	uint32_t lapic_id = smp_request.response->bsp_lapic_id;
	ioapic_redirect_irq(lapic_id, irq + 32, irq, false);
}

void irq_deregister(uint8_t irq) { irq_handlers[irq] = NULL; }