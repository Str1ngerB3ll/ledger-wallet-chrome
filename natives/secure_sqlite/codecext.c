/*
 *
 * AES SQLite Codec implementation
 * (C) 2014 Ledger
 * Based on botansqlite3 git@github.com:OlivierJG/botansqlite3.git
 *
 */

#include "sqlite3.c"
#include "codecext.h"

unsigned char LWCHandleError(void *codec)
{
    const char *error = LWCGetAndResetError(codec);
    if (error) {
        sqlite3Error((sqlite3*)LWCGetDb(codec), 0x1ED5E, "Encryption Error: %s", error);
        return 1;
    }
    return 0;
}

// Free the encryption codec, called from pager.c (address passed in sqlite3PagerSetCodec)
void sqlite3PagerFreeCodec(void *codec)
{
  LWCFreeCodec(codec);
}

// Report the page size to the codec, called from pager.c (address passed in sqlite3PagerSetCodec)
void sqlite3CodecSizeChange(void *codec, int pageSize, int reserve)
{
    LWCSetPageSize(codec, pageSize);
}

// Encrypt/Decrypt functionality, called by pager.c
void* sqlite3Codec(void *codec, void *data, Pgno pageNum, int mode)
{
    if (codec == NULL) //Db not encrypted
        return data;

    switch(mode)
    {
    case 0: // Undo a "case 7" journal file encryption
    case 2: // Reload a page
    case 3: // Load a page
        LWCDecrypt(codec, pageNum, data);
        break;
    case 6: // Encrypt a page for the main database file
    case 7:
        data = LWCEncrypt(codec, pageNum, data);
        break;
    }
    LWCHandleError(codec);

    return data;
}

/*
** Specify the key for an encrypted database.  This routine should be
** called right after sqlite3_open().
**
** The code to implement this API is not available in the public release
** of SQLite.
**
*/
int sqlite3_key(sqlite3 *db, const void *key, int keyLength)
{
  // The key is only set for the main database, not the temp database
  return sqlite3CodecAttach(db, 0, key, keyLength);
}

int sqlite3_key_v2(sqlite3 *db, const char *dbName, const void *key, int keyLength)
{
  return sqlite3_key(db, key, keyLength);
}

/*
** Change the key on an open database.  If the current database is not
** encrypted, this routine will encrypt it.  If pNew==0 or nNew==0, the
** database is decrypted.
**
** The code to implement this API is not available in the public release
** of SQLite.
*/
int sqlite3_rekey(sqlite3 *db, const void *key, int keyLength)
{
 // Not supported yet
  return SQLITE_ERROR;
}

int sqlite3_rekey_v2(sqlite3 *db, const char *dbName, const void *key, int keyLength)
{
  return sqlite3_rekey(db, key, keyLength);
}

/*
** Specify the activation key for a SEE database.  Unless
** activated, none of the SEE routines will work.
*/
void sqlite3_activate_see(const char *passPhrase)
{
 // Useless
}

int sqlite3CodecAttach(sqlite3 *db, int nDb, const void *key, int keyLength)
{
  void *codec;

  if (key == NULL && keyLength <= 0 && nDb != 0)
  {
    void *mainCodec = sqlite3PagerGetCodec(sqlite3BtreePager(db->aDb[0].pBt));
    codec = LWCCloneCodec(mainCodec);
  }
  else
    codec = LWCCreateNewCodec(db);
    LWCSetCodecEncryptionKey(codec, (unsigned char *)key, keyLength);

  if (codec)
  {
    sqlite3PagerSetCodec(
      sqlite3BtreePager(db->aDb[nDb].pBt),
      sqlite3Codec, sqlite3CodecSizeChange, sqlite3PagerFreeCodec, codec
    );
  }

  if (LWCHandleError(codec))
    return SQLITE_ERROR;
  return SQLITE_OK;
}

void sqlite3CodecGetKey(sqlite3* db, int nDb, void **zKey, int *nKey)
{
    // The unencrypted password is not stored for security reasons
    // therefore always return NULL
    *zKey = NULL;
    *nKey = -1;
}