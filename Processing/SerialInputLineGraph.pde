/* Graphing sketch

 This program developed in order to create a line graph of the
 data received from the FluorescentMeter.ino code, implemented
 for Arduino. It takes ASCII-encoded strings from the serial port
 at 9600 baud rate and graphs them in a line graph. It expects
 values in the range 0 to 1023, followed by a newline, or newline
 and carriage return. A grid is also there in order to make the
 interpretation easier.


 Created 20 Apr 2005
 Updated 18 Jul 2015
 by Anastasios Glaros
*/

 import processing.serial.*;
 
 PGraphics topLayer;    // Initialize a new layer
 PrintWriter output;    // Initialize a new writer
 
 Serial myPort;         // The serial port
 int xPos = 1;          // horizontal position of the graph
 
 // Random RGB values for the color of the lines
 int R = int(random(50, 255));
 int G = int(random(50, 255));
 int B = int(random(50, 255));
 
 // Grid line intervals (about to change in a more sophisticated way)
 int XgridSize = 50;
 int YgridSize = 16;
 
 // Connect the points with lines for a line graph (about to change in a more sophisticated way)
 int maxPoints = 1;   // How many points to consider for each line
 PVector[] vectors = new PVector[maxPoints]; // Memorizing previous points
 int ctr = 0;
 int tctr = 0;

void setup () {
  // The width of the window have been set to 500px and the number
  // of the expected points is 100. The points are going to be draw
  // with a 5px interval. (A more sophisticated way is feasible)
  // set the window size:
  size(500, 256);
  smooth();

  String outFileName = "positions.txt";
  output = createWriter(outFileName);

  // List all the available serial ports
  // if using Processing 2.1 or later, use Serial.printArray()
  println(Serial.list());
 
  // Open whatever port is the one you're using.
  myPort = new Serial(this, Serial.list()[0], 9600);
 
  // don't generate a serialEvent() unless you get a newline character:
  myPort.bufferUntil('\n');
 
  // Create a top layer in order to draw the grid over the graph
  topLayer= createGraphics(width,height);
 
  // set inital background:
  background(0);
}
 
void draw () {
  topLayer.beginDraw();
  topLayer.stroke(255);
  topLayer.strokeWeight(1);
   
  // Print the grid lines (a more sophisticated way is feasible)
  for(int i = 0; i < width / XgridSize; i++){
    topLayer.line(i*XgridSize, 0, i*XgridSize, height);
  }
  for (int i = 0; i < height / YgridSize; i++){
    topLayer.line(0, i*YgridSize, width, i*YgridSize);
  }
   
  PFont f = createFont("Courier New",12,true);
  topLayer.textFont(f);
  topLayer.fill(255);                        // Specify font color 
  topLayer.text("0",0,256);
  topLayer.text("512",0,128);
  topLayer.text("1023",0,10);
  topLayer.text("50",250,256);
  topLayer.text("100",480,256);
  
  topLayer.endDraw();
   
  image( topLayer.get(0,0,width,height), 0,0);
}

void serialEvent (Serial myPort) {
  // get the ASCII string:
  String inString = myPort.readStringUntil('\n');
 
  if (inString != null) {
    // trim off any whitespace:
    inString = trim(inString);
  }
  output.println(inString);
   
  // convert to an int and map to the screen height:
  float inByte = float(inString);
  inByte = map(inByte, 0, 1023, 0, height);
  
  // Initialize a draw:
  stroke(R,G,B);
  strokeWeight(3);
  float y = height - inByte;
  float x = xPos;
  
  // Draw a point
  if(xPos <= 1) {
    point(xPos, height - inByte);
  }
  else {
    for (int i = 0; i < min(tctr, maxPoints); ++i) {
      // Draw a line to connect the previous with the current points
      line(vectors[i].x,vectors[i].y,x,y);
    }
  }
  // Update the vector to the new point
  vectors[ctr] = new PVector(xPos,y);
  ctr = (ctr + 1) % maxPoints;
  tctr += 1;
  
  // Every 100 points a new data are expected.
  // At the edge of the screen, go back to the beginning:
  if (xPos >= width - 4) {
    xPos = 1;
    
    // Choose a new color for the new line
    R = int(random(0, 255));
    G = int(random(0, 255));
    B = int(random(0, 255));
    
    output.println("End of the READ");
    output.flush();
  }
  else {
    // increment the horizontal position:
    xPos += 5;
  }
}
