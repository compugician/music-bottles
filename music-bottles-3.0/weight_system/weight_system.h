enum ChangeType {
  added,
  removed
};

enum ObjectType {
  cap,
  bottle,
  both
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
