#include "Packet.h"

void *writeBool(void *buffer, uShort *size, bool value) {
    *((bool *)buffer) = value;
    *(size) += sizeof(bool);
    return buffer + sizeof(bool);
}

void *writeUByte(void *buffer, uShort *size, uByte value) {
    *((uByte *)buffer) = value;
    *(size) += sizeof(uByte);
    return buffer + sizeof(uByte);
}

void *writeUShort(void *buffer, uShort *size, uShort value) {
    *((uShort *)buffer) = value;
    *(size) += sizeof(uShort);
    return buffer + sizeof(uShort);
}

void *writeSInt(void *buffer, uShort *size, sInt value) {
    *((sInt *)buffer) = value;
    *(size) += sizeof(sInt);
    return buffer + sizeof(sInt);
}

void *readBool(void *buffer, uShort *size, bool *value) {
    *value = *((bool *)buffer);
    *(size) -= sizeof(bool);
    return buffer + sizeof(bool);
}

void *readUByte(void *buffer, uShort *size, uByte *value) {
    *value = *((uByte *)buffer);
    *(size) -= sizeof(uByte);
    return buffer + sizeof(uByte);
}

void *readUShort(void *buffer, uShort *size, uShort *value) {
    *value = *((uShort *)buffer);
    *(size) -= sizeof(uShort);
    return buffer + sizeof(uShort);
}

void *readSInt(void *buffer, uShort *size, sInt *value) {
    *value = *((sInt *)buffer);
    *(size) -= sizeof(sInt);
    return buffer + sizeof(sInt);
}
