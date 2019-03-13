#include <TM1637.h>
//#include <EEPROM.h>

#define K_MIN_UP (3)
#define K_MIN_DN (2)
#define K_SEC_UP (8)
#define K_SEC_DN (7)
#define K_GO     (4)
#define K_LOAD1  (1)
#define K_LOAD2  (0)
#define K_SAVE1  (9)
#define K_SAVE2  (10)
#define L_CAMERA (A2)
#define L_ON     (6)
#define L_OFF    (5)
#define RELAY    (A3)

TM1637 tm1637(A4,A5);

unsigned long last_key=millis(),  // kbd timer
              last_sec=millis(),  // tick
              last_light=millis();
int           exptime=240;          // time, sec
bool          b_go=0;             // enable flag
byte          et_h= 0, et_l= 0,   // exptime high,low
              etn_h=0, etn_l=0;   // new h,l

void setup(){
  pinMode(K_MIN_UP,INPUT_PULLUP);
  pinMode(K_MIN_DN,INPUT_PULLUP);
  pinMode(K_SEC_UP,INPUT_PULLUP);
  pinMode(K_SEC_DN,INPUT_PULLUP);
  pinMode(K_GO    ,INPUT_PULLUP);
  pinMode(K_LOAD1 ,INPUT_PULLUP);
  pinMode(K_LOAD2 ,INPUT_PULLUP);
  pinMode(K_SAVE1 ,INPUT_PULLUP);
  pinMode(K_SAVE2 ,INPUT_PULLUP);
  pinMode(L_CAMERA,OUTPUT);
  digitalWrite(L_CAMERA,0);
  pinMode(L_ON,    OUTPUT);
  digitalWrite(L_ON,0);
  pinMode(L_OFF,   OUTPUT);
  digitalWrite(L_OFF,1);
  pinMode(RELAY,   OUTPUT);
  digitalWrite(RELAY,0);
  tm1637.init();
  tm1637.set(6);
  tm1637.point(1);
  timeshow();
}

void timeshow(){
  int tmp;
  byte tmin,tsec,digit[4];
  tmp=exptime;
  tmin=tmp/60;
  tsec=tmp%60;
  digit[0]=tmin/10;
  digit[1]=tmin%10;
  digit[2]=tsec/10;
  digit[3]=tsec%10;
  tm1637.display(0,digit[0]);
  tm1637.display(1,digit[1]);
  tm1637.display(2,digit[2]);
  tm1637.display(3,digit[3]);
}
/*
void timeload(byte n){
  switch(n){
    case 1:
      et_h=EEPROM.read(2);
      et_l=EEPROM.read(3);
    break;
    case 2:
      et_h=EEPROM.read(4);
      et_l=EEPROM.read(5);
    break;
  }
}

void timesave(byte n){
  timeload(n);
  switch(n){
    case 1:
      if(et_h!=etn_h) EEPROM.write(2,etn_h);
      if(et_l!=etn_l) EEPROM.write(3,etn_l);
    break;
    case 2:
      if(et_h!=etn_h) EEPROM.write(4,etn_h);
      if(et_l!=etn_l) EEPROM.write(5,etn_l);
    break;
  }
}

void timeconv(byte dir){
  switch(dir){               // 1=time->EEPROM (store), 2=EEPROM->time (recover)
    case 1: etn_h=exptime>>8; etn_l=exptime-etn_h; break;
    case 2: exptime=et_h<<8+et_l; break;
  }
}
*/
void uvstart(){
  b_go=1;
  digitalWrite(L_ON,1);
  digitalWrite(L_OFF,0);
  digitalWrite(RELAY,1);
}

void uvstop(){
  b_go=0;
  digitalWrite(L_ON,0);
  digitalWrite(L_OFF,1);
  digitalWrite(RELAY,0);
}

void cameralight(){
  digitalWrite(L_CAMERA,1);
  last_light=millis();
}

void loop(){

  if(millis()-last_key>=100){
    if(!b_go){
      if(!digitalRead(K_MIN_UP)){ if(exptime<=600) exptime+=60; else exptime=660; timeshow(); }   // modify time
      if(!digitalRead(K_MIN_DN)){ if(exptime>=60)  exptime-=60; else exptime=0;   timeshow(); }
      if(!digitalRead(K_SEC_UP) && (exptime<660)){ exptime++;  timeshow(); }
      if(!digitalRead(K_SEC_DN) && (exptime>0))  { exptime--;  timeshow(); }
      if(!digitalRead(K_LOAD1)){ exptime=240; timeshow(); cameralight(); }   // EEPROM
      if(!digitalRead(K_LOAD2)){ exptime=420; timeshow(); cameralight(); }
      //if(!digitalRead(K_SAVE1)){ timeconv(1); timesave(1); timeshow(); }
      //if(!digitalRead(K_SAVE2)){ timeconv(1); timesave(2); timeshow(); }
    }

    if(!digitalRead(K_GO)){                                       // start/stop
      if(millis()-last_key<500) return;
      if(!b_go){
        if(exptime>0) uvstart();
      }else{
        uvstop();
      }
    }

    last_key=millis();
  }

  if(millis()-last_sec>=1000){
    if(b_go){
      if(exptime>0){
        exptime--;
        timeshow();
      }else uvstop();
    }
    last_sec=millis();
  }

  if(millis()-last_light>=20000) digitalWrite(L_CAMERA,0);
}
