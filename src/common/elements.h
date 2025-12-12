#ifndef ELEMENTS_H
#define ELEMENTS_H

#include <stdlib.h>

#include "types.h"

typedef struct  {
    ElementType type;
    void* data;
} Element;

typedef enum {
    TEXT, SECTION, BUTTON
} ElementType;

int ResizeElementsArray(Element*** arrayPtr, size_t* countPrt, size_t newCount);

// Creates an element from types not already defined
Element* CreateUniqueElement(ElementType type, void* data);

typedef struct Text {
    float scale;
    char* text;
    Color color;
} Text;

Text* CreateText(char* text, float scale, Color color);
Element* CreateTextElement(Text* text);


typedef struct Section {
    Vector2 size;
    Color color;
    Element** children;
    int childrenCount;
} Section;

Section* CreateSection(Vector2 size, Color color, Element* child);
void AddSectionChild(Section* section, Element* newChild);
Element* CreateSectionElement(Section* section);

typedef struct Button {
    Vector2 size;
    Color color;
    Text* text;
    void (*onClick)(void);
} Button;

Section* CreateButton(Vector2 size, Color color, Text* text, void (*onClick)(void));
Element* CreateButtonElement(Button* button);

#endif