#include "stdio.h"
#include "timer.h"
#include "network.h"

char rawPacket[] = "00000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000100001000000000000100010100000000000000000010100000000000000000000100000000000000010000000001000110110111011011111100000010101000000000010000001011000000101010000000000100000011000111110100000000011111010000010000000000010100000000000000000001001000011001010110110001101100011011110010000001010111011011110111001001101100011001000010000110001010100000100000000100111001";

typedef struct {
    uint8_t dest[6];
    uint8_t src[6];
    uint16_t type;
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
} IPFrame;

typedef struct {
    uint16_t sourcePort;
    uint16_t destinationPort;
    uint16_t length;
    uint16_t checksum;
} UDPFrame;

uint8_t charToByte(char number[8]) {
    uint8_t result = 0;
    for (int i = 0; i < 8; i++) {
        result += (number[i] - '0') << (7 - i);
    }

    return result;
}

EthernetFrame parseEthernetFrame(char *payload) {
    EthernetFrame ethFrame;
    for (int i = 0; i < 6; i++) {
        ethFrame.dest[i] = charToByte(payload + i * 8);
        ethFrame.src[i] = charToByte(payload + (i + 6) * 8);
    }

    ethFrame.type = charToByte(payload + 12 * 8) << 8 | charToByte(payload + 13 * 8);
    return ethFrame;
}

IPFrame parseIPFrame(char *payload) {
    IPFrame ipFrame;
    ipFrame.version = charToByte(payload) >> 4;
    ipFrame.headerLength = charToByte(payload) & 0x0F;
    ipFrame.typeOfService = charToByte(payload + 1 * 8);
    ipFrame.totalLength = charToByte(payload + 2 * 8) << 8 | charToByte(payload + 3 * 8);
    ipFrame.identification = charToByte(payload + 4 * 8) << 8 | charToByte(payload + 5 * 8);
    ipFrame.flags = charToByte(payload + 6 * 8) >> 5;
    ipFrame.fragmentOffset = charToByte(payload + 6 * 8) & 0x1F << 8 | charToByte(payload + 7 * 8);
    ipFrame.timeToLive = charToByte(payload + 8 * 8);
    ipFrame.protocol = charToByte(payload + 9 * 8);
    ipFrame.headerChecksum = charToByte(payload + 10 * 8) << 8 | charToByte(payload + 11 * 8);
    for (int i = 0; i < 4; i++) {
        ipFrame.source[i] = charToByte(payload + (12 + i) * 8);
        ipFrame.destination[i] = charToByte(payload + (16 + i) * 8);
    }

    return ipFrame;
}

UDPFrame parseUDPFrame(char *payload) {
    UDPFrame udpFrame;
    udpFrame.sourcePort = charToByte(payload) << 8 | charToByte(payload + 1 * 8);
    udpFrame.destinationPort = charToByte(payload + 2 * 8) << 8 | charToByte(payload + 3 * 8);
    udpFrame.length = charToByte(payload + 4 * 8) << 8 | charToByte(payload + 5 * 8);
    udpFrame.checksum = charToByte(payload + 6 * 8) << 8 | charToByte(payload + 7 * 8);

    return udpFrame;
}

void printEthernetFrame(EthernetFrame *frame) {
    printf("\nEthernet Frame:\n");
    printf("Destination: ");
    for (int i = 0; i < 6; i++) {
        printf("%d", frame->dest[i]);
        if (i < 5) printf(":");
    }
    printf("\tSource: ");
    for (int i = 0; i < 6; i++) {
        printf("%d", frame->src[i]);
        if (i < 5) printf(":");
    }
    printf("\tType: %d\n", frame->type);
}

void printIPFrame(IPFrame *frame) {
    printf("\nIP Frame:\n");
    printf("Version: %u\t", frame->version);
    printf("Header Length: %u\t", frame->headerLength);
    printf("Type of Service: %u\t", frame->typeOfService);
    printf("Total Length: %u\t", frame->totalLength);
    printf("Identification: %u\t", frame->identification);
    printf("Flags: %u\t", frame->flags);
    printf("Fragment Offset: %u\t", frame->fragmentOffset);
    printf("Time to Live: %u\t", frame->timeToLive);
    printf("Protocol: %u\t", frame->protocol);
    printf("Header Checksum: %d\t", frame->headerChecksum);

    printf("Source IP: ");
    for (int i = 0; i < 4; i++) {
        printf("%u", frame->source[i]);
        if (i < 3) printf(".");
    }

    printf("\tDestination IP: ");
    for (int i = 0; i < 4; i++) {
        printf("%u", frame->destination[i]);
        if (i < 3) printf(".");
    }

    printf("\n");
}

void printUDPFrame(UDPFrame *frame) {
    printf("\nUDP Frame:\n");
    printf("Source Port: %u\t", frame->sourcePort);
    printf("Destination Port: %u\t", frame->destinationPort);
    printf("Length: %u\t", frame->length);
    printf("Checksum: %d\n", frame->checksum);
}

void printPayload(char *payload, int length) {
    printf("\nPayload: ");
    for (int i = 0; i < length; i += 8) {
        putc(charToByte(payload + i));
    }
    putc('\n');
}

int main() {
    uint64_t start = getNanos();
    printf("Current time: %d\n", start);

    EthernetFrame ethFrame = parseEthernetFrame(rawPacket);
    IPFrame ipFrame;
    UDPFrame udpFrame;

    printEthernetFrame(&ethFrame);

    if (ethFrame.type == 0x0800) {
        ipFrame = parseIPFrame(rawPacket + 14 * 8);
        printIPFrame(&ipFrame);

        if (ipFrame.protocol == 17) {
            udpFrame = parseUDPFrame(rawPacket + 14 * 8 + ipFrame.headerLength * 8);
            printUDPFrame(&udpFrame);

            printPayload(rawPacket + 42 * 8, (ipFrame.totalLength - ipFrame.headerLength*4 - 8) * 8);
        } else
            printf("Not a UDP packet\n");
    } else
        printf("Not an IP packet\n");

    printf("\n\nTime elapsed: %d ns", getNanos() - start);

    return 0;
}
