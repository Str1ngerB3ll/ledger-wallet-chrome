/*
 *
 * AES SQLite Codec implementation
 * (C) 2014 Ledger
 * Based on botansqlite3 git@github.com:OlivierJG/botansqlite3.git
 *
 */

#ifndef _AES_CODEC_H_
#define _AES_CODEC_H_

#include "codec.h"
#include <cstdlib>
#include <openssl/aes.h>

//This is definited in sqlite.h and very unlikely to change
#define SQLITE_MAX_PAGE_SIZE 32768

class AesCodec : public Codec {

public:
	AesCodec(const void *db);
 	AesCodec(const Codec& codec);
	virtual void setEncryptionKey(const unsigned char *encryptionKey, const int encryptionKeyLength);
	virtual unsigned char *encrypt(int page, unsigned char *data);
	virtual void decrypt(int page, unsigned char *data);
	virtual int popError();

private:
	AES_KEY _encryptKey;
	AES_KEY _decryptKey;

	unsigned char _page[SQLITE_MAX_PAGE_SIZE];
};

#endif