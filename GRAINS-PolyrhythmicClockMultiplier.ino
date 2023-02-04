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



void
setup(void)
{
  pinMode(A0, INPUT); //A
  pinMode(A1, INPUT); //B
  pinMode(A3, INPUT); //C
  pinMode(A2, INPUT); //D
  pinMode(0, OUTPUT); //M(C)
  pinMode(1, OUTPUT); //S(C)
}

boolean gate = false;
boolean oldgate = false;
boolean beatone = false;
boolean combo = false;
boolean count = false;

uint64_t totaltime = 100;
uint64_t timer = 0;
uint64_t prevtimer = 0;

int pulsewidthstop1 = 0;
int pulsewidthstop0 = 0;
int poly1 = 1;
int poly0 = 1;

void loop() {

  gate = analogRead(A1) > 700;
  if (gate != oldgate) { //only trigger on change
    if (gate) { 
      totaltime = (timer + prevtimer) / 2; //average of latest beat time in ticks and previous beat time 
                                           //(ran into issues with slight fluctuations, this makes them
                                           //less notable in a very botched way. probably a proper solution
                                           //to this but it works for now
      prevtimer = timer; 
      timer = 0; //reset timer
      beatone = true; //ensure both fire on beat one. wasn't doing this sometimes. i could debug or just do this
      
    } else {
      if (analogRead(A1) > 100) { //while b is the clock input, that doesn't stop us hiding some analogue
                                  //in that binary. 100 is just below west
        combo = true; //enable combo mode
      } else {
        combo = false;
      }
    }

    oldgate = gate;
  }
  poly1 = (analogRead(A3) >> 6) + 1; //read c and
  poly0 = (analogRead(A2) >> 6) + 1; //d respectively, convert them to a value from 1-16 inclusive
  
  if (timer%(totaltime/poly1) == 0 || beatone) { //the equation here checks if the current tick is
                                                 //a multiple of the beat time, devided by the 
                                                 //desired polyrhythm. also always if beat one
    digitalWrite(1, HIGH);
    pulsewidthstop1 = timer + ((analogRead(A0)-586) << 3); //calculate when the pulse should end
                                                           //based on A
  }
  if (timer%(totaltime/poly0) == 0 || beatone) { //see above. this is for the secondary output
    digitalWrite(0, HIGH);
    if (combo) {
      digitalWrite(1, HIGH); //combo mode
    }
    pulsewidthstop0 = timer + ((analogRead(A0)-586) << 3); //see above. for secondary output
    
  }
  if (timer == pulsewidthstop1) { //end pulse
    digitalWrite(1, LOW);
  }
  if (timer == pulsewidthstop0) { //end pulse
    digitalWrite(0, LOW);
    if (combo) {
      digitalWrite(1, LOW); //combo mode
    }
  }

  beatone = false;
  timer += 1;
}
