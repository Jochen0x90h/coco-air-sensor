#include <coco/Array.hpp>
#include <coco/InputDevice.hpp>
#include <coco/platform/Loop_emu.hpp>
#include <string>


namespace coco {

/**
 * Implementation of an emulated sensor that can measure up to 8 values
 */
class Sensor_emu : public InputDevice, public Loop_emu::GuiHandler {
public:
    /// Configuration of an emulated measured value (e.g. temperature)
    struct Config {
        // initial value of emulated sensor
        float initialValue;

        // minimum value
        float min;

        // maximum value
        float max;

        // value step
        float step;

        // number of decimals to display (negative to disalbe suppression of trailing zeros)
        int decimalCount;

        // unit of measurement
        String unit;
    };

    /**
     * Constructor
     * @param loop event loop
     * @param config array of configurations for up to 8 emulated measured values
     * @param id unique id for gui
     */
    Sensor_emu(Loop_emu &loop, Array<const Config> config, int id);
    ~Sensor_emu() override;

    // InputDevice methods
    int get(void *data, int size) override;
    [[nodiscard]] Awaitable<Events> untilInput(int sequenceNumber) override;

protected:
    void handle(Gui &gui) override;

    Array<const Config> config;
    int id;
    int sequenceNumber = 0;
    float values[8];
};

} // namespace coco
