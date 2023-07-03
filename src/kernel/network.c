#include "stdio.h"
#include "timer.h"
#include "network.h"

char rawPacket[] = "00000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000100001000000000000100010100000000000000000010100000000000000000000100000000000000010000000001000110110111011011111100000010101000000000010000001011000000101010000000000100000011000111110100000000011111010000010000000000010100000000000000000001001000011001010110110001101100011011110010000001010111011011110111001001101100011001000010000110001010100000100000000100111001";

typedef struct {
    uint8_t dest[6];
    uint8_t src[6];
    uint16_t type;
    uint8_t payload[];
} EthernetFrame;

typedef struct {
    uint8_t version: 4;
    uint8_t headerLength: 4;
    uint8_t typeOfService;
    uint16_t totalLength;
    uint16_t identification;
    uint16_t flags: 3;
    uint16_t fragmentOffset: 13;
    uint8_t timeToLive;
    uint8_t protocol;
    uint16_t headerChecksum;
    uint8_t source[4];
    uint8_t destination[4];
    uint8_t options[];
} IPFrame;

typedef struct {
    uint16_t sourcePort;
    uint16_t destinationPort;
    uint16_t length;
    uint16_t checksum;
    uint8_t payload[];
} UDPFrame;

uint8_t charToByte(char number[8]) {
    uint8_t result = 0;
    for (int i = 0; i < 8; i++) {
        result += (number[i] - '0') << (7 - i);
    }
}

EthernetFrame parseEthernetFrame(char *rawPacket) {
    EthernetFrame ethFrame;
    for (int i = 0; i < 6; i++) {
        ethFrame.dest[i] = charToByte(rawPacket + i * 8);
        ethFrame.src[i] = charToByte(rawPacket + (i + 6) * 8);
    }

    ethFrame.type = charToByte(rawPacket + 12 * 8) << 8 | charToByte(rawPacket + 13 * 8);
    return ethFrame;
}


int main() {
    uint64_t start = getMillis();
    printf("Starting network...\nCurrent time: %d\n\n", start);

    EthernetFrame ethFrame = parseEthernetFrame(rawPacket);
    IPFrame ipFrame;
    UDPFrame udpFrame;

    // Print ethernet frame
    printf("Ethernet frame:\n");
    printf(
            "Dest: %x:%x:%x:%x:%x:%x\n",
            ethFrame.dest[0], ethFrame.dest[1],
            ethFrame.dest[2], ethFrame.dest[3],
            ethFrame.dest[4], ethFrame.dest[5]
    );

    printf("Time elapsed: %d\n", getMillis() - start);

    return 0;
}
