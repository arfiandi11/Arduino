/* 
NodeMCU ESP32 Chart (D3.js)

Author : ChungYi Fu (Taiwan)  2018-05-13 21:00

Command Format :  
http://STAIP   (default:LineChart)
http://STAIP/?chartType=LineChart
http://STAIP/?chartType=AreaChart
http://STAIP/?chartWidth=value
http://STAIP/?chartHeight=value
http://STAIP/?showNumber=value
http://STAIP/?yScaleMax=value
http://STAIP/?yScaleMin=value
(It is necessary to load D3.js from webside,so you can't link to chart page by using APIP address)

http://192.168.4.1/?resetwifi=ssid;password
*/

String chartType="";     //LineChart or AreaChart
String chartData="";     //Data Format:  hh:mm:ss,temperature,humidity;
int showNumber=10;        //Number of data records
int timeInterval=5000;   //ms
int chartWidth=600;      //px
int chartHeight=600;     //px
int yScaleMax=200;       //Maximum value of the Y axis
int yScaleMin=-100;      //Minimum value of the Y axis
String xTitle="Time";
String yTitle="Temperature&Humidity";
unsigned long time1,time2;
int count=0;           

#include <WiFi.h>

const char* ssid     = "";   //your network SSID
const char* password = "";   //your network password

const char* apssid = "ESP32 Chart";
const char* appassword = "12345678";         //AP password requires at least 8 characters.

WiFiServer server(80);

String Feedback="", Command="",cmd="",str1="",str2="",str3="",str4="",str5="",str6="",str7="",str8="",str9="";
byte ReceiveState=0,cmdState=1,strState=1,questionstate=0,equalstate=0,semicolonstate=0;

void setup()
{
  Serial.begin(115200);
  delay(10);
  
  WiFi.mode(WIFI_AP_STA);
  
  //WiFi.config(IPAddress(192, 168, 201, 100), IPAddress(192, 168, 201, 2), IPAddress(255, 255, 255, 0));
  WiFi.begin(ssid, password);
  delay(1000);
  Serial.println("");
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  long int StartTime=millis();
  while (WiFi.status() != WL_CONNECTED) 
  {
      delay(500);
      if ((StartTime+5000) < millis()) break;
  } 
  Serial.println("");
  Serial.println("STAIP address: ");
  Serial.println(WiFi.localIP());

  
  if (WiFi.localIP().toString()!="0.0.0.0")
    WiFi.softAP((WiFi.localIP().toString()+"_"+(String)apssid).c_str(), appassword);
  else
    WiFi.softAP(apssid, appassword);
    
  //WiFi.softAPConfig(IPAddress(192, 168, 4, 1), IPAddress(192, 168, 4, 1), IPAddress(255, 255, 255, 0)); 
  Serial.println("");
  Serial.println("APIP address: ");
  Serial.println(WiFi.softAPIP());    
  server.begin(); 
  
  time1 = millis();
}

void loop()
{
  time2 = millis();
  if (time2>=(time1+timeInterval))
  {
    //Sensor Data
    int Temperature = rand()%300-100;    
    int Humidity = rand()%100;  
    
    int t=time2/1000;
    t%=86400;
    chartData=chartData+String(t/3600)+":"+String((t%3600)/60)+":"+String(t%60)+","+Temperature+","+Humidity+";";
    time1 = time2;
    if (count==showNumber)
      chartData=chartData.substring(chartData.indexOf(";")+1);
    else
      count++;
    //Serial.println(chartData);
  }
    
  Command="";cmd="";str1="";str2="";str3="";str4="";str5="";str6="";str7="";str8="";str9="";
  ReceiveState=0,cmdState=1,strState=1,questionstate=0,equalstate=0,semicolonstate=0;

  WiFiClient client = server.available();

  if (client) 
  { 
    String currentLine = "";

    while (client.connected()) 
    {
      if (client.available()) 
      {
        char c = client.read();             
        
        getCommand(c);
                
        if (c == '\n') 
        {
          if (currentLine.length() == 0) 
          {
            client.println("HTTP/1.1 200 OK");
            client.println("Access-Control-Allow-Headers: Origin, X-Requested-With, Content-Type, Accept");
            client.println("Access-Control-Allow-Methods: GET,POST,PUT,DELETE,OPTIONS");
            client.println("Content-Type: text/html; charset=utf-8");
            client.println("Access-Control-Allow-Origin: *");
            //client.println("Connection: close");
            client.println();
            client.println("<!DOCTYPE HTML>");
            client.println("<html><head>");
            client.println("<meta charset=\"UTF-8\">");
            client.println("<meta http-equiv=\"Access-Control-Allow-Origin\" content=\"*\">");
            client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">");
            client.println("<script src=\"http://d3js.org/d3.v4.min.js\"></script>");
            client.println("</head>");
            client.println("<script>");
            client.println("  function createChart(input_type,input_data,input_width,input_height,input_title_x,input_title_y,input_yscale_max,input_yscale_min) {");
            client.println("    var margin = {top: 50, right: 50, bottom: 70, left: 50};");
            client.println("    var width = input_width - margin.left - margin.right;");
            client.println("    var height = input_height - margin.top - margin.bottom;");
            client.println("    if (document.getElementById('fustyles_chart'))");
            client.println("    {");
            client.println("      d3.selectAll(\"svg > *\").remove();");
            client.println("      var svg = d3.select('#fustyles_chart').attr('width', width + margin.left + margin.right).attr('height', height + margin.top + margin.bottom).append('g').attr('transform', `translate(${margin.left}, ${margin.top})`);");
            client.println("    }");
            client.println("    else");
            client.println("    {");
            client.println("      var sheet = document.createElement('style');");
            client.println("      sheet.innerHTML = \"body {font-size: 100%;}.area1 {fill: red;}.area2 {fill: blue;}.line1 {fill: none;stroke: red;stroke-width: 2px;}.line2 {fill: none;stroke: blue;stroke-width: 2px;}.point1 {fill:red;stroke:red;}.point2 {fill:blue;stroke:blue;}\";");   
            client.println("      document.head.appendChild(sheet);");
            client.println("      var svg = d3.select('body').append('svg').attr('id','fustyles_chart').attr('width', width + margin.left + margin.right).attr('height', height + margin.top + margin.bottom).append('g').attr('transform', `translate(${margin.left}, ${margin.top})`);");
            client.println("    }");
            client.println("    var string = \"time,temperature,humidity\\n\"+input_data.replace(/;/ig,\"\\n\");");
            client.println("    var data = d3.csvParse(string);");
            client.println("    var parseTime = d3.timeParse(\"%H:%M:%S\");");
            client.println("    data.forEach(function(d){");
            client.println("      d.time = parseTime(d.time);");
            client.println("      d.temperature = d.temperature;");
            client.println("      d.humidity = d.humidity;");
            client.println("    });");
            client.println("    var xScale = d3.scaleTime().range([0, width]);");
            client.println("    var yScale = d3.scaleLinear().range([height, 0]);");
            client.println("    xScale.domain(d3.extent(data, d => d.time));");
            client.println("    yScale.domain([input_yscale_min, input_yscale_max]);");
            client.println("    if (input_type=='AreaChart')");
            client.println("    {");
            client.println("      var area2 = d3.area().x(d => xScale(d.time)).y0(height).y1(d => yScale(d.humidity));");
            client.println("      svg.append(\"path\").data([data]).attr(\"class\", \"area2\").attr(\"d\", area2);");
            client.println("    }");
            client.println("    var line2 = d3.line().x(d => xScale(d.time)).y(d => yScale(d.humidity));"); 
            client.println("    svg.append('path').data([data]).attr('class', 'line2').attr('d', line2);");
            client.println("    if (input_type=='AreaChart')");
            client.println("    {");
            client.println("      var area1 = d3.area().x(d => xScale(d.time)).y0(height).y1(d => yScale(d.temperature));");
            client.println("      svg.append(\"path\").data([data]).attr(\"class\", \"area1\").attr(\"d\", area1);");  
            client.println("    }");
            client.println("    var line1 = d3.line().x(d => xScale(d.time)).y(d => yScale(d.temperature));");    
            client.println("    svg.append('path').data([data]).attr('class', 'line1').attr('d', line1);");   
            client.println("    svg.append(\"g\").attr(\"transform\", \"translate(0,\" + height + \")\").call(d3.axisBottom(xScale).tickFormat(d3.timeFormat(\"%H:%M:%S\"))).selectAll(\"text\").style(\"text-anchor\", \"end\").attr(\"dx\", \"-.8em\").attr(\"dy\", \".15em\").attr(\"transform\", \"rotate(-65)\");");
            client.println("    svg.append(\"text\").attr(\"transform\", \"translate(\" + (width+25) + \" ,\" + (height-10) + \")\").style(\"text-anchor\", \"middle\").text(input_title_x);");
            client.println("    svg.append(\"g\").call(d3.axisLeft(yScale));");
            client.println("    svg.append(\"text\").attr(\"y\", -30 ).attr(\"x\", 40 ).attr(\"dy\", \"1em\").style(\"text-anchor\", \"middle\").text(input_title_y);");
            client.println("    data.forEach(function(d){");
            client.println("      svg.append('circle').attr(\"cx\",xScale(d.time)).attr(\"cy\",yScale(d.temperature)).attr(\"r\",2).attr(\"title\",'test').attr(\"class\", \"point1\").append(\"svg:title\").text(d.temperature);");
            client.println("      svg.append('circle').attr(\"cx\",xScale(d.time)).attr(\"cy\",yScale(d.humidity)).attr(\"r\",2).attr(\"class\", \"point2\").append(\"svg:title\").text(d.humidity);");   
            client.println("    });");
            client.println("  }");
            client.println("</script>");
            client.println("<body onload=\"createChart('"+chartType+"','"+chartData+"',"+chartWidth+","+chartHeight+",'"+xTitle+"','"+yTitle+"',"+yScaleMax+","+yScaleMin+");setTimeout('location.reload();',"+timeInterval+");\">");
            client.println("</body></html>");
            client.println();
                        
            Feedback="";
            break;
          } else {
            currentLine = "";
          }
        } 
        else if (c != '\r') 
        {
          currentLine += c;
        }

        if ((currentLine.indexOf("/?")!=-1)&&(currentLine.indexOf(" HTTP")!=-1))
        {
          currentLine="";
          Feedback="";
          ExecuteCommand();
        }
      }
    }
    delay(1);
    client.stop();
  } 
}

void ExecuteCommand()
{
  Serial.println("");
  Serial.println("Command: "+Command);
  Serial.println("cmd= "+cmd+" ,str1= "+str1+" ,str2= "+str2+" ,str3= "+str3+" ,str4= "+str4+" ,str5= "+str5+" ,str6= "+str6+" ,str7= "+str7+" ,str8= "+str8+" ,str9= "+str9);
  Serial.println("");

  if (cmd=="chartType")
  {
    chartType=str1;
  }
  else if (cmd=="chartWidth")
  {
    chartWidth=str1.toInt();
  }  
  else if (cmd=="chartHeight")
  {
    chartHeight=str1.toInt();
  }    
  else if (cmd=="showNumber")
  {
    if (str1.toInt()<showNumber)
    {
      chartData="";
      count=0;
    }
    showNumber=str1.toInt();
  }  
  else if (cmd=="yScaleMax")
  {
    yScaleMax=str1.toInt();
  }
  else if (cmd=="yScaleMin")
  {
    yScaleMin=str1.toInt();
  }  
  else if (cmd=="resetwifi")
  {
    WiFi.begin(str1.c_str(), str2.c_str());
    Serial.print("Connecting to ");
    Serial.println(str1);
    long int StartTime=millis();
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(500);
        if ((StartTime+5000) < millis()) break;
    } 
    Serial.println("");
    Serial.println("STAIP: "+WiFi.localIP().toString());
    Feedback="STAIP: "+WiFi.localIP().toString();
  }   
  else 
  {
    Feedback="Command is not defined";
  }
}

void getCommand(char c)
{
  if (c=='?') ReceiveState=1;
  if ((c==' ')||(c=='\r')||(c=='\n')) ReceiveState=0;
  
  if (ReceiveState==1)
  {
    Command=Command+String(c);
    
    if (c=='=') cmdState=0;
    if (c==';') strState++;
  
    if ((cmdState==1)&&((c!='?')||(questionstate==1))) cmd=cmd+String(c);
    if ((cmdState==0)&&(strState==1)&&((c!='=')||(equalstate==1))) str1=str1+String(c);
    if ((cmdState==0)&&(strState==2)&&(c!=';')) str2=str2+String(c);
    if ((cmdState==0)&&(strState==3)&&(c!=';')) str3=str3+String(c);
    if ((cmdState==0)&&(strState==4)&&(c!=';')) str4=str4+String(c);
    if ((cmdState==0)&&(strState==5)&&(c!=';')) str5=str5+String(c);
    if ((cmdState==0)&&(strState==6)&&(c!=';')) str6=str6+String(c);
    if ((cmdState==0)&&(strState==7)&&(c!=';')) str7=str7+String(c);
    if ((cmdState==0)&&(strState==8)&&(c!=';')) str8=str8+String(c);
    if ((cmdState==0)&&(strState>=9)&&((c!=';')||(semicolonstate==1))) str9=str9+String(c);
    
    if (c=='?') questionstate=1;
    if (c=='=') equalstate=1;
    if ((strState>=9)&&(c==';')) semicolonstate=1;
  }
}
