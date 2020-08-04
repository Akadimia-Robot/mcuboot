/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Copyright (c) 2019 JUUL Labs
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include "bootutil/bootutil.h"
#include "bootutil_priv.h"
#include "swap_priv.h"
#include "bootutil/bootutil_log.h"

#include "swap_status.h"

#include "mcuboot_config/mcuboot_config.h"

MCUBOOT_LOG_MODULE_DECLARE(mcuboot);

#if defined(MCUBOOT_SWAP_USING_STATUS)

static inline size_t
boot_status_sector_size(const struct boot_loader_state *state, size_t sector)
{
    return state->status.sectors[sector].fs_size;
}

static inline uint32_t
boot_status_sector_off(const struct boot_loader_state *state,
                    size_t sector)
{
    return state->status.sectors[sector].fs_off -
           state->status.sectors[0].fs_off;
}

int
swap_erase_trailer_sectors(const struct boot_loader_state *state,
                           const struct flash_area *fap)
{
//    uint8_t slot;
    uint32_t sector;
    uint32_t trailer_sz;
    uint32_t total_sz;
    uint32_t off, sub_offs;
    uint32_t sz;
    int fa_id_primary;
    int fa_id_secondary;
    uint8_t image_index;
    int rc;

    BOOT_LOG_DBG("Erasing trailer; fa_id=%d", fap->fa_id);

    const struct flash_area *fap_stat;

    rc = flash_area_open(FLASH_AREA_IMAGE_SWAP_STATUS, &fap_stat);
    assert (rc == 0);

    image_index = BOOT_CURR_IMG(state);
    fa_id_primary = flash_area_id_from_multi_image_slot(image_index,
            BOOT_PRIMARY_SLOT);
    fa_id_secondary = flash_area_id_from_multi_image_slot(image_index,
            BOOT_SECONDARY_SLOT);

    if (fap->fa_id == fa_id_primary) {
//        slot = BOOT_PRIMARY_SLOT;
    } else if (fap->fa_id == fa_id_secondary) {
//        slot = BOOT_SECONDARY_SLOT;
    } else {
        return BOOT_EFLASH;
    }

    sub_offs = BOOT_SWAP_STATUS_SIZE*(fap->fa_id);

    /* delete starting from last sector and moving to beginning */
    // TODO: calculate last sector of status sub-area
//    sector = boot_img_num_sectors(state, slot) - 1;
    sector = boot_status_num_sectors(state) - 1;
//    trailer_sz = boot_trailer_sz(BOOT_WRITE_SZ(state));
    trailer_sz = BOOT_SWAP_STATUS_SIZE;
    total_sz = 0;
    do {
//        sz = boot_img_sector_size(state, slot, sector);
        sz = boot_status_sector_size(state, sector);
//        off = boot_img_sector_off(state, slot, sector);
        off = boot_status_sector_off(state, sector) + sub_offs;
        rc = boot_erase_region(fap_stat, off, sz);
        assert(rc == 0);

        sector--;
        total_sz += sz;
    } while (total_sz < trailer_sz);

    flash_area_close(fap_stat);

    return rc;
}

int
swap_status_init(const struct boot_loader_state *state,
                 const struct flash_area *fap,
                 const struct boot_status *bs)
{
//    struct boot_swap_state swap_state;
//    uint8_t image_index;
    int rc;
//
//#if (BOOT_IMAGE_NUMBER == 1)
//    (void)state;
//#endif
//
//    image_index = BOOT_CURR_IMG(state);
//
//    BOOT_LOG_DBG("initializing status; fa_id=%d", fap->fa_id);
//
//    rc = boot_read_swap_state_by_id(FLASH_AREA_IMAGE_SECONDARY(image_index),
//            &swap_state);
//    assert(rc == 0);
//
//    if (bs->swap_type != BOOT_SWAP_TYPE_NONE) {
//        rc = boot_write_swap_info(fap, bs->swap_type, image_index);
//        assert(rc == 0);
//    }
//
//    if (swap_state.image_ok == BOOT_FLAG_SET) {
//        rc = boot_write_image_ok(fap);
//        assert(rc == 0);
//    }
//
//    rc = boot_write_swap_size(fap, bs->swap_size);
//    assert(rc == 0);
//
//#ifdef MCUBOOT_ENC_IMAGES
//    rc = boot_write_enc_key(fap, 0, bs);
//    assert(rc == 0);
//
//    rc = boot_write_enc_key(fap, 1, bs);
//    assert(rc == 0);
//#endif
//
//    rc = boot_write_magic(fap);
//    assert(rc == 0);

    return 0;
}

int
swap_read_status(struct boot_loader_state *state, struct boot_status *bs)
{
//    const struct flash_area *fap;
//    uint32_t off;
//    uint8_t swap_info;
//    int area_id;
    int rc;
//
//    bs->source = swap_status_source(state);
//    switch (bs->source) {
//    case BOOT_STATUS_SOURCE_NONE:
//        return 0;
//
//#if MCUBOOT_SWAP_USING_SCRATCH
//    case BOOT_STATUS_SOURCE_SCRATCH:
//        area_id = FLASH_AREA_IMAGE_SCRATCH;
//        break;
//#endif
//
//    case BOOT_STATUS_SOURCE_PRIMARY_SLOT:
//        area_id = FLASH_AREA_IMAGE_PRIMARY(BOOT_CURR_IMG(state));
//        break;
//
//    default:
//        assert(0);
//        return BOOT_EBADARGS;
//    }
//
//    rc = flash_area_open(area_id, &fap);
//    if (rc != 0) {
//        return BOOT_EFLASH;
//    }
//
//    rc = swap_read_status_bytes(fap, state, bs);
//    if (rc == 0) {
//        off = boot_swap_info_off(fap);
//        rc = flash_area_read_is_empty(fap, off, &swap_info, sizeof swap_info);
//        if (rc == 1) {
//            BOOT_SET_SWAP_INFO(swap_info, 0, BOOT_SWAP_TYPE_NONE);
//            rc = 0;
//        }
//
//        /* Extract the swap type info */
//        bs->swap_type = BOOT_GET_SWAP_TYPE(swap_info);
//    }
//
//    flash_area_close(fap);
//
    return rc;
}

int boot_status_num_sectors(const struct boot_loader_state *state)
{
    return (int)(BOOT_SWAP_STATUS_SIZE / boot_status_sector_size(state, 0));
}

#endif /* defined(MCUBOOT_SWAP_USING_MOVE) */
