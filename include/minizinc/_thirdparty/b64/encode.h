// :mode=c++:
/*
encode.h - c++ wrapper for a base64 encoding algorithm

This is part of the libb64 project, and has been placed in the public domain.
For details, see http://sourceforge.net/projects/libb64
*/
#ifndef BASE64_ENCODE_H
#define BASE64_ENCODE_H

#define BUFFERSIZE 4096

#include <iostream>

namespace base64
{
	extern "C"
	{
#include "cencode.h"
	}

	struct encoder
	{
		base64_encodestate _state;
		int _buffersize;

		encoder(int buffersize_in = BUFFERSIZE)
			: _buffersize(buffersize_in)
		{
			base64_init_encodestate(&_state);
		}

		int encode(char value_in)
		{
			return base64_encode_value(value_in);
		}

		std::streamsize encode(const char* code_in, const std::streamsize length_in, char* plaintext_out)
		{
			return base64_encode_block(code_in, static_cast<int>(length_in), plaintext_out, &_state);
		}

		int encode_end(char* plaintext_out)
		{
			return static_cast<int>(base64_encode_blockend(plaintext_out, &_state));
		}

		void encode(std::istream& istream_in, std::ostream& ostream_in)
		{
			base64_init_encodestate(&_state);
			//
			const int N = _buffersize;
			char* plaintext = new char[N];
			char* code = new char[2 * N];
			std::streamsize plainlength;
			std::streamsize codelength;

			do
			{
				istream_in.read(plaintext, N);
				plainlength = istream_in.gcount();
				//
				codelength = encode(plaintext, plainlength, code);
				ostream_in.write(code, codelength);
			} while(istream_in.good() && plainlength > 0);

			codelength = encode_end(code);
			ostream_in.write(code, codelength);
			//
			base64_init_encodestate(&_state);

			delete[] code;
			delete[] plaintext;
		}
	};

} // namespace base64

#endif // BASE64_ENCODE_H

