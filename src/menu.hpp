#pragma once

#include "global.hpp"
#include <vector>

class Menu;

class MenuItem {
private:
  String title;
  std::function<void()> callback;
  Menu* submenu = nullptr;
  bool is_exit = false;

public:
  MenuItem(String _title, std::function<void()> _callback = nullptr) {
    title = _title;
    callback = _callback;
  }

  MenuItem(String _title, Menu* _submenu)
  {
    title = _title;
    submenu = _submenu;
    callback = nullptr;
  }

  static MenuItem ExitItem()
  {
    MenuItem item("[Exit]");
    item.is_exit = true;
    return item;
  }

  void RunCallback(bool do_exit = 0, bool* exit_menu = nullptr)
  {
    if (exit_menu) {
      *exit_menu = do_exit;
    }
    if (callback) callback();
  }

  const String& GetTitle() const {
    return title;
  }

  Menu* GetSubmenu() const {
    return submenu;
  }

  bool IsExit() const {
    return is_exit;
  }
};

class Menu {
private:
  bool do_exit = false;
  std::vector<MenuItem> items;
  int selected_index = 0;
  int scroll_offset = 0;
  Menu* parent = nullptr;
  unsigned long last_scroll_time = 0;
  int text_scroll_offset = 0;

  float selection_y = 0;
  float target_selection_y = 0;

  float scrollbar_y = 0;
  float target_scrollbar_y = 0;

  float enter_progress = 0;
  bool entering = false;
  unsigned long anim_start = 0;

public:
  bool is_rendering = false;

  void AddItem(const MenuItem& item)
  {
    items.push_back(item);
  }

  void Revive(Menu* _parent = nullptr, bool from_child = false, bool from_exit = false)
  {
    is_rendering = true;
    do_exit = false;
    selected_index = 0;
    scroll_offset = 0;
    text_scroll_offset = 0;
    last_scroll_time = millis();
    selection_y = 0;
    target_selection_y = 0;
    scrollbar_y = 0;
    target_scrollbar_y = 0;
    if (_parent) parent = _parent;

    if (parent) {
        bool hasExit = false;
        for (auto& it : items) {
            if (it.IsExit()) { hasExit = true; break; }
        }
        if (!hasExit) items.push_back(MenuItem::ExitItem());
    }

    enter_progress = 0;
    entering = true;
    anim_start = millis();
  }

  void MoveUp()
  {
    if (selected_index > 0) {
      selected_index--;
      text_scroll_offset = 0;
      last_scroll_time = millis();
      if (selected_index < scroll_offset) {
        scroll_offset = selected_index;
      }
    }
  }

  void MoveDown()
  {
      if (selected_index < (int)items.size() - 1) {
          selected_index++;
          text_scroll_offset = 0;
          last_scroll_time = millis();
          int maxVisible = display.height() / (8 + 2);
          if (selected_index >= scroll_offset + maxVisible) {
              scroll_offset = selected_index - maxVisible + 1;
          }
      }
  }

  Menu* Select(bool* exit_menu = nullptr)
  {
    if (items.empty()) return nullptr;
    if (items[selected_index].IsExit()) {
      if (parent) {
        parent->Revive(nullptr, false, true);
        return parent;
      }
      do_exit = true;
      return nullptr;
    }
    Menu* submenu = items[selected_index].GetSubmenu();
    if (submenu) {
      submenu->Revive(this, true, false);
      return submenu;
    }
    items[selected_index].RunCallback(true, exit_menu);
    return nullptr;
  }

  Menu* HandleButtons()
  {
    if (!entering)
    {
      if (ReadButtonWait(BUTTON_RIGHT))
          MoveDown();
      else if (ReadButtonWait(BUTTON_LEFT))
          MoveUp();
      else if (ReadButtonWait(BUTTON_CENTER)) {
        HaltTillRelease(BUTTON_CENTER);  
        return Select();
      }
    }
    
    return nullptr;
  }

  Menu* GetParent() {
    return parent;
  }

    void Render(bool dont_display = false)
    {
        if (!is_rendering) return;
        if (do_exit) { is_rendering = false; return; }

        int display_width = display.width();
        int display_height = display.height();

        int line_height = 8 + 2;
        int visible_lines = display_height / line_height;

        display.clearDisplay();

        unsigned long now = millis();

        if (entering) {
            unsigned long elapsed = millis() - anim_start;
            enter_progress = min(1.0f, elapsed / 200.0f);
            if (enter_progress >= 1.0f) entering = false;
        }

        target_selection_y = (selected_index - scroll_offset) * line_height;
        selection_y += (target_selection_y - selection_y) * 0.3f;
        if (fabs(target_selection_y - selection_y) < 0.5f) {
            selection_y = target_selection_y;
        }

        display.drawRect(0, 0, display_width, display_height, SSD1306_WHITE);

        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);

        bool show_scrollbar = (int)items.size() > visible_lines;

        if (show_scrollbar) {
            int track_h = display_height;
            int bar_h = (visible_lines * track_h) / items.size();
            int max_scroll = items.size() - visible_lines;
            int target_y = 0;
            if (max_scroll > 0) {
                target_y = (scroll_offset * (track_h - bar_h)) / max_scroll;
            }
            target_scrollbar_y = target_y;
            scrollbar_y += (target_scrollbar_y - scrollbar_y) * 0.2f;
            if (fabs(target_scrollbar_y - scrollbar_y) < 0.5f) {
                scrollbar_y = target_scrollbar_y;
            }
        }

        display.setTextWrap(false);

        for (int i = 0; i < visible_lines; i++) {
            int item_index = i + scroll_offset;
            if (item_index >= (int)items.size()) break;

            float item_offset = (1.0f - enter_progress) * 12.0f;
            int pos_y = (i + 1) * line_height + (int)item_offset;

            const char* text = items[item_index].GetTitle().c_str();
            int text_width = strlen(text) * 6;
            int available_width = show_scrollbar ? display_width - 10 : display_width - 6;

            if (item_index == selected_index) {
              int box_width = show_scrollbar ? display_width - 6 : display_width - 2;
              display.fillRect(1, (int)(selection_y + 1 + item_offset), box_width, line_height, SSD1306_WHITE);
              display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);

              if (text_width > available_width) {
                  static unsigned long last_scroll_time = 0;
                  static unsigned long reset_delay = 0;

                  if (now - last_scroll_time > 30) {
                      text_scroll_offset++;
                      last_scroll_time = now;
                  }

                  int x = 4 - text_scroll_offset;

                  if (x + text_width > 0) {
                      display.setCursor(x, pos_y - 8);
                      display.print(text);
                      reset_delay = 0;
                  } else {
                      if (reset_delay == 0) reset_delay = millis();
                      text_scroll_offset = -available_width;
                      reset_delay = 0;
                  }
              } else {
                  display.setCursor(4, pos_y - 8);
                  display.print(text);
              }

              display.setTextColor(SSD1306_WHITE);
          } else {
              int maxChars = available_width / 6;
              for (int c = 0; c < maxChars && text[c] != '\0'; c++) {
                  display.setCursor(4 + c * 6, pos_y - 8);
                  display.write(text[c]);
              }
          }
        }

        if (show_scrollbar) {
            int bar_x = display_width - 4;
            int track_h = display_height;
            int bar_h = (visible_lines * track_h) / items.size();
            display.drawRect(bar_x, 0, 3, display_height, SSD1306_WHITE);
            display.fillRect(bar_x + 1, (int)scrollbar_y, 1, bar_h, SSD1306_WHITE);
        }

        display.setTextWrap(true);

        if (!dont_display)
            display.display();
    }

    const String& GetSelectedTitle() const
    {
        static String empty = "";
        if (items.empty()) return empty;
        return items[selected_index].GetTitle();
    }
};
