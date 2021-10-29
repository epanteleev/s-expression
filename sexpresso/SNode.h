#pragma once

class Sexpression;

class SNode {
public:
    SNode() = default;
    virtual ~SNode() = default;

public:
    virtual Sexpression& addChild(Sexpression &&sexpression) = 0;
};
