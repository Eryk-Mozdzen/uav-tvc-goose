#ifndef NVM_H
#define NVM_H

#include <stddef.h>
#include <stdint.h>

void nvm_read(const uint32_t address, void *dest, const size_t len);
void nvm_write(const uint32_t address, const void *src, const size_t len);

#endif
