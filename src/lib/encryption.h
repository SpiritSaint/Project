#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <iostream>
#include <memory>
#include <openssl/evp.h>
#include <openssl/pem.h>

#include "exceptions.h"
#include "configuration.h"

class encryption {
private:
    EVP_PKEY * _public_key = nullptr;
    EVP_PKEY * _private_key = nullptr;
    EVP_PKEY_CTX * _public_key_context = nullptr;
    EVP_PKEY_CTX * _private_key_context = nullptr;
    int block_size;
public:
    std::string hide(std::string & input);
    std::string show(std::string & input);
    encryption(std::shared_ptr<configuration> const & config);
    ~encryption();
};

#endif // ENCRYPTION_H
