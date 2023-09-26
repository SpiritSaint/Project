#ifndef OPENBSD_H
#define OPENBSD_H

#include <cstdlib>     /* srand, rand */
#include <cstddef>
#include <cstring>
#include <ctime>

inline
void arc4random_buf(void *buf, size_t nbytes)
{

    for( size_t n = 0; n < nbytes; ++ n)
        ((char*)(buf))[n] = rand() %256;
}

inline
void arc4random_init(void)
{
    srand( (unsigned int) time(NULL));
}


#endif // OPENBSD_H