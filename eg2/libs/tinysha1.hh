// clang-format off
/*
*
* TinySHA1 - a header only implementation of the SHA1 algorithm in C++. Based
* on the implementation in boost::uuid::details.
*
* SHA1 Wikipedia Page: http://en.wikipedia.org/wiki/SHA-1
*
*/
#pragma once

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "eg2/libs/typedefs.h"

namespace sha1 {
    class SHA1 {
    public:
        typedef u32 digest32_t[5];
        typedef u8 digest8_t[20];

        inline static u32 LeftRotate(u32 value, usize count) { return (value << count) ^ (value >> (32 - count)); }

        SHA1(void) { reset(); }

        virtual ~SHA1(void) {}

        SHA1(const SHA1 &s) { *this = s; }

        const SHA1 &operator=(const SHA1 &s) {
            memcpy(m_digest, s.m_digest, 5 * sizeof(u32));
            memcpy(m_block, s.m_block, 64);
            m_blockByteIndex = s.m_blockByteIndex;
            m_byteCount = s.m_byteCount;
            return *this;
        }

        SHA1 &reset(void) {
            m_digest[0] = 0x67452301;
            m_digest[1] = 0xEFCDAB89;
            m_digest[2] = 0x98BADCFE;
            m_digest[3] = 0x10325476;
            m_digest[4] = 0xC3D2E1F0;
            m_blockByteIndex = 0;
            m_byteCount = 0;
            return *this;
        }

        SHA1 &processByte(u8 octet) {
            this->m_block[this->m_blockByteIndex++] = octet;
            ++this->m_byteCount;
            if (m_blockByteIndex == 64) {
                this->m_blockByteIndex = 0;
                processBlock();
            }
            return *this;
        }

        SHA1 &processBlock(const void *const start, const void *const end) {
            const u8 *begin = static_cast<const u8 *>(start);
            const u8 *finish = static_cast<const u8 *>(end);
            while (begin != finish) {
                processByte(*begin);
                begin++;
            }
            return *this;
        }

        SHA1 &processBytes(const void *const data, usize len) {
            const u8 *block = static_cast<const u8 *>(data);
            processBlock(block, block + len);
            return *this;
        }

        const u32 *getDigest(digest32_t digest) {
            usize bitCount = this->m_byteCount * 8;
            processByte(0x80);
            if (this->m_blockByteIndex > 56) {
                while (m_blockByteIndex != 0) processByte(0);
                while (m_blockByteIndex < 56) processByte(0);
            } else while (m_blockByteIndex < 56) processByte(0);
            processByte(0);
            processByte(0);
            processByte(0);
            processByte(0);
            processByte(static_cast<unsigned char>((bitCount >> 24) & 0xFF));
            processByte(static_cast<unsigned char>((bitCount >> 16) & 0xFF));
            processByte(static_cast<unsigned char>((bitCount >> 8) & 0xFF));
            processByte(static_cast<unsigned char>((bitCount) & 0xFF));

            memcpy(digest, m_digest, 5 * sizeof(u32));
            return digest;
        }

        const u8 *getDigestBytes(digest8_t digest) {
            digest32_t d32;
            getDigest(d32);
            usize di = 0;
            digest[di++] = ((d32[0] >> 24) & 0xFF);
            digest[di++] = ((d32[0] >> 16) & 0xFF);
            digest[di++] = ((d32[0] >> 8) & 0xFF);
            digest[di++] = ((d32[0]) & 0xFF);

            digest[di++] = ((d32[1] >> 24) & 0xFF);
            digest[di++] = ((d32[1] >> 16) & 0xFF);
            digest[di++] = ((d32[1] >> 8) & 0xFF);
            digest[di++] = ((d32[1]) & 0xFF);

            digest[di++] = ((d32[2] >> 24) & 0xFF);
            digest[di++] = ((d32[2] >> 16) & 0xFF);
            digest[di++] = ((d32[2] >> 8) & 0xFF);
            digest[di++] = ((d32[2]) & 0xFF);

            digest[di++] = ((d32[3] >> 24) & 0xFF);
            digest[di++] = ((d32[3] >> 16) & 0xFF);
            digest[di++] = ((d32[3] >> 8) & 0xFF);
            digest[di++] = ((d32[3]) & 0xFF);

            digest[di++] = ((d32[4] >> 24) & 0xFF);
            digest[di++] = ((d32[4] >> 16) & 0xFF);
            digest[di++] = ((d32[4] >> 8) & 0xFF);
            digest[di++] = ((d32[4]) & 0xFF);
            return digest;
        }

    protected:
        void processBlock(void) {
            u32 w[80];
            for (usize i = 0; i < 16; i++) {
                w[i] = (m_block[i * 4 + 0] << 24);
                w[i] |= (m_block[i * 4 + 1] << 16);
                w[i] |= (m_block[i * 4 + 2] << 8);
                w[i] |= (m_block[i * 4 + 3]);
            }
            for (usize i = 16; i < 80; i++) w[i] = LeftRotate((w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16]), 1);

            u32 a = m_digest[0];
            u32 b = m_digest[1];
            u32 c = m_digest[2];
            u32 d = m_digest[3];
            u32 e = m_digest[4];

            for (usize i = 0; i < 80; ++i) {
                u32 f = 0;
                u32 k = 0;

                if (i < 20) {
                    f = (b & c) | (~b & d);
                    k = 0x5A827999;
                } else if (i < 40) {
                    f = b ^ c ^ d;
                    k = 0x6ED9EBA1;
                } else if (i < 60) {
                    f = (b & c) | (b & d) | (c & d);
                    k = 0x8F1BBCDC;
                } else {
                    f = b ^ c ^ d;
                    k = 0xCA62C1D6;
                }
                u32 temp = LeftRotate(a, 5) + f + e + k + w[i];
                e = d;
                d = c;
                c = LeftRotate(b, 30);
                b = a;
                a = temp;
            }

            m_digest[0] += a;
            m_digest[1] += b;
            m_digest[2] += c;
            m_digest[3] += d;
            m_digest[4] += e;
        }

    private:
        digest32_t m_digest;
        u8 m_block[64];
        usize m_blockByteIndex;
        usize m_byteCount;
    };
}