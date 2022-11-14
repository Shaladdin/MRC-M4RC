// outputs pin
#define buzzer D0
#define lamp D3
#define fan D4
const int outputs[] = {lamp, fan, buzzer};

// ir
#define irDalam D5
#define irLuar D6
const int inputs[] = {irDalam, irLuar};

void pinInit()
{
    // initialize outputs pins
    for (int i = 0; i < sizeof(outputs) / sizeof(outputs[0]); i++)
        pinMode(outputs[i], OUTPUT);
    // initialize inputs pins
    for (int i = 0; i < sizeof(inputs) / sizeof(inputs[0]); i++)
        pinMode(inputs[i], INPUT);
}