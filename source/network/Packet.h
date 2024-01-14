#pragma once
#include "../engine/dtypes.h"

extern void *writeBool(void *buffer, uShort *size, bool value);
extern void *writeUByte(void *buffer, uShort *size, uByte value);
extern void *writeUShort(void *buffer, uShort *size, uShort value);
extern void *writeSInt(void *buffer, uShort *size, sInt value);

extern void *readBool(void *buffer, uShort *size, bool *value);
extern void *readUByte(void *buffer, uShort *size, uByte *value);
extern void *readUShort(void *buffer, uShort *size, uShort *value);
extern void *readSInt(void *buffer, uShort *size, sInt *value);
