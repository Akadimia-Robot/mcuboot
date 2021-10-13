/*
 * Copyright (c) 2021 EEMBC
 * Copyright (c) 2020 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
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
 * limitations under the License
 */
#include <stdio.h>
#include <bootutil/bootutil_public.h>
#include <bootutil/bootutil.h>
#include <bootutil/bootutil_log.h>
#include <flash_map_backend/flash_map_backend.h>

struct area_desc {
    const char *name;
    uint8_t id;
};

static const struct area_desc areas[] = {
    {"primary", 0},
    {"secondary", 1}
};

int32_t print_hex(const uint8_t *array, size_t size) {
    if (array == NULL) {
        return -1;
    }
    for (size_t i = 0; i < size; i++) {
        if (i % 10 == 0) printf("\n");
        printf("0x%02x ", (unsigned char)array[i]);
    }
    return 0;
}

int32_t print_bytestr(const uint8_t *bytes, size_t len)
{
    if (bytes == NULL)
        return( -1 );

    for(unsigned int idx=0; idx < len; idx++)
    {
        if (idx % 10 == 0) printf("\n");
        printf("%02X", bytes[idx]);
    }
    return( 0 );
}

static const char *swap_state_flag_str(uint8_t flag)
{
    switch (flag) {
    case BOOT_FLAG_SET:
        return "set";
    case BOOT_FLAG_BAD:
        return "bad";
    case BOOT_FLAG_UNSET:
        return "unset";
    case BOOT_FLAG_ANY:
        return "any";
    }

    return "unknown";
}


static const char *swap_type_str(uint8_t type)
{
    switch (type) {
    case BOOT_SWAP_TYPE_NONE:
        return "none";
    case BOOT_SWAP_TYPE_TEST:
        return "test";
    case BOOT_SWAP_TYPE_PERM:
        return "perm";
    case BOOT_SWAP_TYPE_REVERT:
        return "revert";
    case BOOT_SWAP_TYPE_FAIL:
        return "fail";
    }

    return "unknown";
}

static const char *swap_state_magic_str(uint8_t magic)
{
    switch (magic) {
    case BOOT_MAGIC_GOOD:
        return "good";
    case BOOT_MAGIC_BAD:
        return "bad";
    case BOOT_MAGIC_UNSET:
        return "unset";
    case BOOT_MAGIC_ANY:
        return "any";
    case BOOT_MAGIC_NOTGOOD:
        return "notgood";
    }

    return "unknown";
}

#if 0
static int cmd_mcuboot_info_area( const struct area_desc *area)
{
    struct mcuboot_img_header hdr;
    struct boot_swap_state swap_state;
    int err;
    printf("\n");
    err = boot_read_bank_header(area->id, &hdr, sizeof(hdr));
    if (err) {
        printf( "\nfailed to read %s area (%u) %s: %d\n",
                area->name, area->id, "header", err);
        return err;
    }

    printf( "%s area (%u):\n", area->name, area->id);
    printf( "  version: %u.%u.%u+%u\n",
            (unsigned int) hdr.h.v1.sem_ver.major,
            (unsigned int) hdr.h.v1.sem_ver.minor,
            (unsigned int) hdr.h.v1.sem_ver.revision,
            (unsigned int) hdr.h.v1.sem_ver.build_num);
    printf( "  image size: %u\n",
            (unsigned int) hdr.h.v1.image_size);

    err = boot_read_swap_state_by_id(area->id, &swap_state);
    if (err) {
        printf( "\nfailed to read %s area (%u) %s: %d\n",
                area->name, area->id, "swap state", err);
        return err;
    }

    printf( "  magic: %s\n",
            swap_state_magic_str(swap_state.magic));

    printf( "  swap type: %s\n",
            swap_type_str(swap_state.swap_type));

    printf( "  copy done: %s\n",
            swap_state_flag_str(swap_state.copy_done));
    printf( "  image ok: %s\n",
            swap_state_flag_str(swap_state.image_ok));

    return 0;
}
#endif
#if 0
static int cmd_mcuboot_info()
{
    int i;
    int j = MCUBOOT_SLOT_SIZE;
    uint32_t *ptr1 = &flash_layout[j - 0x30];
    uint32_t *ptr2 = &flash_layout[j + j - 0x30];

    printf( "swap type: %s\n", swap_type_str(mcuboot_swap_type()));
    printf( "confirmed: %d\n", boot_is_img_confirmed());
    printf( "");

    for (i = 0; i < sizeof(areas) / sizeof (areas[0] ); i++) {
        printf("\n");
/* // Skip trailers
        if (areas[i].id == 0)
        {
            printf("\nTrailer (primary) at (0x%x):\n", (unsigned int) &flash_layout[j - 0x30]);
            print_hex((uint8_t*)ptr1, 0x90);
            printf("\n");
        }
        else
        {
            printf("\nTrailer (secondary) at (0x%x):\n", (unsigned int)  &flash_layout[j + j - 0x30]);
            print_hex((uint8_t*)ptr2, 0x90);
            printf("\n");
        }
*/        cmd_mcuboot_info_area(&areas[i]);
    }

    return 0;
}
#endif

extern uint32_t flash_layout[MCUBOOT_SLOT_SIZE+MCUBOOT_SLOT_SIZE+MCUBOOT_SCRATCH_SIZE];

fih_int FIH_SUCCESS = 0;
fih_int FIH_FAILURE = 1;



/* Firmware image created with:
 * scripts/imgtool.py sign -k ../root-ec-p256.pem --align 8 --version 1.2.3 --header-size 128 -S 4048 loaded.bin loaded.signed
 */
unsigned char firmware_image[] = {
  0x3d, 0xb8, 0xf3, 0x96, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00,
  0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x07, 0x69, 0x96, 0x00, 0x10, 0x00, 0x20, 0x00, 0xc6, 0x9b, 0x54, 0xe0,
  0x65, 0x57, 0xf6, 0xed, 0x62, 0x19, 0xb1, 0x15, 0xfc, 0x99, 0xae, 0x1c,
  0xc3, 0x6d, 0xd1, 0x82, 0x16, 0xe7, 0x61, 0x8f, 0x26, 0xdc, 0xe5, 0x09,
  0xfe, 0x7b, 0x7f, 0x9e, 0x01, 0x00, 0x20, 0x00, 0xe3, 0x04, 0x66, 0xf6,
  0xb8, 0x47, 0x0c, 0x1f, 0x29, 0x07, 0x0b, 0x17, 0xf1, 0xe2, 0xd3, 0xe9,
  0x4d, 0x44, 0x5e, 0x3f, 0x60, 0x80, 0x87, 0xfd, 0xc7, 0x11, 0xe4, 0x38,
  0x2b, 0xb5, 0x38, 0xb6, 0x22, 0x00, 0x46, 0x00, 0x30, 0x44, 0x02, 0x20,
  0x4c, 0x46, 0x8c, 0x03, 0xd3, 0x2f, 0xdb, 0x64, 0xd6, 0xfa, 0x6f, 0x10,
  0xbd, 0x91, 0x35, 0xe2, 0x74, 0xbd, 0x80, 0xfe, 0x5e, 0xc5, 0x46, 0xef,
  0x79, 0x43, 0x3e, 0xae, 0x82, 0xe3, 0xf8, 0x98, 0x02, 0x20, 0x1c, 0x6f,
  0x06, 0x7e, 0x8e, 0xd3, 0xdc, 0x1f, 0xf1, 0x78, 0xa9, 0xc1, 0xa6, 0x7b,
  0x91, 0x54, 0xcd, 0x23, 0x3a, 0x9d, 0x95, 0x31, 0x79, 0x67, 0x4d, 0x88,
  0x0a, 0xb0, 0xd2, 0x19, 0x9f, 0xf4
};
//unsigned int firmware_image_len = 378;
unsigned int firmware_image_len = sizeof(firmware_image) / sizeof(unsigned char);

/* Firmware image created with:
 * scripts/imgtool.py sign -k ../root-ec-p256.pem --align 8 --version 2.0.0 --header-size 128 -S 4048 loaded.bin loaded.signed
 */
unsigned char firmware_image2[] = {
  0x3d, 0xb8, 0xf3, 0x96, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00,
  0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x07, 0x69, 0x97, 0x00, 0x10, 0x00, 0x20, 0x00, 0xe9, 0x9b, 0x8e, 0xc8,
  0x7c, 0xfe, 0x29, 0xbc, 0x2b, 0x46, 0x65, 0x9c, 0x1e, 0x01, 0xb5, 0xa3,
  0x28, 0xb9, 0xa2, 0x78, 0x28, 0xb2, 0xbf, 0x36, 0xda, 0x18, 0x81, 0xe7,
  0xf0, 0x42, 0x3b, 0x43, 0x01, 0x00, 0x20, 0x00, 0xe3, 0x04, 0x66, 0xf6,
  0xb8, 0x47, 0x0c, 0x1f, 0x29, 0x07, 0x0b, 0x17, 0xf1, 0xe2, 0xd3, 0xe9,
  0x4d, 0x44, 0x5e, 0x3f, 0x60, 0x80, 0x87, 0xfd, 0xc7, 0x11, 0xe4, 0x38,
  0x2b, 0xb5, 0x38, 0xb6, 0x22, 0x00, 0x47, 0x00, 0x30, 0x45, 0x02, 0x20,
  0x01, 0x1d, 0x88, 0xef, 0xc6, 0x06, 0x72, 0x81, 0x92, 0xb1, 0xce, 0xe5,
  0x80, 0x27, 0x1b, 0x9d, 0x43, 0x68, 0x9f, 0xe0, 0xf2, 0x48, 0x10, 0xcf,
  0x97, 0x51, 0x82, 0x1c, 0xb8, 0xb1, 0x3a, 0x23, 0x02, 0x21, 0x00, 0x86,
  0x51, 0x6d, 0x0a, 0x01, 0x9e, 0x84, 0x9d, 0x12, 0x1a, 0x48, 0x9f, 0xe2,
  0xa9, 0x30, 0xfb, 0x04, 0x9d, 0xb2, 0x86, 0x15, 0x8d, 0x95, 0xfd, 0xde,
  0x25, 0x9a, 0xf0, 0xa5, 0x18, 0xe6, 0x84
};
//unsigned int firmware_image2_len = 379;
unsigned int firmware_image2_len = sizeof(firmware_image2) / sizeof(unsigned char);


struct flash_pages_info {
        int start_offset; /* offset from the base of flash address */
        size_t size;
        uint32_t index;
};


int main(void)
{
    struct boot_rsp rsp;
    fih_int res = FIH_FAILURE;

    printf("---> Writing initial image to flash (primary slot)\n");

    /* Write working image into primary slot */
    flash_example(firmware_image, firmware_image_len, 0);

    //cmd_mcuboot_info();

    printf("\nImage #1 at (0x%x):\n", (unsigned int) flash_layout);
    print_hex( (uint8_t*) &flash_layout[0], 200);
    printf("\n");

    res = boot_go(&rsp);

    if (res != FIH_SUCCESS )
    {
        BOOT_LOG_ERR("Unable to find bootable image");
        return -1;
    }

    BOOT_LOG_INF("Bootloader chainload address offset: 0x%x\n",
                 rsp.br_image_off);

    //cmd_mcuboot_info();

    printf("---> Writing new image to secondary slot and set it pending\n");

    /* Write new image into secondary slot */
    flash_example(firmware_image2, firmware_image2_len, 1);

    printf("\nImage #2 at (x%lx):\n", (unsigned int)  flash_layout + MCUBOOT_SLOT_SIZE);
    print_hex( (uint8_t*) &flash_layout[MCUBOOT_SLOT_SIZE], 200); // +36
    printf("\n");

    res = boot_set_pending(0); // not permanent
    //res = boot_set_pending_multi(0, 1);

    if (res != 0 )
    {
        BOOT_LOG_ERR("Unable to set image pending");
        return -1;
    }

    //cmd_mcuboot_info();

    res = boot_go(&rsp);

    if (res != FIH_SUCCESS )
    {
        BOOT_LOG_ERR("Unable to find bootable image");
        return -1;
    }

    //cmd_mcuboot_info();

    printf("---> Confirm image as good.\n"); // make permanent

    res = boot_set_confirmed();

    if (res != 0 )
    {
        BOOT_LOG_ERR("Unable to confirm image");
        return -1;
    }

    //cmd_mcuboot_info();

    printf("---> Boot confirmed image.\n");

    res = boot_go(&rsp);

    if (res != FIH_SUCCESS )
    {
        BOOT_LOG_ERR("Unable to find bootable image");
        return -1;
    }

    BOOT_LOG_INF("Bootloader chainload address offset: 0x%x\n",
                 rsp.br_image_off);

    //cmd_mcuboot_info();

    printf("\nImage #1 at (0x%x):\n", (unsigned int) flash_layout);
    print_hex( (uint8_t*) &flash_layout[0], 200);
    printf("\n");

    printf("\nImage #2 at (0x%lx):\n", (unsigned int)  flash_layout + MCUBOOT_SLOT_SIZE);
    print_hex( (uint8_t*) &flash_layout[MCUBOOT_SLOT_SIZE], 200);
    printf("\n");

    return(0);
}
