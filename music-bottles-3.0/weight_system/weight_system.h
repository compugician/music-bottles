enum ChangeType {
  added,
  removed
};

enum ObjectType {
  cap,
  bottle,
  both,
  none
};

enum ObjectShape {
  cone,
  straight,
  sphere
};

struct Change {
  bool valid;
  ChangeType type;
  ObjectType object;
  ObjectShape shape;
};

struct BottleState {
  bool bottleOn;
  bool capOn;
};

struct Bottles {
  BottleState cone;
  BottleState straight;
  BottleState sphere;
};


//*** move code below to c file ***/

String bottleToString(BottleState bs) {
  String result = "";
  result=((bs.bottleOn)?"Bottle: ON ":"Bottle: OFF");
  result+=" ";
  result+=((bs.capOn)?"Cap: ON ":"Cap: OFF");

  return result;
}

String bottleToStringShort(BottleState bs) {
  String result = "";
  result=((bs.bottleOn)?" ON  ":" OFF ");
  result+="   ";
  result+=((bs.capOn)?" ON  ":" OFF ");

  return result;  
}

String stateToString(Bottles b) {
  return "         Bottle   Cap\nCone:     "
        +bottleToStringShort(b.cone)+"\n"
        +"Straight: "+bottleToStringShort(b.straight)+"\n"
        +"Sphere:   "+bottleToStringShort(b.sphere)+"\n";
}

String changeToString(Change c) {
  String result = "Change: ";
  result += (c.valid) ? "VALID" : "INVALID";
  result += "\t";
  result += (c.type == added) ? "ADDED" : "REMOVED";
  result += "\t";
  switch (c.object) {
    case cap:
      result += "CAP";
      break;
    case bottle:
      result += "BOTTLE";
      break;
    case both:
      result += "BOTH";
      break;
    default:
      result += "UNKNONW";
      break;
  }

  result += "\t";

  switch (c.shape) {
    case cone:
      result += "CONE";
      break;
    case straight:
      result += "STRAIGHT";
      break;
    case sphere:
      result += "SPHERE";
      break;
    default:
      result += "UNKNOWN";
      break;
  }

  return result;
}

