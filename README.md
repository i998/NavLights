# NavLights
Simple navigation lights for RC planes (or other vehicles) based on Arduino

Navigation Lights with Battery Voltage Alarm (optional - TODO)  
V0.8  - test version based on project:protothread Library

Websites: http://harteware.blogspot.com.au/2010/11/protothread-powerfull-library.html
           http://harteware.blogspot.com.au/2010/11/protothread-and-arduino-first-easy.html
           http://dunkels.com/adam/pt/examples.html               

Download: https://storage.googleapis.com/google-code-archive-downloads/v2/code.google.com/arduinode/pt.zip


-----------Design Notes-------------------------------------------------------
 
 1. The protothread Library allows to  run several threads in  parallel which updates global variables to 
    exchange information betweeh the therads. Local variables are not safe (protothreads do not save the 
    stack context across a blocking call, local variables are not preserved when the protothread blocks - 
    please see the protothread library documentation for details) so each thread uses its own set of local
    variables as a precaution to ensure other similar threads do not affect them.  
 
 2. There is a Control Thread (pt0) that can switch on and  off each other thread. Control Thread  can detect 
    some input changes such as battery voltage, switch off a normal blinking thread and switch on an "emergency"
    blinking thread and a sound tone thread. That is how different audio and  visual effects can be made.
 
 3. In  this  version voltage measurement for each LiPO cell has not been implemented. Instead the Control Thread 
    can read voltage (0-5v only!) from one of the Arduino input pins and  switch the threads just to  prove functionality. 
 
 4. DEBUG mode could potentially affect the Brightness Boost Thread  as debug info output via serial port  takes too 
    much of CPU time.  
 
 5. It has a separate thread to fade-in and fade out a LED using Arduino PWM function.
 
-----------End of Design Notes------------------------------------------------
