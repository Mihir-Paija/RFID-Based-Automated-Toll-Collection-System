
#include <SPI.h>
#include <Servo.h>
#include <MFRC522.h>
#include <Keypad.h>
#include <Buzzer.h>

#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance.

#define IRPin1 4
#define IRPin2 3

const int buzzer = 2;

const byte ROWS = 4;  //four rows
const byte COLS = 3;  //four columns

char keys[ROWS][COLS] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' }
};

byte rowPins[ROWS] = { 8, 9, 10, A3 };  //connect to the row pinouts of the keypad
byte colPins[COLS] = { 5, 6, 7 };       //connect to the column pinouts of the keypad

//Create an object of keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

Servo servo;

char key;

#define charge 200
int cardBalance = 400;

int car = 0;  //1 for enter and 0 for exit
int check = 0;

void setup() {
  Serial.begin(9600);  // Initiate a serial communication
  SPI.begin();         // Initiate  SPI bus
  mfrc522.PCD_Init();  // Initiate MFRC522
  Serial.println("Approximate your card to the reader...");
  Serial.println();

  pinMode(IRPin1, INPUT);
  pinMode(IRPin2, INPUT);

  pinMode(buzzer, OUTPUT);  // Set buzzer - pin 2 as an output

  servo.attach(A4);
}
void loop() {
  if (digitalRead(IRPin1) == 1 && car == 0) {
    return;
  } else {
    car = 1;  //car enter
  }

  //Following code run only when car entered(i.e. car == 1)
  Serial.println("Approximate your card to the reader...");
  Serial.println();
  
  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content = "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  if (content.substring(1) == "43 EA 49 A0")  //change here the UID of the card/cards that you want to give access ... use Dumpinfo code to get UID of perticular card/tag
  {
    Serial.println("Authorized access");
    Serial.println();
    Serial.print("Available Balance: ");
    Serial.println(cardBalance);

    if (cardBalance > charge)
      cardBalance -= charge;

    else {
      Serial.println("Insufficient Balance");
      Serial.print("Card Balance: ");
      Serial.println(cardBalance);
      Serial.println("Please enter the recharge amount");
      int amount = 0;

      Serial.print("Key Pressed : ");
      while (1) {
        key = keypad.getKey();  // Read the key
        if (key >= '0' && key <= '9') {
          amount = amount * 10 + (key - '0');
          Serial.print(key);
        }

        if (key == '#')
          break;
      }
      Serial.println();
      Serial.println("Successful Recharge!");
      int add = amount - charge;
      cardBalance += add;

      Serial.print("Current Balance: ");
      Serial.println(cardBalance);
    }

    delay(100);
    servo.write(90);  // tell servo to go to position in variable 'pos'
    delay(15);        // waits 15 ms for the servo to reach the position

    tone(buzzer, 700);  // Send 1KHz sound signal...
    delay(500);         // ...for 0.5 sec
    noTone(buzzer);     // Stop sound...
    Serial.println("Have a safe journey");
    delay(100);
    Serial.println("Move Ahead");
  
    while (digitalRead(IRPin2) != 0) {
    }
    Serial.println("Closing");
    car = 0; //car exits

    servo.write(0);  // tell servo to go to position in variable 'pos'
    delay(15);       // waits 15 ms for the servo to reach the position

    delay(1000);
  }

  else {
    Serial.println("Access denied");
    delay(1000);
    car = 0;
  }
 
}