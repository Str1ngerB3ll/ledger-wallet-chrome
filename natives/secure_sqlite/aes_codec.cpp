/*
 *
 * AES SQLite Codec implementation
 * (C) 2014 Ledger
 * Based on botansqlite3 git@github.com:OlivierJG/botansqlite3.git
 *
 */

#include "aes_codec.h"
#include <iostream>

AesCodec::AesCodec(const void *db) : Codec(db)
{

}

AesCodec::AesCodec(const Codec& codec) : Codec(codec)
{

}

void AesCodec::setEncryptionKey(const unsigned char *encryptionKey, const int encryptionKeyLength)
{
	if (encryptionKeyLength != 32)
	{
		setError("Encryption key should be 32 bytes length.");
		return ;
	}
	Codec::setEncryptionKey(encryptionKey, encryptionKeyLength);
	AES_set_encrypt_key(encryptionKey, 256, &_encryptKey);
    AES_set_decrypt_key(encryptionKey, 256, &_decryptKey);
}

unsigned char *AesCodec::encrypt(int page, unsigned char *data)
{
	unsigned char iv[] = "asdfghjklpoiuytr";
	AES_cbc_encrypt(data, _page, getPageSize(), &_encryptKey, iv, AES_ENCRYPT);
	return _page;
}

void AesCodec::decrypt(int page, unsigned char *data)
{
	unsigned char iv[] = "asdfghjklpoiuytr";
	AES_cbc_encrypt(data, _page, sizeof(_page), &_decryptKey, iv, AES_DECRYPT);
	memcpy(data, _page, getPageSize());
}

int AesCodec::popError()
{
	return 1;
}
