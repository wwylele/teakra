#pragma once

#include <stdint.h>

extern "C" {

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
void Teakra_SetRecvDataHandler(TeakraContext* context, uint8_t index,
                               Teakra_InterruptCallback handler, void* userdata);

void Teakra_SetSemaphore(TeakraContext* context, uint16_t value);
void Teakra_ClearSemaphore(TeakraContext* context, uint16_t value);
void Teakra_MaskSemaphore(TeakraContext* context, uint16_t value);
void Teakra_SetSemaphoreHandler(TeakraContext* context, Teakra_InterruptCallback handler,
                                void* userdata);
uint16_t Teakra_GetSemaphore(const TeakraContext* context);

void Teakra_Run(TeakraContext* context, unsigned cycle);

void Teakra_SetAHBMCallback(TeakraContext* context, Teakra_AHBMReadCallback read,
                            Teakra_AHBMWriteCallback write, void* userdata);

void Teakra_SetAudioCallback(TeakraContext* context, Teakra_AudioCallback callback, void* userdata);
}
