#ifndef QSYNTHI2_VOICEDATA_H
#define QSYNTHI2_VOICEDATA_H

#include "QSynthi2/types.h"


class VoiceData {

public:

    std::atomic<Decimal> gain{0};

    std::atomic<Decimal> frequency{0};

    std::atomic<int> lineOfInterestShape{0};
    std::atomic<Decimal> lineOfInterestX{0};
    std::atomic<Decimal> lineOfInterestY{0};
    std::atomic<Decimal> lineOfInterestLength{0};
    std::atomic<Decimal> lineOfInterestRotation{0};
    std::atomic<Decimal> circleOfInterestWidth{0};
    std::atomic<Decimal> circleOfInterestHeight{0};
    std::atomic<Decimal> circleOfInterestFraction{0};

    bool isActive() {
        return gain > 0;
    }

};


#endif //QSYNTHI2_VOICEDATA_H
