/*
 * Copyright (c) Pavol Rusnak, SatoshiLabs
 *
 * Licensed under TREZOR License
 * see LICENSE file for details
 */

#include "py/objstr.h"

#include "trezor-crypto/pbkdf2.h"

typedef struct _mp_obj_Pbkdf2_t {
    mp_obj_base_t base;
    union {
        PBKDF2_HMAC_SHA256_CTX ctx256;
        PBKDF2_HMAC_SHA512_CTX ctx512;
    };
    int prf;
} mp_obj_Pbkdf2_t;

STATIC mp_obj_t mod_TrezorCrypto_Pbkdf2_update(mp_obj_t self, mp_obj_t data);

/// def trezor.crypto.pbkdf2(prf: str, password: bytes, salt: bytes, iterations: int=None) -> Pbkdf2:
///     '''
///     Create a PBKDF2 context
///     '''
STATIC mp_obj_t mod_TrezorCrypto_Pbkdf2_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 3, 4, false);
    mp_obj_Pbkdf2_t *o = m_new_obj(mp_obj_Pbkdf2_t);
    o->base.type = type;

    mp_buffer_info_t prf;
    mp_get_buffer_raise(args[0], &prf, MP_BUFFER_READ);
    mp_buffer_info_t password;
    mp_get_buffer_raise(args[1], &password, MP_BUFFER_READ);
    mp_buffer_info_t salt;
    mp_get_buffer_raise(args[2], &salt, MP_BUFFER_READ);

    if (password.len == 0) {
        password.buf = "";
    }
    if (salt.len == 0) {
        salt.buf = "";
    }

    o->prf = 0;
    if (prf.len == 11 && memcmp(prf.buf, "hmac-sha256", prf.len) == 0) {
        pbkdf2_hmac_sha256_Init(&(o->ctx256), password.buf, password.len, salt.buf, salt.len);
        o->prf = 256;
    } else
    if (prf.len == 11 && memcmp(prf.buf, "hmac-sha512", prf.len) == 0) {
        pbkdf2_hmac_sha512_Init(&(o->ctx512), password.buf, password.len, salt.buf, salt.len);
        o->prf = 512;
    } else
    if (o->prf == 0) {
        mp_raise_ValueError("Invalid PRF");
    }
    // constructor called with iterations as fourth parameter
    if (n_args > 3) {
        mod_TrezorCrypto_Pbkdf2_update(MP_OBJ_FROM_PTR(o), args[3]);
    }
    return MP_OBJ_FROM_PTR(o);
}

/// def trezor.crypto.pbkdf2.update(self, iterations: int) -> None:
///     '''
///     Update a PBKDF2 context
///     '''
STATIC mp_obj_t mod_TrezorCrypto_Pbkdf2_update(mp_obj_t self, mp_obj_t iterations) {
    mp_obj_Pbkdf2_t *o = MP_OBJ_TO_PTR(self);
    uint32_t iter = mp_obj_get_int(iterations);
    if (o->prf == 256) {
        pbkdf2_hmac_sha256_Update(&(o->ctx256), iter);
    }
    if (o->prf == 512) {
        pbkdf2_hmac_sha512_Update(&(o->ctx512), iter);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mod_TrezorCrypto_Pbkdf2_update_obj, mod_TrezorCrypto_Pbkdf2_update);

/// def trezor.crypto.pbkdf2.key(self) -> bytes:
///     '''
///     Retreive derived key
///     '''
STATIC mp_obj_t mod_TrezorCrypto_Pbkdf2_key(mp_obj_t self) {
    mp_obj_Pbkdf2_t *o = MP_OBJ_TO_PTR(self);
    vstr_t vstr;
    if (o->prf == 256) {
        PBKDF2_HMAC_SHA256_CTX ctx;
        memcpy(&ctx, &(o->ctx256), sizeof(PBKDF2_HMAC_SHA256_CTX));
        vstr_init_len(&vstr, SHA256_DIGEST_LENGTH);
        pbkdf2_hmac_sha256_Final(&ctx, (uint8_t *)vstr.buf);
        memset(&ctx, 0, sizeof(PBKDF2_HMAC_SHA256_CTX));
    }
    if (o->prf == 512) {
        PBKDF2_HMAC_SHA512_CTX ctx;
        memcpy(&ctx, &(o->ctx512), sizeof(PBKDF2_HMAC_SHA512_CTX));
        vstr_init_len(&vstr, SHA512_DIGEST_LENGTH);
        pbkdf2_hmac_sha512_Final(&ctx, (uint8_t *)vstr.buf);
        memset(&ctx, 0, sizeof(PBKDF2_HMAC_SHA512_CTX));
    }
    return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_TrezorCrypto_Pbkdf2_key_obj, mod_TrezorCrypto_Pbkdf2_key);

STATIC mp_obj_t mod_TrezorCrypto_Pbkdf2___del__(mp_obj_t self) {
    mp_obj_Pbkdf2_t *o = MP_OBJ_TO_PTR(self);
    memset(&(o->ctx256), 0, sizeof(PBKDF2_HMAC_SHA256_CTX));
    memset(&(o->ctx512), 0, sizeof(PBKDF2_HMAC_SHA512_CTX));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_TrezorCrypto_Pbkdf2___del___obj, mod_TrezorCrypto_Pbkdf2___del__);

STATIC const mp_rom_map_elem_t mod_TrezorCrypto_Pbkdf2_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_update), MP_ROM_PTR(&mod_TrezorCrypto_Pbkdf2_update_obj) },
    { MP_ROM_QSTR(MP_QSTR_key), MP_ROM_PTR(&mod_TrezorCrypto_Pbkdf2_key_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&mod_TrezorCrypto_Pbkdf2___del___obj) },
};
STATIC MP_DEFINE_CONST_DICT(mod_TrezorCrypto_Pbkdf2_locals_dict, mod_TrezorCrypto_Pbkdf2_locals_dict_table);

STATIC const mp_obj_type_t mod_TrezorCrypto_Pbkdf2_type = {
    { &mp_type_type },
    .name = MP_QSTR_Pbkdf2,
    .make_new = mod_TrezorCrypto_Pbkdf2_make_new,
    .locals_dict = (void*)&mod_TrezorCrypto_Pbkdf2_locals_dict,
};
