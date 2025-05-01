    #include <M5StickCPlus2.h> // Use M5StickCP2 library
    #include "M5MicPeakRMS.h" // Include the custom library header

    // Create an instance of the library
    M5MicPeakRMS micPeakRMS;

    // Variables to store previous display values to reduce flickering
    float prev_peak_rms = -1.0;
    float prev_current_rms = -1.0;


    /**
     * @brief Main setup function.
     */
    void setup() {
        Serial.begin(115200);
        delay(500);
        Serial.println("\n--- M5MicPeakRMS Library Example (Current & Peak) ---");

        // Initialize M5StickCPlus2 Core using StickCP2 object, but M5 config
        Serial.println("Initializing StickCP2...");
        auto cfg = M5.config(); // Keep M5.config() as previously requested
        StickCP2.begin(cfg);    // Use StickCP2.begin()
        Serial.println("StickCP2 Initialized.");

        // Disable speaker if needed (before initializing the library)
        Serial.println("Disabling Speaker...");
        StickCP2.Speaker.end(); // Use StickCP2.Speaker
        Serial.println("Speaker disabled.");

        // Initialize the library, passing the StickCP2 object
        Serial.println("Initializing M5MicPeakRMS library...");
        if (!micPeakRMS.begin(StickCP2)) { // Pass the StickCP2 object
            Serial.println("!!! Library initialization failed!");
            StickCP2.Display.fillScreen(RED); // Use StickCP2.Display
            StickCP2.Display.setCursor(10,10);
            StickCP2.Display.println("Lib Init FAIL");
            while(true); // Halt
        }
        Serial.println("Library Initialized.");

        // Setup Display using StickCP2 object
        StickCP2.Display.setRotation(1);
        StickCP2.Display.fillScreen(BLACK);
        StickCP2.Display.setTextColor(WHITE);
        StickCP2.Display.setTextSize(2);
        StickCP2.Display.setCursor(10, 10);
        StickCP2.Display.println("Mic RMS"); // Changed Title slightly
        StickCP2.Display.setTextSize(1);
        StickCP2.Display.setCursor(10, StickCP2.Display.height() - 20);
        StickCP2.Display.println("Press BtnA to Reset Peak");
        StickCP2.Display.setTextSize(2); // Back to normal size

        // Display initial values
        StickCP2.Display.setCursor(10, 40);
        StickCP2.Display.printf("Peak: %.2f", micPeakRMS.getPeakRMS());
        StickCP2.Display.setCursor(10, 70); // Position for Current RMS
        StickCP2.Display.printf("Curr: %.2f", micPeakRMS.getCurrentRMS());


        Serial.println("--- Setup Complete ---");
    }

    /**
     * @brief Main loop function.
     */
    void loop() {
        StickCP2.update(); // Update button states using StickCP2.update()

        // Update the library (reads mic, calculates RMS, updates peak)
        micPeakRMS.update();

        // Get current values
        float current_rms = micPeakRMS.getCurrentRMS();
        float peak_rms = micPeakRMS.getPeakRMS();

        // Check for reset button press
        if (StickCP2.BtnA.wasPressed()) { // Use StickCP2.BtnA
            Serial.println("BtnA pressed. Waiting 1 second before reset...");
            delay(1000); // Add a 1-second (1000 milliseconds) delay

            micPeakRMS.resetPeak(); // Reset peak in the library AFTER the delay
            peak_rms = micPeakRMS.getPeakRMS(); // Update local variable after reset

            // Force display update after reset
            prev_peak_rms = -1.0; // Force redraw by setting previous to invalid
            prev_current_rms = -1.0;
        }

        // --- Optimized Display Update ---
        bool update_display = false;

        // Update Peak display if it changed
        if (peak_rms != prev_peak_rms) {
             StickCP2.Display.fillRect(80, 40, StickCP2.Display.width() - 80, 20, BLACK); // Clear old peak value
             StickCP2.Display.setCursor(10, 40);
             StickCP2.Display.printf("Peak: %.2f", peak_rms);
             prev_peak_rms = peak_rms;
             update_display = true;
             Serial.printf(">>> New Peak: %.2f\n", peak_rms); // Print peak only when it changes
        }

        // Update Current display if it changed significantly (adjust threshold if needed)
        if (abs(current_rms - prev_current_rms) > 0.01) { // Update only if changed noticeably
             StickCP2.Display.fillRect(80, 70, StickCP2.Display.width() - 80, 20, BLACK); // Clear old current value
             StickCP2.Display.setCursor(10, 70); // Position for Current RMS
             StickCP2.Display.printf("Curr: %.2f", current_rms);
             prev_current_rms = current_rms;
             update_display = true;
        }

        // Optional: Print current RMS to serial continuously for debugging
        // Serial.printf("Current: %.2f, Peak: %.2f\n", current_rms, peak_rms);


        // No delay needed here usually, as update() handles timing via Mic.record()
        delay(20); // Small delay can sometimes help reduce display tearing/CPU load
    }
    
