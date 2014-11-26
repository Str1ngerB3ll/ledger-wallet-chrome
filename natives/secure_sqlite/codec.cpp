/*
 *
 * AES SQLite Codec implementation
 * (C) 2014 Ledger
 * Based on botansqlite3 git@github.com:OlivierJG/botansqlite3.git
 *
 */

#include "codec.h"
#include "codec_c_interface.h"
#include <cstdlib>
#include "aes_codec.h"

Codec::Codec(const void *db) {
	_db = db;
}

Codec::Codec(const Codec& codec) {
	_db = codec.getDb();
	setEncryptionKey(codec.getEncryptionKey(), codec.getEncryptionKeyLength());
}

void Codec::setEncryptionKey(const unsigned char *encryptionKey, const int encryptionKeyLength)
{
	_encryptionKey = const_cast<unsigned char *>(encryptionKey);
}

void Codec::setPageSize(int pageSize)
{
	_pageSize = pageSize;
}

const unsigned char *Codec::getEncryptionKey() const
{
	return _encryptionKey;
}

const int Codec::getEncryptionKeyLength() const
{
	return _encryptionKeyLength;
}

const void *Codec::getDb() const
{
	return _db;
}

const int Codec::getPageSize() const
{
	return _pageSize;
}

void Codec::setError(const char *error)
{
	_error = const_cast<char *>(error);
}

const char *Codec::getAndResetError()
{
	const char* error = _error;
	error = NULL;
	return error;
}

extern "C" {

const char *LWCGetAndResetError(void *codec)
{
	return static_cast<Codec *>(codec)->getAndResetError();
}

unsigned char *LWCEncrypt(void *codec, int page, unsigned char *data)
{
	return static_cast<Codec *>(codec)->encrypt(page, data);
}

void LWCDecrypt(void *codec, int page, unsigned char *data)
{
	static_cast<Codec *>(codec)->decrypt(page, data);
}

void LWCSetPageSize(void *codec, int pageSize)
{
	static_cast<Codec *>(codec)->setPageSize(pageSize);
}

void LWCFreeCodec(void *codec)
{
	delete static_cast<Codec *>(codec);
}

void *LWCCreateNewCodec(void *db)
{
	return new AesCodec(db);
}

void *LWCCloneCodec(void *codec)
{
	return new AesCodec(*static_cast<Codec *>(codec));
}

void LWCSetCodecEncryptionKey(void *codec, unsigned char *key, int keyLength)
{
	static_cast<Codec *>(codec)->setEncryptionKey(key, keyLength);
}

void *LWCGetDb(void *codec)
{
	return const_cast<void *>(static_cast<Codec *>(codec)->getDb());
}

}
