#include "encryption.h"

std::string encryption::hide(std::string & input) {
    _public_key_context = EVP_PKEY_CTX_new(_public_key, nullptr);

    if (!_public_key_context) {
        EVP_PKEY_CTX_free(_public_key_context);
    }

    std::vector<std::string> blocks;
    for (size_t i = 0; i < input.length(); i += block_size) {
        blocks.push_back(input.substr(i, block_size));
    }

    if (EVP_PKEY_encrypt_init(_public_key_context) <= 0 ) {
        throw EncryptionException();
    }

    if (EVP_PKEY_CTX_set_rsa_padding(_public_key_context, RSA_PKCS1_OAEP_PADDING) <= 0) {
        throw EncryptionException();
    }

    std::string result;

    for (const std::string& block : blocks) {
        std::size_t size;
        if (EVP_PKEY_encrypt(_public_key_context, nullptr, &size, reinterpret_cast<const unsigned char *>(block.data()), block.length()) <= 0) {
            EVP_PKEY_CTX_free(_public_key_context);
            throw EncryptionException();
        }

        unsigned char output[size];
        EVP_PKEY_encrypt(_public_key_context, output, &size, reinterpret_cast<const unsigned char *>(block.data()), block.length());

        result.append(std::string { reinterpret_cast<char*>(output), size });
        result.append("<EOL>");
    }
    EVP_PKEY_CTX_free(_public_key_context);

    return result;
}

std::string encryption::show(std::string & input) {
    _private_key_context = EVP_PKEY_CTX_new(_private_key, nullptr);
    if (!_private_key_context) {
        EVP_PKEY_CTX_free(_private_key_context);
    }

    if (EVP_PKEY_decrypt_init(_public_key_context) <= 0 ) {
        throw DecryptionException();
    }

    if (EVP_PKEY_CTX_set_rsa_padding(_private_key_context, RSA_PKCS1_OAEP_PADDING) <= 0) {
        throw DecryptionException();
    }

    std::istringstream ss(input);

    std::string result;
    std::vector<std::string> blocks;

    size_t position = 0;
    std::string delimiter = "<EOL>";
    while ((position = input.find(delimiter)) != std::string::npos) {
        std::string block = input.substr(0, position);
        input.erase(0, position + delimiter.length());

        std::size_t size;

        if (EVP_PKEY_decrypt(_private_key_context, nullptr, &size, reinterpret_cast<const unsigned char *>(block.data()), block.length()) <= 0) {
            EVP_PKEY_CTX_free(_private_key_context);
            throw DecryptionException();
        }

        unsigned char output[size];
        EVP_PKEY_decrypt(_private_key_context, output, &size, reinterpret_cast<const unsigned char *>(block.data()), block.length());

        result.append(std::string { reinterpret_cast<char*>(output), size });
    }

    EVP_PKEY_CTX_free(_private_key_context);

    return result;
}

encryption::encryption(std::shared_ptr<configuration> const& config) {
    FILE * public_key_file = fopen(config->_rsa._public_key.c_str(), "rb");
    if (!public_key_file) {
        throw PublicKeyNotFoundException();
    }

    _public_key = PEM_read_PUBKEY(public_key_file, nullptr, nullptr, nullptr);
    if (!_public_key) {
        throw PublicKeyInvalidException();
    }

    FILE * private_key_file = fopen(config->_rsa._private_key.c_str(), "rb");

    if (!private_key_file) {
        throw PrivateKeyNotFoundException();
    }

    _private_key = PEM_read_PrivateKey(private_key_file, nullptr, nullptr, nullptr);
    fclose(private_key_file);
    if (!_private_key) {
        throw PrivateKeyInvalidException();
    }

    block_size = (config->_rsa._size / 8) - 42;
}

encryption::~encryption() {
    EVP_PKEY_free(_public_key);
    EVP_PKEY_free(_private_key);
}
