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

#define LEFT false
#define RIGHT true

#define PIN_TRASH_SENSOR 2

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

  _fuzzy = new Fuzzy();

  FuzzyInput *pot = new FuzzyInput(1);

  FuzzySet *baja = new FuzzySet(0, 80, 240, 320);
  pot->addFuzzySet(baja);

  FuzzySet *mediabaja = new FuzzySet(240, 320, 320, 400);
  pot->addFuzzySet(mediabaja);

  FuzzySet *media = new FuzzySet(320, 400, 560, 640);
  pot->addFuzzySet(media);

  FuzzySet *mediaalta = new FuzzySet(560, 640, 640, 720);
  pot->addFuzzySet(mediaalta);

  FuzzySet *alta = new FuzzySet(640, 720, 960, 1023);
  pot->addFuzzySet(alta);

  _fuzzy->addFuzzyInput(pot);

  FuzzyOutput *velocidad = new FuzzyOutput(1);

  FuzzySet *lento = new FuzzySet(0, 20, 60, 80);
  velocidad->addFuzzySet(lento);

  FuzzySet *mediolento = new FuzzySet(60, 80, 80, 100);
  velocidad->addFuzzySet(mediolento);

  FuzzySet *medio = new FuzzySet(80, 100, 140, 160);
  velocidad->addFuzzySet(medio);

  FuzzySet *medioveloz = new FuzzySet(140, 160, 160, 180);
  velocidad->addFuzzySet(medioveloz);

  FuzzySet *veloz = new FuzzySet(160, 180, 240, 255);
  velocidad->addFuzzySet(veloz);

  _fuzzy->addFuzzyOutput(velocidad);

  // IF pot = baja THEN velocidad = lento
  FuzzyRuleAntecedent *ifPotBaja = new FuzzyRuleAntecedent();
  ifPotBaja->joinSingle(baja);
  FuzzyRuleConsequent *thenVelocidadLento = new FuzzyRuleConsequent();
  thenVelocidadLento->addOutput(lento);
  FuzzyRule *Regla01 = new FuzzyRule(1, ifPotBaja, thenVelocidadLento);

  _fuzzy->addFuzzyRule(Regla01);

  // IF pot = mediabaja THEN velocidad = mediolento
  FuzzyRuleAntecedent *ifPotMediaBaja = new FuzzyRuleAntecedent();
  ifPotMediaBaja->joinSingle(mediabaja);
  FuzzyRuleConsequent *thenVelocidadMedioLento = new FuzzyRuleConsequent();
  thenVelocidadMedioLento->addOutput(mediolento);
  FuzzyRule *Regla02 = new FuzzyRule(1, ifPotMediaBaja, thenVelocidadMedioLento);

  _fuzzy->addFuzzyRule(Regla02);

  // IF pot = media THEN velocidad = medio
  FuzzyRuleAntecedent *ifPotMedia = new FuzzyRuleAntecedent();
  ifPotMedia->joinSingle(media);
  FuzzyRuleConsequent *thenVelocidadMedio = new FuzzyRuleConsequent();
  thenVelocidadMedio->addOutput(medio);
  FuzzyRule *Regla03 = new FuzzyRule(1, ifPotMedia, thenVelocidadMedio);

  _fuzzy->addFuzzyRule(Regla03);

  // IF pot = mediaalta THEN velocidad = medioveloz
  FuzzyRuleAntecedent *ifPotMediaAlta = new FuzzyRuleAntecedent();
  ifPotMediaAlta->joinSingle(mediaalta);
  FuzzyRuleConsequent *thenVelocidadMedioVeloz = new FuzzyRuleConsequent();
  thenVelocidadMedioVeloz->addOutput(medioveloz);
  FuzzyRule *Regla04 = new FuzzyRule(1, ifPotMediaAlta, thenVelocidadMedioVeloz);

  _fuzzy->addFuzzyRule(Regla04);

  // IF pot = alta THEN velocidad = veloz
  FuzzyRuleAntecedent *ifPotAlta = new FuzzyRuleAntecedent();
  ifPotAlta->joinSingle(alta);
  FuzzyRuleConsequent *thenVelocidadVeloz = new FuzzyRuleConsequent();
  thenVelocidadVeloz->addOutput(veloz);
  FuzzyRule *Regla05 = new FuzzyRule(1, ifPotAlta, thenVelocidadVeloz);

  _fuzzy->addFuzzyRule(Regla05);
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

  _readSpeed();

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

void Vacuum::_readSpeed()
{
  int readPot, output;

  // Lectura del potenciómetro
  readPot = analogRead(A0);

  // Se indica que la variable readPot representa la señal de entrada
  _fuzzy->setInput(1, readPot);

  // Se realiza la fuzzificación
  _fuzzy->fuzzify();

  // Se realiza la defuzzificación asignando su valor final a la variable output
  output = _fuzzy->defuzzify(1);

  // Se envía la señal de salida al motor
  _left_motor.setSpeed(output);
  _right_motor.setSpeed(output);

  Serial.print("Potenciometro: "); // Se imprime la lectura del POT, así
  Serial.print(readPot);           // como la velocidad enviada al motor.
  Serial.print(" Salida: ");
  Serial.println(output);
  delay(10);
}
