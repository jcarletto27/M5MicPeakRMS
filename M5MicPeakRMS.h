    #ifndef M5MicPeakRMS_h
    #define M5MicPeakRMS_h

    // Include the library that defines m5::M5StickCPlus2
    #include <M5StickCPlus2.h>
    #include <math.h>

    // Define configuration constants within the header or allow configuration
    #define MIC_LIB_BUFFER_SAMPLES 256
    #define MIC_LIB_SAMPLE_RATE 16000

    class M5MicPeakRMS {
    public:
        // Constructor
        M5MicPeakRMS();

        // Destructor (optional, for cleanup if needed)
        // ~M5MicPeakRMS();

        /**
         * @brief Initializes the microphone and necessary resources.
         * @param stick Reference to the initialized M5StickCPlus2 object.
         * Use m5::M5StickCPlus2 which is the actual type.
         * @return true if initialization was successful, false otherwise.
         */
        bool begin(m5::M5StickCPlus2& stick); // <--- Corrected Type

        /**
         * @brief Reads audio, calculates RMS, and updates the peak value.
         * Needs to be called repeatedly in the main loop.
         */
        void update();

        /**
         * @brief Gets the current peak RMS value recorded since the last reset.
         * @return The peak RMS value as a float.
         */
        float getPeakRMS();

        /**
         * @brief Resets the stored peak RMS value back to 0.0f.
         * @return The new peak value (always 0.0f).
         */
        float resetPeak();

        /**
         * @brief Checks if a new peak was detected in the last update cycle.
         * @return true if a new peak was detected, false otherwise.
         */
        bool newPeakDetected();


    private:
        /**
         * @brief Calculates the Root Mean Square (RMS) value of an audio buffer.
         * @param buffer Pointer to the buffer containing 16-bit audio samples.
         * @param samples The number of samples in the buffer.
         * @return The calculated RMS value as a float.
         */
        float _calculateRMS(int16_t* buffer, size_t samples);

        // Pointer to the M5StickCPlus2 object
        m5::M5StickCPlus2* _stickPtr = nullptr; // <--- Corrected Type

        // Internal state variables
        float _peakRmsValue = 0.0f;
        int16_t* _micBuffer = nullptr;
        const size_t _bufferSamples = MIC_LIB_BUFFER_SAMPLES;
        const size_t _bufferSizeBytes = MIC_LIB_BUFFER_SAMPLES * sizeof(int16_t);
        bool _initialized = false;
        bool _newPeakFlag = false; // Flag to indicate if a new peak was found in the last update
    };

    #endif // M5MicPeakRMS_h
    
