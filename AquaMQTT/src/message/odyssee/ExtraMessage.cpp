#include "message/odyssee/ExtraMessage.h"

namespace aquamqtt::message::odyssee
{

ExtraMessage::ExtraMessage(uint8_t* data, uint8_t* previous) : mData(data)
{
    mCreatedWithoutPrevious = (previous == nullptr);
    compareWith(previous);
}

void ExtraMessage::compareWith(uint8_t* data)
{
    if (data == nullptr)
    {
        return;
    }

    uint8_t diffIndices[ENERGY_MESSAGE_LENGTH_NEXT] = { 0 };
    size_t  numDiffs                                = 0;
    compareBuffers(mData, data, ENERGY_MESSAGE_LENGTH_NEXT, diffIndices, &numDiffs);

    for (int i = 0; i < numDiffs; ++i)
    {
        auto indiceChanged = diffIndices[i];

        switch (indiceChanged)
        {
            default:
                break;
        }
    }
}

uint8_t ExtraMessage::getLength()
{
    return EXTRA_MESSAGE_LENGTH_ODYSSEE;
}

}  // namespace aquamqtt::message::odyssee