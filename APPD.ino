/////////////////////////////////////////////////////
// 
//    AUTOMATED PHOTO-SENSITIVE POINTING DEVICE
//    Written By: Connor Edwards
//    Date: 21/2/16
//    (c) Noval Technologies, 2016
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
// SETTING UP THE STEPPERS
/////////////////////////////////////////////////////
int stepper_driver_2_ccw[] = {
    11, 10, 12, 9
};

int stepper_driver_1_ccw[] = {
    8, 5, 7, 6 
};

int stepper_driver_2_cw[] = {
    9, 12, 10, 11
};

int stepper_driver_1_cw[] = {
    6, 7, 5, 8
};

// A variable to track the steppers
int stepper_1_pos = 0;
int stepper_2_pos = 0;

// Setting the number of inputs
const int num_of_inputs = 4;

// Data for getting the data form the Serial bus
int SERIAL_turn_amount = 0;

/////////////////////////////////////////////////////
// SETTING UP THE LDR
/////////////////////////////////////////////////////
const int time_between_pass = 10000; // 60000ms = 1 Minute
const int pass_amount = 12;

// Inputs of the LDR 
int ldrPin = A4;
int dataBuffer[pass_amount];

/////////////////////////////////////////////////////
// CODE RAN ON STARTUP
/////////////////////////////////////////////////////
void setup() {
    
    // Initialising the inputs
    init_inputs();
    
    // Starting the Serial bus @ 9600 baud
    Serial.begin(9600);
    
    // Printing welcome message
    Serial.println("DEBUG MODE - SERIAL STARTED @ 9600 BAUD");
    
    // Start LED blink
    led_blink(1000);
    
}

/////////////////////////////////////////////////////
// CONTINUOUS PROGRAM LOOP
/////////////////////////////////////////////////////
void loop() {
    
    // Clearing out the data buffer
    clean_buffer(dataBuffer);
    
    /////////////////////////////////////////////////////
    // FOR DEBUGGING PURPOSES
    /////////////////////////////////////////////////////    
    // Checking if there is data on the Serial bus
    if (Serial.available() > 0) {
        // There is data, so get it
        SERIAL_turn_amount = Serial.parseInt();
        // Printing it back to the User:
        Serial.print("I received a: ");
        Serial.println(SERIAL_turn_amount);
        // Calling the move function
        move_stepper(stepper_driver_1_ccw, SERIAL_turn_amount, 50);
        delay(1000);
        move_stepper(stepper_driver_1_cw, SERIAL_turn_amount, 50);
        delay(1000);
        move_stepper(stepper_driver_2_ccw, SERIAL_turn_amount, 50);
        delay(1000);
        move_stepper(stepper_driver_2_cw, SERIAL_turn_amount, 50);
    }
    
    /////////////////////////////////////////////////////
    // GENERAL APPLICATION
    /////////////////////////////////////////////////////
    // Taking a pass of the room with the LDR connected to STEPPER 1
    for (int pass_step = 0; pass_step < pass_amount; pass_step++) {
        // Incrementing the stepper
        move_stepper(stepper_driver_1_cw, 1, 50);
        // For each step, we pass the analogue read to the dataBuffer
        int ldrRead = analogRead(ldrPin);
        // Adding it to the buffer
        dataBuffer[pass_step] = ldrRead;
        
        Serial.println(dataBuffer[pass_step]);
        
        // Slight delay
        delay(1000);
    }
    
    evaluate_data(dataBuffer);
    
    delay(time_between_pass);
    
}

/////////////////////////////////////////////////////
// FUNCTIONS
/////////////////////////////////////////////////////

// Function for flashing the LED on startup
void led_blink(int flash_len) {
    for (int _step = 0; _step < 5; _step++) {
        digitalWrite(13, HIGH);
        delay(1000);
        digitalWrite(13, LOW);
        delay(flash_len);
    }
}

// Function for initialising inputs
void init_inputs() {
    
    // Setting the LDR independently
    pinMode(ldrPin, INPUT);
    
    // Looping through all the outputs and setting their direction and status
    for (int index = 0; index < num_of_inputs; index++) {
        
        // Setting pins LOW and as OUTPUTS 1
        pinMode(stepper_driver_1_cw[index], OUTPUT);
        digitalWrite(stepper_driver_1_cw[index], LOW);
        
        // Setting pins LOW and as OUTPUTS 2
        pinMode(stepper_driver_2_cw[index], OUTPUT);
        digitalWrite(stepper_driver_2_cw[index], LOW);
        
    }   
    
}

// Function for actuating the stepper shaft
void move_stepper(int stepper_to_control[], int move_amount, int _speed) {
    
    // Attempting to move the Rotor
    for (int counter = 0; counter < move_amount; counter++) {
        // Magnetising each coil
        for (int index = 0; index < num_of_inputs; index++) {
            digitalWrite(stepper_to_control[index], HIGH);
            delay(_speed);
            digitalWrite(stepper_to_control[index], LOW);
        }   
    }
    
}

// Function used to evaluate the collected data
void evaluate_data(int data_buffer[]) {
    
    // Homing the LDR stepper
    move_stepper(stepper_driver_1_ccw, 12, 50);
    
    // Homing the 2nd stepper
    move_stepper(stepper_driver_2_cw, stepper_2_pos, 50);
    
    // Resetting the position of stepper 2
    stepper_2_pos = 0;
    
    // Brief delay
    delay(1000);
    
    // Getting the data
    int pointer_buffer = 0; // Variable to store the pointer
    int greatest_value = 0; // Variable to store the greatest value
    
    // Relaying that info to the 2nd stepper
    for (int stackPointer = 0; stackPointer < 12; stackPointer++) {
        // Getting the data
        int curr_data = dataBuffer[stackPointer];
        // Checking it against the value currently in the greatest buffer
        if (curr_data > greatest_value) {
            // It's greater so store the new value
            greatest_value = curr_data;
            // Set the new pointer
            pointer_buffer = stackPointer;
        }
    }
    
   // Moving the 2nd stepper
   move_stepper(stepper_driver_2_ccw, pointer_buffer, 50);
   
   // Setting the position
   stepper_2_pos = pointer_buffer;
    
}

// Function used to clean the buffer
void clean_buffer(int buffer_to_clean[]) {
    // Cleaning each node of the buffer
    for (int index = 0; index < pass_amount; index++) {
        buffer_to_clean[index] = 0;
    }
}
