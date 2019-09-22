#include <string.h> // for memset, etc.
#include <stdio.h>  // for printf

#include "bu.h"

// NOTE: In this code "word" always refers to a uint32_t

// Copy dest = src
void bu_cpy(bigunsigned *dest, bigunsigned *src) {//done
  uint16_t cnt = src->used;
  dest->used = cnt;
  dest->base = 0;

  // reset upper 0s in dest
  memset(dest->digit, 0, sizeof(uint32_t)*BU_DIGITS-cnt);

  uint8_t i_dest = dest->used;
  uint8_t i_src = src->base;

  while (cnt-- > 0) {
    dest->digit[i_dest--] = src->digit[i_src--];
  }
}

// Set to 0
void bu_clear(bigunsigned *a_ptr) {//Done
  memset(a_ptr->digit, 0, sizeof(uint32_t)*BU_DIGITS);
  a_ptr->used = 0;
  a_ptr->base = 0;
}


// Shift in place a bigunsigned by cnt bits to the left
// Example: beef shifted by 4 results in beef0
void bu_shl(bigunsigned* a_ptr, bigunsigned* a_ptr2, uint16_t cnt){//nope, things got messed up
  //  a <<= cnt
  // Shift in place a big unsigned by cnt bits to the left
  // Example: beef shifted by 4 results in beef0
  // Produce a = b + c

  //if cnt = 0, there is no shift
  if(cnt == 0)
  {
    return;
  }
  uint16_t wrds = cnt >> 5; // # of whole words to shift
  uint16_t bits = cnt &0x1f;// number of bits in a word to shift

  uint32_t mask = 0xffffffff << bits;

  uint32_t carry = 0;

  for(uint16_t i = a_ptr->used; i < BU_DIGITS; i += 1)
  {
    a_ptr2->digit[(uint_t)(a_ptr2->base+1)] = 0;
  }//this removes unnecessary trash values

  //this might be a possible overflow situation
  a_ptr2->digit[a_ptr2->base-1] = 0;

  a_ptr2->used = BU_DIGITS<=(a_ptr2->used + wrds) ? BU_DIGITS : (a_ptr2->used + wrds);

  uint8_t s1 = a_ptr2->base + a_ptr2->used;
  uint8_t difference = (a_ptr->used + wrds) <= BU_DIGITS ? a_ptr->used : a_ptr->used -(a_ptr->used + wrds)%BU_DIGITS;
  uint8_t s2 = a_ptr->base + difference;

  //May be a faster mathod, carry issues may crop up though
  for(uint8_t i = 0; i<= difference; i += 1)
  {
    carry = (a_ptr->digit[s2 - i -1] & mask) >> (BU_BITS_PER_DIGIT - bits);
    a_ptr2->digit[s1 - i] = (a_ptr2->digit[s2 - i] << bits) |carry;
  }

  if(a_ptr2->digit[(uint8_t)(a_ptr2->used + a_ptr2->base)] != 0 && a_ptr2->used < BU_DIGITS)
  {
    a_ptr2->used += 1;
  }
}

void bu_shl_ip(bigunsigned* a_ptr, uint16_t cnt) {//done
// You implement. Avoid memory copying as much as possible.
//may need to revisit this. . .
  if(cnt == 0){
    return;
  }
  bu_shl(a_ptr, a_ptr, cnt);
}

void bu_shr(bigunsigned* a_ptr, bigunsigned* a_ptr2, uint16_t cnt){
  if(cnt == 0){
    return;
  }

  uint16_t wrds = cnt >> 5; // # of whole words to shift
  uint16_t bits = cnt &0x1f;// number of bits in a word to shift

  uint32_t mask = 0xffffffff << (BU_BITS_PER_DIGIT - bits);

  uint32_t carry = 0;
  uint8_t ind = 0;

  //shifting more than is used == 0
  if(wrds >= a_ptr2->used)
  {
    bu_clear(a_ptr);
    return;
  }


  for(uint16_t i = a_ptr->used; i < BU_DIGITS; i += 1)
  {
    a_ptr2->digit[(uint_t)(a_ptr2->base+1)] = 0; //I think this can be the same as bu_shl
  }//this removes unnecessary trash values

  a_ptr2->base = a_ptr->base;

  //May need to revise for wrds==very large
  for (uint8_t i = 0; i < wrds; i += 1) {
    a_ptr2->digit[a_ptr2->base] = 0;
    a_ptr2->base = a_ptr2->base + 1;
    a_ptr2->used -= 1;
  }

  for(uint8_t i = 0; i < a_ptr2->used; i += 1)
  {
    ind = (a_ptr2->base + 1) % BU_DIGITS;
    carry = ((a_ptr2->digit[(ind+1)] & mask) << (BU_BITS_PER_DIGIT - bits));
    a_ptr2->digit[ind] = (a_ptr->digit[index] >> bits) | carry;
  }

  if(a_ptr2->used > 0)
  {
    if(a_ptr2->digit[(a_ptr2->used + a_ptr2->base - 1) % BU_DIGITS] == 0)
    {
      a_ptr2->used -=1;
    }
  }
}


void bu_shr_ip(bigunsigned* a_ptr, uint16_t cnt){
  if(cnt == 0){
    return;
  }
  bu_shr_ip(a_ptr, a_ptr, cnt);
}


void bu_add(bigunsigned *a_ptr, bigunsigned *b_ptr, bigunsigned *c_ptr) {//done
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


void bu_add_ip(bigunsigned *a_ptr, bigunsigned *b_ptr){//come back to this
  uint64_t carry = 0;
  uint16_t min_used = a_ptr->used >= b_ptr->used ? a_ptr->used : b_ptr->used;
  //uint64_t nxt;
  //uint16_t cnt = 0;
  //uint8_t  b_dig = b_ptr->base;
  //uint8_t  a_dig = b_ptr->base;
  //uint8_t  a_dig = 0;
  a_pnt->digit[min_used + 1] = 0;
  a_ptr->used = min_used;

  for(uint16_t i = 0; i < min_used; i++)
  {
    carry = (carry>>32) + (uint64_t)(a_ptr->digit[(uint8_t)(i + a_ptr->base)])
            + (uint64_t)(b_ptr->digit[(uint8_t)(i + b_ptr->base)]);
    a_ptr->digit[(uint8_t)(i + a_ptr->base)] = (uint32_t)carry;
  }

  if(a_ptr->used < BU_DIGITS && a_ptr->digit[(uint8_t)(a_ptr->base + a_ptr->used)] != 0){
    a_ptr->used += 1;
  }
}


void bu_mul_digit(bigunsigned *a_ptr, bigunsigned *b_ptr, uint32_t d){//I skipped a thing
  uint64_t product;
  //I use a an array for the carries to avoid long carry propagation
  bigunsigned carry;
  bu_clear(&carry);

  carry.used = b_ptr->used + 1;//there is a chance this isn't relevant
  a_ptr->used = b_ptr->used;

  for(uint32_t i = 0; i < b_ptr->used; i++)
  {
  product = (uint64_t)(d)*(uint64_t)(b_ptr->digit[b_ptr->base + i])//do I need a mask here?
  a_ptr->digit[a_ptr->base + i] = (uint32_t)product;//mask?
  carry.digit[a_ptr->base + i] = (uint32_t)(product >> 32);
  }

  while(a_ptr->used < BU_DIGITS && a_ptr->digit[a_ptr->used + a_ptr->base])//mask?
  {
    a_ptr->used++;
  }

  bu_add_ip(a_ptr, &carry);

//weird thing?
}


void bu_mul_digit_ip(bigunsigned *a_ptr, uint32_t d){
  bu_mul_digit(a_ptr, a_ptr, d);//this might cause an assignment issue, but should be okay
}


//may want to consider addressing sized of b & c for a different method. . .
void bu_mul(bigunsigned *a_ptr, bigunsigned *b_ptr, bigunsigned *c_ptr){
  bigunsigned carry;
  //bigunsigned temp;
  uint16_t shift = 0;
  bu_clear(&carry);
  bu_clear(a_ptr);

  for(uint32_t i = 0; i < c_ptr->used; i++){
    bu_mul_digit(&carry, b_ptr, c_ptr->digit[(uint8_t)(b_ptr->base+i)]);
    shift = i << 5;
    bu_shl_ip(&carry, shift);
    bu_add_ip(a_ptr, &carry);//there is probably a better place to do this, but risks propogating carries in the carry
  }
}


void bu_mul_ip(bigunsigned *a_ptr, bigunsigned *b_ptr){
  bu_mul(a_ptr, a_ptr, b_ptr);
}


void bu_sqr(bigunsigned *a_ptr, bigunsigned *b_ptr){
  bu_mul(a_ptr, b_ptr, b_ptr);
}


void bu_sqr_ip(bigunsigned *a_ptr){
  bu_mul_ip(a_ptr, a_ptr);
}


// return the length in bits (should always be less or equal to 32*a->used)
uint16_t bu_len(bigunsigned *a_ptr) {//done
  uint16_t res = a_ptr->used<<5;
  uint32_t bit_mask = 0x80000000;
  uint32_t last_wrd = a_ptr->digit[a_ptr->base+a_ptr->used-1];

  while (bit_mask && !(last_wrd&bit_mask)) {
    bit_mask >>= 1;
    res--;
  }
  return res;
}

// Read from a string of hex digits
//
// TODO: This is wrong. See the test main.c
//       Modify to resolve 'endian' conflict.
//       Also modify to permit strings to include whitespace
//        that will be ignored. For example, "DEAD BEEF" should
//        be legal input resulting in the value 0xDEADBEEF.

void bu_readhex(bigunsigned * a_ptr, char *s) {//probably okay
  bu_clear(a_ptr);

  unsigned pos = 0;
  char *s_ptr = s;

  //fixed endian conflict here
  int ind = (strlen(s) - 1);
  while (ind > 0 && pos < BU_MAX_HEX) {
    if(!isspace(s[ind])){
      a_ptr->digit[pos>>3] |= (((uint32_t)hex2bin(*s_ptr)) << ((pos & 0x7)<<2));
      pos++;
    }
    ind--;
    //s_ptr++;
  }

  a_ptr->used = (pos>>3) + ((pos&0x7)!=0);
}

//
void bu_dbg_printf(bigunsigned *a_ptr) {
  printf("Used %x\n", a_ptr->used);
  printf("Base %x\n", a_ptr->base);
  uint16_t i = a_ptr->used;
  printf("Digits: ");
  while (i-- > 0)
    printf("%8x ", a_ptr->digit[a_ptr->base+i]);
  printf("Length: %x\n", bu_len(a_ptr));
}
