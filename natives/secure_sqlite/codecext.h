/*
 *
 * AES SQLite Codec implementation
 * (C) 2014 Ledger
 * Based on botansqlite3 git@github.com:OlivierJG/botansqlite3.git
 *
 */

#include <stdlib.h>
#include "codec_c_interface.h"

unsigned char LWCHandleError(void *codec);

void* sqlite3Codec(void *codec, void *data, Pgno pageNum, int mode);

void sqlite3PagerFreeCodec(void *codec);

void sqlite3CodecSizeChange(void *codec, int pageSize, int reserve);

int sqlite3CodecAttach(sqlite3 *db, int nDb, const void *zKey, int nKey);

void sqlite3CodecGetKey(sqlite3* db, int nDb, void **zKey, int *nKey);
