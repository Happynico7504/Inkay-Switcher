// Minimal SHA256 implementation for integrity verification
#pragma once

#include <cstring>
#include <cstdint>
#include "eg2/libs/typedefs.h"

namespace sha256 {
    inline constexpr u32 k_sha256[64] = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
    };

    class SHA256 {
    public:
        typedef u8 digest_t[32];

        SHA256() { reset(); }

        SHA256& reset() {
            m_state[0] = 0x6a09e667;
            m_state[1] = 0xbb67ae85;
            m_state[2] = 0x3c6ef372;
            m_state[3] = 0xa54ff53a;
            m_state[4] = 0x510e527f;
            m_state[5] = 0x9b05688c;
            m_state[6] = 0x1f83d9ab;
            m_state[7] = 0x5be0cd19;
            m_len = 0;
            m_bufLen = 0;
            return *this;
        }

        SHA256& update(const void* data, usize len) {
            const u8* ptr = (const u8*)data;
            while (len > 0) {
                usize space = 64 - m_bufLen;
                usize chunk = len < space ? len : space;
                memcpy(m_buf + m_bufLen, ptr, chunk);
                m_bufLen += chunk;
                ptr += chunk;
                len -= chunk;
                m_len += chunk * 8;
                if (m_bufLen == 64) {
                    process_block();
                    m_bufLen = 0;
                }
            }
            return *this;
        }

        void finalize(digest_t out) {
            m_buf[m_bufLen++] = 0x80;
            if (m_bufLen > 56) {
                memset(m_buf + m_bufLen, 0, 64 - m_bufLen);
                process_block();
                m_bufLen = 0;
            }
            memset(m_buf + m_bufLen, 0, 56 - m_bufLen);
            u64 len_bits = m_len;
            for (int i = 7; i >= 0; i--) {
                m_buf[56 + i] = (u8)(len_bits & 0xff);
                len_bits >>= 8;
            }
            process_block();
            for (int i = 0; i < 8; i++) {
                out[i * 4 + 0] = (u8)(m_state[i] >> 24);
                out[i * 4 + 1] = (u8)(m_state[i] >> 16);
                out[i * 4 + 2] = (u8)(m_state[i] >> 8);
                out[i * 4 + 3] = (u8)(m_state[i] & 0xff);
            }
        }

    private:
        u32 m_state[8];
        u64 m_len;
        u8 m_buf[64];
        usize m_bufLen;

        static const u32 k[64];

        void process_block() {
            u32 w[64];
            for (int i = 0; i < 16; i++) {
                w[i] = ((u32)m_buf[i*4] << 24) | ((u32)m_buf[i*4+1] << 16) |
                       ((u32)m_buf[i*4+2] << 8) | (u32)m_buf[i*4+3];
            }
            for (int i = 16; i < 64; i++) {
                u32 s0 = rightrotate(w[i-15], 7) ^ rightrotate(w[i-15], 18) ^ (w[i-15] >> 3);
                u32 s1 = rightrotate(w[i-2], 17) ^ rightrotate(w[i-2], 19) ^ (w[i-2] >> 10);
                w[i] = w[i-16] + s0 + w[i-7] + s1;
            }

            u32 a = m_state[0], b = m_state[1], c = m_state[2], d = m_state[3];
            u32 e = m_state[4], f = m_state[5], g = m_state[6], h = m_state[7];

            for (int i = 0; i < 64; i++) {
                u32 S1 = rightrotate(e, 6) ^ rightrotate(e, 11) ^ rightrotate(e, 25);
                u32 ch = (e & f) ^ ((~e) & g);
                u32 temp1 = h + S1 + ch + k_sha256[i] + w[i];
                u32 S0 = rightrotate(a, 2) ^ rightrotate(a, 13) ^ rightrotate(a, 22);
                u32 maj = (a & b) ^ (a & c) ^ (b & c);
                u32 temp2 = S0 + maj;

                h = g;
                g = f;
                f = e;
                e = d + temp1;
                d = c;
                c = b;
                b = a;
                a = temp1 + temp2;
            }

            m_state[0] += a;
            m_state[1] += b;
            m_state[2] += c;
            m_state[3] += d;
            m_state[4] += e;
            m_state[5] += f;
            m_state[6] += g;
            m_state[7] += h;
        }

        static u32 rightrotate(u32 x, u32 n) { return (x >> n) | (x << (32 - n)); }
    };

    inline std::string hex_string(const u8* hash, usize len) {
        static const char* hex = "0123456789abcdef";
        std::string result;
        for (usize i = 0; i < len; i++) {
            result += hex[hash[i] >> 4];
            result += hex[hash[i] & 0xf];
        }
        return result;
    }
}
