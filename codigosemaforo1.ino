const unsigned long t_GREEN= 6000; // 6 segundos
const unsigned long t_YELLOW= 2000; // 2 segundos
const unsigned long t_RED = 6000;   // 6 segundos


// Clase para el semáforo de autos
class SemaforoAuto {
  public:
    enum State {RED, YELLOW, GREEN};

  private:
    int redPin, yellowPin, greenPin;//pines de los leds del semaforo
    State currentState;//estado actual
    State lastState;//estado anterior al actual (nos sirve para cambiar de amarillo a rojo o verde).
    int buttonPin1; // Agregado: pin del botón asociado al semáforo
    int buttonPin2;//boton asociado a su espejo pero que tambien afecta su estado.
    unsigned long lastChangeTime;//tiempo en el que se realizo el ultimo cambio de estado.
    

  public:
    SemaforoAuto(int rPin, int yPin, int gPin, int bPin1, int bPin2, State initialState)
      : redPin(rPin), yellowPin(yPin), greenPin(gPin), buttonPin1(bPin1), buttonPin2(bPin2),currentState(initialState){
      pinMode(redPin, OUTPUT);
      pinMode(yellowPin, OUTPUT);
      pinMode(greenPin, OUTPUT);
      pinMode(buttonPin1, INPUT_PULLUP); // configurar el pin del botón como entrada con resistencia de pull-up (manda ceros)
      pinMode(buttonPin2, INPUT_PULLUP); // configurar el pin del botón como entrada con resistencia de pull-up (manda ceros)
      update();

      lastChangeTime = 0;//tiempo de ultimo cambio
    }

    void changeState() {
      unsigned long currentTime = millis();//se usa millis porque me permite interrumpir un estado mediante el boton.
      switch (currentState) {
        case RED:
          lastState = currentState; 
          if (digitalRead(buttonPin2) == LOW) {
          currentState= YELLOW;
          lastChangeTime = currentTime;
          } else if (currentTime - lastChangeTime >= t_RED) {
          currentState= YELLOW;
          lastChangeTime = currentTime;
          }
          break;
        case GREEN:
          lastState = currentState; 
          if (digitalRead(buttonPin1) == LOW) {
          currentState= YELLOW;
          lastChangeTime = currentTime;
          } else if (currentTime - lastChangeTime >= t_GREEN) {
          currentState= YELLOW;
          lastChangeTime = currentTime;
          }
          break;
        case YELLOW:
          if (currentTime - lastChangeTime >= t_YELLOW) {
            if (lastState == GREEN) {
              currentState = RED;
            } else if (lastState == RED) {
              currentState = GREEN;
            }
          lastState = currentState; 
          lastChangeTime = currentTime;
          }
          
          break;
      }
      update();
    }
    void sincronizar(SemaforoAuto* espejo){
    switch(espejo->getState()) {
            case RED:
                setState(GREEN); // Utiliza el método setState para cambiar el estado
                break;
            case GREEN:
                setState(RED); // Utiliza el método setState para cambiar el estado
                break;
            case YELLOW:
                setState(YELLOW); // Utiliza el método setState para cambiar el estado
                break;
        }
    update();
   }

        void update() {
      digitalWrite(redPin, currentState == RED ? HIGH : LOW);
      digitalWrite(yellowPin, currentState == YELLOW ? HIGH : LOW);
      digitalWrite(greenPin, currentState == GREEN ? HIGH : LOW);
    }

    State getState() const { return currentState; }

    void setState(State state) {
      currentState = state;
      update();
    }
};


// Clase para el semáforo de peatones
class SemaforoPeaton {
  public:
    enum State {RED, GREEN};

  private:
    int redPin, greenPin;
    State currentState;

  public:
    SemaforoPeaton(int rPin, int gPin)
      : redPin(rPin), greenPin(gPin) {
      pinMode(redPin, OUTPUT);
      pinMode(greenPin, OUTPUT);
      
      update();
    }

    void changeState() {
      currentState = (currentState == RED) ? GREEN : RED;
      update();
    }

    void sincronizarPeaton(SemaforoAuto* guia){
      switch(guia->getState()){
          case SemaforoAuto::RED:
                setState(GREEN); // Utiliza el método setState para cambiar el estado
                break;
            case SemaforoAuto::GREEN:
                setState(RED); // Utiliza el método setState para cambiar el estado
                break;
            case SemaforoAuto::YELLOW:
                //no se hace nada
                break;
      }
      update();
    }

    void update() {
      digitalWrite(redPin, currentState == RED ? HIGH : LOW);
      digitalWrite(greenPin, currentState == GREEN ? HIGH : LOW);
    }

    State getState() const { return currentState; }

    void setState(State state) {
      currentState = state;
      update();
    }
};

// Instanciación de los semáforos
SemaforoAuto AC(0, 1, 2, 10,11, SemaforoAuto::RED);
SemaforoAuto BD(4, 5, 6, 11, 10, SemaforoAuto::GREEN);
SemaforoPeaton NS(8, 9);
SemaforoPeaton EO(12, 13);

void setup() { 
  
  // No es necesario llamar a update() aquí para los semáforos de autos ya que ya se llama dentro de su constructor
  
}
void loop() {
  
  BD.sincronizar(&AC);
  EO.sincronizarPeaton(&AC);
  NS.sincronizarPeaton(&BD);
  AC.changeState();
  
}
