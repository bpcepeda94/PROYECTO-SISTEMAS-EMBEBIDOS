#include <Wire.h> 
#include <LiquidCrystal_I2C.h> //Ambas librerias se instalan desde Programa >> Incluir Libreria >> Administrar Bibliotecas

#include <Keypad.h> //Libreria para teclado matricial

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns

//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'7','8','9','A'},
  {'4','5','6','B'},
  {'1','2','3','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {9, 8, 7, 6}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {5, 4, 3, 2}; //connect to the column pinouts of the keypad
//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

//Crear el objeto lcd  dirección  0x27 y 16 columnas x 2 filas
//por defecto 0x20
LiquidCrystal_I2C lcd (0x27, 16, 2);


//Se guarda los datos de la pantalla en una matriz booleana
bool matriz[8][8] = {
  {1, 1, 1, 1, 1, 1, 1, 1},
  {1, 1, 1, 1, 1, 1, 1, 1},
  {1, 1, 1, 1, 1, 1, 1, 1},
  {1, 1, 1, 1, 1, 1, 1, 1},
  {1, 1, 1, 1, 1, 1, 1, 1},
  {1, 1, 1, 1, 1, 1, 1, 1},
  {1, 1, 1, 1, 1, 1, 1, 1},
  {1, 1, 1, 1, 1, 1, 1, 1}
};

byte rowPins_LED[8] = {29, 28, 27, 26, 25, 24, 23, 22}; //Pines de filas de matriz de led
byte colPins_LED[8] = {30, 31, 32, 33, 34, 35, 36, 37}; //Pines de filas de matriz de led
byte colActual = 0;

void mostrarMatriz(bool matriz[8][8], byte col, byte rowPins_LED[8], byte colPins_LED[8]){
  if(col == 0){
    digitalWrite(colPins_LED[7], HIGH);
  }else{
    digitalWrite(colPins_LED[col-1], HIGH);
  }
  for(byte i = 0; i < 8; i++){
    if(matriz[col][i]){
      digitalWrite(rowPins_LED[i], HIGH); 
    }else{
      digitalWrite(rowPins_LED[i], LOW);
    }
  }
  digitalWrite(colPins_LED[col], LOW);
}

//Guarda el orden de salida de los camiones
//lo que guarda es la posicion del camion que salio
uint8_t ordenSalida[64] = {0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0};
byte finCola = 0; //Me dice cuantos camiones han salido

void guardarSalida(byte i, byte j, uint8_t ordenSalida[64]){
  ordenSalida[finCola] = i*8 + j;
  finCola++;
}

uint8_t obternerSalida(uint8_t ordenSalida[64]){
  uint8_t valor = ordenSalida[0]; //almaceno en valor el primer camion que salio
  if(finCola > 0){
    for(byte i= 0; i < finCola; i++){
      ordenSalida[i] = ordenSalida[i+1];
    }
    finCola --;
    return valor;
  }else{
    return 0;
  }
}


void sacarIndustrial(bool matriz[8][8]){
  uint8_t industriales = 0; //cuenta industriales que se va encontrando
  for(byte i = 0; i < 8; i++){
    for(byte j = 0; j < 4; j++){
      if(matriz[i][j]){ //como los camiones iran desapareciendo de abajo hacia arriba y de derecha a izquierda, el cuenta en sentido contrario
        industriales ++; 
      }else{//cuando se encuentre con un espacio desocupado, entonces desocupara el espacio anterior;
        if(j != 0){
          matriz[i][j-1] = 0;
          guardarSalida(i, j-1, ordenSalida); //guarda en el buffer este orden de salida
          break;
        }else{
          matriz[i-1][3] = 0;
          guardarSalida(i-1, 3, ordenSalida); //guarda en el buffer este orden de salida
          break;
        }
      }
      if(industriales == 32){
        matriz[7][3] = 0; // como la matriz esta llena, saca el ultimo elemento(Camion)
        guardarSalida(7, 3, ordenSalida); //guarda en el buffer este orden de salida
        break;
      }
    }
  }
}
void sacarResidencial(bool matriz[8][8]){
  uint8_t residenciales = 0;
  for(byte i = 0; i < 8; i++){
    for(byte j = 4; j < 8; j++){
      if(matriz[i][j]){ //como los camiones iran desapareciendo de abajo hacia arriba y de derecha a izquierda, el cuenta en sentido contrario
        residenciales ++; 
      }else{//cuando se encuentre con un espacio desocupado, entonces desocupara el espacio anterior;
        if(j != 4){
          matriz[i][j-1] = 0;
          guardarSalida(i, j-1, ordenSalida);
          break;
        }else{
          matriz[i-1][7] = 0;
          guardarSalida(i-1, 7, ordenSalida);
          break;
        }
      }
      if(residenciales == 32){
        matriz[7][7] = 0; // como la matriz esta llena, saca el ultimo elemento(Camion)
        guardarSalida(7, 7, ordenSalida);
        break;
      }
    }
  }
}

uint8_t contarIndustriales(bool matriz[8][8]){
  uint8_t industriales = 0;
  for(byte i = 0; i < 8; i++){
    for(byte j = 0; j < 4; j++){
      if(matriz[i][j]){
        industriales ++;
      }
    }
  }
  return industriales;
}

uint8_t contarResidenciales(bool matriz[8][8]){
  uint8_t residenciales = 0;
  for(byte i = 0; i < 8; i++){
    for(byte j =4; j < 8; j++){
      if(matriz[i][j]){
        residenciales ++;
      }
    }
  }
  return residenciales;
}

uint8_t residencialesDisponibles = 32;
uint8_t industrialesDisponibles = 32;



enum state{
  //Estado Inicial: espera el ingreso del tipo de residuo (Industrial o Residencial)
  E_WAIT_TYPE_RESIDUO,

  //Estado para ingresar la cantidad de kilos 
  E_INGRESAR_KILOS,

  //Estado para seleccionar la cantidad de camiones, mostrar el precio, y actualizar la matriz de led.
  E_LOGISTICA, 
};

state flujo = E_WAIT_TYPE_RESIDUO;

bool isResidencial = false; 
bool isIndustrial = false;

unsigned long kilos = 0; //almacenara los kilos ingresados
byte nDigits = 0; //almacena el numero de digitos de kilos ingresados

uint8_t precioKiloIndustrial = 2;
uint8_t precioKiloResidencial = 1;

unsigned int precioTotal = 0;
int camionesUsar = 0;

void imprimirMatriz(bool matriz[8][8]){
  Serial.println(" ");
  for(byte i = 0; i < 8; i++){
    for(byte j = 0; j < 4; j++){
      Serial.print(matriz[i][j]);
      Serial.print(" ");
    }
    Serial.print("   ");
    for(byte j = 4; j < 8; j++){
      Serial.print(matriz[i][j]);
      Serial.print(" ");
    }
    Serial.println("");
  }
}



void setup() {

  Serial.begin(9600);
  Serial.println("Hola");
  // Inicializar el LCD
  lcd.init();

  //inicializa pines de matriz
  for(byte i = 0; i < 8; i ++){
    pinMode(rowPins_LED[i], OUTPUT);
    pinMode(colPins_LED[i], OUTPUT);
    digitalWrite(rowPins_LED[i], LOW);
    digitalWrite(colPins_LED[i], HIGH);
  }
  //Encender la luz de fondo.
  lcd.backlight();

  lcd.setCursor(0,0);
  lcd.print("  Recoleccion  ");
  lcd.setCursor(0, 1);
  lcd.print("    Basura     ");

  delay(1000);
  
  lcd.clear();
  residencialesDisponibles = contarResidenciales(matriz); //cuenta desde la matriz de leds los residenciales disponibles
  industrialesDisponibles = contarIndustriales(matriz); //cuenta desde la matriz de leds los industriales disponibles.

  //Actualiza en la LCD
  lcd.setCursor(0,0);
  lcd.print("R");
  lcd.print(residencialesDisponibles);
  lcd.setCursor(4, 0);
  lcd.print("I");
  lcd.print(industrialesDisponibles);

 imprimirMatriz(matriz);
}

void loop() {
  mostrarMatriz(matriz, colActual, rowPins_LED,colPins_LED);

  if(colActual == 7){
    colActual = 0;
  }else{
    colActual ++;
  }
  
  char customKey = customKeypad.getKey(); //va a estar preguntando si se presionan las teclas del teclado matricial

  switch(flujo){
    case E_WAIT_TYPE_RESIDUO:
      if(customKey){
        if(customKey == 'A'){ //presiona boton "Industrial"
          if(industrialesDisponibles > 0){
            isIndustrial = true;
            isResidencial = false;
            //una vez ingesado el tipo de residuo, avanza al siguiente estado (INgresar kilos)
            flujo = E_INGRESAR_KILOS;
            //Actualizo en la LCD el tipo de Residuo
            lcd.setCursor(10, 0);
            lcd.print("I");
          }
        }
        if(customKey == 'B'){ //presiona boton "Residencial"
          if(residencialesDisponibles > 0){
            isIndustrial = false;
            isResidencial = true;
            //una vez ingesado el tipo de residuo, avanza al siguiente estado (INgresar kilos)
            flujo = E_INGRESAR_KILOS;
            //Actualizo en la LCD el tipo de Residuo
            lcd.setCursor(10, 0);
            lcd.print("R");
          }
        }
      }
      break;
      
    case E_INGRESAR_KILOS:
      //La tecla asterisco servira para borrar  y una vez borrado todo servira para cancelar 
      //la operacion de ingresar kilos y regresar a escoger el tipo de residuo
      //la tecla # confirma los kilos ingresados y avanza al siguiente estado
      if(customKey == '*'){
        if(kilos == 0){//no hay kilos entonces regresa a pedir el tipo de residuo
          flujo = E_WAIT_TYPE_RESIDUO;
          isIndustrial = false;
          isResidencial = false;
          lcd.setCursor(10, 0);
          lcd.print(" "); //se borra el tipo de residuo anterior
        }
        else{
          kilos = kilos/10; //le quita el digito de unidad a los kilos
          nDigits = nDigits - 1; //borra un digito
          lcd.setCursor(nDigits, 1); //borra el ultimo digito (unidades) de la LCD
          lcd.print("k ");
        }
      }
      if(customKey == '#'){ //confirma los kilos ingresados y avanza al siguiente estado
        if(kilos != 0){ //Si no hay kilos ingresados -> no puede avanzar
          if(isIndustrial){
            if(kilos > 1000*industrialesDisponibles){
              kilos = 1000*industrialesDisponibles;
              lcd.setCursor(0, 1);
              lcd.print("max");
              lcd.print(kilos);
            }
          }else if(isResidencial){
            if(kilos > 500*residencialesDisponibles){
              kilos = 500*residencialesDisponibles;
              lcd.setCursor(0,1);
              lcd.print("max");
              lcd.print(kilos);
            }
          }
          Serial.print(kilos);
          flujo = E_LOGISTICA;          
        }
      }
      if(customKey){
        int digito = customKey - '0'; //transforma la tecla presionada en entero
        if(digito >= 0 and digito <=9){ //confirma que sea un digito
          if(digito == 0){
            if(kilos != 0){ //no puede ingresar 0 si no ha ingresado un numero antes
              kilos = kilos*10 + digito; //recalcula los kilos
              lcd.setCursor(nDigits, 1); //actualiza los kilos en la lcd
              lcd.print(digito);
              lcd.print("k");
              nDigits = nDigits + 1; //aumenta la cantidad de digitos ingresada
            }
          }else{
            kilos = kilos*10 + digito;
            lcd.setCursor(nDigits, 1);
            lcd.print(digito);
            lcd.print("k");
            nDigits = nDigits + 1;
          }
        }        
      }
      break;

    case E_LOGISTICA:
      if(isIndustrial){
        precioTotal = kilos*precioKiloIndustrial;
        camionesUsar = ceil(kilos/1000.0); //Usa una tonelada
      }else if(isResidencial){
        precioTotal = kilos*precioKiloResidencial;
        camionesUsar = ceil(kilos/500.0); //Usa media tonelada
      }
      lcd.setCursor(9, 1);
      lcd.print("$");
      lcd.print(precioTotal);

      lcd.setCursor(11, 0);
      lcd.print(camionesUsar);

      for(byte i = 0; i < camionesUsar; i++){
        if(isIndustrial){
          sacarIndustrial(matriz);
        }else{
          sacarResidencial(matriz);
        }
        imprimirMatriz(matriz);
        //esto es para que el delay no afecte la matriz
        unsigned long tiempo = millis();
        while(millis() - tiempo < 1000){
          mostrarMatriz(matriz, colActual, rowPins_LED,colPins_LED);

          if(colActual == 7){
            colActual = 0;
          }else{
            colActual ++;
          }
          
        }
      }

      lcd.clear();
      residencialesDisponibles = contarResidenciales(matriz); //cuenta desde la matriz de leds los residenciales disponibles
      industrialesDisponibles = contarIndustriales(matriz); //cuenta desde la matriz de leds los industriales disponibles.

      //Actualiza en la LCD
      lcd.setCursor(0,0);
      lcd.print("R");
      lcd.print(residencialesDisponibles);
      lcd.setCursor(4, 0);
      lcd.print("I");
      lcd.print(industrialesDisponibles);

      kilos = 0;
      isResidencial = false;
      isIndustrial = false;
      nDigits = 0; //almacena el numero de digitos de kilos ingresados
      flujo = E_WAIT_TYPE_RESIDUO;
      break;
  }
  
  if(customKey == 'C'){
    if(finCola != 0){
      uint8_t coord= obternerSalida(ordenSalida);
      uint8_t i = coord/8;
      uint8_t j = coord - i*8;
      matriz[i][j] = 1;
      imprimirMatriz(matriz);
      //reiniciar LCD
      lcd.clear();
      residencialesDisponibles = contarResidenciales(matriz); //cuenta desde la matriz de leds los residenciales disponibles
      industrialesDisponibles = contarIndustriales(matriz); //cuenta desde la matriz de leds los industriales disponibles.

      //Actualiza en la LCD
      lcd.setCursor(0,0);
      lcd.print("R");
      lcd.print(residencialesDisponibles);
      lcd.setCursor(4, 0);
      lcd.print("I");
      lcd.print(industrialesDisponibles);
      //esto es para que el delay no afecte la matriz
      unsigned long tiempo = millis();
      while(millis() - tiempo < 1000){
        mostrarMatriz(matriz, colActual, rowPins_LED,colPins_LED);

        if(colActual == 7){
          colActual = 0;
        }else{
          colActual ++;
        }
        
      }
    }
  }

}
