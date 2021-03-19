/* Receive Incoming USB Host MIDI using functions.  As usbMIDI
   reads incoming messages, handler functions are run.
   See the InputRead example for the non-function alterative.

   This very long example demonstrates all possible handler
   functions.  Most applications need only some of these.
   This example is meant to allow easy copy-and-paste of the
   desired functions.

   Use the Arduino Serial Monitor to view the messages
   as Teensy receives them by USB MIDI

   You must select MIDI from the "Tools > USB Type" menu

   This example code is in the public domain.
*/

#include <USBHost_t36.h>

USBHost myusb;
MIDIDevice midi1(myusb);

elapsedMicros timeMicros;
elapsedMicros monotonicTimeMicros;
elapsedMillis timeMillis;
const byte encoderPin1 = 0;
const byte encoderPin1b = 33;
const byte motorPin1 = 1;
volatile int encoderCount1 = 0;
volatile int encoderCount1b = 0;
//volatile int avgEncoderCount1 = 0;
//volatile int avgEncoderCount1b = 0;
volatile int filteredEncoderCount1 = 0;
volatile int filteredEncoderCount1b = 0;
volatile int skipCount1 = 0;
volatile int skipCount1b = 0;
volatile int lastEncoderCountTimeMicros1 = 0;
volatile int lastEncoderCountTimeMicros1b = 0;
volatile int diffTimeMicros1 = 0;
volatile int diffTimeMicros1b = 0;
const float EMA_a = 0.4;
volatile float avgDiffTimeMicros1 = 0;
volatile float avgDiffTimeMicros1b = 0;
float lastPartialEncoderCount1 = 0;
float lastPartialEncoderCount1b = 0;
float filteredEncoderFrequency1y = 0;
float filteredEncoderFrequency1xy = 0;
float filteredEncoderFrequency1by = 0;
float filteredEncoderFrequency1bxy = 0;
int currentNote = 0;

void setup() {
  analogWriteFrequency(motorPin1, 36621.09); // get rid of annoying high whine by making the pwm frequency inaudible
  analogWriteResolution(12); // goes with the frequency above according to https://www.pjrc.com/teensy/td_pulse.html
  Serial.begin(115200);

  // Wait 1.5 seconds before turning on USB Host.  If connected USB devices
  // use too much power, Teensy at least completes USB enumeration, which
  // makes isolating the power issue easier.
  delay(1500);
  Serial.println("USB Host InputFunctions example");
  delay(10);
  myusb.begin();

  midi1.setHandleNoteOn(myNoteOn);
  midi1.setHandleNoteOff(myNoteOff);
  midi1.setHandleAfterTouchPoly(myAfterTouchPoly);
  midi1.setHandleControlChange(myControlChange);
  midi1.setHandleProgramChange(myProgramChange);
  midi1.setHandleAfterTouchChannel(myAfterTouchChannel);
  midi1.setHandlePitchChange(myPitchChange);
  // Only one of these System Exclusive handlers will actually be
  // used.  See the comments below for the difference between them.
  midi1.setHandleSystemExclusive(mySystemExclusiveChunk);
  midi1.setHandleSystemExclusive(mySystemExclusive);
  midi1.setHandleTimeCodeQuarterFrame(myTimeCodeQuarterFrame);
  midi1.setHandleSongPosition(mySongPosition);
  midi1.setHandleSongSelect(mySongSelect);
  midi1.setHandleTuneRequest(myTuneRequest);
  midi1.setHandleClock(myClock);
  midi1.setHandleStart(myStart);
  midi1.setHandleContinue(myContinue);
  midi1.setHandleStop(myStop);
  midi1.setHandleActiveSensing(myActiveSensing);
  midi1.setHandleSystemReset(mySystemReset);
  // This generic System Real Time handler is only used if the
  // more specific ones are not set.
  midi1.setHandleRealTimeSystem(myRealTimeSystem);

  pinMode(encoderPin1, INPUT_PULLDOWN);
  pinMode(encoderPin1b, INPUT_PULLDOWN);
  pinMode(motorPin1, OUTPUT);
  //attachInterrupt(digitalPinToInterrupt(encoderPin1), incrementEncoderCount1, CHANGE);
  //attachInterrupt(digitalPinToInterrupt(encoderPin1b), incrementEncoderCount1b, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderPin1), incrementEncoderCount1, RISING);
  attachInterrupt(digitalPinToInterrupt(encoderPin1b), incrementEncoderCount1b, RISING);
}

void loop() {
  // The handler functions are called when midi1 reads data.  They
  // will not be called automatically.  You must call midi1.read()
  // regularly from loop() for midi1 to actually read incoming
  // data and run the handler functions as messages arrive.
  myusb.Task();
  midi1.read();

  //analogWrite(motorPin1, map(currentNote, 36, 83, (120 * 4096)/256, 4096));
  analogWrite(motorPin1, (180 * 4096)/256);
  //analogWrite(motorPin1, (225 * 4096)/256);

  // every 50ms, print a value for the serial plotter
  if (timeMillis > 50) {
    timeMillis = 0;

    float timeSeconds = (float) timeMicros / 1000000;

    if (encoderCount1b > 0 || encoderCount1b > 0) {
      int microsSinceLastEncoderCount1 = monotonicTimeMicros - lastEncoderCountTimeMicros1;
      int microsSinceLastEncoderCount1b = monotonicTimeMicros - lastEncoderCountTimeMicros1b;
      float partialEncoderCount1 = (float) microsSinceLastEncoderCount1 / avgDiffTimeMicros1;
      float partialEncoderCount1b = (float) microsSinceLastEncoderCount1b / avgDiffTimeMicros1b;
      float encoderFrequency1 = (float) encoderCount1 / 20 / timeSeconds;
      float encoderFrequency1b = (float) encoderCount1b / 20 / (4 * timeSeconds);
      float filteredEncoderFrequency1 = (float) filteredEncoderCount1 / 20 / timeSeconds;
      float filteredEncoderFrequency1b = (float) filteredEncoderCount1b / 20 / (4 * timeSeconds);
      float encoderCount1x = (float) encoderCount1 + partialEncoderCount1 - lastPartialEncoderCount1;
      float encoderCount1bx = (float) encoderCount1b + partialEncoderCount1b - lastPartialEncoderCount1b;
      float filteredEncoderCount1x = (float) filteredEncoderCount1 + partialEncoderCount1 - lastPartialEncoderCount1;
      float filteredEncoderCount1bx = (float) filteredEncoderCount1b + partialEncoderCount1b - lastPartialEncoderCount1b;
      float encoderFrequency1x = encoderCount1x / 20 / timeSeconds;
      float encoderFrequency1bx = encoderCount1bx / 20 / timeSeconds;
      float filteredEncoderFrequency1x = (float) filteredEncoderCount1x / 20 / timeSeconds;
      float filteredEncoderFrequency1bx = (float) filteredEncoderCount1bx / 20 / (4 * timeSeconds);
      filteredEncoderFrequency1y = 0.6 * filteredEncoderFrequency1 + 0.4 * filteredEncoderFrequency1y;
      filteredEncoderFrequency1by = 0.6 * filteredEncoderFrequency1b + 0.4 * filteredEncoderFrequency1by;
      filteredEncoderFrequency1xy = 0.4 * filteredEncoderFrequency1x + 0.6 * filteredEncoderFrequency1xy;
      filteredEncoderFrequency1bxy = 0.4 * filteredEncoderFrequency1bx + 0.6 * filteredEncoderFrequency1bxy;
      // in case we ever get a nan here, let's recover from it
      if (isnan(filteredEncoderFrequency1xy)) { filteredEncoderFrequency1xy = 0; }
      if (isnan(filteredEncoderFrequency1bxy)) { filteredEncoderFrequency1bxy = 0; }
      lastPartialEncoderCount1 = partialEncoderCount1;
      lastPartialEncoderCount1b = partialEncoderCount1b;

      // Serial.print("Count1:");
      // Serial.print(encoderCount1);

      // Serial.print(",");
      // Serial.print("Filtered Count1:");
      // Serial.print(filteredEncoderCount1);

      // Serial.print(",");
      // Serial.print("Count1x:");
      // Serial.print(encoderCount1x);

      // Serial.print(",");
      // Serial.print("Filtered Count1x:");
      // Serial.print(filteredEncoderCount1x);

      //Serial.print(",");
      //Serial.print("Count1b:");
      //Serial.print(encoderCount1b);

      //Serial.print(",");
      //Serial.print("Filtered Count1b:");
      //Serial.print(filteredEncoderCount1b);

      //Serial.print(",");
      //Serial.print("Count1bx:");
      //Serial.print(encoderCount1bx);

      //Serial.print(",");
      //Serial.print("Filtered Count1bx:");
      //Serial.print(filteredEncoderCount1bx);

      //Serial.print(",");
      //Serial.print("Diff1:");
      //Serial.print(diffTimeMicros1);

      //Serial.print(",");
      //Serial.print("Diff1b:");
      //Serial.print(diffTimeMicros1b);

      //Serial.print(",");
      Serial.print("freq1:");
      Serial.print(encoderFrequency1);

      // Serial.print(",");
      // Serial.print("filtered_freq1:");
      // Serial.print(filteredEncoderFrequency1);

      // Serial.print(",");
      // Serial.print("freq1x:");
      // Serial.print(filteredEncoderFrequency1x);

      // Serial.print(",");
      // Serial.print("freq1y:");
      // Serial.print(filteredEncoderFrequency1y);

      Serial.print(",");
      Serial.print("freq1xy:");
      Serial.print(filteredEncoderFrequency1xy);

      Serial.print(",");
      Serial.print("freq1b:");
      Serial.print(encoderFrequency1b);

      // Serial.print(",");
      // Serial.print("filtered_freq1b:");
      // Serial.print(filteredEncoderFrequency1b);

      // Serial.print(",");
      // Serial.print("freq1bx:");
      // Serial.print(filteredEncoderFrequency1bx);

      // Serial.print(",");
      // Serial.print("freq1by:");
      // Serial.print(filteredEncoderFrequency1by);

      Serial.print(",");
      Serial.print("freq1bxy:");
      Serial.print(filteredEncoderFrequency1bxy);

      Serial.println("");
    } else {
      //Serial.print(".");
    }

    timeMicros = 0;
    encoderCount1 = 0;
    encoderCount1b = 0;
    filteredEncoderCount1 = 0;
    filteredEncoderCount1b = 0;
  }
}

void incrementEncoderCount1() {
  encoderCount1 += 1;
  diffTimeMicros1 = monotonicTimeMicros - lastEncoderCountTimeMicros1;
  if (diffTimeMicros1 < avgDiffTimeMicros1 / 2) {
    if (skipCount1 < 3) {
      // skip bad value!
      skipCount1 += 1;
      //Serial.print("1 Bad: ");
      //Serial.print(diffTimeMicros1);
      //Serial.print(", Avg: ");
      //Serial.print(avgDiffTimeMicros1);
      //Serial.println("");
    } else {
      // reset, we skipped too many times
      avgDiffTimeMicros1 = 0;
    }
  } else {
    skipCount1 = 0;
    filteredEncoderCount1 += 1;
    if (avgDiffTimeMicros1 == 0 && lastEncoderCountTimeMicros1 != 0) {
      // initial value
      avgDiffTimeMicros1 = diffTimeMicros1;
    } else if (avgDiffTimeMicros1 != 0) {
      avgDiffTimeMicros1 = EMA_a * diffTimeMicros1 + (1.0 - EMA_a) * avgDiffTimeMicros1;
      // in case we ever get a nan here, let's recover from it
      if (isnan(avgDiffTimeMicros1)) { avgDiffTimeMicros1 = 0; }
    }
  }
  lastEncoderCountTimeMicros1 = monotonicTimeMicros;
}

void incrementEncoderCount1b() {
  encoderCount1b += 1;
  diffTimeMicros1b = monotonicTimeMicros - lastEncoderCountTimeMicros1b;
  if (diffTimeMicros1b < avgDiffTimeMicros1b / 2) {
    if (skipCount1b < 3) {
      // skip bad value!
      skipCount1b += 1;
      //Serial.print("1b Bad: ");
      //Serial.print(diffTimeMicros1b);
      //Serial.print(", Avg: ");
      //Serial.print(avgDiffTimeMicros1b);
      //Serial.println("");
    } else {
      // reset, we skipped too many times
      avgDiffTimeMicros1b = 0;
    }
  } else {
    skipCount1b = 0;
    filteredEncoderCount1b += 1;
    if (avgDiffTimeMicros1b == 0 && lastEncoderCountTimeMicros1b != 0) {
      // initial value
      avgDiffTimeMicros1b = diffTimeMicros1b;
    } else if (avgDiffTimeMicros1b != 0) {
      avgDiffTimeMicros1b = EMA_a * diffTimeMicros1b + (1.0 - EMA_a) * avgDiffTimeMicros1b;
      // in case we ever get a nan here, let's recover from it
      if (isnan(avgDiffTimeMicros1b)) { avgDiffTimeMicros1b = 0; }
    }
  }
  lastEncoderCountTimeMicros1b = monotonicTimeMicros;
}

void myNoteOn(byte channel, byte note, byte velocity) {
  // When a USB device with multiple virtual cables is used,
  // midi1.getCable() can be used to read which of the virtual
  // MIDI cables received this message.
  Serial.print("Note On, ch=");
  Serial.print(channel, DEC);
  Serial.print(", note=");
  Serial.print(note, DEC);
  Serial.print(", velocity=");
  Serial.println(velocity, DEC);
  currentNote = note;
}

void myNoteOff(byte channel, byte note, byte velocity) {
  Serial.print("Note Off, ch=");
  Serial.print(channel, DEC);
  Serial.print(", note=");
  Serial.print(note, DEC);
  Serial.print(", velocity=");
  Serial.println(velocity, DEC);
  currentNote = 0;
}

void myAfterTouchPoly(byte channel, byte note, byte velocity) {
  Serial.print("AfterTouch Change, ch=");
  Serial.print(channel, DEC);
  Serial.print(", note=");
  Serial.print(note, DEC);
  Serial.print(", velocity=");
  Serial.println(velocity, DEC);
}

void myControlChange(byte channel, byte control, byte value) {
  Serial.print("Control Change, ch=");
  Serial.print(channel, DEC);
  Serial.print(", control=");
  Serial.print(control, DEC);
  Serial.print(", value=");
  Serial.println(value, DEC);
}

void myProgramChange(byte channel, byte program) {
  Serial.print("Program Change, ch=");
  Serial.print(channel, DEC);
  Serial.print(", program=");
  Serial.println(program, DEC);
}

void myAfterTouchChannel(byte channel, byte pressure) {
  Serial.print("After Touch, ch=");
  Serial.print(channel, DEC);
  Serial.print(", pressure=");
  Serial.println(pressure, DEC);
}

void myPitchChange(byte channel, int pitch) {
  Serial.print("Pitch Change, ch=");
  Serial.print(channel, DEC);
  Serial.print(", pitch=");
  Serial.println(pitch, DEC);
}


// This 3-input System Exclusive function is more complex, but allows you to
// process very large messages which do not fully fit within the midi1's
// internal buffer.  Large messages are given to you in chunks, with the
// 3rd parameter to tell you which is the last chunk.  This function is
// a Teensy extension, not available in the Arduino MIDI library.
//
void mySystemExclusiveChunk(const byte *data, uint16_t length, bool last) {
  Serial.print("SysEx Message: ");
  printBytes(data, length);
  if (last) {
    Serial.println(" (end)");
  } else {
    Serial.println(" (to be continued)");
  }
}

// This simpler 2-input System Exclusive function can only receive messages
// up to the size of the internal buffer.  Larger messages are truncated, with
// no way to receive the data which did not fit in the buffer.  If both types
// of SysEx functions are set, the 3-input version will be called by midi1.
//
void mySystemExclusive(byte *data, unsigned int length) {
  Serial.print("SysEx Message: ");
  printBytes(data, length);
  Serial.println();
}

void myTimeCodeQuarterFrame(byte data) {
  static char SMPTE[8]={'0','0','0','0','0','0','0','0'};
  static byte fps=0;
  byte index = data >> 4;
  byte number = data & 15;
  if (index == 7) {
    fps = (number >> 1) & 3;
    number = number & 1;
  }
  if (index < 8 || number < 10) {
    SMPTE[index] = number + '0';
    Serial.print("TimeCode: ");  // perhaps only print when index == 7
    Serial.print(SMPTE[7]);
    Serial.print(SMPTE[6]);
    Serial.print(':');
    Serial.print(SMPTE[5]);
    Serial.print(SMPTE[4]);
    Serial.print(':');
    Serial.print(SMPTE[3]);
    Serial.print(SMPTE[2]);
    Serial.print('.');
    Serial.print(SMPTE[1]);  // perhaps add 2 to compensate for MIDI latency?
    Serial.print(SMPTE[0]);
    switch (fps) {
      case 0: Serial.println(" 24 fps"); break;
      case 1: Serial.println(" 25 fps"); break;
      case 2: Serial.println(" 29.97 fps"); break;
      case 3: Serial.println(" 30 fps"); break;
    }
  } else {
    Serial.print("TimeCode: invalid data = ");
    Serial.println(data, HEX);
  }
}

void mySongPosition(uint16_t beats) {
  Serial.print("Song Position, beat=");
  Serial.println(beats);
}

void mySongSelect(byte songNumber) {
  Serial.print("Song Select, song=");
  Serial.println(songNumber, DEC);
}

void myTuneRequest() {
  Serial.println("Tune Request");
}

void myClock() {
  Serial.println("Clock");
}

void myStart() {
  Serial.println("Start");
}

void myContinue() {
  Serial.println("Continue");
}

void myStop() {
  Serial.println("Stop");
}

void myActiveSensing() {
  Serial.println("Actvice Sensing");
}

void mySystemReset() {
  Serial.println("System Reset");
}

void myRealTimeSystem(uint8_t realtimebyte) {
  Serial.print("Real Time Message, code=");
  Serial.println(realtimebyte, HEX);
}

void printBytes(const byte *data, unsigned int size) {
  while (size > 0) {
    byte b = *data++;
    if (b < 16) Serial.print('0');
    Serial.print(b, HEX);
    if (size > 1) Serial.print(' ');
    size = size - 1;
  }
}
