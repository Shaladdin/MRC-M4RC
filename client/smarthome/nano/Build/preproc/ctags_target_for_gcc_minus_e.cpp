# 1 "c:\\Users\\Shaladdin\\Robotik\\Project\\LOMBA\\MRC\\client\\smarthome\\nano\\nano.ino"
# 2 "c:\\Users\\Shaladdin\\Robotik\\Project\\LOMBA\\MRC\\client\\smarthome\\nano\\nano.ino" 2

void setup()
{
    Serial.begin(115200);
    pinInit();
    SensorInit();
}

void loop()
{
    SensorRun();
}
