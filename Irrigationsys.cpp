#include <stdio.h>
#include "pico/stdlib.h"
#include "i2c-display-lib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "pico/multicore.h"

//drySoilCount = amount of dry sensors, sensorNum = which sensor is dry
const uint VALVE_PINS[3] = {17, 18, 19};
bool activeValves[3] = {false, false, false};

uint16_t totalCycles = 0;
bool maintenanceMode = false;
uint8_t maintenanceSeconds = 30;
absolute_time_t maintenanceStart;

uint16_t pumpSpeed = 0;
uint16_t servoSpeed = 500;

bool enable = false;
 
volatile bool motion_detected = false;

const uint pump_pin = 16;
const uint SERVO_PIN = 14;

void readMoisture(bool drySensors[3])
{
    for(uint8_t i = 0; i < 3; i++)
    {
        adc_select_input(i);
        drySensors[i] = adc_read() > 1500;
    }
}

void controlValves(bool drySensors[3], bool enable)
{
    for(uint8_t i = 0; i < 3; i++)
    {
        gpio_put(VALVE_PINS[i], drySensors[i] && enable);
    }
}

//Pump PWM
void initPump(uint pumpPin) {
    gpio_set_function(pumpPin, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(pumpPin);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 64.f);
    pwm_config_set_wrap(&config, 39062.f);
    pwm_init(slice, &config, true);
}

// Set pump speed (0.0 = off, 1.0 = full speed)
void setPumpSpeed(uint pumpPin, float dutyCycle) {
    if (dutyCycle < 0.0f) dutyCycle = 0.0f;
    if (dutyCycle > 1.0f) dutyCycle = 1.0f;
    pwm_set_gpio_level(pumpPin, dutyCycle * 39062.0f);
}

// Convert microseconds to PWM level and set servo position
void setServoPWM(uint servoPin, float pulseWidth) {
    pwm_set_gpio_level(servoPin, (pulseWidth / 20000.0f) * 39062.0f);
}

void initServo(uint servoPin) {
    gpio_set_function(servoPin, GPIO_FUNC_PWM);
    uint sliceNumber = pwm_gpio_to_slice_num(servoPin);

    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 64.f);
    pwm_config_set_wrap(&config, 39062.f);

    pwm_init(sliceNumber, &config, true);
    //make 1500 or 90 deg neutral position
    setServoPWM(servoPin, 500); 
}

/*
//passive buzzer control (PWM)
//buzzer pwm
void initBuzzer(uint buzzer) {
    gpio_set_function(buzzer, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(buzzer);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 125.0f); 
    pwm_config_set_wrap(&config, 1000);      
    pwm_init(slice, &config, true);
    pwm_set_gpio_level(buzzer, 500);      
}

void toggleBuzzer(uint buzzer, bool toggle) {
    uint slice = pwm_gpio_to_slice_num(buzzer);
    pwm_set_enabled(slice, toggle);
}
*/

//30sec timer when pump starts
uint8_t cycles = 0;
uint8_t seconds[3] = {30, 30, 30};
bool timerActive[3] = {false, false, false};
absolute_time_t lastUpdate[3];

void startTimer(int valve)
{
    absolute_time_t now = get_absolute_time();

    if (!timerActive[valve])
    {
        seconds[valve] = 30;
        timerActive[valve] = true;
        lastUpdate[valve] = now;
    }

    if (timerActive[valve] && absolute_time_diff_us(lastUpdate[valve], now) >= 1000000)
    {
        lastUpdate[valve] = now;

        if (seconds[valve] > 0)
        {
            seconds[valve]--;
        }

        // work without a loop
        char ch[16];
        
        char f0[4], f1[4], f2[4];
        if (seconds[0] > 0) {
            snprintf(f0, sizeof(f0), "%02d ", seconds[0]); 
        } else { 
            snprintf(f0, sizeof(f0), "Fn ");
        }
        if (seconds[1] > 0){
            snprintf(f1, sizeof(f1), "%02d ", seconds[1]); 
        }else {
            snprintf(f1, sizeof(f1), "Fn ");
        }
        if (seconds[2] > 0){ 
            snprintf(f2, sizeof(f2), "%02d ", seconds[2]);
        } else {
            snprintf(f2, sizeof(f2), "Fn ");
        }
        // stitch into ch safely
        snprintf(ch, sizeof(ch), "%s%s%s", f0, f1, f2);

        lcd_setCursor(0, 0);
        lcd_print(ch);

        lcd_setCursor(1, 0);
        lcd_print("S1 S2 S3");

        /* 
        // Single string and spacing
        char ch[16];
        for (int i = 0; i < 3; i++)
        {
            if (seconds[i] > 0)
                sprintf(&ch[i * 3], "%02d ", seconds[i]); 
            else
                sprintf(&ch[i * 3], "Fn");            
        }
        lcd_clear();
        lcd_setCursor(0, 0);
        lcd_print(ch);
        sleep_ms(10);

        char sensors[16];
        sprintf(sensors, "S1 S2 S3");
        lcd_setCursor(1, 0);
        lcd_print(sensors);
        sleep_ms(10);
*/

        if (seconds[valve] == 0)
        {
            timerActive[valve] = false;
            gpio_put(VALVE_PINS[valve], false);
            activeValves[valve] = false;

             // Increment total cycles following each cycle completed
            totalCycles++;
            if (totalCycles >= 2)
            {
                maintenanceMode = true;
                maintenanceStart = now;
                maintenanceSeconds = 30;

                for(int i = 0; i < 3; i++)
                {
                    timerActive[i] = false;
                    gpio_put(VALVE_PINS[i], false);
                    activeValves[i] = false;
                }

                setPumpSpeed(pump_pin, 0);
                setServoPWM(SERVO_PIN, 500);
/* 
                lcd_clear();
                lcd_setCursor(0, 0);
                lcd_print("Maintenance");
                */
            }
        }

        //setServoPWM(SERVO_PIN, 500);
        //setPumpSpeed(pump_pin, 0);
        //pumpSpeed = 0;
        //servoSpeed = 500;
    }
}



//Declaration and initiation of motion sensor
const uint motion_sensor = 15;
const uint red = 13;

absolute_time_t last_motion_time;

void core1_entry() {
    

    while (true) {
        
        bool current_state = gpio_get(motion_sensor);

        if (current_state) 
        {
            // Motion just started
            motion_detected = true;
            last_motion_time = get_absolute_time();
        }

        //If motion_detected, check if 10 have passed
        if (motion_detected && absolute_time_diff_us(last_motion_time, get_absolute_time()) > 10 * 1000000) 
        {
            motion_detected = false;
            
        }

        sleep_ms(50);
        
    }
}



int main()
{
    stdio_init_all();
    adc_init();

    //LCD init
    lcd_setAddr(0x27);      
    lcd_init(0, 1);         
    sleep_ms(200);
    lcd_clear();

    //Moisture sensors
    const uint moisture_1 = 26;
    const uint moisture_2 = 27;
    const uint moisture_3 = 28;
    adc_gpio_init(moisture_1);
    adc_gpio_init(moisture_2);
    adc_gpio_init(moisture_3);
  
    //Valve pins
    for(uint i = 0; i < 3; i++)
    {
        gpio_init(VALVE_PINS[i]);
        gpio_set_dir(VALVE_PINS[i], GPIO_OUT); 
    }  
    
    //Servo and pump
    initServo(SERVO_PIN);
    gpio_init(pump_pin);
    gpio_set_dir(pump_pin, GPIO_OUT);
    gpio_put(pump_pin, false);
    initPump(pump_pin);

    //Motion sensor and LED
    gpio_init(motion_sensor);
    gpio_set_dir(motion_sensor, GPIO_IN);
    gpio_init(red);
    gpio_set_dir(red, GPIO_OUT);

    //if(maintenance > 0) multicore_launch_core1(core1_entry);

    while (true) 
    {

         //test if maintenance is required
            while (maintenanceMode) {
                absolute_time_t now = get_absolute_time();
                if (absolute_time_diff_us(maintenanceStart, now) >= 1000000){
                    maintenanceStart = now;
                    if (maintenanceSeconds > 0){
                    maintenanceSeconds--;
                    lcd_clear();
                    lcd_setCursor(0, 0);
                    lcd_print("Maintenance");

                    lcd_setCursor(1, 0);
                    char signMaintenance[8];
                    sprintf(signMaintenance, "Wait: %02d", maintenanceSeconds);
                    lcd_print(signMaintenance);
                    }else{
                    maintenanceMode = false;
                    totalCycles = 0;
                    lcd_clear();
                    }
                }
                sleep_ms(10);
                
            }
    
        bool drySensors[3] = {false, false, false};
        readMoisture(drySensors);
        gpio_put(red, false);
        adc_fifo_drain();

        // Count dry sensors
        int dryCount = 0;
        for (int i = 0; i < 3; i++) 
        {
            if (drySensors[i]) dryCount++;
        }

        if(!motion_detected)
        {   
            // Start new valves if any dry and not yet active
            for (int i = 0; i < 3; i++) 
            {
                if (drySensors[i] && !activeValves[i]) 
                {
                    gpio_put(VALVE_PINS[i], true);
                    activeValves[i] = true;
                    startTimer(i);  // start that valve’s timer
                }

                if (timerActive[i]) 
                {
                    startTimer(i);  // update that valve’s timer
                }
            }

            int activeCount = 0;
            for (int i = 0; i < 3; i++) 
            {
                if (activeValves[i]) activeCount++;
            }

            if (activeCount == 1) 
            {
                servoSpeed = 1000;
                pumpSpeed = 1000;
            } 
            else if (activeCount == 2) 
            {
                servoSpeed = 1800;
                pumpSpeed = 1800;
            } 
            else if (activeCount >= 3) 
            {
                servoSpeed = 2400;
                pumpSpeed = 2400;
            } 
            else 
            {
                servoSpeed = 500;
                pumpSpeed = 0;
            }

            setPumpSpeed(pump_pin, pumpSpeed);
            setServoPWM(SERVO_PIN, servoSpeed);
            //reduce a bit here and see
            sleep_ms(200);  
        }
        else
        {
            gpio_put(red, true);
            lcd_clear();
            lcd_print("ALERT");
            sleep_ms(50);

            setPumpSpeed(pump_pin, 0);
            setServoPWM(SERVO_PIN, 500);
        }
    }
}
