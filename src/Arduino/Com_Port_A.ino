bool is_request = false;

struct Answer
{
  float x = 0.65;
  float y = 19.534;
  float z = 812.7788;
  float w = 0.78974563;
  int precision[4] = {2,3,4,8};
} answer;

void setup() {
  Serial.begin(115200);
}

void loop() {
  //read incoming data
  String incomeData = "";
  if (Serial.available())
  {
    incomeData = Serial.readString();
    incomeData.trim();
  }

  //check listener
  if (incomeData.equals("request package"))
    is_request = true;
  else if (incomeData.equals("cancel auto request"))
    is_request = false;
  else if(incomeData.length() > 0)
  {
    Serial.println("Invalid Command");
    delay(1000);
    return;
  }

  //send package
  if (is_request)
  {
    String buf;
    //write precisions
    for(auto elem : answer.precision)
        buf += String(elem) + " ";
    
    //write values
    char res[20];
    dtostrf(answer.x, 2, 2, res); //float to string Arduino func
    buf += res;
    buf += " ";
    dtostrf(answer.y, 3, 3, res); //float to string Arduino func
    buf += res;
    buf += " ";
    dtostrf(answer.z, 4, 4, res); //float to string Arduino func
    buf += res;
    buf += " ";
    dtostrf(answer.w, 8, 8, res); //float to string Arduino func
    buf += res;

    //count number of digits to send
    int count = 0;
    for(auto elem : buf)
    {
        if(elem != ' ')
          count++;
    }

    //send data
    String count_array = " " + String(count) + " ";
    Serial.println(count_array + buf);
    is_request = false;
    delay(1000);
  }
}
