#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#define BU_DIGITS 256
#define BU_MAX_HEX (BU_DIGITS<<3)
#define BU_BITS (BU_DIGITS<<5)
#define BU_BITS_PER_DIGIT 32


typedef struct {
  uint32_t digit[BU_DIGITS];
  uint16_t used;  // number of non-leading zero digits
  uint8_t base;  // start from this index
} bigunsigned;



// Convert single ascii character hex digit to numerical value
// Assumes given a true hex digit, can be lower or upper case
// 
uint8_t hex2bin(char c) {
  if ('0' <= c && c <= '9')
    return c - '0';
  else
    return tolower(c)-'a' + 0xa;
}

// Set big unsigned to 0
void bu_clear(bigunsigned *a_ptr) {
  memset(a_ptr->digit, 0, sizeof(uint32_t)*BU_DIGITS);
  a_ptr->used = 0;
  a_ptr->base = 0;
}

// Shift in place a big unsigned by cnt bits to the left
// Example: beef shifted by 4 results in beef0
void bu_shl_ip(bigunsigned* a_ptr, uint16_t cnt) {
  uint16_t wrds = cnt >> 5; // # of whole words to shift
  uint16_t bits = cnt &0x1f;// number of bits in a word to shift

  uint32_t mask = 0xffffffff << bits;

  // You implement. Avoid memory copying a much as possible.
}

// Produce a = b + c
void bu_add(bigunsigned *a_ptr, bigunsigned *b_ptr, bigunsigned *c_ptr) {
  uint8_t carry = 0;
  uint64_t nxt;
  uint16_t cnt = 0;
  uint16_t min_used = b_ptr->used <= c_ptr->used 
                      ? b_ptr->used : c_ptr->used;
  uint8_t  b_dig = b_ptr->base;
  uint8_t  c_dig = c_ptr->base;
  uint8_t  a_dig = 0;

  while (cnt < min_used) {
    nxt = ((uint64_t)b_ptr->digit[b_dig++]) 
          + (uint64_t)(c_ptr->digit[c_dig++]) + carry;
    carry = 0 != (nxt&0x100000000);
    a_ptr->digit[a_dig++] = (uint32_t)nxt;
    cnt++;
  }

  while (cnt < b_ptr->used && carry) {
    nxt = ((uint64_t)b_ptr->digit[b_dig++]) + carry;
    carry = 0 != (nxt&0x100000000);
    a_ptr->digit[a_dig++] = (uint32_t)nxt;
    cnt++;
  }

  while (cnt < b_ptr->used) {
    a_ptr->digit[a_dig++] = b_ptr->digit[b_dig++];
    cnt++;
  }  

  while (cnt < c_ptr->used && carry) {
    nxt = ((uint64_t)c_ptr->digit[c_dig++]) + carry;
    carry = 0 != (nxt&0x100000000);
    a_ptr->digit[a_dig++] = (uint32_t)nxt;
    cnt++;
  }

  while (cnt < c_ptr->used) {
    a_ptr->digit[a_dig++] = c_ptr->digit[c_dig++];
    cnt++;
  }

  while (cnt < BU_DIGITS && carry) {
    a_ptr->digit[a_dig++] = 1;
    carry = 0;
    cnt++;
  }

  a_ptr->base = 0;
  a_ptr->used = cnt;
}
void bu_cpy(bigunsigned *dest, bigunsigned *src) {
  uint16_t cnt = src->used;
  dest->used = cnt;
  dest->base = 0;
  memset(dest->digit, 0, sizeof(uint32_t)*BU_DIGITS-cnt);

  uint8_t i_dest = 0;
  uint8_t i_src = src->base;

  while (cnt-- > 0) {
    dest->digit[i_dest--] = src->digit[i_src--];
  }

}

// return the length in bits (should always be less or equal to 32*a->used)
uint16_t bu_len(bigunsigned *a_ptr) {
  uint16_t res = a_ptr->used<<5;
  uint32_t bit_mask = 0x80000000;
  uint32_t last_wrd = a_ptr->digit[a_ptr->base+a_ptr->used-1];

  printf("Last %x", last_wrd);
  while (bit_mask && (last_wrd&bit_mask)==0) {
    bit_mask >>= 1;
    res--;
  }
  return res;
}

void bu_readhex(bigunsigned * a_ptr, char *s) {
  bu_clear(a_ptr);

  unsigned pos = 0;
  char *s_ptr = s;
  while (*s_ptr && pos < BU_MAX_HEX) {
    a_ptr->digit[pos>>3] |= (((uint32_t)hex2bin(*s_ptr)) << ((pos & 0x7)<<2));
    pos++;
    s_ptr++;
  }
  a_ptr->used = (pos>>3) + ((pos&0x7)!=0);
}

void bu_dbg_printf(bigunsigned *a_ptr) {
  printf("Used %x\n", a_ptr->used);
  uint16_t i = a_ptr->used;
  printf("Digits: ");
  while (i-- > 0)
    printf("%8x ", a_ptr->digit[a_ptr->base+i]);
  printf("\n");
}

int main() {
  bigunsigned a;
  char s[BU_MAX_HEX+1];

  bu_readhex(&a,"CAB51AFFBEEF");
  bu_dbg_printf(&a);
  printf("\n\n%d\n", bu_len(&a));

  return 0;
}