#include "teakra/teakra.h"
#include "teakra/teakra_c.h"

extern "C" {

struct TeakraObject {
    Teakra::Teakra teakra;
};

TeakraContext* Teakra_Create() {
    return new TeakraContext;
}

void Teakra_Destroy(TeakraContext* context) {
    delete context;
}

void Teakra_Reset(TeakraContext* context) {
    context->teakra.Reset();
}

uint8_t* Teakra_GetDspMemory(TeakraContext* context) {
    return context->teakra.GetDspMemory().data();
}

int Teakra_SendDataIsEmpty(const TeakraContext* context, uint8_t index) {
    return context->teakra.SendDataIsEmpty(index);
}

void Teakra_SendData(TeakraContext* context, uint8_t index, uint16_t value) {
    context->teakra.SendData(index, value);
}

int Teakra_RecvDataIsReady(const TeakraContext* context, uint8_t index) {
    return context->teakra.RecvDataIsReady(index);
}

uint16_t Teakra_RecvData(TeakraContext* context, uint8_t index) {
    return context->teakra.RecvData(index);
}

void Teakra_SetRecvDataHandler(TeakraContext* context, uint8_t index,
                               Teakra_InterruptCallback handler, void* userdata) {
    context->teakra.SetRecvDataHandler(index, [=]() { handler(userdata); });
}

void Teakra_SetSemaphore(TeakraContext* context, uint16_t value) {
    context->teakra.SetSemaphore(value);
}
void Teakra_ClearSemaphore(TeakraContext* context, uint16_t value) {
    context->teakra.ClearSemaphore(value);
}
void Teakra_MaskSemaphore(TeakraContext* context, uint16_t value) {
    context->teakra.MaskSemaphore(value);
}

void Teakra_SetSemaphoreHandler(TeakraContext* context, Teakra_InterruptCallback handler,
                                void* userdata) {
    context->teakra.SetSemaphoreHandler([=]() { handler(userdata); });
}

uint16_t Teakra_GetSemaphore(const TeakraContext* context) {
    return context->teakra.GetSemaphore();
}

void Teakra_Run(TeakraContext* context, unsigned cycle) {
    context->teakra.Run(cycle);
}

void Teakra_SetAHBMCallback(TeakraContext* context, Teakra_AHBMReadCallback read,
                            Teakra_AHBMWriteCallback write, void* userdata) {
    Teakra::AHBMCallback callback;
    callback.read8 = [=](uint32_t address) { return read(userdata, address); };
    callback.write8 = [=](uint32_t address, uint8_t value) { write(userdata, address, value); };
    context->teakra.SetAHBMCallback(callback);
}

void Teakra_SetAudioCallback(TeakraContext* context, Teakra_AudioCallback callback,
                             void* userdata) {
    context->teakra.SetAudioCallback(
        [=](std::array<std::int16_t, 2> samples) { callback(userdata, samples.data()); });
}
}
