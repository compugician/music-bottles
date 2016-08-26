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

