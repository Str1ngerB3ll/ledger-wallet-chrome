/*
 *
 * AES SQLite Codec implementation
 * (C) 2014 Ledger
 * Based on botansqlite3 git@github.com:OlivierJG/botansqlite3.git
 *
 */

#ifndef _CODEC_H_
#define _CODEC_H_

extern "C" {
extern void JLOG(const char *format);
}

 class Codec
 {

 public:
 	Codec(const void *db);
 	Codec(const Codec& codec);
 	virtual void setEncryptionKey(const unsigned char *encryptionKey, const int encryptionKeyLength);
 	virtual unsigned char *encrypt(int page, unsigned char *data) = 0;
 	virtual void decrypt(int page, unsigned char *data) = 0;
 	virtual const char *getAndResetError();
 	virtual void setPageSize(int pageSize);
 	virtual ~Codec();

 	const void *getDb() const;
	const unsigned char *getEncryptionKey() const;
	const int getEncryptionKeyLength() const;

protected:
	const int getPageSize() const;

	void setError(const char *error);

private:
	const void *_db;
	unsigned char * _encryptionKey;
	int _encryptionKeyLength;
	int _pageSize;
	char *_error;

 };

#endif