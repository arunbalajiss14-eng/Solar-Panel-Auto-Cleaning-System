#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

// ===============================
// Wi-Fi
// ===============================

const char* ssid = "Solar_Wiper";
const char* password = "12345678";

WebServer server(80);

// ===============================
// Servo
// ===============================

Servo wiper;

const int SERVO_PIN = 18;

// Wiper movement limits
const int LEFT_ANGLE = 20;
const int RIGHT_ANGLE = 160;

// Current position
int angle = 20;

// Direction
int direction = 1;

// Wiper ON/OFF
bool wiperRunning = false;

// Movement speed
unsigned long previousMillis = 0;

// Increase this value for slower movement
// Decrease for faster movement
const int wiperSpeed = 20;


// ===============================
// Mobile Web Page
// ===============================

const char webpage[] PROGMEM = R"rawliteral(

<!DOCTYPE html>

<html>

<head>

<meta name="viewport"
      content="width=device-width, initial-scale=1">

<title>Solar Panel Wiper</title>

<style>

body {
    font-family: Arial;
    text-align: center;
    background-color: #111;
    color: white;
    padding-top: 40px;
}

h1 {
    color: #ff3333;
    font-size: 32px;
}

p {
    font-size: 18px;
}

button {
    width: 240px;
    height: 65px;
    margin: 12px;
    border: none;
    border-radius: 12px;
    font-size: 21px;
    font-weight: bold;
}

.start {
    background-color: #00c853;
    color: white;
}

.stop {
    background-color: #ff1744;
    color: white;
}

.home {
    background-color: white;
    color: black;
}

</style>

</head>


<body>

<h1>☀ SOLAR PANEL WIPER</h1>

<p>Automatic Solar Panel Cleaning System</p>


<button class="start"
onclick="location.href='/start'">

START WIPER

</button>


<br>


<button class="stop"
onclick="location.href='/stop'">

STOP WIPER

</button>


<br>


<button class="home"
onclick="location.href='/home'">

HOME

</button>


</body>

</html>

)rawliteral";


// ===============================
// Home Page
// ===============================

void handleRoot()
{
    server.send(
        200,
        "text/html",
        webpage
    );
}


// ===============================
// Start Wiper
// ===============================

void handleStart()
{
    wiperRunning = true;

    server.sendHeader(
        "Location",
        "/"
    );

    server.send(
        303
    );

    Serial.println("WIPER STARTED");
}


// ===============================
// Stop Wiper
// ===============================

void handleStop()
{
    wiperRunning = false;

    server.sendHeader(
        "Location",
        "/"
    );

    server.send(
        303
    );

    Serial.println("WIPER STOPPED");
}


// ===============================
// Home Position
// ===============================

void handleHome()
{
    wiperRunning = false;

    angle = 90;

    direction = 1;

    wiper.write(90);

    server.sendHeader(
        "Location",
        "/"
    );

    server.send(
        303
    );

    Serial.println("WIPER HOME");
}


// ===============================
// Setup
// ===============================

void setup()
{

    Serial.begin(115200);


    // Servo

    wiper.setPeriodHertz(50);

    wiper.attach(
        SERVO_PIN,
        500,
        2400
    );


    // Start at left side

    angle = LEFT_ANGLE;

    wiper.write(angle);


    // ===========================
    // ESP32 Wi-Fi Access Point
    // ===========================

    WiFi.mode(WIFI_AP);

    WiFi.softAP(
        ssid,
        password
    );


    Serial.println();

    Serial.println(
        "=============================="
    );

    Serial.println(
        "SOLAR PANEL WIPER"
    );

    Serial.println(
        "=============================="
    );


    Serial.print(
        "WiFi Name: "
    );

    Serial.println(ssid);


    Serial.print(
        "Password: "
    );

    Serial.println(password);


    Serial.print(
        "IP Address: "
    );

    Serial.println(
        WiFi.softAPIP()
    );


    // ===========================
    // Web Server
    // ===========================

    server.on(
        "/",
        handleRoot
    );


    server.on(
        "/start",
        handleStart
    );


    server.on(
        "/stop",
        handleStop
    );


    server.on(
        "/home",
        handleHome
    );


    server.begin();


    Serial.println(
        "Web server started"
    );

}


// ===============================
// Loop
// ===============================

void loop()
{

    // Handle phone requests

    server.handleClient();


    // ===========================
    // Wiper Movement
    // ===========================

    if (wiperRunning)
    {

        unsigned long currentMillis =
            millis();


        if (
            currentMillis - previousMillis
            >= wiperSpeed
        )
        {

            previousMillis =
                currentMillis;


            // Change angle

            angle =
                angle + direction;


            // Reached right side

            if (
                angle >= RIGHT_ANGLE
            )
            {

                angle =
                    RIGHT_ANGLE;

                direction = -1;

            }


            // Reached left side

            if (
                angle <= LEFT_ANGLE
            )
            {

                angle =
                    LEFT_ANGLE;

                direction = 1;

            }


            // Move servo

            wiper.write(angle);

        }

    }

}