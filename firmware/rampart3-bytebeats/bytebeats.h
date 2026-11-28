#pragma once

// Globals from rampart3-bytebeats.ino
extern volatile long t;
extern volatile int a, b, c, result;
extern volatile int currentBank;
extern volatile Bank banks[4];
extern volatile int enc_offset;
extern volatile int aMax, aMin, bMax, bMin, cMax, cMin;
extern uint16_t Acc[3];
extern unsigned long freq[3];
extern unsigned int tune;

#include "nyblybyte.h"

// Prototypes for functions defined later in this file
void rythmical(int prog);
void melodious(int prog);
void noisy(int prog);
void nybly(int prog);

void bytebeats_process() {
  switch (currentBank) {
    case 0:
      rythmical(banks[0].currentProg);
      break;
    case 1:
      melodious(banks[1].currentProg);
      break;
    case 2:
      noisy(banks[2].currentProg);
      break;
    case 3:
      nybly(banks[3].currentProg);
      break;
  }

  if (enc_offset != 0) {
    t += enc_offset;
  }
}

// for wavetable voice

const byte sine256[] PROGMEM = { // sine wavetable
  0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 3, 4, 5, 5, 6, 7, 9, 10, 11, 12, 14, 15, 17, 18, 20, 21, 23, 25, 27, 29, 31, 33, 35, 37, 40, 42, 44, 47, 49, 52, 54, 57, 59, 62, 65, 67, 70, 73, 76, 79, 82, 85, 88, 90, 93, 97, 100, 103, 106, 109, 112, 115, 118, 121, 124, 128,
  128, 131, 134, 137, 140, 143, 146, 149, 152, 155, 158, 162, 165, 167, 170, 173, 176, 179, 182, 185, 188, 190, 193, 196, 198, 201, 203, 206, 208, 211, 213, 215, 218, 220, 222, 224, 226, 228, 230, 232, 234, 235, 237, 238, 240, 241, 243, 244, 245, 246,
  248, 249, 250, 250, 251, 252, 253, 253, 254, 254, 254, 255, 255, 255, 255, 255, 255, 255, 254, 254, 254, 253, 253, 252, 251, 250, 250, 249, 248, 246, 245, 244, 243, 241, 240, 238, 237, 235, 234, 232, 230, 228, 226, 224, 222, 220, 218, 215, 213, 211,
  208, 206, 203, 201, 198, 196, 193, 190, 188, 185, 182, 179, 176, 173, 170, 167, 165, 162, 158, 155, 152, 149, 146, 143, 140, 137, 134, 131, 128, 124, 121, 118, 115, 112, 109, 106, 103, 100, 97, 93, 90, 88, 85, 82, 79, 76, 73, 70, 67, 65,
  62, 59, 57, 54, 52, 49, 47, 44, 42, 40, 37, 35, 33, 31, 29, 27, 25, 23, 21, 20, 18, 17, 15, 14, 12, 11, 10, 9, 7, 6, 5, 5, 4, 3, 2, 2, 1, 1, 1, 0
};
//played like so
//

/* set parameter limits */
inline void setLimits(int a1, int a2, int b1, int b2, int c1, int c2, int d, int e) {
  aMax = a2; aMin = a1;
  bMax = b2; bMin = b1;
  cMax = c2; cMin = c1;
  srateMin = d; srateMax = e; 
}

/* pentatonic

    A(t) = 1
    C(t) = 32/27
    D(t) = 4/3
    E(t) = 3/2
    G(t) = 16/9
*/
/*
   I, Mark Washeim do not assert any Copyright for the result formulas. They belong to the public domain
   when no other copyright has been asserted.

   Various bytebeat formulas. some from my old midi/arduboy sketches, nyblybyte has a lot of references
   https://github.com/erlehmann/algorithmic-symphonies and
   https://raw.githubusercontent.com/schollz/nyblcore/main/bytebeat/bytebeat.ino
*/

// return t*(((t>>11)^((t>>11)-1)^7)%11)
// t*((0xDEADBEEF>>((t>>11)&37)&7)*(1/4)*(0xFEFEED>>((t>>13)&42)&5))
// birthday fades in and oerut
//       result = ~t>>11 ^ a | ( (t>>a)&(t<<b) * 1968|(t>>c)%1976 ) / ( t%(a+b+c) | t%2012)  ;
// from here nybly see the header file for information
// nice 8, 17, ( bb28 | t << c), bb32 << c (or |), 34 great, 37 ( also  << c, | c), b39 ( | t>>c)
// normal bb0,bb4 bb11 ( bb19 bb21 | t << c) (bb22 + c) bb23 + c, bb33 << c, bb35 << c,
// 8192 slow bb5 bb7 bb9 (bb13 | c) bb14 bb16 (bb29 & t>>c), bb30 << c, bb36 | t <<  c;

// first prog, bank 2
int z = 0;
int y = 0;
int f = 0;

void rythmical(int pb1) {

  switch (pb1) {
    case 1:  // FINE
      setLimits(6, 16, 1, 16, 1, 9, 2000, 3000 ); // aMin, aMax, etc
      //result = ( ~t >>7? ~t>>3:~t>>5) * ( (127 & t * ( b & t >> 9) ) < (254 & t * ( 2 + ( c & t >> a ) ) ) );
      result = t >> c ^ t & 1 | t + (t ^ t >> 13) - t * ( (t >> 5 ? b : a) & t >> ( 8 - ( a >> 1 )  ) ) * 24;
      // ( t%8 ? ~t >> 9 & b: ~t >>11 & c )
      break;
    case 2: // FINE 
      setLimits(1, 16, 1, 8, 1, 16, 2000, 3000 );
      result =  t >> 4 ^ t & c | t + (t ^ t >> 8) - t * ((t >> 3 ? b : a) & t >> ( 5 - ( b >> 1 )  ) ) * 24;
      // ^ t << 1 & (a & 12 ? t >> 4 : t >> 10)
      break;
    case 3: // BORKED move to noise
      setLimits(1, 32, 1, 32, 1, 32, 2000, 3000);
      result = t >> (2 * a) ^ t & c | t + (t ^ t >> 21) - t * ((t % 8 ? b : a) & t >> ( 8 - ( b >> 5 )  ) ) * 24;
      break;

    case 4: // ADJUST SLOW move to end of noise
      setLimits(8, 32, 8, 32, 1, 8, 2000, 3000);
      //SRATE = 7192;
      //if (t > 65536) t = -65536;
      result = ((t % 32 ? ~t >> 2 : t >> 3) & t + (t >> a) + ( ~ (t ^ b % 2 | t >> c) )  ) * 12;

      //result=  ((t%4 ? c : b) & ((t<<1)^-(t>>a&1) &  ( 127 << (t+ b) | (t >> c)));
      break;

    case 5:  // BORKED
      // always sirens noisy based on https://www.pouet.net/topic.php?post=388938
      setLimits(1, 16, 1, 16, 1, 16, 2000, 4000); // aMin, aMax, etc
      //result =  (t >> 3 & a) + (t * ( 0xC4 + a >> ( t >> 3 % b) & (13 + c) ) | t >> c ) + (int)cos( t * 0xC4 + b);
      result = t ^ ( t >> c) - t | t >> a * t<<1 & (t&3 ? t >> b: t >> 13 ) ^ ( (t >> 3 ? 8 : 3 ) & t >> 128) * 24;
      break;

    case 6:  // FINE  move to noise
      // poetaster the devil.  FINE
      // pulses and mana, mana.three controls can cross conflict or harmonize nice. turn c up first
      setLimits(1, 16, 1, 16, 1, 16, 2000, 4000);
      result = t *  t << 1 & (t & 7 ? t >> 3 : t >> c) ^ ((t >> 7 ? 2 : b) & t >> (c + a)) | t + ((t ^ t >> 13)) | a * t >> b ^ t & c * 24;
      break;
    case 7:  // FINE
      // poetaster, noisy bassline , arps with tight snare  ... evolves. needs work?
      setLimits(1, 12, 1, 12, 1, 12, 2000, 4000);
      result = t * ((t >> 7 ? a : c ) & t >> a ) ^ t << 1 & (t & 7 ? t >> 5 : t >> c) - b * t >> 3 ^ t & (42 - b) * 24;
      break;
    case 8: // FINE only works on a few speed conditions (3000, 3048)
      // started with vizmut paper pp. 6 https://arxiv.org/pdf/1112.1368.pdf // modified to keep from crashing :) divisions replaced with multiplication :)
      setLimits(8, 12, 1, 12, 1, 12, 2912, 3056);
      result = t >> 4 |  t & (  ( t >> c ) / ( t >> b - ( t >> a ) & - t >> 7 - ( t >> 15 ) ) ) * 24;
      //       t >>4  | t &  (  ( t >> 5 ) / ( t >> 7 − ( t >> 15 ) & − t >> 7 − ( t >> 15 ) ) )
      break;
    case 9:  // CrAZy to noise
      // variation t+(t&1)+(t>>5)*(t>>1)/1|t>>4|t>>8  nice bass drones with whirring open hats? or damped cymbals https://dollchan.net/btb/res/3.html#258
      setLimits(3, 10, 1, 8, 5, 12, 2000, 4000);
      result = ( t + ( t & 1) + ( t >> a ) * ( t >> 1 ) / 1 | t >> b | t >> c ) * 12;
      break;
    case 10: // FINE
      // scratch percussion on th extreme (try * c) // FINE
      setLimits(1, 32, 1, 32, 1, 12, 2000, 4000);
      bb37_set(a, b);
      result =  ( bb37() & t >> c ) * 128;
      // too quiet (byte & t>>c caps at ~255) - louder: result = (bb37() & t >> c) * 128;
      break;
    case 11: // FINE
      // 5824 hz is nice. quaks like a duck // FINE
      setLimits(1, 16, 1, 16, 0, 32, 2000, 4000);
      bb22_set(a, b);
      result =  ( bb22() - c ) * 128 ;
      break;
    case 12: //  FINE sort of
      // Nightmachines https://forum.aemodular.com/post/235 ( t*4|t|t>>3&t+t/4&t*12|t*8>>10|t/20&t+140)&t>>4
      setLimits(3, 8, 8, 16, 20, 30, 2000, 4000);
      result  = ( (t * 4 | t | t >> a & t + t / 4 & t * 12 | t * 8 >> b | t / c & t + 140) & t >> 4 ) * 128; 
      // *128: &t>>4 caps at ~4095 (~6% full scale)
      break;
    case 13: // BORKED slow
      // droid http://www.pouet.net/topic.php?which=8357&page=10#c388999
      setLimits(3, 8, 3, 8, 3, 8, 2000, 4000);
      result = t >> a & 1 ? t >> b : -t >> c;
      enc_offset = 4; // set speed :)
      break;
    case 14:  // FINE
      setLimits(2, 8, 2, 8, 2, 8, 2000, 3000);
      result = ( ( t % 16 ? t >> a & t >> 7 : t >> 11 & t >> b ) * ( t % 8 ? ~t >> 9 & b : ~t >> 11 & c ) & 64 ) * 128; 
      // was &64 only (0/64) - *128 -> 0/8192; still a binary gate & silent at some settings
      break;
    case 15:  // FINE 
      // https://forum.arduino.cc/t/one-line-algorithmic-music/73409
      // (t*(4|t>>13&3)>>(~t>>11&1)&128|t*(t>>11&t>>13)*(~t>>9&3)&127)^(t&4096?(t*(t^t%255)|t>>4)>>1:t>>3|(t&8192?t<<2:t))
      setLimits(1, 16, 1, 16, 1, 8, 2000, 3000);
      result =  ( t & (a * 16) ? ( t * ( t ^ t % (b * 8) ) | t >> a ) >> 31 : t >> b | ( t & (c * 32) ? t << c : t ) )   ;
      break;
    case 16: // FINE 
      // started with vizmut paper pp. 6 https://arxiv.org/pdf/1112.1368.pdf
      // was (slow ~0.12Hz drone + harsh broadband, reads "thin/barely there"; the / term could divide by zero):
      setLimits(1, 16, 1, 8, 1, 8, 3000, 5000);
      result = t >> c | t | (( t >> 5 ) * ( t % 2 ? t >> b / 2 : t >> b)) | ( t % 3 ? t >> a / 3 : t >> a );
      break;
    case 17: // nice
      setLimits(1, 128, 1, 128, 64, 128, 2000, 4000);
      bb34_set(a, b);
      result = bb34()  * c ;
      // was bb34()*c: bb34() is a byte(0-255) capped ~8160 -> centered + boosted to full scale
      break;
    case 18: //FINE
      // ola http://countercomplex.blogspot.com/2011/10/algorithmic-symphonies-from-one-line-of.html?showComment=1318368575135#c8615755234789420078
      //2*(1-(t+10>>(t>>9&t>>14)&t>>4&-2))*((t>>10^t+(t>>6&127)>>10)&1)*32+128
      setLimits(4, 16, 4, 16, 4, 12, 2000, 4000);
      //result = 2 * (1 - (t + 10 >> (t >> a & t >> b)&t >> c & -2)) * ((t >> 10 ^ t + (t >> 6 & 127) >> 10) & 1) * 32 + 128;
      result = 2 * (1 - (t + 10 >> (t >> a & t >> b)&t >> c & -2)) * ((t >> 10 ^ t + (t >> 6 & 127) >> 10) & 1) * 8192 + 4096;
      enc_offset = 2;
      break;
    case 19: //FINE move to noise
      // https://www.pouet.net/topic.php?post=589098
      setLimits(1, 11, 1, 11, 1, 11, 2000, 4000);
      result = t * (0xC298C298C298 >> (t >> a)&t >> b)&t >> c;
      break;
    case 20:  // FINE
      // FIX rythm straight rythmic, great range! // tejeez 2011-10-05 #countercomplex
      setLimits(1, 16, 1, 16, 0, 16, 2000, 4000);
      result = ( ~t >> 2) * ((127 & t * (b & t >> 10)) < (245 & t * (2 + (c & t >> a))));
      break;
    case 21: // FINE
      //  FIX rythm? Street Surfer by skurk, raer (2011-09-30) https://www.pouet.net/topic.php?which=8357&page=4#c388479
      setLimits(1, 16, 1, 16, 0, 16, 2000, 4000);
      result  = t & (4096) ? t / 2 * (t ^ t % (a << 1)) | t >> 5 : t / (a << 1) | (t & (b << 7) ? 4 * t : t) * 24;
      enc_offset = 2;
      break;
    case 22: // WELL 
      //https://www.pouet.net/topic.php?post=587236 ((t*((t>>12)^((t>>12)-1)))&(t>>5)^(t>>6))+t+128
      setLimits(1, 16, 1, 16, 12, 24, 2000, 4000);
      result = ((t * ((t >> 12) ^ ((t >> c) - a))) & (t >> 5) ^ (t >> 6)) * b - (2 * a); //+ t + 128;
      break;
    case 23: // BORKED
      // http://viznut.fi/demos/unix/bytebeat_formulas.txt
      setLimits(1, 8, 1, 16, 16, 24, 2000, 4000);
      result  = t * ((t >> a) + 20) * t >> 14 * t >> b >> t * c * 24;
      //enc_offset = 1;
      break;
    case 24: //FINE noise
      // RYTHM !https://www.pouet.net/topic.php?post=587236 (115|t)* (256 - (t>>(9 - 2*((t>>14)%2) )) ) * ((256-(t>>4)%256))/256
      setLimits(1, 12, 8, 16, 1, 11, 2000, 4000);
      result = (115|t)* (256 - (t>>(a - 2*((t>>b)%2) )) ) * ((256-(t>>c)%256))/256;
      //  t * (0xC298C298C298 >> (t >> a)&t >> b)&t >> c * 24;
      break;
    case 26:  // Nice counter rythm
      // https://www.reddit.com/r/bytebeat/comments/1gwe43y/the_new_unnamed_bytebeat_by_george_on_blogspot/
      setLimits(1, 8, 1, 8, 1, 8, 2000, 4000);
      result = (t * a) | (t ^ t >> b * (t >> c)) * 24 ;
      break;
    case 25: // FINE
      // drums where did this come from? THIS must come at the end of the list.
      setLimits(0, 36, 0, 16, 10, 20, 2000, 4000);
      // a 18, b 6, c 10
      int u = 0;
      // quiet`
      //result = ((((u = t & 0xfff) & 0 + ((u + 1 << (a + (t >> c & 1 * b))) / u) & 255) / (u >> 8)) & 240 - 128) * 3;
      result = ((((u = t & 0xfff) & 0 + ((u + 1 << (a + (t >> c & 1 * b))) / u) & 255) / (u >> 8)) & 240 - 128) * 256;
      enc_offset = 2;
      break;
      // (t>>(t>>13&31)&128)+((t&t>>12)*t>>12)+3e5/(t%16384)
  }
}

// https://forum.arduino.cc/t/one-line-algorithmic-music/73409
//      // (t*(4|t>>13&3)>>(~t>>11&1)&128|t*(t>>11&t>>13)*(~t>>9&3)&127)^(t&4096?(t*(t^t%255)|t>>4)>>1:t>>3|(t&8192?t<<2:t))
// https://www.pouet.net/topic.php?post=394926
//int z = 0;
//      result = (z=(t>>(1+(t>>13&1))) * (t>>15 & 0xfd) * (t >> 11 & 0xdf) * (t >> 7 & 0xcc))*0+((t<<1))&z | ((t<<2))&z;

void melodious(int pb2) {

  switch (pb2) {
    case 1:  // FINE 
      // isislovecruft https://www.noisebridge.net/wiki/Bytebeat
      //(~t>>2)*(2+(42&2*t*(7&t>>10))<(24&t*((3&t>>14)+2)))
      setLimits(36, 72, 16, 64, 1, 16, 2000, 4000);
      result = (~t >> 2) * (2 + (a & 2 * t * (7 & t >> 10)) < (b & t * ((3 & t >> c) + 2)));
      break;
    case 2: // FINE
      // wiretapped https://www.noisebridge.net/wiki/Bytebeat (1+(t>>10)%7)*(t*(1+(t>>13)%4)%(24+9*(t>>14)%8)&16)*10
      setLimits(4, 16, 8, 32, 1, 64, 2000, 5000);
      result = (1 + (t >> 10) % 7) * (t * (1 + (t >> a) % 4) % (b + 9 * (t >> c) % 8)) * (c * 5);
      break;
    case 3: // FINE
      // wiretapped, too u11*(t*(1|(t>>10-(t>>17)%4)%8)&(8<<(t>>13)%4)*(1|(t>>15)%8))*(t>>10)
      setLimits(1, 10, 1, 8, 1, 16, 2000, 5000);
      result = 11 * (t * (1 | (t >> a - (t >> 17) % b) % c) & (8 << (t >> 13) % b) * (1 | (t >> 15) % 8)) * (t >> 10);
      break;
    case 4:  // BORKED
      //VISMUT  https://forum.arduino.cc/t/one-line-algorithmic-music/73409
      // (t * (4 |t >> 13 & 3 ) >> ( ~t >> 11 & 1)  & 128  | t * ( t >> 11 & t >> 13 ) * ( ~t>> 9 & 3 ) & 127 ) ^ ( t & 4096 ? ( t * ( t ^ t % 255 ) | t >> 4 ) >> 1 : t >> 3 |( t &8192 ? t << 2 : t ) )
      setLimits(1, 13, 1, 15, 1, 11, 2000, 4000);
      result = (t * (4 | t >> 13 & b ) >> ( ~t >> 11 & 1 ) & 128 | t * ( t >> a & t >> 13 ) * ( ~t >> c & 3 ) & 127 ) ^ ( t & 4096 ? ( t * ( t ^ t % 255 ) | t >> 4 ) >> 1 : t >> 3 | ( t & 8192 ? t << 2 : t ) );
      break;
    case 5: //FINE move to rythmic
      // glitch https://www.reddit.com/r/bytebeat/comments/ufvuio/electricity/ 12 / 16 / 12
      setLimits(8, 24, 8, 32, 8, 24, 3000, 3048);
      result = (t >> 12 & 3 ? ((t >> a) % ((t >> b) % 6 / (3 & t >> c) + 1) + 1) * t & -t >> 4 : ((t >> 1) % ((t >> b) % 6 / (3 & t >> a) + 1) + 1) * t & t); // +5E3/(t/4&4095);
      break;
    case 6: // FINE
      // variation on 45 MAKE this TWO
      setLimits(0, 32, 0, 32, 0, 32, 3000, 4000); // aMin, aMax, etc
      result = ( ~t >> 2 ? ~t >> 3 : ~t >> 2) * ( (127 & t * ( b & t >> 10) ) < (245 & t * ( 2 + ( c & t >> a ) ) ) );
      enc_offset = 2;
      break;
    case 7: // FINE 
      // variation on 45 MAKE this ONE
      setLimits(0, 32, 0, 32, 0, 32, 3000, 4000); // aMin, aMax, etc
      result = ( ~t >> 7 ? ~t >> 3 : ~t >> 4) * ( (245 & t * ( b & t >> 10) ) < (127 & t * ( 2 + ( c & t >> a ) ) ) );
      break;
    case 8: // FINE  move to noise
      //t*(t>>9&10|t>>11&24^t>>10&15&t>>15) 
      //http://countercomplex.blogspot.com/2011/10/algorithmic-symphonies-from-one-line-of.html?showComment=1317761042403#c4044676811126652266
      setLimits(1, 16, 1, 16, 1, 24, 3000, 4000); // aMin, aMax, etc
      result = t * (t >> a & 10 | t >> b & 24 ^ t >> 10 & c & t >> 15) * 32;
      break;
    case 9:  // FINE
      // poetaster long distorted carnival number
      setLimits(1, 12, 1, 16, 1, 16, 3000, 5000);
      result = (t * a & t >> b | t * c & t >> 7 | t * 3 & t / 1024) * 64;
      enc_offset = 3;
      break;
    // maybe also in rythmical?
    case 10:  // FINE
      // poetaster twisted calliope distortion can rock :)
      setLimits(1, 30, 1, 15, 1, 15, 3000, 5000);
      result = (  t * b & c / (a << 2) | t * b & t >> c | t * ( c + b ) & t >> a) * 64;
      enc_offset = 2;
      break;
    case 11: // FINE
      // base arps.  http://www.pouet.net/topic.php?which=8357&page=9#c388930",
      setLimits(8, 16, 8, 16, 8, 16, 2000, 3000);
      result =   ( t * ( ( a + ( 1 ^ t >> 10 & b ) ) * ( b + ( a & t >> 14 ) ) ) ) | ( t >> c  ) * 32;
      enc_offset = 1;
      break;
    case 12: // FINE
      // FIX rythm, broken poetaster fast to slow arps + staccato noise and robot voice = breaks with 9 o'clock, 5 to 12 o'clock, 3 o'clock
      setLimits(0, 64, 0, 16, 0, 16, 1000, 2000);
      result = (( t >> c | t << b ) & (a + 1))  * (( t >> (b + 1) ) | t >> ( t << 21)) ;
      break;
    case 13:  // FINE move to noise
      // poetaster pulse drone 
      setLimits(1, 32, 1, 24, 16, 32, 4000, 6000);
      result = ((t * a) & ( t >> 5 | t << 2 )  ) | ( (t * b) & ( t >> 4 | t << 3)) | ((t * c) & ( t >> 3 | t << 4 ) ) * 12 ;
      break;
    case 14:  //  move to noise
      // beyon http://countercomplex.blogspot.com/2011/10/algorithmic-symphonies-from-one-line-of.html?showComment=1318081308422#c4945456935852459432
      setLimits(1, 16, 1, 32, 6, 12, 2000, 3000);
      result = t >> 6 ^ t & 37 | t + (t ^ t >> a) - t * ((t % 24 ? 2 : 6)&t >> b)^t << 1 & (t & 598 ? t >> 4 : t >> c) * 128;
      break;
    case 15: // FINE 
      // poeaster variation with melodic with shells  variation on https://dollchan.net/btb/res/3.html#78
      // ( ( t >> 10 | t * 5 ) & ( t >> 8 | t * 4 ) & ( t >> 4 | t * 6 ) );
      setLimits(0, 16, 1, 16, 8, 16, 2000, 3000);
      result = ( ( t >> a | t * 5 ) & ( t >> ( a + 2 ) | t * b ) & ( t >> b | t * c ) ) * 32;
      enc_offset = 2;
      break;
    case 16: // FINE 
      // try * c? but nice and rockin the way it is.
      setLimits(1, 68, 1, 68, 0, 32, 2000, 3000);
      bb32_set(a, b);
      result = (bb32() | t >> c) * 128;
      enc_offset = 3;
      break;
    case 17: // FINE
      setLimits(1, 64, 1, 64, 0, 32, 2000, 3000);
      bb8_set(a, b);
      result = (bb8() | t >> c) * 128;
      break;
    case 18: // FINE
      // pad melodies 2 & 3 part with noise crackle on c
      setLimits(1, 128, 1, 68, 0, 32, 3024, 4000);
      bb35_set(a, b);
      result = (bb35() | t >> c) * 128;
      enc_offset = 2;
      break;
    case 19: // FINE to noise
      // same as 49 origin. more 4/4 tech qwowloon wow.
      setLimits(1, 32, 1, 32, 1, 8, 2000, 4000);
      result =   (t * ((3 + ( 1 ^ t >> a & 5 ) ) * ( 5 + ( 3 & t >> b ) ) ) ) >> ( t >> c & 3 );
      enc_offset = 2;
      //result = t >> c ^ t & 1 | t + (t ^ t >> 13) - t * ((t >> 5 ? b : a) & t >> ( 8 - ( a >> 1 )  ) );
      break;
    case 20:  // BORKED
      // poetaster drone, organ, perc
      setLimits(1, 8, 1, 16, 1, 8, 4000, 8000);
      result = ( ( t * a & t >> 4 ) | ( t * b & t >> 7 ) | ( t * c &  t) ) - ( t >> b ? 13 : 8) * 32;
      enc_offset = 3;
      break;
    case 21: // FINE move to noise
      // http://entropedia.co.uk/generative_music/#v3b64K0otKS3KU9AoUdMosbMz09TWKLGx0QCxDQ014yAMY01tY03NGgjHRNXMBKQKxNTU1FTT0AUyTTU1rRX0tRRCihKTsxWcixLTSioVkioVAhJLcxQ8EitTixW09AE=
      setLimits(1, 8, 11, 16, 1, 24, 4000, 8000);
      result = (t & (t >> 6) + (t << ((t >> 11) ^ ((t >> b) + a)) | ((t >> c % 64) + (t >> c))) & (-t >> 5)) * 32;
      //result = (t&(t>>6)+(t<<((t>>11)^((t>>13)+3))|((t>>14%64)+(t>>14)))&(-t>>5));
      break;
case 22: // REALLY noisy
      // https://www.pouet.net/topic.php?post=394926
      // (t*(4|t>>13&3)>>(~t>>11&1)&128|t*(t>>11&t>>13)*(~t>>9&3)&127)^(t&4096?(t*(t^t%255)|t>>4)>>1:t>>3|(t&8192?t<<2:t))
      setLimits(7, 13, 9, 15, 3, 11, 2000, 4000);
      result = (z = (t >> (1 + (t >> a & 1))) * (t >> a & 0xfd) * (t >> c & 0xdf) * (t >> 7 & 0xcc)) * 0 + ((t << 1))&z | ((t << 2))&z;
      //result = (t*(4|t>>13&3)>>(~t>>11&1)&128|t*(t>>11&t>>13)*(~t>>9&3)&127)^(t&4096?(t*(t^t%255)|t>>4)>>1:t>>3|(t&8192?t<<2:t));
      break;
    case 23: // BORKED
      // t>>4|t*t*(t>>6&8^8)*(t>>11^t/3>>12)/(7+(t>>10&t>>14&3)) https://www.pouet.net/topic.php?post=401683
      setLimits(11, 24, 8, 16, 8, 24, 3072, 4000);
      result = t >> 4 | t * (t >> 6 & 8 ^ 8) * (t >> a ^ t / 3 >> 12) / (7 + (t >> b & t >> c & 3));
      //result =  t>>4|t*t*(t>>6&8^8)*(t>>11^t/3>>12)/(7+(t>>10&t>>14&3));
      break;
    case 24: // BORKED
      //https://www.pouet.net/ ((t>>6)&(t<<3)/(t*(t>>11)%(3+((t>>16)%22))))
      setLimits(3, 9, 8, 16, 8, 16, 2000, 3000);
      //result = ((t >> a) & (t << 3) / (t * (t >> 11) % (3 + ((t >> c) % (b * 11)))));
      result = ((t >> a) & (t << 3) / (t * (t >> b) % (3 + ((t >> 16) % c))));
      //result = ((t>>6)&(t<<3)/(t*(t>>11)%(3+((t>>16)%22))));

      enc_offset = 2; // set speed
      break;
    case 25: // BORKED
      //https://www.pouet.net/topic.php?post=587236 (t>>(4+(1&t>>(5*(1+(t>>(11-(1&t>>(11+(1&t>>13))))))))))
      setLimits(1, 5, 11, 22, 11, 16, 2000, 4000);
      result = (t >> (4 + (a & t >> (5 * (a + (t >> (11 - (1 & t >> (b + (a & t >> c)))))))))) * 24;
      //result = (t>>(4+(1&t>>(5*(1+(t>>(11-(1&t>>(11+(1&t>>13))))))))));
      break;
    case 26: // FINE
      // abstract base arps
      //https://www.pouet.net/topic.php?post=401561
      setLimits(1, 9, 1, 9, 9, 15, 3088, 4000);
      //8*t*t*(t>>(t>>10)%3+15)/(3+(t>>10&(t>>15&3|4)))|t/16
      //result = 8*t*t*(t>>(t>>(a*10))%3+c)/(3+(t>>(a*10)&(t>>c&a|b)))|t/16;
      result = 8 * t * (t >> (t >> 10) % 3 + c) / (3 + (t >> 10 & (t >> c & a | b))) | t / 16 * 32;
      break;
    case 27:
      // wavetable sine. could probably add a bunch of (t&15)*(-t&15)*(((t&16)/8)-1)*128/65+128
      setLimits(0, 1023, 0, 1023, 0, 1023, 2000, 4000);
      tune = 404 - a;
      tune = tune << 3;
      freq[0] = a + tune;
      freq[1] = b + tune;
      freq[2] = c + tune;
      for (int i = 0; i < 3; i++) {
        Acc[i] += freq[i];
      }
      result = ( pgm_read_byte(&sine256[Acc[0] >> 8]) + pgm_read_byte(&sine256[Acc[1] >> 8]) + pgm_read_byte(&sine256[Acc[2] >> 8]) ) / 3 * 128;
      // too quiet (sine amp only ~+/-43) - louder: result = ( pgm_read_byte(&sine256[Acc[0] >> 8]) + pgm_read_byte(&sine256[Acc[1] >> 8]) + pgm_read_byte(&sine256[Acc[2] >> 8]) ) / 3 * 128;
      break;
    case 28:
      setLimits(0, 1023, 0, 1023, 0, 1023, 4000, 6000);
      tune = 404 - a;
      freq[0] = a * a ;
      freq[1] = (b + 64) * a;
      freq[2] = (c + 64) * a;
      for (int i = 0; i < 3; i++) {
        Acc[i] += freq[i];
      }
      //result = ( pgm_read_byte(&sine256[Acc[0] >> 8]) + pgm_read_byte(&sine256[Acc[1] >> 8]) + pgm_read_byte(&sine256[Acc[2] >> 8]) ) / 3;
      result = ( pgm_read_byte(&sine256[Acc[0] >> 8]) + pgm_read_byte(&sine256[Acc[1] >> 8]) + pgm_read_byte(&sine256[Acc[2] >> 8]) ) / 3 * 128;
      break;
   case 29:
      setLimits(0, 1023, 0, 1023, 0, 1023, 2000, 4000);
      tune = 404 - a;
      freq[0] = a * a ;
      freq[1] = (b + 64) * a;
      freq[2] = (c + 64) * a;
      for (int i = 0; i < 3; i++) {
        Acc[i] += freq[i];
      }
      //result =  ( ((Acc[0] >> 8) & 0x80 ) + ((Acc[1] >> 8) & 0x80 ) + ((Acc[2] >> 8) & 0x80 ) ) / 3;
      result = ( ((Acc[0] >> 8) & 0x80 ) + ((Acc[1] >> 8) & 0x80 ) + ((Acc[2] >> 8) & 0x80 ) ) / 3 * 128;
      break;
//    case 29: //saw phase in honour of https://ressources.labomedia.org/_media/bytebeats_beginners_guide_ttnm_v1-5.pdf
//      setLimits(48, 72, 32, 64, 8, 48);
//      result = t % a | t % b | t % c;
//      break;



      /*
        case 17: // 216  t*(t^t+(t>>15|1)^(t-1280^t)>>10); http://viznut.fi/demos/unix/bytebeat_formulas.txt
        setLimits(1, 9, 12, 18, 320, 640);
        result = t * (t ^ t + (t >> b | a) ^ (t - c ^ t) >> 10);
        break;
        case 20: //poetaster requires 4* normal speed // does not seem to work
        setLimits(32, 39, 32, 47, 32, 58);
        result = t * ( ( ( t >> 11 * a / 27 ) & ( ( t >> 11 * b / 27 ) - 1 ) ^ 7) * 11 * c / 27);
        //result = t * ( ( ( t%2? t>>11*a/27: t>>11*a/9 ) ^ ( ( t >> 11*b/9 ) - 1 ) ^ 7) ^ 11*c/9);
        break;*/
      /*
        case 22:  // xpansive 2011-09-29 https://www.pouet.net/topic.php?which=8357&page=3#c388375
        // t * (t >> 8 | t >> 9) & 46 & t >> 8 ^ (t & t >> 13 | t >> 6);
        setLimits(40, 56, 1, 9, 0, 16);
        result = t * (t >> 8 | t >> 9) & a & t >> 8 ^ (t & t >> c | t >> b);
        break;
      */
      /*
        case 24: // https://www.pouet.net/topic.php?post=401900 (z=3*t>>t/4096%4&-t%(t>>16|16)*t>>14&8191)/(z>>6|1)*4;
        setLimits(1, 7, 1, 9, 11, 24);
        result = (z = b * t >> t / 4096 % 4 & -t % (t >> c | c) * t >> 14 & 8191) / (z >> 6 | a) * 4;
        break;
      */


  }

}

void noisy(int pb3) {
  switch (pb3) {
    case 1: // FINE move to melodic
      // poetaster helicopters has some arps with b in the middle, various! can also humm with bells.
      setLimits(8, 24, 1, 16, 1, 16, 3000, 4000);
      result = ( (t >> a) - ( t >> a & t )  + ( t >> t &  b ) ) + ( t * ((t >> c) & c ) );
      break;
    case 2:  //FINE  move to rythmical
      setLimits(1, 12, 1, 16, 1, 16, 2000, 3000);
      result =  t >> ( ( a + 1 )  & b & t >> 8 ) ^ ( t & t >> a | t >> c ); /// maybe t >> 16?
      enc_offset = 3;
      break;
    case 3:  //  FINe move to noise
      // poetaster breaky, jungle stuff a at one oclock, c middle, etc
      setLimits(1, 12, 1, 16, 1, 16, 3000, 4000);
      result = a + ( ( t >> a + 1 ) ) * (t >> c | b | t >> ( t >> 16) )  * 12;
      break;
    case 4:  // FINE move to rythmic
      // poetaster techno stuff but slow it down and it's rockin
      setLimits(6, 12, 1, 16, 6, 12, 3000, 4000);
      result =  (t & t >> b | t << b >> c) ^ ( t  &  t >> a | t << a >> b) & ( t & t >> c | t << c >> a) * 12 ;
      break;
    case 5:  // FINE techno move to rythmical
      // poetaster counter point bleeps with a hihat or reverb stuff NAH at 2 times medlodic
      setLimits(4, 16, 4, 16, 1, 8, 2400, 3024);
      result =  ( t  &  t >> c | t + c << 4) | (t & t >> a | t + a << 3) ^ ( t & t >> b | t + b << 2 ) * 24;
      enc_offset = 2;
      break;
    case 6: // FINE move to noise
      // // poetaster fx goes long not like th original nice variety http://www.pouet.net/topic.php?which=8357&page=17#c389829",
      // (t*(t>>12)*64+(t>>1)*(t>>10)*(t>>11)*48)>>(((t>>16)|(t>>17))&1
      setLimits(1, 16, 1, 16, 1, 16, 3000, 4000);
      result = ( t * (t >> c) * 64 + ( t >> 1 ) * ( t >> b ) * (t >> 7) * 48 ) >> ( ( (t >> 16) | (t >> a) ) + 1) * 12;
      break;
    case 7:  // FINE move to rythmic
      // poetaster c high, techno wheep, b pitch in the middle, a patterned blips fast to slow
      setLimits(1, 24, 1, 12, 1, 16, 3000, 5000);
      result =  t * ( t >> b & ( t >> b ? 13 : 8)  ) - t >> c ^ t & 21 | t + (t ^ t >> a)  ^ t << 1 & (t & a ? t >> 5 : t >> 3);
      break;
    case 8:  // FINE move to  noise
      // poetaster descending / ascending bleep arps, lasers, a high bass run
      setLimits(1, 24, 1, 12, 1, 16, 2000, 3000);
      result = t - ((t & ((t >> a))) + ( a | t >> c )) & (t >> ( c + 1)) | (t >> b) & (t * (t >> a)) * 8;
      break;
    case 9:  // FINE belongs in rythmic
      // poetaster also a drone, but melodic basic with perc blurbs hh hits goes to techno with 3 part development
      setLimits(1, 16, 1, 16, 16, 96, 4000, 6000);
      result =  ( t >> a | t - b ) & ( t - a | t >> b ) * c;
      enc_offset = 3;
      break;
    case 10:   // FINE move melodic
      // started with vizmut paper pp. 5 https://arxiv.org/pdf/1112.1368.pdf = hacked to bits :)
      setLimits(2, 32, 2, 32, 2, 7, 2000, 3000);
      result = t - b & ( ( t >> a | t >> b ) ) ^ t - c & ( ( t >> b | t << c * 2 ) ) ^ t - a & ( ( t >> c | t >> 3 ) ) ;
      enc_offset = 2;
      break;
    case 11:  // BORKED
      // started with vizmut paper pp. 6 https://arxiv.org/pdf/1112.1368.pdf
      setLimits(1, 16, 1, 16, 1, 16, 2000, 4000);
      result = t - b & ( (t >> a | t << 4 ) ) ^ t - c & ( ( t >> b | t << 3 ) ) ^ t - a & ( ( t >> c | t << 2 ) ) * 24 ;
      enc_offset = 2;
      //result = (int)(t/1e7 * t * t + c ) % 127 | t >> c ^ t >> b | t % 127 + ( t >> a ) | t ;
      break;
    // the bb funcs are from nyblybyte.h
    case 12:  // FINE move to rythmical
      setLimits(1, 68, 1, 68, 0, 32, 2000, 3000);
      bb19_set( a, b);
      result = (bb19() | t >> c) * 128;
      break;
    case 13: // BORKED?
      // it evolves noisy. lots of breaks to silence
      setLimits(1, 68, 1, 68, 1, 16, 2000, 4000);
      bb39_set(a, b);
      result =  bb39() |  t >> c * 128;
      break;
    case 14: //FINE move to melodic
      // yeah, it's nice and evolves a lot :)
      setLimits(1, 68, 1, 68, 0, 32, 3056, 3520);
      bb17_set(a, b);
      result = (bb17() | t >> c) * 128;
      break;
    case 15: // FINE
      // no needs 8000 hz and takes a while 
      setLimits(1, 16, 1, 16, 0, 32, 3056, 4000);
      bb23_set(a, b);
      //result =  bb23() - c;
      result = (bb23() | t >> c ) * 128;
      break;
    case 16:// ?? borked
      setLimits(1, 16, 1, 16, 0, 16, 2000, 4000);
      bb33_set(a, b);
      //result =  bb33() >>  c;
      result = (bb33() - c) * 128;
      break;
    case 17: // FINE move to melodic
      // rythm bit //https://www.blogger.com/profile/02935728280207314233
      setLimits(4, 16, 2, 16, 2, 16, 2000, 3000);
      result = ((t < 16384) ? (t * a & t * 2 >> b) : (t < 32768) ? (t * a & t * 2 >> c) : (t < 49152) ? (t * (b)&t * 2 >> a) : (t < 65536) ? (t * (b)&t * 2 >> c) : 0 ) * 32;
      if (t >= 65536)t = 0;
      enc_offset = 1;
      break;
    case 18:  // BORKED 
      // poetaster a synth lead with breaks, and stuff :) c 12 oclock and it starts being a wave form
      setLimits(1, 16, 1, 32, 0, 16, 2000, 3000);
      result = (t >> 3 ? b : a) * t >> c ^ t % c | t + (t ^ t >> a) * c;
      enc_offset = 2;
      break;
    case 19:  // FINE  move to rythmic
      // poetaster mad techno helicopter bird flock this is an evil hack.
      setLimits(1, 16, 1, 16, 0, 16, 2000, 3000);
      result =    ( t / ( a * b ) ) & ( ( t >> a ) & t >> b ) * ( t << c) + a ^ ( t >> (t / b ) ) ;//^ ( t >> ( 16 * c ) ) ;
      enc_offset = 2;
      break;
    case 20:  // FINE  move to melodic
      // poetaster can do techno ! it's great when you find it ;)
      setLimits(0, 15, 0, 11, 0, 9, 2000, 3000);
      result = ( ( (t >> 9 ? a : b) & t * (t >> b) % (t >> c) - (t >> b) ) * (t >> a) ) ;
      break;

  }
}



void nybly(int pb4) {

  switch (pb4) {
    case 1: // BORKED
      setLimits(1, 228, 1, 228, 2, 16, 2000, 2392);
      bb0_set(a, b);
      result =  ( bb0() & t >> c ) * 128;
      break;

    case 2: // FINE move to melodic
      setLimits(1, 228, 1, 228, 2, 8, 3000, 4000);
      //SRATE = 4072;
      bb1_set(a, b);
      result =  ( bb1() & t >> c ) * 128;
      break;

    case 3: // FINE move to rythmical
      setLimits(1, 228, 1, 228, 2, 8, 3056, 4000);
      //SRATE = 4072;
      bb2_set(a, b);
      result =  ( bb2() & t >> c ) * 128;
      break;

    case 4: // FINE move to noise
      setLimits(1, 228, 1, 228, 2, 8, 2000, 4000);
      //SRATE = 2408;
      bb3_set(a, b);
      result =  ( bb3() & t >> c ) * 128;
      break;

    case 5: // FINE belongs in  rythmic
      setLimits(1, 228, 1, 228, 2, 8, 3056, 4000);
      bb4_set(a, b);
      result =  ( bb4() & t >> c ) * 128;
      break;
    case 6: //  FINE to melodic
      setLimits(1, 228, 1, 228, 1, 8, 2616, 3000);
      bb5_set(a, b);
      result =  ( bb5() & t >> c ) * 128;
      break;
    case 7: // BORKED belongs in noise has long pauses
      setLimits(1, 228, 1, 228, 1, 8, 2000, 4000);
      bb6_set(a, b);
      result =  ( bb6() & t >> c ) * 128; 
      break;
    case 8: // BORKED move to melodic
      setLimits(1, 228, 1, 228, 1, 8, 2000, 4000);
      bb7_set(a, b);
      result =  ( bb7() & t >> c ) * 128; 
      break;
    case 9: // FINE move to melodic TECHNO
      setLimits(1, 228, 1, 228, 1, 8, 2000, 3000);
      bb8_set(a, b);
      result =  ( bb8() & t >> c ) * 128; 
      break;
    case 10: // FINE move to rythmical
      setLimits(1, 228, 1, 228, 1, 8, 2000, 3000);
      bb9_set(a, b);
      result =  ( bb9() & t >> c ) * 128; 
      break;
    case 11: // FINE move to melodic
      setLimits(1, 228, 1, 228, 1, 8, 2000, 3000);
      bb10_set(a, b);
      result =  ( bb10() & t >> c ) * 128; 
      break;
    case 12: // FINE, move to melodic
      setLimits(1, 228, 1, 228, 1, 8, 2000, 3000);
      bb11_set(a, b);
      result =  ( bb11() & t >> c ) * 128; 
      break;
    case 13: // FINE, belongs to the noise
      setLimits(1, 228, 1, 228, 1, 8, 3000, 4000);
      bb12_set(a, b);
      result =  ( bb12() & t >> c ) * 128; 
      break;
    case 14: // FINE, move to rythmic
      setLimits(1, 228, 1, 228, 1, 8, 2300, 3000);
      //SRATE = 6120;
      bb13_set(a, b);
      result =  ( bb13() & t >> c ) * 128; 
      break;
    case 15: // FINE belongs to melodic
      setLimits(1, 228, 1, 228, 1, 8, 3000, 4000);
      //SRATE = 2536;
      bb14_set(a, b);
      result =  ( bb14() & t >> c ) * 128; 
      break;
    case 16: // FINE move to rythmic
      setLimits(1, 228, 1, 228, 1, 8, 2000, 3048);
      //SRATE = 3176;
      bb15_set(a, b);
      result =  ( bb15() & t >> c ) * 128; 
      break;
    case 17: // FINE move to melodic
      setLimits(1, 228, 1, 228, 1, 8, 3056, 4000);
      bb16_set(a, b);
      result =  ( bb16() & t >> c ) * 128; 
      break;
    case 18: // FINE to noise
      setLimits(1, 228, 1, 228, 1, 8, 2000, 4000);
      //SRATE = 2408;
      bb17_set(a, b);
      result =  ( bb17() & t >> c ) * 128; 
      break;
    case 19: // FINE to melodic
      setLimits(1, 228, 1, 228, 1, 8, 2000, 4000);
      //SRATE = 7912;
      bb18_set(a, b);
      result =  ( bb18() & t >> c ) * 128; 
      break;
    case 20: // FINE to rythmic
      setLimits(1, 228, 1, 228, 1, 8, 2000, 4000);
      //SRATE = 1128;
      bb19_set(a, b);
      result =  ( bb19() & t >> c ) * 128; 
      break;
    case 21:// FINE move to noise
      setLimits(1, 228, 1, 228, 1, 8, 2000, 4000);
      //SRATE = 1512;
      bb20_set(a, b);
      result =  ( bb20() & t >> c ) * 128; 
      break;
    case 22: // FINE move to melodic
      setLimits(1, 228, 1, 228, 1, 8, 2000, 4000);
      //SRATE = 3816;
      bb21_set(a, b);
      //if (t >= 65536) t = 0;
      result =  ( bb21() & t >> c ) * 128; 
      break;
    case 23: // Fine move to noise
      setLimits(1, 228, 1, 228, 1, 8, 2000, 4000);
      //SRATE = 2920;
      bb24_set(a, b);
      result =  ( bb24() & t >> c ) * 128; 
      break;
    case 24: // FINE move to rythmic 
      setLimits(1, 228, 1, 228, 1, 8, 2000, 4000);
      //SRATE = 2920;
      bb25_set(a, b);
      result =  ( bb25() & t >> c ) * 128; 
      break;
    case 25: // FINE move to noise 
      setLimits(1, 228, 1, 228, 1, 8, 2000, 4000);
      //SRATE = 3560;
      bb26_set(a, b);
      result =  ( bb26() & t >> c ) * 128; 
      break;
    case 26: // FINE move to noise
      setLimits(1, 228, 1, 228, 1, 8, 2000, 4000);
      //SRATE = 3560;
      bb27_set(a, b);
      result =  ( bb27() & t >> c ) * 128; 
      break;
    case 27: // BORKED
      setLimits(1, 228, 1, 228, 1, 8, 2000, 4000);
      //SRATE = 2792;
      bb28_set(a, b);
      result =  ( bb28() & t >> c ) * 128;
      break;
    case 28: // FINE move to noise
      setLimits(1, 228, 1, 228, 1, 8, 2000, 4000);
      //SRATE = 2024;
      bb29_set(a, b);
      result =  ( bb29() & t >> c ) * 128;
      break;
    case 29: // FINE move to rythmic
      setLimits(1, 228, 1, 228, 1, 8, 2000, 4000);
      //SRATE = 2536;
      bb30_set(a, b);
      result =  ( bb30() & t >> c ) * 128; 
      break;
    case 30: // FINE move to rythmic
      setLimits(1, 228, 1, 228, 1, 8, 2000, 4000);
      //SRATE = 4072;
      bb39_set(a, b);
      result =  ( bb39() & t >> c ) * 128; 
      break;
    default:
      setLimits(1, 228, 1, 228, 1, 8, 2000, 4000);
      bb7_set(a, b);
      result =  ( bb7() & t >> c ) * 128; 
      break;
  }
}
