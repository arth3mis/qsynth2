#ifndef QSYNTHI2_VOICEDATA_H
#define QSYNTHI2_VOICEDATA_H

#include "QSynthi2/types.h"


class VoiceData {

public:

    std::atomic<Decimal> gain{0};

    std::atomic<Decimal> scanlineStartX{0};
    std::atomic<Decimal> scanlineStartY{0};
    std::atomic<Decimal> scanlineEndX{0};
    std::atomic<Decimal> scanlineEndY{0};

    bool isActive() {
        return gain > 0;
    }

};


#endif //QSYNTHI2_VOICEDATA_H
