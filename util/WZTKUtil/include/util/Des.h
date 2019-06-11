#pragma once

#include <string>
#include <mutex>
using std::string;

namespace Util {

    enum cDesMode {
        dmEncry, dmDecry
    };

    class CDes {
        unsigned char SubKey[16][6];

        unsigned char BitPMC1[56] ={
            56, 48, 40, 32, 24, 16, 8,
            0, 57, 49, 41, 33, 25, 17,
            9, 1, 58, 50, 42, 34, 26,
            18, 10, 2, 59, 51, 43, 35,
            62, 54, 46, 38, 30, 22, 14,
            6, 61, 53, 45, 37, 29, 21,
            13, 5, 60, 52, 44, 36, 28,
            20, 12, 4, 27, 19, 11, 3
        };

        unsigned char BitPMC2[48] ={
            13, 16, 10, 23, 0, 4,
            2, 27, 14, 5, 20, 9,
            22, 18, 11, 3, 25, 7,
            15, 6, 26, 19, 12, 1,
            40, 51, 30, 36, 46, 54,
            29, 39, 50, 44, 32, 47,
            43, 48, 38, 55, 33, 52,
            45, 41, 49, 35, 28, 31
        };

        unsigned char BitIP[64] ={
            57, 49, 41, 33, 25, 17, 9, 1,
            59, 51, 43, 35, 27, 19, 11, 3,
            61, 53, 45, 37, 29, 21, 13, 5,
            63, 55, 47, 39, 31, 23, 15, 7,
            56, 48, 40, 32, 24, 16, 8, 0,
            58, 50, 42, 34, 26, 18, 10, 2,
            60, 52, 44, 36, 28, 20, 12, 4,
            62, 54, 46, 38, 30, 22, 14, 6
        };

        unsigned char BitCP[64] ={
            39, 7, 47, 15, 55, 23, 63, 31,
            38, 6, 46, 14, 54, 22, 62, 30,
            37, 5, 45, 13, 53, 21, 61, 29,
            36, 4, 44, 12, 52, 20, 60, 28,
            35, 3, 43, 11, 51, 19, 59, 27,
            34, 2, 42, 10, 50, 18, 58, 26,
            33, 1, 41, 9, 49, 17, 57, 25,
            32, 0, 40, 8, 48, 16, 56, 24
        };

        int BitExp[48] ={
            31, 0, 1, 2, 3, 4, 3, 4, 5, 6, 7, 8, 7, 8, 9, 10,
            11, 12, 11, 12, 13, 14, 15, 16, 15, 16, 17, 18, 19, 20, 19, 20,
            21, 22, 23, 24, 23, 24, 25, 26, 27, 28, 27, 28, 29, 30, 31, 0
        };

        unsigned char BitPM[32] ={
            15, 6, 19, 20, 28, 11, 27, 16, 0, 14, 22, 25, 4, 17, 30, 9,
            1, 7, 23, 13, 31, 26, 2, 8, 18, 12, 29, 5, 21, 10, 3, 24
        };

        unsigned char sBox[8][64] ={
            { 14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7,
                0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8,
                4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0,
                15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13},

            { 15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10,
                3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5,
                0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15,
                13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9},

            { 10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8,
                13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1,
                13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7,
                1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12},

            { 7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15,
                13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9,
                10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4,
                3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14},

            { 2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9,
                14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6,
                4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14,
                11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3},

            { 12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11,
                10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8,
                9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6,
                4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13},

            { 4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1,
                13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6,
                1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2,
                6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12},

            { 13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7,
                1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2,
                7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8,
                2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}
        };

    public:
        CDes();
        ~CDes();
    protected:
        void Initialize();
        void UnInitialize();
        void MakeKey(unsigned char* byInKey);
        void permutation(unsigned char* inData);
        void permutationChoose1(unsigned char* inData, unsigned char* outData);
        void permutationChoose2(unsigned char* inData, unsigned char* outData);
        void cycleMove(unsigned char* inData, unsigned char bitMove);
        void desData(cDesMode mDesMode, unsigned char* inData, unsigned char* outData);
        void initPermutation(unsigned char* inData);
        void encry(unsigned char* intData, unsigned char* subKey, unsigned char* outData);
        void expand(unsigned char* inData, unsigned char* outData);
        unsigned char si(unsigned char s, unsigned char inByte);
        void StrToHex(string strText, string& strHex);
        void HexToInt(string strHex, int& itData);
        void conversePermutation(unsigned char* inData);
        /**
         * \brief 当加密的密钥长度小于8时，需要在尾部填充0。
         * \param [in][out] strKeyText 密钥，如果长度小于八将在尾部填充0x00
         */
        void ForMatKeyStr(string &strKeyText);
    public:
        /**
         * \brief 加密字符串为十六进制格式的字符串（“0D0ADEAD7AB8”这样的格式）
         * \param strInText 需要加密的字符串。
         * \param strOutText 加密后返回的数据。
         * \param strKeyText 密钥字符串。
         * \return 无
         */
        void EncryStrHex(string strInText, string& strOutText, string strKeyText);
        void EncryStrHex(const char* chrInText, char* chrOutText, const char* chrKeyText);
        void EncryStrHex(unsigned char *pchr_in, unsigned long llen, string &str_out, string str_key);
        /**
         * \brief 解密十六进制格式字符串，字符串格式如"0D0ADEAD7AB8"
         * 
         * \param strInText 十六进制格式的字符串，如"0D0ADEAD7AB8"
         * \param [out] strOutText 解密后返回的字符串。
         * \param strKeyText 密钥
         * \remark 对于输入的每两个字符串都会按照十六进制被转化为对应的一个字节，然后重新组合成一个缓冲区后解密。
         */
        void DecryStrHex(string strInText, string& strOutText, string strKeyText);
        void DecryStrHex(const char* chrInText, char* chrOutText, const char* chrKeyText);
        /**
         * Decrypt a HEX-type string which encrypted by EncryStrHex series functions.
         * @param str_in the input string to decrypted, it must be encrypted by `EncryStrHex` series functions.
         * @param pchr_out [out] The data encrypted, You need to delete the memory block this paramter returned.
         * @param llen [out] return the decrypted data length.
         * @param str_key the key.
         */
        void DecryStrHex(string str_in, unsigned char **pchr_out, unsigned long &llen, string str_key);
        /**
         * \brief 加密字符串，如果字符串长度不是8的整数倍则在其后补充NULL。
         * 
         * \param strInText 需要加密的字符串（或者二进制数据）。这里必须传值，因为这个参数指向的数据长度不是8的整数倍时需要填充0。
         * \param strOutText 加密后的数据，请不要用strOutText.cstr()，加密后的string存放的是二进制数据。
         * \return 无。
         * \remark 这个函数被设计用于加密字符串，但是也可以用来加密二进制数据(string中可以存放二进制数据)。
         * 但如果加密的是二进制数据且长度不是8的整数倍，那么解密出来的数据会多出几个NULL。
         * 请特别留意这个缺陷！如果长度不再另外的地方加以判断可能导致你的程序发生错误！
         * 如果加密的是二进制数据，强烈推荐在二进制数据的头部使用4个字节用于指明数据的实际长度。
         */
        void EncryStr(string strInText, string& strOutText, string strKeyText);
        void EncryStr(unsigned char *pchr_in, unsigned long llen, string &str_out, string str_key);
        /**
         * \brief 解密数据
         * 
         * \param
         * \return
         */
        void DecryStr(string strInText, string& strOutText, string strKeyText);
        void DecryStr(string str_in, unsigned char **pchr_out, unsigned long &llen, string str_key);
    public:
    protected:
        std::recursive_mutex m_CS_DES_ENC;
        std::recursive_mutex m_CS_DES_DEC;
    };
};

