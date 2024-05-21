#ifndef SIMULATION_H
#define SIMULATION_H

#include "QSynthi2/types.h"

class Simulation {
public:

    virtual ~Simulation() = default;

    virtual const CSimMatrix& getNextFrame(num timestep, const ModulationData& modulationData) = 0;
    virtual void reset() = 0;
};


#include <thread>
#include <mutex>
#include <chrono>
class SimThread {
public:

    explicit SimThread(const std::shared_ptr<Simulation>& s) {
        sim = s;
        newestFrame = -1;
        started = false;
        terminate = false;
        t = std::thread(&SimThread::simulationLoop, this);
    }

    ~SimThread() {
        t.join();
        frameBuffer.forEach([](const CSimMatrix* m) { delete m; });
    }

    void simulationLoop() {
        while (!terminate) {
            if (started) {
                appendFrame(new CSimMatrix(sim->getNextFrame(0.2, {})));
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
        }
    }

    // TODO size limiter idea: delete later frames if list becomes too long (keep some from beginning)
    //      they will be set to nullptr in the list and can be recalculated when needed
    //      (e.g. when a request for i=0 comes in and 1000-10000 are missing, start calcing them immediately
    //      -> maybe add more worker threads to do so?

    void appendFrame(CSimMatrix* f) {
        std::lock_guard lock(frameAccessMutex);
        newestFrame = static_cast<long>(frameBuffer.append(f));
    }

    CSimMatrix* getFrame(const size_t i) {
        std::lock_guard lock(frameAccessMutex);
        if (newestFrame == -1 || i > newestFrame) {
            return nullptr;
        }
        return frameBuffer[i];
    }

    std::mutex frameAccessMutex;
    std::mutex parameterMutex;

    std::atomic<bool> started;
    std::atomic<bool> terminate;
    std::atomic<long> newestFrame;

private:
    std::thread t;
    std::shared_ptr<Simulation> sim;
    List<CSimMatrix*> frameBuffer;
};

#endif //SIMULATION_H
