#include <M5StickCPlus2.h> // Use M5StickCP2 library
#include "M5MicPeakRMS.h" // Include the custom library header

// Create an instance of the library
M5MicPeakRMS micPeakRMS;

/**
 * @brief Main setup function.
 */
void setup() {
    Serial.begin(115200);
    delay(500);
    Serial.println("\n--- M5MicPeakRMS Library Example (StickCP2 Object with Reset Delay) ---");

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
    StickCP2.Display.println("Mic RMS Peak");
    StickCP2.Display.setTextSize(1);
    StickCP2.Display.setCursor(10, StickCP2.Display.height() - 20);
    StickCP2.Display.println("Press BtnA to Reset Peak");
    StickCP2.Display.setTextSize(2);

    // Display initial peak
    StickCP2.Display.fillRect(0, 40, StickCP2.Display.width(), 30, BLACK); // Use StickCP2.Display
    StickCP2.Display.setCursor(10, 40);
    StickCP2.Display.printf("Peak: %.2f", micPeakRMS.getPeakRMS());

    Serial.println("--- Setup Complete ---");
}

/**
 * @brief Main loop function.
 */
void loop() {
    StickCP2.update(); // Update button states using StickCP2.update()

    // Update the library (reads mic, calculates RMS, updates peak)
    micPeakRMS.update();

    // Check for reset button press
    if (StickCP2.BtnA.wasPressed()) { // Use StickCP2.BtnA
        Serial.println("BtnA pressed. Waiting 1 second before reset...");
        delay(1000); // Add a 1-second (1000 milliseconds) delay

        micPeakRMS.resetPeak(); // Reset peak in the library AFTER the delay
        // Update display immediately after reset
        StickCP2.Display.fillRect(0, 40, StickCP2.Display.width(), 30, BLACK); // Use StickCP2.Display
        StickCP2.Display.setCursor(10, 40);
        StickCP2.Display.printf("Peak: %.2f", micPeakRMS.getPeakRMS());
    }

    // Check if the library detected a new peak in the last update
    if (micPeakRMS.newPeakDetected()) {
         // Update display only when a new peak is found
        StickCP2.Display.fillRect(0, 40, StickCP2.Display.width(), 30, BLACK); // Use StickCP2.Display
        StickCP2.Display.setCursor(10, 40);
        StickCP2.Display.printf("Peak: %.2f", micPeakRMS.getPeakRMS());

        // Optional: Print to serial when new peak is detected by library
        Serial.print(">>> New Peak Detected by Library: ");
        Serial.println(micPeakRMS.getPeakRMS(), 2);
    }

    // No delay needed here usually, as update() handles timing via Mic.record()
    // delay(10); // Add if needed
}
