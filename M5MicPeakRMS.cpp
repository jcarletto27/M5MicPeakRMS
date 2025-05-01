    #include "M5MicPeakRMS.h"

    // Constructor implementation (if needed)
    M5MicPeakRMS::M5MicPeakRMS() {
        // Initialize default values if necessary
    }

    // Destructor implementation (optional, for cleanup)
    // M5MicPeakRMS::~M5MicPeakRMS() {
    //     if (_micBuffer != nullptr) {
    //         free(_micBuffer);
    //         _micBuffer = nullptr;
    //     }
    // }

    /**
     * @brief Initializes the microphone and necessary resources.
     * Accepts the correct m5::M5StickCPlus2 reference.
     */
    bool M5MicPeakRMS::begin(m5::M5StickCPlus2& stick) { // <--- Corrected Type
        if (_initialized) {
            Serial.println("[M5MicPeakRMS] Already initialized.");
            return true; // Already initialized
        }

        Serial.println("[M5MicPeakRMS] Initializing...");
        _stickPtr = &stick; // Store the reference to the M5StickCPlus2 object

        // 1. Allocate Buffer
        Serial.println("[M5MicPeakRMS] Allocating Mic Buffer...");
        _micBuffer = (int16_t*)heap_caps_malloc(_bufferSizeBytes, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
        if (_micBuffer == nullptr) {
            Serial.println("[M5MicPeakRMS] PSRAM allocation failed, trying internal DMA memory...");
            _micBuffer = (int16_t*)heap_caps_malloc(_bufferSizeBytes, MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA);
        }
        if (_micBuffer == nullptr) {
            Serial.println("[M5MicPeakRMS] !!! Failed to allocate Mic buffer!");
            return false; // Allocation failed
        }
        Serial.printf("[M5MicPeakRMS] Mic Buffer allocated (%d bytes) at %p\n", _bufferSizeBytes, _micBuffer);

        // 2. Initialize Mic
        // Assumes speaker state is handled by the calling sketch.
        Serial.println("[M5MicPeakRMS] Attempting to initialize Mic...");
        // Use the stored pointer to access the Mic object
        if (!_stickPtr->Mic.begin()) {
            Serial.println("[M5MicPeakRMS] !!! Failed to initialize Mic!");
            free(_micBuffer); // Clean up allocated buffer
            _micBuffer = nullptr;
            return false; // Mic init failed
        }
        Serial.println("[M5MicPeakRMS] Mic Initialized Successfully.");

        _initialized = true;
        _peakRmsValue = 0.0f; // Start with peak at 0
        _newPeakFlag = false;
        Serial.println("[M5MicPeakRMS] Initialization Complete.");
        return true;
    }

    /**
     * @brief Reads audio, calculates RMS, and updates the peak value.
     */
    void M5MicPeakRMS::update() {
        if (!_initialized || _micBuffer == nullptr || _stickPtr == nullptr) {
            return; // Not initialized or error state
        }

        _newPeakFlag = false; // Reset flag at the start of each update

        // Attempt to record audio using the stored pointer
        bool success = _stickPtr->Mic.record(_micBuffer, _bufferSamples, MIC_LIB_SAMPLE_RATE);

        if (success) {
            // Data was read successfully, calculate RMS
            float currentRms = _calculateRMS(_micBuffer, _bufferSamples);

            // Check if this RMS value is higher than the stored peak
            if (currentRms > _peakRmsValue) {
                _peakRmsValue = currentRms; // Update the stored peak value
                _newPeakFlag = true; // Set the flag indicating a new peak
            }
        }
    }

    /**
     * @brief Gets the current peak RMS value.
     */
    float M5MicPeakRMS::getPeakRMS() {
        return _peakRmsValue;
    }

    /**
     * @brief Resets the stored peak RMS value.
     */
    float M5MicPeakRMS::resetPeak() {
        _peakRmsValue = 0.0f;
        _newPeakFlag = false; // Reset flag on manual reset too
        Serial.println("[M5MicPeakRMS] Peak Reset.");
        return _peakRmsValue;
    }

    /**
      * @brief Checks if a new peak was detected in the last update cycle.
      */
    bool M5MicPeakRMS::newPeakDetected() {
        return _newPeakFlag;
    }


    /**
     * @brief Internal function to calculate RMS.
     */
    float M5MicPeakRMS::_calculateRMS(int16_t* buffer, size_t samples) {
         if (samples == 0 || buffer == nullptr) {
            return 0.0f;
        }

        double sumOfSquares = 0.0;
        for (size_t i = 0; i < samples; i++) {
            double sampleValue = (double)buffer[i];
            sumOfSquares += sampleValue * sampleValue;
        }

        double meanSquare = sumOfSquares / samples;
        if (meanSquare < 0) meanSquare = 0; // Prevent issues with sqrt
        return sqrt(meanSquare);
    }
    
