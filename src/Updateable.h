/*
Copyright (c) 2023-2023 Webtech Projekt
*/

#pragma once

#include "Defnies.h"

class Updateable
{
public:
    virtual void Update(uint64_t difftime) {}
};