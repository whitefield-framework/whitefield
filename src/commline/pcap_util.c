#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <sys/time.h>

/*---------------------------------------------------------------------------*/
/* CITT CRC16 polynomial ^16 + ^12 + ^5 + 1 */
/*---------------------------------------------------------------------------*/
unsigned short crc16_add(unsigned char b, unsigned short acc)
{
    /*
   * acc  = (unsigned char)(acc >> 8) | (acc << 8);
   * acc ^= b;
   * acc ^= (unsigned char)(acc & 0xff) >> 4;
   * acc ^= (acc << 8) << 4;
   * acc ^= ((acc & 0xff) << 4) << 1;
   **/

    acc ^= b;
    acc = (acc >> 8) | (acc << 8);
    acc ^= (acc & 0xff00) << 4;
    acc ^= (acc >> 8) >> 4;
    acc ^= (acc & 0xff00) >> 5;
    return acc;
}

unsigned short crc16_data(const unsigned char *data, int len, unsigned short acc)
{
    int i;

    for (i = 0; i < len; ++i) {
        acc = crc16_add(*data, acc);
        ++data;
    }
    return acc;
}

#define WRITEINT(VAL)  \
    valint = (int)VAL; \
    fwrite((void *)&valint, sizeof(valint), 1, (FILE *)handle);

#define WRITESHORT(VAL)    \
    valshort = (short)VAL; \
    fwrite((void *)&valshort, sizeof(valshort), 1, (FILE *)handle);

void pcap_write(void *handle, const uint8_t *buf, int buflen)
{
    struct timeval tv;
    int            valint;
    unsigned short crc;

    if (!handle)
        return;

    crc = crc16_data((const unsigned char *)buf, buflen, 0);
    memcpy((void *)&buf[buflen], &crc, 2);
    buflen += 2;
    gettimeofday(&tv, NULL);
    WRITEINT(tv.tv_sec);
    WRITEINT(tv.tv_usec);
    WRITEINT(buflen);
    WRITEINT(buflen);
    fwrite(buf, buflen, 1, (FILE *)handle);
    fflush((FILE *)handle);
}

void *pcap_init(const char *fname)
{
    int   valint;
    short valshort;
    FILE *handle;

    handle = fopen(fname, "wb");
    if (!handle)
        return NULL;
    WRITEINT(0xa1b2c3d4);
    WRITESHORT(0x0002);
    WRITESHORT(0x0004);
    WRITEINT(0);
    WRITEINT(0);
    WRITEINT(4096);
    WRITEINT(195);
    return (void *)handle;
}

#if 0
int main(void)
{
  void *handle;
  uint8_t buf[128];
  handle = pcap_init("packet.pcap");
  if(handle) {
    pcap_write(handle, buf, sizeof(buf));
    fclose(handle);
  }
}
#endif
