# M5MicPeakRMS Library for M5StickC Plus 2

## Description

This Arduino library provides a simple way to measure the peak Root Mean Square (RMS) audio level using the built-in microphone on the M5StickC Plus 2. It continuously samples the microphone, calculates the RMS value for short buffers, and keeps track of the highest RMS value encountered since the last reset.

The peak value can be reset using the device's side button (BtnA).

## Hardware Compatibility

* **M5StickC Plus 2**

## Dependencies

This library requires the following dependencies to be installed in your Arduino environment:

* **Platform:**
    * `m5stack:esp32` version `2.1.4` (or compatible)
* **Libraries:**
    * `M5StickCPlus2` version `1.0.1` (or compatible)
    * `M5Unified` version `0.2.5` (or compatible)
    * `M5GFX` version `0.2.6` (or compatible)

*You can typically install these libraries via the Arduino Library Manager.*

## Installation

1.  Download the `M5MicPeakRMS` library files (`M5MicPeakRMS.h`, `M5MicPeakRMS.cpp`).
2.  Create a folder named `M5MicPeakRMS` inside your Arduino sketchbook's `libraries` folder (e.g., `Documents/Arduino/libraries/M5MicPeakRMS`).
3.  Copy the downloaded `.h` and `.cpp` files into this new folder.
4.  Restart the Arduino IDE.

## Usage

1.  **Include Headers:** Include the main M5StickCPlus2 library and the custom library header in your sketch.
    ```cpp
    #include <M5StickCP2.h> // Or M5StickCPlus2.h depending on your preference
    #include "M5MicPeakRMS.h"
    ```
2.  **Create Instance:** Create an instance of the `M5MicPeakRMS` class.
    ```cpp
    M5MicPeakRMS micPeakRMS;
    ```
3.  **Initialize M5StickCPlus2:** In your `setup()` function, initialize the main M5StickCPlus2 object. Note that the library currently expects the `StickCP2` object instance, even if you include `<M5StickCPlus2.h>`. You also need to use `M5.config()`.
    ```cpp
    void setup() {
        Serial.begin(115200);
        auto cfg = M5.config(); // Required
        StickCP2.begin(cfg);    // Initialize the StickCP2 object

        // IMPORTANT: Disable speaker if using Mic
        StickCP2.Speaker.end();
        // ... other setup ...
    ```
4.  **Initialize Library:** Initialize the `M5MicPeakRMS` library instance, passing a reference to the initialized `StickCP2` object (which has the type `m5::M5StickCPlus2`).
    ```cpp
        if (!micPeakRMS.begin(StickCP2)) {
            Serial.println("Library initialization failed!");
            // Handle error
            while(true);
        }
        // ... display setup ...
    }
    ```
5.  **Update in Loop:** Call the `update()` method continuously in your main `loop()` function. This handles reading the microphone and updating the internal peak value. Also call `StickCP2.update()` to read button states.
    ```cpp
    void loop() {
        StickCP2.update(); // Update M5Stick button states etc.
        micPeakRMS.update(); // Update RMS calculation and peak tracking
        // ... rest of loop code ...
    }
    ```
6.  **Get Peak Value:** Use `getPeakRMS()` to retrieve the highest RMS value recorded since the last reset.
    ```cpp
    float peak = micPeakRMS.getPeakRMS();
    Serial.printf("Current Peak RMS: %.2f\n", peak);
    ```
7.  **Reset Peak:** Use `resetPeak()` to set the stored peak value back to zero. This is typically triggered by a button press.
    ```cpp
    if (StickCP2.BtnA.wasPressed()) {
        delay(1000); // Optional delay
        micPeakRMS.resetPeak();
        // Update display if needed
    }
    ```
8.  **Check for New Peak:** Use `newPeakDetected()` to see if the `update()` call found a new peak value in the most recent cycle. This is useful for optimizing display updates.
    ```cpp
    if (micPeakRMS.newPeakDetected()) {
        // Update display only when the peak changes
        StickCP2.Display.printf("Peak: %.2f", micPeakRMS.getPeakRMS());
    }
    ```

## Example Sketch (`PeakRMS_Example.ino`)

```cpp
#include <M5StickCP2.h> // Use M5StickCP2 library
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
API Referencebool begin(m5::M5StickCPlus2& stick): Initializes the library, microphone, and allocates buffers. Requires a reference to the initialized StickCP2 object (which has the type m5::M5StickCPlus2). Returns true on success, false on failure.void update(): Reads a buffer from the microphone, calculates its RMS, and updates the internal peak value if necessary. Call this repeatedly in your loop().float getPeakRMS(): Returns the highest RMS value recorded since the last call to resetPeak().float resetPeak(): Resets the stored peak RMS value to 0.0f.bool newPeakDetected(): Returns true if the last call to update() resulted in a new peak value being stored, false otherwise. Useful for optimizing display updates.NotesThe M5StickC Plus 2 microphone and speaker often share resources. The example code disables the speaker using StickCP2.Speaker.end(). Ensure you do this if you are not using the speaker.The library relies on the M5Stack library internals for microphone access via StickCP2.Mic.record().The begin() method currently expects the StickCP2 object instance, which has the type m5::M5StickCPlus2.License(Optional) Consider adding a license, e.g.:This project is licensed under the
