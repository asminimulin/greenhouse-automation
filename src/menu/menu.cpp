#include "menu.hpp"

#include <logging.hpp>

#include "config.hpp"
#include "idle.hpp"

namespace {

static constexpr uint8_t menuItemsCount = 9;
static MenuItem menuItems[menuItemsCount];
static uint8_t menuItemsUsed;
static uint8_t currentItemId;
static uint8_t childPosition;

void refreshChildPosition() {
  uint8_t parentId = menuItems[currentItemId].parent;
  childPosition = 0;
  for (int i = 0; i < currentItemId; ++i) {
    if (parentId == menuItems[i].parent) {
      childPosition++;
    }
  }
}

void nextItem() {
  uint8_t parentId = menuItems[currentItemId].parent;
  for (int i = 1; i < menuItemsUsed; ++i) {
    uint8_t id = (currentItemId + i) % menuItemsUsed;
    if (menuItems[id].parent == parentId) {
      if (currentItemId > id) {
        childPosition = 0;
      } else {
        childPosition++;
      }
      currentItemId = id;
      return;
    }
  }
  idle();
}

void previousItem() {
  uint8_t parentId = menuItems[currentItemId].parent;
  for (int i = 1; i < menuItemsUsed; ++i) {
    uint8_t id = (menuItemsUsed + currentItemId - i) % menuItemsUsed;
    if (menuItems[id].parent == parentId) {
      currentItemId = id;
      refreshChildPosition();
      return;
    }
  }
  idle();
}

void selectFirstChild() {
  auto id = currentItemId;
  for (int i = 0; i < menuItemsUsed; ++i) {
    if (menuItems[i].parent == id) {
      childPosition = 0;
      currentItemId = i;
      return;
    }
  }
  idle();
}

void selectParent() {
  if (currentItemId == 0) {
    return;
  }
  currentItemId = menuItems[currentItemId].parent;
  refreshChildPosition();
}

}  // namespace

namespace ns_menu {

void init() {
  menuItemsUsed = 0;
  currentItemId = 0;
  childPosition = 0;

  MenuItem root;
  root.name = F("Menu");
  root.parent = -1;
  root.isLeaf = false;
  root.isActivated = false;
  addItem(root);
}

uint8_t addItem(const MenuItem& newItem) {
  if (menuItemsUsed == menuItemsCount) {
    logging::error() << F("No more menu items to hold full menu");
    idle();
  }
  MenuItem& item = menuItems[menuItemsUsed];
  menuItemsUsed++;
  item.name = newItem.name;
  item.parent = newItem.parent;
  item.value = newItem.value;
  item.isLeaf = newItem.isLeaf;
  item.isActivated = newItem.isActivated;
  item.validator = newItem.validator;
  item.hasCustomRepresenation = newItem.hasCustomRepresenation;
  item.represent = newItem.represent;
  return menuItemsUsed - 1;
}

void handleIncrement() {
  MenuItem& item = menuItems[currentItemId];
  if (item.isLeaf && item.isActivated) {
    if (!item.validator || item.validator(item.ownValue + 1)) {
      item.ownValue++;
    }
    return;
  }
  if (isEnabled()) {
    nextItem();
  }
}

void handleDecrement() {
  MenuItem& item = menuItems[currentItemId];
  if (item.isLeaf && item.isActivated) {
    if (!item.validator || item.validator(item.ownValue - 1)) {
      item.ownValue--;
    }
    return;
  }
  if (isEnabled()) {
    previousItem();
  }
}

void handleIncrementPress() {
  MenuItem& item = menuItems[currentItemId];
  if (item.isLeaf) {
    if (!item.isActivated) {
      item.isActivated = true;
      item.ownValue = *item.value;
    }
    return;
  }
  selectFirstChild();
}

void handleDecrementPress() {
  MenuItem& item = menuItems[currentItemId];
  if (item.isLeaf && item.isActivated) {
    if (!item.validator || item.validator(item.ownValue)) {
      item.isActivated = false;
      *(item.value) = item.ownValue;
      greenhouse.saveSettings();
    }
    return;
  }
  if (currentItemId != 0) {
    selectParent();
  }
}

bool isEnabled() { return currentItemId != 0; }

uint8_t getRoot() { return 0; }

uint8_t getParent() { return menuItems[currentItemId].parent; }

uint8_t getCurrentMenuItemId() { return currentItemId; }

void renderMenu(char* firstRow, char* secondRow) {
  MenuItem& item = menuItems[currentItemId];
  if (item.isLeaf && item.isActivated) {
    auto fstring = reinterpret_cast<PGM_P>(item.name);
    strcpy_P(firstRow, fstring);
    if (item.hasCustomRepresenation) {
      item.represent(item.ownValue, secondRow);
    } else {
      sprintf(secondRow, "%d*", item.ownValue);
    }
  } else {
    auto fstring = reinterpret_cast<PGM_P>(menuItems[getParent()].name);
    strcpy_P(firstRow, fstring);
    fstring = reinterpret_cast<const char*>(item.name);
    int index = 0;
    index = sprintf(secondRow, "%d.", childPosition + 1);
    strcpy_P(&secondRow[index], fstring);
  }
}

}  // namespace ns_menu