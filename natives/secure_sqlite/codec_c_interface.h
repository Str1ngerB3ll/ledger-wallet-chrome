/*
 *
 * AES SQLite Codec implementation
 * (C) 2014 Ledger
 * Based on botansqlite3 git@github.com:OlivierJG/botansqlite3.git
 *
 */

#ifndef _CODEC_C_INTERFACE_H_
#define _CODEC_C_INTERFACE_H_

#ifdef __cplusplus
extern "C" {
#endif

const char *LWCGetAndResetError(void *codec);

void LWCFreeCodec(void *codec);

unsigned char *LWCEncrypt(void *codec, int page, unsigned char *data);

void LWCDecrypt(void *codec, int page, unsigned char *data);

void LWCSetPageSize(void *codec, int pageSize);

void *LWCCreateNewCodec(void *db);

void *LWCCloneCodec(void *codec);

void LWCSetCodecEncryptionKey(void *codec, unsigned char *key, int keyLength);

void *LWCGetDb(void *codec);

#ifdef __cplusplus
}
#endif

#endif