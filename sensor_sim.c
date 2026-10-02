// University of Maryland - Cyber Physical Systems Engineering
// Lab 4: Sensor Simulator
// September 30, 2026
// Jason Winters
// Intro to C - Dr. Nestor Tiglao - TA Reta Gela



//    ------------------------    LIBRARIES / CONSTANTS    ------------------------

// Libraries
#include <errno.h>    // '.h' is a header file that we are referencing
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Constants
#define DEFAULT_SAMPLE_COUNT 10U //U is an unsigned int constant (only magnitude)
#define MAX_SAMPLE_COUNT 100U
#define WARNING_LOW_mC 18000
#define WARNING_HIGH_mC 30000
#define FAILURE_CODE_mC (-999000)

// Define the mode and status types
typedef enum {
    MODE_NORMAL,         //0
    MODE_WARNING,        //1
    MODE_FAILURE         //2
} sensor_mode_t;

typedef enum {
    STATUS_OK,           //0
    STATUS_WARNING,      //1
    STATUS_FAILURE       //2
} sensor_status_t;



//    ----------------------------    FUNCTIONS    ----------------------------------

// Converts the mC degrees to C
// Input: temperature in mC
// Returns: temperature in C
static double to_celsius(int32_t temperature_mC)
{
    return (double)temperature_mC / 1000.0;
}


// Generate a derminisitic base reading based off of the 8 offsets
// Input: index of reading
// Returns: reading with an offset cooresponding to index % 8 within the offests_mC array [0 to 7]  
static int32_t base_reading(size_t index)
{
    static const int32_t offsets_mC[] = {
        0, 750, -100, 250, -200, 500, -250, 1000
    };
    const size_t length = sizeof offsets_mC / sizeof offsets_mC[0];
    return 24000 + offsets_mC[index % length];                            //return the offset added to base value 24000
}


// FIX, should run through here first, not purely base reading
static int32_t generate_reading(sensor_mode_t mode, size_t index)
{
    const int32_t base = base_reading(index);
        
        switch (mode) {
            /* TODO: normal mode returns base unchanged. */
            case MODE_NORMAL:
                return base;
            /* TODO: warning mode returns 31500 every fourth sample (indices 3, 7, 11, ...); otherwise return base. */
            case MODE_WARNING: 
                if ((index + 1) % 4 == 0) {
                    return 31500;
                } else {
                    return base;
                }
            /* TODO: failure mode returns FAILURE_CODE_mC every fifth sample (indices 4, 9, 14, ...); otherwise return base. */
            case MODE_FAILURE:
                if ((index + 1) % 5 == 0) {
                    return FAILURE_CODE_mC;
                } else {
                    return base;
                }
            default:
                return base; // Default case, should not happen
        }
        
        
    return base;
}


static sensor_status_t classify_reading(int32_t temperature_mC)
{
    /* TODO: test the failure sentinel first. */
    if (temperature_mC == FAILURE_CODE_mC) {
        return STATUS_FAILURE;
    }
    /* TODO: classify valid values using inclusive OK boundaries. */
    if (temperature_mC >= WARNING_LOW_mC && temperature_mC <= WARNING_HIGH_mC) {
        return STATUS_OK;
    }
    return STATUS_WARNING;
}


static const char *status_text(sensor_status_t status)
{
    switch (status) {
        case STATUS_OK: 
            return "OK";
        case STATUS_WARNING: 
            return "WARNING";
        case STATUS_FAILURE: 
            return "FAILURE";
    }
    return "UNKNOWN";
}

// Input: text and mode pointers
// Ouput: If valid text, set mode to the entered mode, elsewise return invalid.
static int parse_mode(const char *text, sensor_mode_t *mode)
{
    /* Return 0 on success and -1 on invalid arguments or mode text. */
    int valid = 0;
    int invalid = -1;
    
    // Check if text is NULL before testing value of *text
    if (text == NULL || text[0] == '\0') {
        fprintf(stderr, "Error: write valid argv[1] mode next time... exiting program\n");
        return invalid;
    }

    // Check if mode is valid address before testing value of *text
    if (mode == NULL){
        fprintf(stderr, "Error: trouble with mode pointer in parse_mode... exiting program\n");
        return invalid;
    }

    /* Use strcmp() to accept exactly: normal, warning, failure. */
    // strcmp(x,y) returns 0 if the strings (x/y) are equal
    if (strcmp(text, "normal")==0){
        *mode = MODE_NORMAL; 
        return valid;                       // Change mode to normal and return valid
    }
    if (strcmp(text, "warning")==0){
        *mode = MODE_WARNING; 
        return valid;                       // Change mode to warning and return valid
    }
    if (strcmp(text, "failure")==0){
        *mode = MODE_FAILURE; 
        return valid;                       // Change mode to failure and return valid
    }

    
    return invalid; // returns valid if nothing was valid

}

static int parse_count(const char *text, size_t *count)
{
    char *end = NULL;
    unsigned long value;


    if (text == NULL || count == NULL || text[0] == '\0') {
        return -1;
    }

        errno = 0;
        value = strtoul(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' || value < 1UL || value > MAX_SAMPLE_COUNT) return -1;
    
    *count = (size_t)value;

    return 0;
}


// ------------------------------------    MAIN FUNCTION    ------------------------
int main(int argc, char *argv[]) { 
    
    sensor_mode_t mode;
    size_t count = DEFAULT_SAMPLE_COUNT;

    int ok_count = 0;
    int warning_count = 0;
    int failure_count = 0;

    double average_temp = 0.0;             // Final Calculated Temp
    double average_temp_count = 0.0;       // Average Temp

    int maximum_temp;                // Max Temp
    int minimum_temp;                // Min Temp

    // Check for incorrect number of entered arguements
    if (argc < 2) {
        fprintf(stderr, "Too few arguements entered, there must be 2-3 arguments entered.\n");
        return EXIT_FAILURE; // Exit program
    }
    if (argc > 3) {
        fprintf(stderr, "Too many arguements entered, there must be 2-3 arguments entered.\n");
        return EXIT_FAILURE; // Exit program
    }



    // Check that the first arguement argv[1] is valid
    if (parse_mode(argv[1], &mode) != 0){
        fprintf(stderr, "Invalid [mode] entered, please enter either | normal | warning | failure |\n");
        return EXIT_FAILURE; // Exit program
    }


    // Check that the second arguement argv[2] is valid
    if (argc == 3 && parse_count(argv[2], &count) != 0){
        fprintf(stderr, "Invalid [count] entered, please enter number between 1-100\n");
        return EXIT_FAILURE; // Exit program
    }


    // As long as nothing is caught in the above if statements, run the for loop:


    for(size_t i = 0; i < count; i++){                     // Iterate through count number of samples



        // Generate our reading (FIX) 
        int sample_reading = (int)generate_reading(mode, i);                // Set 'reading' variable as current temperature reading



        //This if statement sets MIN/MAX values
        if (i == 0 && sample_reading != FAILURE_CODE_mC) {                                      // Set the initial max and min values
            maximum_temp = sample_reading;
            minimum_temp = sample_reading;
            average_temp_count += sample_reading;
        } else if (sample_reading == FAILURE_CODE_mC) {   // Ignore failure readings
            // Do nothing for failure readings
        } else {                                           // Update max and min based off current reading
            if (sample_reading > maximum_temp) {
                maximum_temp = sample_reading;
            }
            if (sample_reading < minimum_temp) {
                minimum_temp = sample_reading;
            }
            average_temp_count += sample_reading;
        }
   


        // FIX or UNDERSTAND WHAT THIS IS
        switch (classify_reading(sample_reading)) {
        case STATUS_OK:
            ++ok_count;
            break;
        case STATUS_WARNING:
            ++warning_count;
            break;
        case STATUS_FAILURE:
            ++failure_count;
            break;
        }


        
        // SAMPLE PRINT ONE-LINE UPDATE
        size_t j = i + 1;                                                                   // Set j as the sample number, index + 1
        if (classify_reading(sample_reading) == STATUS_FAILURE) {
            printf("sample=%02d, temperature=n/a status=%s\n",                           // Print sample info for failure sample
                (int)j, 
                status_text(classify_reading(sample_reading)));
        } else {
            printf("sample=%02d, temperature=%.3f C status=%s\n",                        // Print sample info for working sample
                (int)j, to_celsius(sample_reading), 
                status_text(classify_reading(sample_reading)));
        }

    
    }

    //  ----------------------------    END SUMMARY PRINT    ----------------------------

    average_temp = (average_temp_count / ((ok_count + warning_count)*1000));       // Calculate average temperature in Celsius

    printf("summary samples=%zu valid=%d ok=%d warning=%d failure=%d\n", 
        count, 
        ok_count + warning_count, 
        ok_count, 
        warning_count, 
        failure_count
    );
    printf("temperature min=%.3f C max=%.3f C average=%.3f C\n", 
        to_celsius(minimum_temp), 
        to_celsius(maximum_temp), 
        average_temp);

}




