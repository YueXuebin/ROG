//! @file encrypt.h


#ifndef _AF_ENCRYPT_H_
#define _AF_ENCRYPT_H_

class Net_Packet;

#include "encrypt.h"

class AfEncrypt : public Encrypt
{
public:
    AfEncrypt();

    virtual ~AfEncrypt();

    virtual void SetPrivateKey(uint32_t key);

    virtual uint32_t GetPrivateKey() { return m_private_key_dword; }

    virtual void SetPublicKey(uint32_t key);

    virtual uint32_t GetPublicKey() { return m_public_key_dword; }

    virtual void EncodeByPubKey(Net_Packet* packet);
    virtual void EncodeByPriKey(Net_Packet* packet);

    virtual void DecodeByPubKey(Net_Packet* packet);
    virtual void DecodeByPriKey(Net_Packet* packet);

private:
    void Encode(Net_Packet* packet, uint8 keys[4]);
    void Decode(Net_Packet* packet, uint8 keys[4]);

private:
    AfEncrypt(const AfEncrypt&);
    AfEncrypt& operator=(const AfEncrypt&);

    uint8   m_private_key_byte[4];
    uint32  m_private_key_dword;

    uint8   m_public_key_byte[4];
    uint32  m_public_key_dword;
};

#endif
