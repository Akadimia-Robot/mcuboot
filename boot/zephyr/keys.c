/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include <bootutil/sign_key.h>

/*
 * Even though this is in principle a Zephyr-specific file, the
 * simulator builds it and uses it as well. Because of that, we can't
 * use Kconfig symbols for key types, and have to rely on the MCUBoot
 * symbols (which Zephyr provides via this header, and the simulator
 * provides via the compiler command line).
 */
#include <mcuboot_config/mcuboot_config.h>

#if defined(MCUBOOT_SIGN_RSA)
#define HAVE_KEYS
extern const unsigned char rsa_pub_key[];
extern unsigned int rsa_pub_key_len;
#elif defined(MCUBOOT_SIGN_EC256)
#define HAVE_KEYS
extern const unsigned char ecdsa_pub_key[];
extern unsigned int ecdsa_pub_key_len;
#else
#error "No public key available for given signing algorithm."
#endif

/*
 * NOTE: *_pub_key and *_pub_key_len are autogenerated based on the provided
 *       key file. If no key file was configured, the array and length must be
 *       provided and added to the build manually.
 */
#if defined(HAVE_KEYS)
const struct bootutil_key bootutil_keys[] = {
    {
#if defined(MCUBOOT_SIGN_RSA)
        .key = rsa_pub_key,
        .len = &rsa_pub_key_len,
#elif defined(MCUBOOT_SIGN_EC256)
        .key = ecdsa_pub_key,
        .len = &ecdsa_pub_key_len,
#endif
    },
};
const int bootutil_key_cnt = 1;
#endif
