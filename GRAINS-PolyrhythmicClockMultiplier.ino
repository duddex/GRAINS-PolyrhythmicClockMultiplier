/*****
 * The original code is written by timothyjackman for the wonkystuff core1.æ module
 * https://forum.aemodular.com/thread/2484/polyrhythmic-clock-multiplier
 * 
 * Changes:
 * Instead of pin 0 and pin 1 this code uses pin 8 and pin 11.
 * Please refer to https://wiki.aemodular.com/pmwiki.php/AeManual/GRAINS
 * 
 * below is the original comment from timothyjackman
 ****/

//polyrhythmic clock multiplier
//
//                    guide
//probably a good idea to set all knobs counter clockwise
//cv B - clock input (unaffected by pulsewidth, will take a moment to understand
//                    clock as it averages the last two beat times)
//knob A - pulsewidth (south west or just above is the sweet spot for typical use)
//knob B - combo mode on/off (while this is also the clock input, we can sneak
//                            a switch in using analogue values. counterclockwise
//                            sets combo mode [explained below] off, while between 
//                            west and north is on. no setting further than north though)
//knob C - set polyrhythm M from 1-16 beats per clock beat in
//knob D - set polyrhythm S from 1-16 beats per clock beat in
//of course knob C and D can be controled with CV if you wish
//m-c polyrhythm 1 out
//s-c polyrhythm 2 out
//
//                    combo mode
//outputs both polyrhythms out of m-c. s-c outputs as
//usual. ensure a short pulsewidth to mitigate overlap
//
//                    other ideas
//set a short pulsewidth and send in a fast clock to drive
//up to audio rates. turn on combo mode and go ham on knobs
//C & D for some gnarly sounds >:)
//
//also it can self clock if one of the polyrhythms is set
//to one. swap the clock input quickly for a wire connecting
//to itself. a little finicky but it's an option in a pinch
//
//
//
//written by timothyjackman referencing the core1 examples
//tested on core1.æ v1.1
//thanks wonkystuff for this awesome module ;)
/* End of original comment from timothyjackman */

//int PULSEWIDTH_INPUT = A0;
int POLY0_INPUT = A2;
int POLY1_INPUT = A1;
int GATE_INPUT = A4;
int OUTPUT1 = 11;
int OUTPUT2 = 8;

void setup(void) {
  Serial.begin(115200);

  //pinMode(PULSEWIDTH_INPUT, INPUT);
  pinMode(POLY0_INPUT, INPUT);
  pinMode(POLY1_INPUT, INPUT);
  pinMode(GATE_INPUT, INPUT);
  pinMode(OUTPUT1, OUTPUT); // OUT (set switch to Grains mode ("G"))
  pinMode(OUTPUT2, OUTPUT); // D

  Serial.println("GRAINS-PolyrhythmicClockMultiplier");
}

boolean gate = false;
boolean oldgate = false;
boolean beatone = false;

uint64_t totaltime = 100;
uint64_t timer = 0;
uint64_t prevtimer = 0;

int pulsewidthstop1 = 0;
int pulsewidthstop0 = 0;
int poly1 = 1;
int poly0 = 1;

void loop() {
  gate = analogRead(GATE_INPUT) > 700;
  if (gate != oldgate) { //only trigger on change
    if (gate) {
      Serial.print("new trigger - ");
      char sbuf[50];
      sprintf(sbuf, "%ld", timer);
      Serial.print("timer: "); Serial.println(sbuf);

      totaltime = (timer + prevtimer) / 2; //average of latest beat time in ticks and previous beat time 
                                           //(ran into issues with slight fluctuations, this makes them
                                           //less notable in a very botched way. probably a proper solution
                                           //to this but it works for now
      prevtimer = timer; 
      timer = 0; //reset timer
      beatone = true; //ensure both fire on beat one. wasn't doing this sometimes. i could debug or just do this
      
    }

    oldgate = gate;
  }

  poly1 = (analogRead(POLY1_INPUT) >> 6) + 1; //read A1 and
  poly0 = (analogRead(POLY0_INPUT) >> 6) + 1; //A2 respectively, convert them to a value from 1-16 inclusive
  
  if (timer%(totaltime/poly1) == 0 || beatone) { //the equation here checks if the current tick is
                                                 //a multiple of the beat time, devided by the 
                                                 //desired polyrhythm. also always if beat one
    digitalWrite(OUTPUT2, HIGH);
    Serial.print("OUTPUT2 HIGH - poly1=");
    Serial.println(poly1);

    //pulsewidthstop1 = timer + ((analogRead(PULSEWIDTH_INPUT)-586) << 3); //calculate when the pulse should end based on A0
    pulsewidthstop1 = timer + 100;
  }

  if (timer%(totaltime/poly0) == 0 || beatone) { //see above. this is for the secondary output
    digitalWrite(OUTPUT1, HIGH);
    Serial.print("OUTPUT1 HIGH - poly0=");
    Serial.println(poly0);

    //pulsewidthstop0 = timer + ((analogRead(PULSEWIDTH_INPUT)-586) << 3); //see above. for secondary output
    pulsewidthstop0 = timer + 100;
  }

  if (timer == pulsewidthstop1) { //end pulse
    Serial.println("timer == pulsewidthstop1 - OUTPUT2 LOW");
    digitalWrite(OUTPUT2, LOW);
  }

  if (timer == pulsewidthstop0) { //end pulse
    Serial.println("timer == pulsewidthstop0 - OUTPUT1 LOW");
    digitalWrite(OUTPUT1, LOW);
  }

  beatone = false;
  timer += 1;
}
