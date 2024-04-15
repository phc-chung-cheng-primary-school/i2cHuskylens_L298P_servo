/***************************************************
 HUSKYLENS An Easy-to-use AI Machine Vision Sensor
 <https://www.dfrobot.com/product-1922.html>
 
 ***************************************************
 This example shows the basic function of library for HUSKYLENS via I2c.
 
 Created 2020-03-13
 By [Angelo qiao](Angelo.qiao@dfrobot.com)
 
 GNU Lesser General Public License.
 See <http://www.gnu.org/licenses/> for details.
 All above must be included in any redistribution
 ****************************************************/

/***********Notice and Trouble shooting***************
 1.Connection and Diagram can be found here
 <https://wiki.dfrobot.com/HUSKYLENS_V1.0_SKU_SEN0305_SEN0336#target_23>
 2.This code is tested on Arduino Uno, Leonardo, Mega boards.
 ****************************************************/



#include "HUSKYLENS.h"
HUSKYLENS huskylens;

//HUSKYLENS green line >> SDA; blue line >> SCL

#include <Servo.h>
Servo servo;
//伺服馬達
void printResult(HUSKYLENSResult result);

int trigPin = 5;    // 發射波
int echoPin = 6;    // 接收波
long duration, cm, leftcm, rightcm;

void setup() {
    Serial.begin(115200);
    Wire.begin();
    while (!huskylens.begin(Wire))
    {
        Serial.println(F("Begin failed!")); //  HUSKYLENS啟動錯誤
        Serial.println(F("1.Please recheck the \"Protocol Type\" in HUSKYLENS (General Settings>>Protocol Type>>Serial 9600)")); //確認埠號設定為9600
        Serial.println(F("2.Please recheck the connection.")); //確認接線正確
        //確認huskylens是否連線
        delay(100);
    }
    //定義輸出輸入腳位
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    pinMode(10, OUTPUT); //motorA
    pinMode(12, OUTPUT); //motorA
    pinMode(11, OUTPUT); //motorB
    pinMode(13, OUTPUT); //motorB
    //L298P-pin10、pin11設定PWM值
    //L298P-pin12、pin13設定馬達正反轉
    servo.attach(9);
    servo.write(90);
    delay(50);
}

void loop() {

  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH); //將時間換算成距離
  cm = (duration/2) / 29.1;     // 換算成公分
  //Serial.println(duration);
  //Serial.println(cm);

  if (cm < 20) {
            mbackward();//後退
            delay(1000);
            stop();


            servo.write(145); //左轉
            delay(1500);
            digitalWrite(trigPin, LOW);
            delayMicroseconds(5);
            digitalWrite(trigPin, HIGH);
            delayMicroseconds(10);
            digitalWrite(trigPin, LOW);
            pinMode(echoPin, INPUT);
            duration = pulseIn(echoPin, HIGH); //將時間換算成距離
            leftcm = (duration/2) / 29.1;     // 換算成公分 左邊距離
            //Serial.println(leftcm);

            servo.write(35);//右轉
            delay(1500);
            digitalWrite(trigPin, LOW);
            delayMicroseconds(5);
            digitalWrite(trigPin, HIGH);
            delayMicroseconds(10);
            digitalWrite(trigPin, LOW);
            pinMode(echoPin, INPUT);
            duration = pulseIn(echoPin, HIGH); //將時間換算成距離
            rightcm = (duration/2) / 29.1;     // 換算成公分 左邊距離
            //Serial.println(rightcm);

            servo.write(90);
            delay(1500);
            //回到中央

            if( rightcm > leftcm){
              mright();//右旋轉 3000ms 
              delay(3000);
              stop();
            }else if( leftcm > rightcm){
              mleft();//左旋轉 3000ms
              delay(3000);
              stop();
            }
          } else if (cm > 20) {
            delay(20);//huskylen偵測等待時間
            if (!huskylens.request()) Serial.println(F("Fail to request data from HUSKYLENS, recheck the connection!")); // HUSKYLENS沒有傳回資料 確認連線正確
            else if(!huskylens.isLearned()) Serial.println(F("Nothing learned, press learn button on HUSKYLENS to learn one!")); // HUSKYLENS沒有學習內容 確認HUSKYLENS至少學到一個
            else if(!huskylens.available()) Serial.println(F("No block or arrow appears on the screen!")); // 沒有框框或是箭頭出現在螢幕
            //else if(!huskylens.available())  // 沒有框框或是箭頭出現在螢幕中，執行超音波模式
            else 
            {
                Serial.println(F("###########"));
                while (huskylens.available())
                {
                    
                    HUSKYLENSResult result = huskylens.read();
                    printResult(result);
                    if(result.width > 120){
                      mright(); // 如果框框的寬度大於120 馬達向右轉 
                      delay(1000);
                      stop();
                    }else{
                      mforward();
                    };
                }
            }
            
            
            mforward();//前進
        }

}

void printResult(HUSKYLENSResult result){
    if (result.command == COMMAND_RETURN_BLOCK){
        Serial.println(String()+F("Block:xCenter=")+result.xCenter+F(",yCenter=")+result.yCenter+F(",width=")+result.width+F(",height=")+result.height+F(",ID=")+result.ID);
    }
    else if (result.command == COMMAND_RETURN_ARROW){
        Serial.println(String()+F("Arrow:xOrigin=")+result.xOrigin+F(",yOrigin=")+result.yOrigin+F(",xTarget=")+result.xTarget+F(",yTarget=")+result.yTarget+F(",ID=")+result.ID);
    }
    else{
        Serial.println("Object unknown!");
    }
}

void mbackward() {
  analogWrite(10, 255);
  digitalWrite(12, HIGH);
  analogWrite(11, 255);
  digitalWrite(13, LOW);
}
void mforward() {
  analogWrite(10, 255);
  digitalWrite(12, LOW);
  analogWrite(11, 255);
  digitalWrite(13, HIGH);
}
void mleft() {
  analogWrite(11, 255);
  digitalWrite(13, HIGH);
  analogWrite(10, 255);
  digitalWrite(12, HIGH);
}
void mright() {
  analogWrite(11, 255);
  digitalWrite(13, LOW);
  analogWrite(10, 255);
  digitalWrite(12, LOW);
}
void stop() {
  analogWrite(11, 0);
  analogWrite(10, 0);
}


