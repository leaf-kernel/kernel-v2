
#include <arch/x86_64/acpi/fadt.h>
#include <arch/x86_64/acpi/rsdt.h>
#include <arch/x86_64/cpu/cpuid.h>
#include <arch/x86_64/drivers/serial.h>
#include <arch/x86_64/mm/mm.h>
#include <sys/boot.h>
#include <tools/panic.h>

bool _xsdt_is_available;
bool _acpi_mode;

void init_acpi() {
	rsdp_t *rsdp = (rsdp_t *)rsdp_request.response->address;
	if(rsdp->revision == 0) {
		_xsdt_is_available = false;
		init_rsdt();
	} else if(rsdp->revision >= 2) {
		_xsdt_is_available = true;
		init_rsdt();
	}
	outb(fadt_table->SMI_CommandPort, fadt_table->AcpiEnable);
	while(inw(fadt_table->PM1aControlBlock) & 1 == 0)
		;
	_acpi_mode = true;
}

void *_find_sdt(char *signature) {
	int entry_divisor = (_use_xsdt() ? 8 : 4);
	int header_length =
		_use_xsdt() ? g_xsdt->header.length : g_rsdt->header.length;
	int entries = (header_length - sizeof(sdt_t)) / entry_divisor;
	for(int i = 0; i < entries; i++) {
		sdt_t *header = NULL;
		if(_use_xsdt()) {
			header = (sdt_t *)(uintptr_t)PHYS_TO_VIRT(g_xsdt->sdt[i]);
		} else {
			header = (sdt_t *)(uintptr_t)PHYS_TO_VIRT(g_rsdt->sdt[i]);
		}
		if(!strncmp(header->signature, signature, 4)) {
			return (void *)header;
		}
	}

	return NULL;
}

bool _use_xsdt() { return _xsdt_is_available; }