#include <coco/Array.hpp>
#include <coco/InputDevice.hpp>
#include <coco/PseudoRandom.hpp>
#include <coco/platform/Loop_native.hpp>
#include <string>


namespace coco {

/**
 * Implementation of an emulated sensor that can measure up to 8 values
 */
class Sensor_native : public InputDevice {
public:
    /// Configuration of an emulated measured value (e.g. temperature)
    struct Config {
        // initial value of emulated sensor
        float initialValue;

        // deviation used for random number generator
        float deviation;
    };

    /**
     * Constructor
     * @param loop event loop
     * @param interval emulated measurement interval
     */
    Sensor_native(Loop_native &loop, Array<const Config> config, Milliseconds<> interval);
    ~Sensor_native() override;

    // InputDevice methods
    int get(void *data, int size) override;
    [[nodiscard]] Awaitable<Events> untilInput(int sequenceNumber) override;

protected:
    void handle();

    Loop_native &loop;
    Array<const Config> config;
    Milliseconds<> interval;
    TimedTask<Callback> callback;

    int sequenceNumber = 0;
    float values[8];
    XorShiftRandom random;
};

} // namespace coco
