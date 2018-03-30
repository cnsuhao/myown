#ifndef __MESSAGE_ENCODER_H__
#define __MESSAGE_ENCODER_H__


class IVarList;

bool text_decode_msg(const char* value, size_t size, IVarList& msg);
int text_encode_msg(const IVarList& msg, char* buf, size_t size);

#endif // END __MESSAGE_ENCODER_H__