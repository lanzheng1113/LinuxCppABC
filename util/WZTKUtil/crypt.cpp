#include "stddef.h"
#include "util/crypt.h"

XorCrypter::XorCrypter(const string& key) {
    if (key.empty()) {
        m_key = "dxxikwoxnixbccvamiwopoikdnwdvnwezdelfgac";
    } else {
        m_key = key;
    }
}

void XorCrypter::SimpleXor_Crype(char *dest, const char *src, int len) {
    if ((dest == NULL) || (src == NULL) || (len <= 0)) {
        return;
    }
    int iKey = 0;
    try {
        for (int i = 0; i != len; i++) {
            dest[i] = (src[i] ^ (m_key[iKey] + len));
            if (iKey == m_key.length() - 1) {
                iKey = 0;
            } else {
                iKey++;
            }
        }
    } catch (...) {
    }
}
