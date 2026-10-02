Purpose            
This is a temperature monitoring code to moniter temperature inputs

Build             
This is a C program compiled and tested in Visual Studio Code

Usage
This code takes temperature inputs and prints a summary based off the data

Mode behavior
Normal and Warning are logged, Warning is flagged. Failure is flagged and exluded.

Thresholds and units   
Temperature input is taken in mC, ok threshold is from 18C to 30C

Example output: 
./sensor_sim warning 5                                                                                       
sample=01, temperature=24.000 C status=OK                   
sample=02, temperature=24.750 C status=OK
sample=03, temperature=23.900 C status=OK
sample=04, temperature=31.500 C status=WARNING
sample=05, temperature=23.800 C status=OK
summary samples=5 valid=5 ok=4 warning=1 failure=0
temperature min=23.800 C max=31.500 C average=25.590 C

Design decisions
Implemented two functions to test whether mode and count were properly entered

Test procedure 
Can either manually test inputs or use the test-output.txt file to test output

Known limitations:
1.Can only enter temperature values from 1 to 100
2.Temperature inputs are limited to what is generated from the base reading function

Author/date - Jason Winters / October 2, 2026