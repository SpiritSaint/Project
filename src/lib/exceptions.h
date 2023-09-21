#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>


class PublicKeyNotFoundException : std::exception {};
class PrivateKeyNotFoundException : std::exception {};
class PublicKeyInvalidException : std::exception {};
class PrivateKeyInvalidException : std::exception {};

class EncryptionException : std::exception {};
class DecryptionException : std::exception {};


#endif //EXCEPTIONS_H
