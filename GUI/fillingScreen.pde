button valves[];
PImage img;
boolean valvesStates[];

void initializeFillingScreen() {
    valves = new button[3];
    
    valves[0] = new button(width / 2 + 100, height - 200, 100, 75, color(0), color(255), 10, "Valve 1");
    valves[1] = new button(width / 2 + 250, height - 200, 100, 75, color(0), color(255), 10, "Valve 2");
    valves[2] = new button(width / 2 + 400, height - 200, 100, 75, color(0), color(255), 10, "Valve 3");
    
    img = loadImage("FuellingSystem.png");
    img.resize(1000, 0);
    
    valvesStates = new boolean[3];
    
    log("INITIALIZED FILLING SCREEN");
}

void fillingScreen() {
    if (state != prevState) {
        log("FILLING SCREEN");
        prevState = state;
    }
    
    image(img, 0, 0);
    
    for (int i = 0; i < valves.length; ++i) {
        boolean anterior = valvesStates[i];
        valvesStates[i] = valves[i].toggle();
        if (anterior != valvesStates[i]) {
            log("CHANGED VALVE STATE - " + (i + 1) + "->" + valvesStates[i]);
        }
    }
    
    colorCodingValves();
}

void colorCodingValves() {
    PVector valvesCoordinates[] = new PVector[3];
    valvesCoordinates[0] = new PVector(375, 242);
    valvesCoordinates[1] = new PVector(397, 711);
    valvesCoordinates[2] = new PVector(858, 94);
    
    for (int i = 0; i < 3; ++i) {
        fill(255, 0, 0);
        if (valvesStates[i]) {
            fill(0, 255, 0);
        }
        ellipse(valvesCoordinates[i].x, valvesCoordinates[i].y, 20, 20);
    }
}