#ifndef AQUAMQTT_OPTITRONIC2_STATE_H
#define AQUAMQTT_OPTITRONIC2_STATE_H

#include <Arduino.h>

#include <cstdint>

#include "message/optitronic2/RegisterMap.h"

namespace aquamqtt
{

/**
 * Thread-safe register state store for Optitronic 2.
 * Stores the latest known register values and tracks changes.
 */
class Optitronic2State
{
public:
    static Optitronic2State& getInstance();

    virtual ~Optitronic2State() = default;

    Optitronic2State(const Optitronic2State&)            = delete;
    Optitronic2State& operator=(const Optitronic2State&) = delete;

    /**
     * Set the MQTT task handle for notifications when data changes
     */
    void setListener(TaskHandle_t handle);

    /**
     * Store register values from a parsed read response.
     * Called by the listener task when it receives valid register data.
     * @param startReg The starting register address
     * @param values   Array of register values
     * @param count    Number of registers
     * @return true if any register value changed
     */
    bool storeRegisters(uint16_t startReg, const uint16_t* values, uint16_t count);

    /**
     * Store a single register write (observed on the bus or initiated by us)
     */
    bool storeSingleRegister(uint16_t reg, uint16_t value);

    /**
     * Read a register value. Returns false if the register has never been populated.
     */
    bool getRegister(uint16_t reg, uint16_t& value) const;

    /**
     * Check if a specific register block has been received at least once
     */
    bool hasBlock(uint16_t startReg) const;

    // --- Typed accessors for common values ---

    float    getWaterTemp() const;
    float    getAmbientTemp() const;
    float    getEvaporatorTemp() const;
    float    getDhwSetpoint() const;
    float    getActiveSetpoint() const;
    float    getEcoDeviation() const;
    float    getKomfortDeviation() const;
    uint16_t getProgram() const;
    uint16_t getAuxHeatMode() const;
    uint16_t getExtInputFunction() const;
    uint16_t getOperatingState() const;
    uint16_t getHeatSource() const;
    uint16_t getCompressorStatus() const;
    bool     isPvActive() const;
    bool     isQuickHeatActive() const;
    float    getQuickHeatTarget() const;
    bool     isForceHeating() const;

    // Installer parameters (available after installer block is read)
    float    getFrostProtectTemp() const;
    float    getBivalentThreshold() const;
    float    getPvTargetSetpoint() const;
    float    getExtSourceMaxTemp() const;
    uint16_t getAntiLegioInterval() const;
    uint16_t getExtSourcePriority() const;

    // --- Statistics ---
    struct Stats
    {
        uint32_t framesReceived;
        uint32_t crcErrors;
        uint32_t registerUpdates;
        uint32_t lastUpdateMs;
    };

    void  updateStats(uint32_t framesRx, uint32_t crcErr);
    Stats getStats() const;

    // Change tracking
    bool     hasChanges() const;
    void     clearChanges();
    uint32_t getChangeFlags() const;

    // Change flag bits
    static constexpr uint32_t CHANGE_SENSORS   = (1UL << 0);
    static constexpr uint32_t CHANGE_SETTINGS  = (1UL << 1);
    static constexpr uint32_t CHANGE_STATE     = (1UL << 2);
    static constexpr uint32_t CHANGE_INSTALLER = (1UL << 3);
    static constexpr uint32_t CHANGE_SCHEDULE  = (1UL << 4);

private:
    Optitronic2State();

    // Register storage: we use a flat array indexed by register address.
    // The address space is sparse but bounded (max 0x0320 + some = ~810 regs).
    // We'll use a compact approach with known blocks only.
    static constexpr uint16_t MAX_REGISTER_ADDR = 0x0321;

    uint16_t mRegisters[MAX_REGISTER_ADDR];
    bool     mRegisterValid[MAX_REGISTER_ADDR];

    TaskHandle_t      mNotify;
    SemaphoreHandle_t mMutex;
    uint32_t          mChangeFlags;

    Stats mStats;

    void notifyListener(uint32_t changeFlag);
};

}  // namespace aquamqtt

#endif  // AQUAMQTT_OPTITRONIC2_STATE_H
