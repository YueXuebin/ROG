//! @file encrypt.h


#ifndef _ENCRYPT_H_
#define _ENCRYPT_H_

#include "types.h"

class Net_Packet;

class Encrypt
{
public:
    Encrypt() { }

    virtual ~Encrypt() { }

    virtual void SetPrivateKey(uint32_t key) = 0;

    virtual uint32_t GetPrivateKey() = 0;

    virtual void SetPublicKey(uint32_t key) = 0;

    virtual uint32_t GetPublicKey() = 0;

    virtual void EncodeByPubKey(Net_Packet* packet) = 0;        // server使用
    virtual void EncodeByPriKey(Net_Packet* packet) = 0;

    virtual void DecodeByPubKey(Net_Packet* packet) = 0;
    virtual void DecodeByPriKey(Net_Packet* packet) = 0;        // server使用


private:
    Encrypt(const Encrypt&);
    Encrypt& operator=(const Encrypt&);
};

#endif
