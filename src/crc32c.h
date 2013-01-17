/**@author Merder Kim <hoxnox@gmail.com> 
 * @date 20130117 22:25:30*/

#include <stdint.h>
#include <stdlib.h>

#ifndef __CRC32C_MASKED_HPP__
#define __CRC32C_MASKED_HPP__

#ifdef __cplusplus
extern "C" {
#endif

uint32_t crc32c_masked(const char * data, size_t datasz);

#ifdef __cplusplus
}
#endif

#endif // __CRC32C_MASKED_HPP__

