# 1 "c:\\Users\\Shaladdin\\Robotik\\Project\\LOMBA\\MRC\\client\\smarthome\\esp\\esp.ino"
# 2 "c:\\Users\\Shaladdin\\Robotik\\Project\\LOMBA\\MRC\\client\\smarthome\\esp\\esp.ino" 2

void setup()
{
    Serial.begin(115200);
    pinInit();
}

void loop()
{
    RoomDetectionRun();
}
