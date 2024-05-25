#define CELL_1 A0
#define CELL_2 A1
#define CELL_3 A2
#define CELL_4 A3
#define CELL_5 A4
#define NTC A5

#define A 1.067802124e-3
#define B 2.122628987e-4
#define C 0.8945603223e-7
#define R1 10000


#define BALANCE_1 2
#define BALANCE_2 3
#define BALANCE_3 4
#define BALANCE_4 5
#define BALANCE_5 6

#define THRESHOLD 0.1

int balance_pins[] = {BALANCE_1, BALANCE_2, BALANCE_3, BALANCE_4, BALANCE_5};
float cell_voltage[5];
bool balance_flag[5] = {false, false, false, false, false};

float Vo;
float logR_NTC, R_NTC, T_NTC;

void getCellVoltage();
void getNTCTemperature();
void printCellVoltage();
int Cells_Disbalance_Check(float voltage[]);
int Balance_Cells(int minVoltageIndex);

void setup(){

    Serial.begin(9600);

    pinMode(CELL_1, INPUT);
    pinMode(CELL_2, INPUT);
    pinMode(CELL_3, INPUT);
    pinMode(CELL_4, INPUT);
    pinMode(CELL_5, INPUT);

    pinMode(BALANCE_1, OUTPUT);
    pinMode(BALANCE_2, OUTPUT);
    pinMode(BALANCE_3, OUTPUT);
    pinMode(BALANCE_4, OUTPUT);
    pinMode(BALANCE_5, OUTPUT);

    Serial.println("Cell Balancing System");

    getCellVoltage();

    digitalWrite(BALANCE_1, HIGH);
    digitalWrite(BALANCE_2, HIGH);
    digitalWrite(BALANCE_3, HIGH);
    digitalWrite(BALANCE_4, HIGH);
    digitalWrite(BALANCE_5, HIGH);

}

void loop(){

    getCellVoltage();

    int minVoltageIndex = Cells_Disbalance_Check(cell_voltage);
    
    if (Balance_Cells(minVoltageIndex) == -1) {
        Serial.println("Cells are balanced");
    } else {
        Serial.print("Balancing cell ");
        Serial.println(minVoltageIndex + 1);
    }

    getNTCTemperature();
    
}

void printCellVoltage(){
    
    Serial.print("Cell 1: ");
    Serial.print(cell_voltage[0]);
    Serial.print("V");

    Serial.print("Cell 2: ");
    Serial.print(cell_voltage[1]);
    Serial.print("V");

    Serial.print("Cell 3: ");
    Serial.print(cell_voltage[2]);
    Serial.print("V");

    Serial.print("Cell 4: ");
    Serial.print(cell_voltage[3]);
    Serial.print("V");

    Serial.print("Cell 5: ");
    Serial.print(cell_voltage[4]);
    Serial.print("V");
}

int Cells_Disbalance_Check(float voltage[]) {
    int minVoltageIndex = 0;
    int maxVoltageIndex = 0;

    // Find the index of the minimum and maximum voltages in the array
    for (int i = 1; i < 5; i++) {
        if (voltage[i] < voltage[minVoltageIndex]) {
            minVoltageIndex = i;
        }
        if (voltage[i] > voltage[maxVoltageIndex]) {
            maxVoltageIndex = i;
        }
    }

    // Check if the difference between the maximum and minimum voltage is greater than THRESHOLD
    if (voltage[maxVoltageIndex] - voltage[minVoltageIndex] > THRESHOLD) {
        return minVoltageIndex; // Return the index of the smallest voltage
    } else {
        return -1; // Return -1 if cells are balanced
    }
}

int Balance_Cells(int minVoltageIndex) {
    
    if(minVoltageIndex == -1){
        return -1;
    }

    while (true)
    {
        getCellVoltage();

        for (int i = 1; i < 5; i++) {
            if (cell_voltage[i]>cell_voltage[minVoltageIndex]) {
                digitalWrite(balance_pins[i], LOW);
            }
            else {
                digitalWrite(balance_pins[i], HIGH);
                balance_flag[i] = true;
            }
        }

        printCellVoltage();
        getNTCTemperature();

        if (balance_flag[0] && balance_flag[1] && balance_flag[2] && balance_flag[3] && balance_flag[4]) {
            balance_flag[0] = false;
            balance_flag[1] = false;
            balance_flag[2] = false;
            balance_flag[3] = false;
            balance_flag[4] = false;
            return -1;
        }

    }
    
}

void getCellVoltage() {
    cell_voltage[0] = analogRead(CELL_1) * (5.0 / 1023.0);
    cell_voltage[1] = (analogRead(CELL_2) * (5.0 / 1023.0) * 2) - cell_voltage[0];
    cell_voltage[2] = (analogRead(CELL_3) * (5.0 / 1023.0) * 3) - cell_voltage[0] - cell_voltage[1];
    cell_voltage[3] = (analogRead(CELL_4) * (5.0 / 1023.0) * 4) - cell_voltage[0] - cell_voltage[1] - cell_voltage[2];
    cell_voltage[4] = (analogRead(CELL_5) * (5.0 / 1023.0) * 5) - cell_voltage[0] - cell_voltage[1] - cell_voltage[2] - cell_voltage[3];
}

void getNTCTemperature() {

    Vo = analogRead(NTC) * (5.0 / 1023.0);
    R_NTC = R1 * (5.0 / Vo - 1);
    logR_NTC = log(R_NTC);
    T_NTC = 1 / (A + B * logR_NTC + C * logR_NTC * logR_NTC * logR_NTC);
    T_NTC = T_NTC - 273.15;

    Serial.print("Temperature: ");
    Serial.print(T_NTC);
}