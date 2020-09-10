#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

struct TeakraObject;
typedef struct TeakraObject TeakraContext;

typedef void (*Teakra_InterruptCallback)(void* userdata);
typedef void (*Teakra_AudioCallback)(void* userdata, int16_t samples[2]);
typedef uint8_t (*Teakra_AHBMReadCallback)(void* userdata, uint32_t address);
typedef void (*Teakra_AHBMWriteCallback)(void* userdata, uint32_t address, uint8_t value);

TeakraContext* Teakra_Create();
void Teakra_Destroy(TeakraContext* context);
void Teakra_Reset(TeakraContext* context);
uint8_t* Teakra_GetDspMemory(TeakraContext* context);

int Teakra_SendDataIsEmpty(const TeakraContext* context, uint8_t index);
void Teakra_SendData(TeakraContext* context, uint8_t index, uint16_t value);
int Teakra_RecvDataIsReady(const TeakraContext* context, uint8_t index);
uint16_t Teakra_RecvData(TeakraContext* context, uint8_t index);
uint16_t Teakra_PeekRecvData(TeakraContext* context, uint8_t index);
void Teakra_SetRecvDataHandler(TeakraContext* context, uint8_t index,
                               Teakra_InterruptCallback handler, void* userdata);

void Teakra_SetSemaphore(TeakraContext* context, uint16_t value);
void Teakra_ClearSemaphore(TeakraContext* context, uint16_t value);
void Teakra_MaskSemaphore(TeakraContext* context, uint16_t value);
void Teakra_SetSemaphoreHandler(TeakraContext* context, Teakra_InterruptCallback handler,
                                void* userdata);
uint16_t Teakra_GetSemaphore(const TeakraContext* context);

uint16_t Teakra_ProgramRead(TeakraContext* context, uint32_t address);
void Teakra_ProgramWrite(TeakraContext* context, uint32_t address, uint16_t value);
uint16_t Teakra_DataRead(TeakraContext* context, uint16_t address, bool bypass_mmio);
void Teakra_DataWrite(TeakraContext* context, uint16_t address, uint16_t value, bool bypass_mmio);
uint16_t Teakra_DataReadA32(TeakraContext* context, uint32_t address);
void Teakra_DataWriteA32(TeakraContext* context, uint32_t address, uint16_t value);
uint16_t Teakra_MMIORead(TeakraContext* context, uint16_t address);
void Teakra_MMIOWrite(TeakraContext* context, uint16_t address, uint16_t value);

uint16_t Teakra_DMAChan0GetSrcHigh(TeakraContext* context);
uint16_t Teakra_DMAChan0GetDstHigh(TeakraContext* context);

uint16_t Teakra_AHBMGetUnitSize(TeakraContext* context, uint16_t i);
uint16_t Teakra_AHBMGetDirection(TeakraContext* context, uint16_t i);
uint16_t Teakra_AHBMGetDmaChannel(TeakraContext* context, uint16_t i);

void Teakra_Run(TeakraContext* context, unsigned cycle);

void Teakra_SetAHBMCallback(TeakraContext* context, Teakra_AHBMReadCallback read,
                            Teakra_AHBMWriteCallback write, void* userdata);

void Teakra_SetAudioCallback(TeakraContext* context, Teakra_AudioCallback callback, void* userdata);
#ifdef __cplusplus
}
#endif
