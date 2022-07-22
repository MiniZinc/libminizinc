/*
cencoder.c - c source to a base64 encoding algorithm implementation

This is part of the libb64 project, and has been placed in the public domain.
For details, see http://sourceforge.net/projects/libb64
*/

#include <minizinc/_thirdparty/b64/cencode.h>

void base64_init_encodestate(base64_encodestate* state_in) {
  state_in->step = step_A;
  state_in->result = 0;
  state_in->stepcount = 0;
  state_in->chars_per_line = BASE64_CENC_DEFCPL;
}

/*
 * Calculate required length of buffer for encoded string when using encoder state structure
 * state_in for encoding and when having input data of size plain_len
 *
 * Return encoded length, or 0 if encoded length + one additional null byte would exceed range of
 * size_t
 */

size_t base64_encode_length(size_t plain_len, base64_encodestate* state_in) {
  size_t retmax = 0, retval;
  size_t cpl = state_in->chars_per_line;

  /*
   * Check for integer overflow. Takes into consideration that gcc may optimise out
   * naive integer overflow checks based on the C standard saying the result of overflow is
   * undefined.
   */
  retmax = ~retmax;
  retval = (plain_len + 2) / 3;
  if (retval > retmax / 4) return 0;

  retval *= 4;

  /* Calculate number of linebreaks */
  if (retval && cpl) {
    size_t numbreaks;

    numbreaks = (retval - 1) / cpl + 1;

    retmax -= retval;

    /* Make sure there's always room for one additional 0 byte on top of our return value */
    if (numbreaks < retmax)
      return numbreaks + retval;
    else
      return 0;
  } else
    return retval;
}

char base64_encode_value(signed char value_in) {
  static const char* encoding = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  if (value_in > 63) return '=';
  return encoding[(int)value_in];
}

#define CHECK_BREAK()                        \
  do {                                       \
    if (cpl && state_in->stepcount >= cpl) { \
      *codechar++ = '\n';                    \
      state_in->stepcount = 0;               \
    }                                        \
    state_in->stepcount++;                   \
  } while (0);

size_t base64_encode_block(const void* plaintext_in, const size_t length_in, char* code_out,
                           base64_encodestate* state_in) {
  const char* plainchar = plaintext_in;
  const char* const plaintextend = plainchar + length_in;
  char* codechar = code_out;
  size_t cpl = state_in->chars_per_line;
  char result;
  char fragment;

  result = state_in->result;

  switch (state_in->step) {
    for (;;) {
      case step_A:
        if (plainchar == plaintextend) {
          state_in->result = result;
          state_in->step = step_A;
          return (size_t)(codechar - code_out);
        }

        CHECK_BREAK();

        fragment = *plainchar++;
        result = (fragment & 0x0fc) >> 2;
        *codechar++ = base64_encode_value(result);
        result = (fragment & 0x003) << 4;
      case step_B:
        if (plainchar == plaintextend) {
          state_in->result = result;
          state_in->step = step_B;
          return (size_t)(codechar - code_out);
        }

        CHECK_BREAK();

        fragment = *plainchar++;
        result |= (fragment & 0x0f0) >> 4;
        *codechar++ = base64_encode_value(result);
        result = (fragment & 0x00f) << 2;
      case step_C:
        if (plainchar == plaintextend) {
          state_in->result = result;
          state_in->step = step_C;
          return (size_t)(codechar - code_out);
        }

        CHECK_BREAK();

        fragment = *plainchar++;
        result |= (fragment & 0x0c0) >> 6;
        *codechar++ = base64_encode_value(result);

        CHECK_BREAK();

        result = (fragment & 0x03f) >> 0;
        *codechar++ = base64_encode_value(result);
    }
  }
  /* control should not reach here */
  return (size_t)(codechar - code_out);
}

size_t base64_encode_blockend(char* code_out, base64_encodestate* state_in) {
  char* codechar = code_out;
  size_t cpl = state_in->chars_per_line;

  switch (state_in->step) {
    case step_B:
      CHECK_BREAK();
      *codechar++ = base64_encode_value(state_in->result);
      CHECK_BREAK();
      *codechar++ = '=';
      CHECK_BREAK();
      *codechar++ = '=';
      break;
    case step_C:
      CHECK_BREAK();
      *codechar++ = base64_encode_value(state_in->result);
      CHECK_BREAK();
      *codechar++ = '=';
      break;
    case step_A:
      break;
  }
  if (state_in->chars_per_line) *codechar++ = '\n';

  return (size_t)(codechar - code_out);
}
