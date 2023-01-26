#pragma once
#include "stddef.h"

typedef struct __attribute__((packed))
{
    char VBESignature[4];
    uint16_t VBEVersion;
    char *OEMStringPointer;
    uint32_t Capabilities;
    uint32_t VideoModePointer;
    uint16_t TotalMemory;
    uint16_t OEMSoftwareRevision;
    char *OEMVenderNamePointer;
    char *OEMProductNamePointer;
    char *OEMProductRevisionPointer;
    uint8_t _reserved[222];
    uint8_t OEMData[256];
} VBEINFOBLOCK_t;

typedef struct __attribute__((packed))
{
    uint16_t ModeAttributes;
    uint8_t WindowAAttributes;
    uint8_t WindowBAttributes;
    uint16_t WindowGranularity;
    uint16_t WindowSize;
    uint16_t WindowASegment;
    uint16_t WindowBSegment;
    uint32_t WindowFunctionPointer;
    uint16_t BytesPerScanline;

    uint16_t XResolution;
    uint16_t YResolution;
    uint8_t XCharacterSize;
    uint8_t YCharacterSize;
    uint8_t NumberOfPlanes;
    uint8_t BitsPerPixel;
    uint8_t NumberOfBanks;
    uint8_t MemoryModel;
    uint8_t BankSize;
    uint8_t NumberOfImagePages;
    uint8_t _reserved1;

    uint8_t RedMaskSize;
    uint8_t RedFieldPosition;
    uint8_t GreenMaskSize;
    uint8_t GreenFieldPosition;
    uint8_t BlueMaskSize;
    uint8_t BlueFieldPosition;
    uint8_t ReservedMaskSize;
    uint8_t ReservedFieldPosition;
    uint8_t DirectColorModeInfo;

    uint32_t PhysicalBasePointer;
    uint32_t _reserved2;
    uint16_t _reserved3;

    uint16_t LinearBytesPerScanline;

    uint16_t BankNumberOfImagePages;
    uint8_t LinearNumberOfImagePages;
    uint8_t LinearRedMaskSize;
    uint8_t LinearRedFieldPosition;
    uint8_t LinearGreenMaskSize;
    uint8_t LinearGreenFieldPosition;
    uint8_t LinearBlueMaskSize;
    uint8_t LinearBlueFieldPosition;
    uint8_t LinearReservedMaskSize;
    uint8_t LinearReservedFieldPosition;
    uint32_t MaxPixelClock;
    uint8_t _reserved4[190];
} MODEINFOBLOCK_t;

typedef struct __attribute__((packed)) 
{
    uint64_t BaseAddress;
    uint64_t Size;
    uint32_t Type;
    uint32_t ACPIAttributeField;
} SMAPENTRY_t;

typedef struct __attribute__((packed)) 
{
    uint16_t NumberOfEntries;
    SMAPENTRY_t Entries[21];
} SMAP_t;

typedef struct __attribute__((packed))
{
    uint8_t JmpBoot[3];
    char OEMName[8];
    uint16_t BytesPerSector;
    uint8_t SectorsPerCluster;
    uint16_t ReservedSectors;
    uint8_t FATCopies;
    uint16_t NumberOfRootEntries;
    uint16_t TotalSectors;
    uint8_t MediaDescriptor;
    uint16_t SectorsPerFAT;
    uint16_t SectorsPerTrack;
    uint16_t NumberOfHeads;
    uint32_t NumberOfHiddenSectors;
    uint16_t TotalSectors2;
    uint8_t DriveNumber;
    uint8_t _reserved;
    uint8_t ExtensionSigniture;
    uint32_t SerialNumber;
    char VolumeLabel[11];
    char FilesystemType[8];
    uint8_t code[438];
    uint32_t VBEInfoBlock;
    uint32_t ModeInfoBlock;
    uint32_t SMAPEntries;
    uint16_t BootMagic;
} BOOTCODE_t;