#include "Vacuum.h"
#include <Arduino.h>
#include <Fuzzy.h>
#include <FuzzyComposition.h>
#include <FuzzyInput.h>
#include <FuzzyIO.h>
#include <FuzzyOutput.h>
#include <FuzzyRule.h>
#include <FuzzyRuleAntecedent.h>
#include <FuzzyRuleConsequent.h>
#include <FuzzySet.h>

Fuzzy* fuzzy = newFuzzy();

#define LEFT false
#define RIGHT true

#define PIN_TRASH_SENSOR 2

#define MOTOR_SPEED 100

#define IDLE_DELAY 10000
#define CLEAN_TIME 2000
#define MAX_MOVE_TIME 2000


Vacuum::Vacuum() : _left_motor(1), _right_motor(2)
{
  pinMode(PIN_TRASH_SENSOR, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  _direction = FORWARD;

  _start_time = 0;
  _run_time = 0;
  _trips = 0;

  FuzzySet* baja = newFuzzySet(0, 80, 240, 320); 
  pot->addFuzzySet(baja);

  FuzzySet* mediabaja = newFuzzySet(240, 320, 320, 400);
  pot->addFuzzySet(mediabaja);

  FuzzySet* media = newFuzzySet(320, 400, 560, 640);
  pot->addFuzzySet(media);

  FuzzySet* mediaalta = newFuzzySet(560, 640, 640, 720);
  pot->addFuzzySet(mediaalta);

  FuzzySet* alta = newFuzzySet(640, 720, 960, 1023);
  pot->addFuzzySet(alta);

  fuzzy->addFuzzyInput(pot);

  FuzzyOutput* velocidad = newFuzzyOutput(1);

  FuzzySet* lento = newFuzzySet(0, 20, 60, 80);
  velocidad->addFuzzySet(lento);

  FuzzySet* mediolento = newFuzzySet(60, 80, 80, 100);
  velocidad->addFuzzySet(mediolento);

  FuzzySet* medio = newFuzzySet(80, 100, 140, 160);
  velocidad->addFuzzySet(medio);

  FuzzySet* medioveloz = newFuzzySet(140, 160, 160, 180);
  velocidad->addFuzzySet(medioveloz);

  FuzzySet* veloz = newFuzzySet(160, 180, 240, 255);
  velocidad->addFuzzySet(veloz);

  fuzzy->addFuzzyOutput(velocidad);
  //IF pot = baja THEN velocidad = lento
  FuzzyRuleAntecedent* ifPotBaja =newFuzzyRuleAntecedent();
  ifPotBaja->joinSingle(baja);
  FuzzyRuleConsequent* thenVelocidadLento =newFuzzyRuleConsequent();
  thenVelocidadLento->addOutput(lento);
  FuzzyRule* Regla01 = newFuzzyRule(1, ifPotBaja, thenVelocidadLento);

  fuzzy->addFuzzyRule(Regla01);

  //IF pot = mediabaja THEN velocidad = mediolento
  FuzzyRuleAntecedent* ifPotMediaBaja = newFuzzyRuleAntecedent();
  ifPotMediaBaja->joinSingle(mediabaja);
  FuzzyRuleConsequent* thenVelocidadMedioLento = newFuzzyRuleConsequent();
  thenVelocidadMedioLento->addOutput(mediolento);
  FuzzyRule* Regla02 = newFuzzyRule(1, ifPotMediaBaja, thenVelocidadMedioLento);

  fuzzy->addFuzzyRule(Regla02);
  //IF pot = media THEN velocidad = medio
  FuzzyRuleAntecedent* ifPotMedia = newFuzzyRuleAntecedent();
  ifPotMedia->joinSingle(media);
  FuzzyRuleConsequent* thenVelocidadMedio = newFuzzyRuleConsequent();
  thenVelocidadMedio->addOutput(medio);
  FuzzyRule* Regla03 = newFuzzyRule(1, ifPotMedia, thenVelocidadMedio);

  fuzzy->addFuzzyRule(Regla03);
  //IF pot = mediaalta THEN velocidad = medioveloz
  FuzzyRuleAntecedent* ifPotMediaAlta = newFuzzyRuleAntecedent();
  ifPotMediaAlta->joinSingle(mediaalta);
  FuzzyRuleConsequent* thenVelocidadMedioVeloz = newFuzzyRuleConsequent();
  thenVelocidadMedioVeloz->addOutput(medioveloz);
  FuzzyRule* Regla04 = newFuzzyRule(1, ifPotMediaAlta, thenVelocidadMedioVeloz);

  fuzzy->addFuzzyRule(Regla04);
  //IF pot = alta THEN velocidad = veloz
  FuzzyRuleAntecedent* ifPotAlta = newFuzzyRuleAntecedent();
  ifPotAlta->joinSingle(alta);
  FuzzyRuleConsequent* thenVelocidadVeloz = newFuzzyRuleConsequent();
  thenVelocidadVeloz->addOutput(veloz);
  FuzzyRule* Regla05 = newFuzzyRule(1, ifPotAlta, thenVelocidadVeloz);

  fuzzy->addFuzzyRule(Regla05);
}

void Vacuum::_startEngine(bool side)
{
  side ? _right_motor.run(_direction) : _left_motor.run(_direction);
}

void Vacuum::_stopEngine(bool side)
{
  side ? _right_motor.run(RELEASE) : _left_motor.run(RELEASE);
}

void Vacuum::_changeDirection()
{
  _direction = _direction == FORWARD ? BACKWARD : FORWARD;
  _trips++;
}

void Vacuum::move()
{
  unsigned long move_time = millis() - _start_time + _run_time;

  _startEngine(LEFT);
  _startEngine(RIGHT);

  if (_start_time != 0 && move_time >= MAX_MOVE_TIME)
  {
    _changeDirection();
    _run_time = 0;
    _start_time = 0;
  }

  if (_start_time == 0)
    _start_time = millis();
}

void Vacuum::stop()
{
  _stopEngine(LEFT);
  _stopEngine(RIGHT);

  if (_start_time != 0)
  {
    _run_time = millis() - _start_time;
    _start_time = 0;
  }
}

void Vacuum::cleanTrash()
{
  stop();

  digitalWrite(LED_BUILTIN, HIGH);
  delay(CLEAN_TIME);
  digitalWrite(LED_BUILTIN, LOW);
}

bool Vacuum::senseTrash()
{
  return digitalRead(PIN_TRASH_SENSOR);
}

int Vacuum::getTrips()
{
  return _trips;
}

void Vacuum::reset()
{
  stop();

  _trips = 0;
  _run_time = 0;

  delay(IDLE_DELAY);
}

void Vacuum::_readSpeed(){
  //Lectura del potenciometro
  readPot = analogRead(A0);

  //Se indica que la variable readPot representa la señal de entrada
  fuzzy->setInput(1, readPot);

  //Se realiza la fuzzificacion
  fuzzy->fuzzify();

  //Se realiza la defuzzificacion asignando su valor final a la variable 
  //output
  output = fuzzy->defuzzify(1);

  //Se envia la señal de salida al motor
  _left_motor.setSpeed(output);
  _right_motor.setSpeed(output);

  Serial.print(“Potenciometro: “); //Se imprime la lectura del POT, asi 
  Serial.print(readPot); //como la velocidad en RPM medida por el 
  Serial.print(“ “); //encoder.
  Serial.print(“Salida: “);
  Serial.print(output);
}
