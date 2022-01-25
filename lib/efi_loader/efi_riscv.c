// SPDX-License-Identifier: GPL-2.0+
/*
 * Defines APIs that allow an OS to interact with UEFI firmware to query
 * information about the boot hart ID.
 *
 * Copyright (c) 2022, Ventana Micro Systems Inc
 */

#define LOG_CATEGORY LOGC_EFI
#include <common.h>
#include <efi_loader.h>
#include <efi_variable.h>
#include <log.h>
#include <asm/global_data.h>
#include <efi_riscv.h>

DECLARE_GLOBAL_DATA_PTR;

static const efi_guid_t efi_guid_riscv_boot_protocol = RISCV_EFI_BOOT_PROTOCOL_GUID;
static efi_uintn_t hartid;

/**
 * efi_riscv_get_boot_hartid() - return boot hart ID
 *
 * @this:		RISCV_EFI_BOOT_PROTOCOL instance
 * @boot_hartid		caller allocated memory to return boot hart id

 * Return:	status code
 */
static efi_status_t EFIAPI
efi_riscv_get_boot_hartid(struct riscv_efi_boot_protocol *this,
					efi_uintn_t *boot_hartid)
{
	log_err(" efi_riscv_get_boot_hartid ENTER\n");
	if ((this == NULL) || (boot_hartid == NULL))
		return EFI_INVALID_PARAMETER;

	*boot_hartid = hartid;

	return EFI_SUCCESS;
}

static const struct riscv_efi_boot_protocol riscv_efi_boot_prot = {
	.revision = RISCV_EFI_BOOT_PROTOCOL_REVISION,
	.get_boot_hartid = efi_riscv_get_boot_hartid
};

/**
 * efi_riscv_register() - register RISCV_EFI_BOOT_PROTOCOL
 *
 *
 * Return:	status code
 */
efi_status_t efi_riscv_register(void)
{
	efi_status_t ret = EFI_SUCCESS;

	/* save boot hart id since gd is not accessible after launching
	 * the kernel
	 */
	hartid = gd->arch.boot_hart;

	ret = efi_add_protocol(efi_root, &efi_guid_riscv_boot_protocol,
			       (void *)&riscv_efi_boot_prot);
	if (ret != EFI_SUCCESS) {
		log_err("Cannot install RISCV_EFI_BOOT_PROTOCOL\n");
	}
	return ret;
}
