#include "util/Des.h"
#include <string.h>
namespace Util
{
	CDes::CDes()
	{
		Initialize();
	}

	CDes::~CDes()
	{
		UnInitialize();
	}

	void CDes::Initialize()
	{
		
	}

	void CDes::UnInitialize()
	{
		
	}

        void CDes::EncryStrHex(string strInText, string& strOutText, string strKeyText)
        {
            std::lock_guard<std::recursive_mutex> locker(m_CS_DES_ENC);
		EncryStr(strInText, strOutText, strKeyText);
		StrToHex(strOutText, strOutText);
	}


	void CDes::DecryStrHex(string strInText, string& strOutText, string strKeyText)
	{
		int nChar;
		string strText;
		for (string::size_type i = 0; i < strInText.size() / 2; i++)
		{
			string strHex = strInText.substr(i * 2, 2);
			HexToInt(strHex, nChar);
			char Chr = nChar;
			strText += Chr;
		}
                {
                    std::lock_guard<std::recursive_mutex> lock(m_CS_DES_DEC);
		DecryStr(strText, strOutText, strKeyText);
                }
	}

	void CDes::EncryStrHex(unsigned char *pchr_in, unsigned long llen, string &str_out, string str_key)
	{
            std::lock_guard<std::recursive_mutex> locker(m_CS_DES_ENC);
		EncryStr(pchr_in, llen, str_out, str_key);
		string strHex;
		StrToHex(str_out, strHex);
		str_out = strHex;
	}

	void CDes::DecryStrHex(string str_in, unsigned char **pchr_in, unsigned long &llen, string str_key)
	{
		int nChar;
		string strText;
		for (string::size_type i = 0; i < str_in.size() / 2; i++)
		{
			string strHex = str_in.substr(i * 2, 2);
			HexToInt(strHex, nChar);
			char Chr = nChar;
			strText += Chr;
		}
                {
                    
                std::lock_guard<std::recursive_mutex> locker(m_CS_DES_DEC);
		DecryStr(strText, pchr_in, llen, str_key);
                }
	}

	void CDes::EncryStrHex(const char* chrInText, char* chrOutText, const char* chrKeyText)
	{
		if (chrOutText == NULL) return;
		string strOutText;
		EncryStrHex(chrInText, strOutText, chrKeyText);
		strcpy(chrOutText, strOutText.c_str());
	}

	void CDes::DecryStrHex(const char* chrInText, char* chrOutText, const char* chrKeyText)
	{
		string strOutText;
		DecryStrHex(chrInText, strOutText, chrKeyText);
		strcpy(chrOutText, strOutText.c_str());
	}


	void CDes::EncryStr(string strInText, string& strOutText, string strKeyText)
	{
		unsigned char byKey[8], byStr[8], byOut[8];
		memset(byKey, 0, 8);
		memset(byStr, 0, 8);
		memset(byOut, 0, 8);

		for (int i = 0; i < 16; i++)
			for (int j = 0; j < 6; j++)
				SubKey[i][j] = 0;

		char chrSpace = 0;
		while (strInText.size() % 8 != 0)
		{
			strInText += chrSpace;
		}
		ForMatKeyStr(strKeyText);
		for (int i = 0; i < 8; i++)
		{
			byKey[i] = strKeyText[i];
		}
		MakeKey(byKey);

		for (string::size_type i = 0; i < strInText.size() / 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				byStr[j] = strInText[i * 8 + j];
			}
			desData(dmEncry, byStr, byOut);
			for (int j = 0; j < 8; j++)
			{
				strOutText += byOut[j];
			}
		}
	}


	void CDes::DecryStr(string strInText, string& strOutText, string strKeyText)
	{
		unsigned char byKey[8], byStr[8], byOut[8];
		memset(byKey, 0, 8);
		memset(byStr, 0, 8);
		memset(byOut, 0, 8);

		for (int i = 0; i < 16; i++)
		{
			for (int j = 0; j < 6; j++)
			{
				SubKey[i][j] = 0;
			}
		}
		ForMatKeyStr(strKeyText);
		for (int i = 0; i < 8; i++)
		{
			byKey[i] = strKeyText[i];
		}
		MakeKey(byKey);
		for (string::size_type i = 0; i < strInText.size() / 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				byStr[j] = strInText[i * 8 + j];
			}
			desData(dmDecry, byStr, byOut);
			for (int j = 0; j < 8; j++)
			{
				strOutText += byOut[j];
			}
		}
		for (string::iterator iter = strOutText.begin(); iter != strOutText.end(); )
		{
			if (*iter == 0)
				iter = strOutText.erase(iter);
			else
				++iter;
		}
	}

	void CDes::EncryStr(unsigned char *pchr_in, unsigned long llen, string &str_out, string str_key)
	{
		unsigned char byKey[8], byStr[8], byOut[8];
		memset(byKey, 0, 8);
		memset(byStr, 0, 8);
		memset(byOut, 0, 8);
		for (int i = 0; i < 16; i++)
			for (int j = 0; j < 6; j++)
				SubKey[i][j] = 0;
		ForMatKeyStr(str_key);

		unsigned int differ_len = ((llen % 8) == 0) ? 8 : 8 - llen % 8 + 8;
		unsigned long align_len = llen + differ_len;
		unsigned char *pby_buf = new unsigned char[align_len];
		memset(pby_buf, 0, align_len);
		pby_buf[align_len - 1] = differ_len;
		for (unsigned long i = 0; i < llen; i++)
			pby_buf[i] = pchr_in[i];
		for (int i = 0; i < 8; i++)
			byKey[i] = str_key[i];
		MakeKey(byKey);

		for (unsigned long i = 0; i < align_len / 8; i++)
		{
			for (int j = 0; j < 8; j++)
				byStr[j] = pby_buf[i * 8 + j];
			desData(dmEncry, byStr, byOut);
			for (int j = 0; j < 8; j++)
				str_out += byOut[j];
		}
		delete [] pby_buf;
	}

	void CDes::DecryStr(string str_in, unsigned char **pchr_out, unsigned long &llen, string str_key)
	{
		unsigned char byKey[8];
		unsigned char byStr[8];
		unsigned char byOut[8];

		for (int i = 0; i < 16; i++)
			for (int j = 0; j < 6; j++)
				SubKey[i][j] = 0;
		ForMatKeyStr(str_key);
		for (int i = 0; i < 8; i++)
			byKey[i] = str_key[i];
		MakeKey(byKey);

		unsigned long ulQ = 0;
		unsigned char *pby_buf = new unsigned char[str_in.size()];
		for (string::size_type i = 0; i < str_in.size() / 8; i++)
		{
			for (int j = 0; j < 8; j++) byStr[j] = str_in[i * 8 + j];
			desData(dmDecry, byStr, byOut);
			for (int j = 0; j < 8; j++)
			{
				pby_buf[ulQ] = byOut[j];
				ulQ++;
			}
		}
		if (ulQ <= 0)
			return;
		unsigned int differ_len = pby_buf[ulQ - 1];
		llen = ulQ - differ_len;
		*pchr_out = pby_buf;
	}

	void CDes::MakeKey(unsigned char* byInKey)
	{
		const unsigned char bitDisplace[16] = { 1,1,2,2, 2,2,2,2, 1,2,2,2, 2,2,2,1 };
		unsigned char outData56[7];
		unsigned char key281[4];
		unsigned char key28r[4];
		unsigned char key56o[7];

		permutationChoose1(byInKey, outData56);

		key281[0] = outData56[0] >> 4;
		key281[1] = (outData56[0] << 4) | (outData56[1] >> 4);
		key281[2] = (outData56[1] << 4) | (outData56[2] >> 4);
		key281[3] = (outData56[2] << 4) | (outData56[3] >> 4);

		key28r[0] = outData56[3] & 0x0F;
		key28r[1] = outData56[4];
		key28r[2] = outData56[5];
		key28r[3] = outData56[6];

		for (int i = 0; i < 16; i++)
		{
			cycleMove(key281, bitDisplace[i]);
			cycleMove(key28r, bitDisplace[i]);

			key56o[0] = (key281[0] << 4) | (key281[1] >> 4);
			key56o[1] = (key281[1] << 4) | (key281[2] >> 4);
			key56o[2] = (key281[2] << 4) | (key281[3] >> 4);
			key56o[3] = (key281[3] << 4) | (key28r[0]);
			key56o[4] = key28r[1];
			key56o[5] = key28r[2];
			key56o[6] = key28r[3];
			permutationChoose2(key56o, SubKey[i]);
		}
	}

	void CDes::permutation(unsigned char* inData)
	{
		unsigned char newData[4];
		int i;
		for (i = 0; i < 4; i++) newData[i] = 0;
		for (i = 0; i < 32; i++)
		{
			if ((inData[BitPM[i] >> 3] & (1 << (7 - (BitPM[i] & 0x07)))) != 0)
			{
				newData[i >> 3] = newData[i >> 3] | (1 << (7 - (i & 0x07)));
			}
		}
		for (i = 0; i < 4; i++)
		{
			inData[i] = newData[i];
		}
	}

	void CDes::permutationChoose1(unsigned char* inData, unsigned char* outData)
	{
		int i;
		for (i = 0; i < 7; i++) outData[i] = 0;
		for (i = 0; i < 56; i++)
		{
			if ((inData[BitPMC1[i] >> 3] & (1 << (7 - (BitPMC1[i] & 0x07)))) != 0)
			{
				outData[i >> 3] = outData[i >> 3] | (1 << (7 - (i & 0x07)));
			}
		}
	}

	void CDes::permutationChoose2(unsigned char* inData, unsigned char* outData)
	{
		int i;
		for (i = 0; i < 6; i++) outData[i] = 0;
		for (int i = 0; i < 48; i++)
		{
			if ((inData[BitPMC2[i] >> 3] & (1 << (7 - (BitPMC2[i] & 0x07)))) != 0)
			{
				outData[i >> 3] = outData[i >> 3] | (1 << (7 - (i & 0x07)));
			}
		}
	}

	void CDes::cycleMove(unsigned char* inData, unsigned char bitMove)
	{
		for (int i = 0; i < bitMove; i++)
		{
			inData[0] = (inData[0] << 1) | (inData[1] >> 7);
			inData[1] = (inData[1] << 1) | (inData[2] >> 7);
			inData[2] = (inData[2] << 1) | (inData[3] >> 7);
			inData[3] = (inData[3] << 1) | ((inData[0] & 0x10) >> 4);
			inData[0] = (inData[0] & 0x0F);
		}
	}

	void CDes::desData(cDesMode mDesMode, unsigned char* inData, unsigned char* outData)
	{
		unsigned char temp[4];
		unsigned char buf[4];
		int i, j;
		for (i = 0; i < 4; i++) temp[i] = 0;
		for (i = 0; i < 4; i++) buf[i] = 0;
		for (i = 0; i < 8; i++) outData[i] = inData[i];

		initPermutation(outData);

		if (mDesMode == dmEncry)
		{
			for (i = 0; i < 16; i++)
			{
				for (j = 0; j < 4; j++) temp[j] = outData[j];
				for (j = 0; j < 4; j++) outData[j] = outData[j + 4];

				unsigned char by8[8];
				for (j = 0; j < 8; j++) by8[j] = outData[j];

				encry(outData, SubKey[i], buf);
				for (j = 0; j < 4; j++) outData[j + 4] = temp[j] ^ buf[j];
			}

			for (j = 0; j < 4; j++) temp[j] = outData[j + 4];
			for (j = 0; j < 4; j++) outData[j + 4] = outData[j];
			for (j = 0; j < 4; j++) outData[j] = temp[j];
		}
		else
			if (mDesMode == dmDecry)
			{
				for (i = 15; i >= 0; i--)
				{
					for (j = 0; j < 4; j++) temp[j] = outData[j];
					for (j = 0; j < 4; j++) outData[j] = outData[j + 4];
					encry(outData, SubKey[i], buf);
					for (j = 0; j < 4; j++) outData[j + 4] = temp[j] ^ buf[j];
				}
				for (j = 0; j < 4; j++) temp[j] = outData[j + 4];
				for (j = 0; j < 4; j++) outData[j + 4] = outData[j];
				for (j = 0; j < 4; j++) outData[j] = temp[j];
			}
		conversePermutation(outData);
	}

	void CDes::initPermutation(unsigned char* inData)
	{
		unsigned char newData[8];
		int i;
		for (i = 0; i < 8; i++) newData[i] = 0;
		for (i = 0; i < 64; i++)
		{
			if ((inData[BitIP[i] >> 3] & (1 << (7 - (BitIP[i] & 0x07)))) != 0)
			{
				newData[i >> 3] = newData[i >> 3] | (1 << (7 - (i & 0x07)));
			}
		}
		for (i = 0; i < 8; i++)
		{
			unsigned char by = newData[i];
			inData[i] = by;
		}
	}

	void CDes::encry(unsigned char* intData, unsigned char* subKey, unsigned char* outData)
	{
		unsigned char outBuf[6];
		unsigned char buf[8];
		int i;
		for (i = 0; i < 6; i++) outBuf[i] = 0;
		for (i = 0; i < 8; i++) buf[i] = 0;

		expand(intData, outBuf);

		for (i = 0; i < 6; i++)
		{
			outBuf[i] = outBuf[i] ^ subKey[i];
		}

		buf[0] = outBuf[0] >> 2;
		buf[1] = ((outBuf[0] & 0x03) << 4) | (outBuf[1] >> 4);
		buf[2] = ((outBuf[1] & 0x0F) << 2) | (outBuf[2] >> 6);
		buf[3] = outBuf[2] & 0x3F;
		buf[4] = outBuf[3] >> 2;
		buf[5] = ((outBuf[3] & 0x03) << 4) | (outBuf[4] >> 4);
		buf[6] = ((outBuf[4] & 0x0F) << 2) | (outBuf[5] >> 6);
		buf[7] = outBuf[5] & 0x3F;

		for (i = 0; i < 8; i++)
		{
			buf[i] = si(i, buf[i]);
		}
		for (i = 0; i < 4; i++)
		{
			outBuf[i] = (buf[i * 2] << 4) | buf[i * 2 + 1];
		}
		permutation(outBuf);
		for (i = 0; i < 4; i++)
		{
			outData[i] = outBuf[i];
		}
	}

	void CDes::expand(unsigned char* inData, unsigned char* outData)
	{
		unsigned char by6[6];
		unsigned char by8[8];
		int i;
		for (i = 0; i < 6; i++)
		{
			by6[i] = outData[i];
		}
		for (i = 0; i < 8; i++)
		{
			by8[i] = inData[i];
		}

		for (i = 0; i < 48; i++)
		{
			if ((inData[BitExp[i] >> 3] & (1 << (7 - (BitExp[i] & 0x07)))) != 0)
			{
				outData[i >> 3] = outData[i >> 3] | (1 << (7 - (i & 0x07)));
				by6[i >> 3] = by6[i >> 3] | (1 << (7 - (i & 0x07)));
			}
		}
	}

	unsigned char CDes::si(unsigned char s, unsigned char inByte)
	{
		unsigned char c;
		c = (inByte & 0x20) | ((inByte & 0x1E) >> 1) | ((inByte & 0x01) << 4);
		return (sBox[s][c] & 0x0F);
	}

	void CDes::StrToHex(string strText, string& strHex)
	{
		strHex.clear();
		for (string::size_type i = 0; i < strText.size(); i++)
		{
			int itChr = strText[i];
			if (itChr < 0) itChr = 0x100 + itChr;
			char pchr[8] = { "" };
			sprintf(pchr, "%.2X", itChr);
			strHex += string(pchr);
		}
	}

	void CDes::HexToInt(string strHex, int& itData)
	{
		sscanf(strHex.c_str(), "%I32x", &itData);
	}

	void CDes::conversePermutation(unsigned char* inData)
	{
		unsigned char newData[8];
		int i;
		for (i = 0; i < 8; i++) newData[i] = 0;
		for (i = 0; i < 64; i++)
		{
			if ((inData[BitCP[i] >> 3] & (1 << (7 - (BitCP[i] & 0x07)))) != 0)
			{
				newData[i >> 3] = newData[i >> 3] | (1 << (7 - (i & 0x07)));
			}
		}
		for (i = 0; i < 8; i++)
		{
			inData[i] = newData[i];
		}
	}

	void CDes::ForMatKeyStr(string &strKeyText)
	{
		while (strKeyText.size() < 8)
		{
			strKeyText = strKeyText + '\x00';
		}
	}
};

