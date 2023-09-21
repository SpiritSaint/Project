#include "encryption.h"

std::string encryption::hide(std::string & input) {
    _public_key_context = EVP_PKEY_CTX_new(_public_key, nullptr);

    if (!_public_key_context) {
        EVP_PKEY_CTX_free(_public_key_context);
    }

    std::size_t size;
    if (EVP_PKEY_encrypt_init(_public_key_context) <= 0 ||
        EVP_PKEY_encrypt(_public_key_context, nullptr, &size, reinterpret_cast<const unsigned char *>(input.data()), input.length()) <= 0) {
        EVP_PKEY_CTX_free(_public_key_context);
        throw EncryptionException();
    }

    unsigned char output[size];
    EVP_PKEY_encrypt(_public_key_context, output, &size, reinterpret_cast<const unsigned char *>(input.data()), input.length());
    EVP_PKEY_CTX_free(_public_key_context);
    return std::string { reinterpret_cast<char*>(output), size };
}

std::string encryption::show(std::string & input) {
    _private_key_context = EVP_PKEY_CTX_new(_private_key, nullptr);
    if (!_private_key_context) {
        EVP_PKEY_CTX_free(_private_key_context);
    }

    std::size_t size;
    if (EVP_PKEY_decrypt_init(_private_key_context) <= 0 ||
        EVP_PKEY_decrypt(_private_key_context, nullptr, &size, reinterpret_cast<const unsigned char *>(input.data()), input.length()) <= 0) {
        EVP_PKEY_CTX_free(_private_key_context);
        throw DecryptionException();
    }

    unsigned char output[size];
    EVP_PKEY_decrypt(_private_key_context, output, &size, reinterpret_cast<const unsigned char *>(input.data()), input.length());
    EVP_PKEY_CTX_free(_private_key_context);
    return std::string { reinterpret_cast<char*>(output), size };
}

encryption::encryption(std::shared_ptr<configuration> const& config) {
    FILE * public_key_file = fopen(config->_keys._public.c_str(), "rb");
    if (!public_key_file) {
        throw PublicKeyNotFoundException();
    }

    _public_key = PEM_read_PUBKEY(public_key_file, nullptr, nullptr, nullptr);
    if (!_public_key) {
        throw PublicKeyInvalidException();
    }

    FILE * private_key_file = fopen(config->_keys._private.c_str(), "rb");

    if (!private_key_file) {
        throw PrivateKeyNotFoundException();
    }

    _private_key = PEM_read_PrivateKey(private_key_file, nullptr, nullptr, nullptr);
    fclose(private_key_file);
    if (!_private_key) {
        throw PrivateKeyInvalidException();
    }
}

encryption::~encryption() {
    EVP_PKEY_free(_public_key);
    EVP_PKEY_free(_private_key);
}
