/*
 * swap_status.h
 *
 *  Created on: Jul 21, 2020
 *      Author: bohd
 */

#ifndef BOOT_BOOTUTIL_SRC_SWAP_STATUS_H_
#define BOOT_BOOTUTIL_SRC_SWAP_STATUS_H_

#ifdef MCUBOOT_SWAP_USING_STATUS

#define BOOT_SWAP_STATUS_ENCK1_SZ       16UL
#define BOOT_SWAP_STATUS_ENCK2_SZ       16UL

struct image_status_trailer {
    uint8_t enc_key1[BOOT_SWAP_STATUS_ENCK1_SZ];
    uint8_t enc_key2[BOOT_SWAP_STATUS_ENCK2_SZ];
    uint32_t swap_size;
    uint8_t swap_type;
    uint8_t copy_done;
    uint8_t image_ok;
    uint8_t magic[BOOT_MAGIC_SZ];
};

#define BOOT_SWAP_STATUS_SWAPSZ_SZ      4UL
#define BOOT_SWAP_STATUS_SWAPINF_SZ     1UL
#define BOOT_SWAP_STATUS_COPY_DONE_SZ   1UL
#define BOOT_SWAP_STATUS_IMG_OK_SZ      1UL

#define BOOT_SWAP_STATUS_MAGIC_SZ BOOT_MAGIC_SZ

#define BOOT_SWAP_STATUS_CNT_SZ         4UL
#define BOOT_SWAP_STATUS_CRC_SZ         4UL

#define BOOT_SWAP_STATUS_ROW_SZ         CY_FLASH_ALIGN

#define BOOT_SWAP_STATUS_PAYLD_SZ       (BOOT_SWAP_STATUS_ROW_SZ -\
                                            BOOT_SWAP_STATUS_CNT_SZ - \
                                            BOOT_SWAP_STATUS_CRC_SZ)

/* number of rows sector-status area should fit into */
#if (BOOT_MAX_IMG_SECTORS % \
        BOOT_SWAP_STATUS_PAYLD_SZ != 0)
#define BOOT_SWAP_STATUS_SECT_ROWS_NUM  ((BOOT_MAX_IMG_SECTORS / \
                                            BOOT_SWAP_STATUS_PAYLD_SZ)+1)
#else
#define BOOT_SWAP_STATUS_SECT_ROWS_NUM  (BOOT_MAX_IMG_SECTORS / \
                                            BOOT_SWAP_STATUS_PAYLD_SZ)
#endif

#define BOOT_SWAP_STATUS_TRAIL_ROWS_NUM 1UL

/* the size of one copy of status area */
#define BOOT_SWAP_STATUS_D_SIZE     BOOT_SWAP_STATUS_ROW_SZ * \
                                    (BOOT_SWAP_STATUS_SECT_ROWS_NUM + \
                                    BOOT_SWAP_STATUS_TRAIL_ROWS_NUM)

/* multiplier which defines how many blocks will be used to reduce Flash wear
 * 1 is for single write wear, 2 - twice less wear, 3 - three times less wear, etc */
#define BOOT_SWAP_STATUS_MULT       2

#define BOOT_SWAP_STATUS_SIZE       (BOOT_SWAP_STATUS_MULT*BOOT_SWAP_STATUS_D_SIZE)

#define BOOT_SWAP_STATUS_SZ_PRIM    BOOT_SWAP_STATUS_SIZE
#define BOOT_SWAP_STATUS_SZ_SEC     BOOT_SWAP_STATUS_SIZE

#define BOOT_SWAP_STATUS_OFFS_PRIM  0UL
#define BOOT_SWAP_STATUS_OFFS_SEC   (BOOT_SWAP_STATUS_OFFS_PRIM + \
                                    BOOT_SWAP_STATUS_SZ_PRIM)
#endif /* MCUBOOT_SWAP_USING_STATUS */

#endif /* BOOT_BOOTUTIL_SRC_SWAP_STATUS_H_ */
