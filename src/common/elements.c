#include "elements.h"

#include <stdio.h>

int ResizeElementsArray(Element*** arrayPtr, size_t* countPrt, size_t newCount) {
    Element** temp = (Element**)realloc(*arrayPtr, newCount * sizeof(Element*));

    if (temp == NULL) {
        fprintf(stderr, "Error: Memory reallocation failed. Array remains unchanged.\n");
        
        return 1; 
    }

    *arrayPtr = temp;
    *countPrt = (int)newCount;
    return 0;
}

Element* CreateUniqueElement(ElementType type, void* data) {
    Element* element = (Element*)malloc(sizeof(Element));
    element->data = data;
    element->type = type;
    return element;
}

Text* CreateText(char* text, float scale, Color color) {
    Text* txt = (Text*)malloc(sizeof(Text));
    txt->text
}
Element* CreateTextElement(Text* text);

Section* CreateSection(Vector2 size, Color color, Element* child);
void AddSectionChild(Section* section, Element* newChild);
Element* CreateSectionElement(Section* section);

Section* CreateButton(Vector2 size, Color color, Text* text, void (*onClick)(void));
Element* CreateButtonElement(Button* button);
