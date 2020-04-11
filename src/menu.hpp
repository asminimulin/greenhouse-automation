#ifndef MENU_HPP
#define MENU_HPP

#include <Arduino.h>

#include "greenhouse/greenhouse.hpp"


extern Greenhouse firstGreenhouse;
extern Greenhouse secondGreenhouse;


struct MenuItem {
    const void* name;
    int8_t* value;
    bool(*validator)(int8_t) = 0;
    void(*represent)(int8_t,char*) = 0;
    int8_t parent: 4;
    uint8_t isLeaf: 1;
    uint8_t isActivated: 1;
    uint8_t hasCustomRepresenation: 1;
    int8_t ownValue;
    MenuItem() {
        name = nullptr;
        value = &ownValue;
        validator = nullptr;
        represent = nullptr;
        parent = 0;
        isLeaf = false;
        isActivated = false;
        hasCustomRepresenation = false;
        ownValue = 0;
    }
};

namespace ns_menu {

void init();


uint8_t addItem(const MenuItem& newItem);


void handleIncrement();


void handleDecrement();


void handleIncrementPress();


void handleDecrementPress();


bool isEnabled();


uint8_t getParent();


uint8_t getRoot();


uint8_t getCurrentMenuItemId();


void renderMenu(char* firstRow, char* secondRow);

}


#endif