
/*Utils*/
static inline void swap (int* x, int* y)
{
    *x ^= *y;
    *y ^= *x;
    *x ^= *y;
} 



/*******
* RC4
********/

/*Convert key string into an array of decimal ASCII identifiers*/
static ushort * convert_key(const char * key, unsigned len)
{
     unsigned short * ret = malloc(sizeof(int) * len);

     unsigned i; for ( i = 0; i < len; ++i )
     {
        ret[i] = key[i];
     }

     return ret;
}


/*RC4 Key-scheduling algorithm*/
static void _ksa(int S[256], const char* key)
{
    int i, j = 0;
    unsigned keylen = strlen(key);

    unsigned short * keynums = convert_key(key, keylen);

    for (i = 0; i < 256; ++i)
        S[i] = i;

    for (i = 0; i < 256; ++i)
    {
        j = (j + S[i] + keynums[i % keylen]) % 256;
        swap(&S[i], &S[j]);
    }

    free(keynums);
}


/*RC4 pseudo-random generation algorithm*/
static int _prga(int S[256])
{
    static int i = 0;
    static int j = 0;

    i = (i+1) % 256;
    j = (j+S[i]) % 256;

    swap(&S[i], &S[j]);

    return S[(S[i]+S[j]) % 256];    
}


/*STATUS -- NOT WORKING*/
static char * rc4(const char* message, const char* key, char* retStr, bool encrypt)
{

    size_t msglen = strlen(message);
    size_t retlen = encrypt ? (msglen*2) : (msglen/2);

    retStr = malloc(retlen+1);
    retStr[retlen] = '\0';

    int p_array[256] = {0};
    _ksa(p_array, key);

    unsigned i; for (i = 0; i < msglen; ++i)
    {
        if (!encrypt)
        {
            char * dst = retStr;
            char * end = retStr + retlen;

            unsigned hex;

            while (dst < end && sscanf(message, "%2x", &hex) == 1)
            {
                *dst++ = _prga(p_array) ^ hex;
                message += 2;
            }
        }
        else 
            sfprintf(&retStr[i*2], "%02X", message[i] ^ _prga(p_array));
    }

    return retStr;
}


extern char* rc4_encrypt(const char* message, const char* key, char* encrypted)
{
    encrypted = rc4(message, key, encrypted, true);
    return encrypted;
}

extern char* rc4_decrypt(const char* message, const char* key, char* decrypted)
{
    decrypted = rc4(message, key, decrypted, false);
    return decrypted;
}
