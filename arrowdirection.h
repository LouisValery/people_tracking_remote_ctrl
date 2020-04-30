#ifndef ARROWDIRECTION_H
#define ARROWDIRECTION_H

class ArrowDirection {
public:
    enum Direction {
        kDirectionDown,
        kDirectionRight,
        kDirectionLeft,
        kDirectionUp
    };

public:
    virtual ~ArrowDirection() {}
    virtual void order_to_robot(Direction) = 0;
};

#endif // ARROWDIRECTION_H
