#pragma once

class Animator;

class IAnimNotify
{
public:
    IAnimNotify();
    virtual ~IAnimNotify();

    virtual void Notify(Animator* animator);
};
