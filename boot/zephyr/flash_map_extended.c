/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 * Copyright (c) 2015 Runtime Inc
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/flash.h>

#include "target.h"

#include <flash_map_backend/flash_map_backend.h>
#include <sysflash/sysflash.h>

#include "bootutil/bootutil.h"
#include "bootutil/bootutil_log.h"

BOOT_LOG_MODULE_DECLARE(mcuboot);

#if (!defined(CONFIG_XTENSA) && DT_HAS_CHOSEN(zephyr_flash_controller))
#define FLASH_DEVICE_ID SOC_FLASH_0_ID
#define FLASH_DEVICE_BASE CONFIG_FLASH_BASE_ADDRESS
#define FLASH_DEVICE_NODE DT_CHOSEN(zephyr_flash_controller)
#elif (defined(CONFIG_XTENSA) && DT_NODE_EXISTS(DT_INST(0, jedec_spi_nor)))
#define FLASH_DEVICE_ID SPI_FLASH_0_ID
#define FLASH_DEVICE_BASE 0
#define FLASH_DEVICE_NODE DT_INST(0, jedec_spi_nor)
#else
#error "FLASH_DEVICE_ID could not be determined"
#endif

static const struct device *flash_dev = DEVICE_DT_GET(FLASH_DEVICE_NODE);

int flash_device_base(uint8_t fd_id, uintptr_t *ret)
{
    if (fd_id != FLASH_DEVICE_ID) {
        BOOT_LOG_ERR("invalid flash ID %d; expected %d",
                     fd_id, FLASH_DEVICE_ID);
        return -EINVAL;
    }
    *ret = FLASH_DEVICE_BASE;
    return 0;
}

const struct flash_area* flash_area_from_multi_image_slot(int image_index, int slot)
{
    switch (slot) {
    case 0: return PRIMARY_IMAGE_FA(image_index);
#if !defined(CONFIG_SINGLE_APPLICATION_SLOT)
    case 1: return SECONDARY_IMAGE_FA(image_index);
#endif
#if defined(CONFIG_BOOT_SWAP_USING_SCRATCH)
    case 2: return SCRATCH_FA;
#endif
    }
    __ASSERT(false, "Reqesting non-existent area should have never happened");
    return NULL;
}

const struct flash_area *flash_area_from_image_slot(int slot)
{
    return flash_area_from_multi_image_slot(0, slot);
}

int flash_area_id_to_multi_image_slot(int image_index, int area_id)
{
    if (area_id == FLASH_AREA_IMAGE_PRIMARY(image_index)) {
        return 0;
    }
#if !defined(CONFIG_SINGLE_APPLICATION_SLOT)
    if (area_id == FLASH_AREA_IMAGE_SECONDARY(image_index)) {
        return 1;
    }
#endif

    BOOT_LOG_ERR("invalid flash area ID");
    return -1;
}

int flash_area_to_multi_image_slot(int image_index, const struct flash_area *fa)
{
    if (fa == PRIMARY_IMAGE_FA(image_index)) {
        return 0;
    }
#if !defined(CONFIG_SINGLE_APPLICATION_SLOT)
    if (fa == SECONDARY_IMAGE_FA(image_index)) {
        return 1;
    }
#endif

    BOOT_LOG_ERR("invalid flash area");
    return -1;
}

#if defined(CONFIG_MCUBOOT_SERIAL_DIRECT_IMAGE_UPLOAD)
const struct flash_area *flash_area_from_direct_image(int image_id)
{
    switch (image_id) {
    case 0:
    case 1:
        return FIXED_PARTITION_ID(slot0_partition);
#if FIXED_PARTITION_EXISTS(slot1_partition)
    case 2:
        return FIXED_PARTITION_ID(slot1_partition);
#endif
#if FIXED_PARTITION_EXISTS(slot2_partition)
    case 3:
        return FIXED_PARTITION_ID(slot2_partition);
#endif
#if FIXED_PARTITION_EXISTS(slot3_partition)
    case 4:
        return FIXED_PARTITION_ID(slot3_partition);
#endif
    }
    return -EINVAL;
}
#endif

int flash_area_sector_from_off(off_t off, struct flash_sector *sector)
{
    int rc;
    struct flash_pages_info page;

    rc = flash_get_page_info_by_offs(flash_dev, off, &page);
    if (rc) {
        return rc;
    }

    sector->fs_off = page.start_offset;
    sector->fs_size = page.size;

    return rc;
}

uint8_t flash_area_get_device_id(const struct flash_area *fa)
{
	(void)fa;
	return FLASH_DEVICE_ID;
}

#define ERASED_VAL 0xff
__weak uint8_t flash_area_erased_val(const struct flash_area *fap)
{
    (void)fap;
    return ERASED_VAL;
}

int flash_area_get_sectors_fa(const struct flash_area *fa, uint32_t *count,
    struct flash_sector *ret)
{
    off_t fa_off = fa->fa_off;
    off_t offset = 0;
    size_t fa_size = fa->fa_size;
    int max_sectors = *count;
    int sector_idx = 0;
    int rc = 0;

    while (rc == 0 && sector_idx < max_sectors && offset < fa_size) {
        struct flash_pages_info fpi;

        rc = flash_get_page_info_by_offs(fa->fa_dev, fa_off + offset, &fpi);

        if (rc == 0) {
            ret[sector_idx].fs_off = fpi.start_offset - fa_off;
            ret[sector_idx].fs_size = fpi.size;
            ++sector_idx;
            offset += fpi.size;
        }
    }

    if (sector_idx >= max_sectors && offset >= fa_size) {
        return -ENOENT;
    }

    return rc;
}
