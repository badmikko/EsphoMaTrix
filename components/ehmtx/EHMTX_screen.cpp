#include "esphome.h"

namespace esphome
{

  EHMTX_screen::EHMTX_screen(EHMTX *config)
  {
    this->config_ = config;
    this->endtime = 0;
    this->alarm = false;
  }

  bool EHMTX_screen::is_alarm() { return this->alarm; }

  bool EHMTX_screen::del_slot(uint8_t _icon)
  {
    if (this->icon == _icon)
    {
      this->endtime = 0;
      this->icon = 0;
      ESP_LOGD(TAG,"delete screen icon: %d",_icon);
      return true;
    }
    return false;
  }

  void EHMTX_screen::update_screen()
  {
    if (millis() - this->config_->last_scroll_time >= this->config_->scroll_intervall && this->pixels_ > (32 - 9))
    {
      this->shiftx_++;
      if (this->shiftx_ > this->pixels_ + (32-9))
      {
          this->shiftx_ = 0;
      }
      this->config_->last_scroll_time = millis();
    }
    if (millis() - this->config_->last_anim_time >= this->config_->anim_intervall &&
        (this->config_->icons[this->icon]->get_current_frame() <
         this->config_->icons[this->icon]->get_animation_frame_count()))
    {
      this->config_->icons[this->icon]->next_frame();
      this->config_->last_anim_time = millis();
    }
  }

  bool EHMTX_screen::active()
  {
    if (this->endtime > 0)
    {
      time_t ts = this->config_->clock->now().timestamp;
      if (ts < this->endtime)
      {
        return true;
      }
    }
    return false;
  }

  void EHMTX_screen::draw_()
  {
    int8_t extraoffset =0;

      if (this->pixels_ > (32 - 9))
      {
        extraoffset =32-9;
      }

    if (this->alarm)
    {
      this->config_->display->print(TEXTSCROLLSTART - this->shiftx_ + extraoffset + this->config_->xoffset, this->config_->yoffset, this->config_->font, this->config_->alarm_color,
                                   this->text.c_str());
    }
    else
    {
      this->config_->display->print(TEXTSCROLLSTART - this->shiftx_ + extraoffset + this->config_->xoffset, this->config_->yoffset, this->config_->font, this->text_color,
                                   this->text.c_str());
    }
    this->config_->display->line(8, 0, 8, 7, esphome::display::COLOR_OFF);
    if (this->alarm)
    {
      this->config_->display->draw_pixel_at(30, 0, this->config_->alarm_color);
      this->config_->display->draw_pixel_at(31, 1, this->config_->alarm_color);
      this->config_->display->draw_pixel_at(31, 0, this->config_->alarm_color);
    }
    this->config_->display->image(0, 0, this->config_->icons[this->icon]);
  }

  void EHMTX_screen::draw()
  {
    this->draw_();
    this->update_screen();
  }

  void EHMTX_screen::set_text(std::string text, uint8_t icon, uint8_t pixel, uint16_t et)
  {
    this->text = text;
    this->pixels_ = pixel;
    this->shiftx_ = 0;
    float dd = (2*(32-9+pixel)*this->config_->scroll_intervall) / 1000;
    this->display_duration = (dd > this->config_->screen_time) ? dd : this->config_->screen_time;
    ESP_LOGD(TAG,"display length text: %s t: %4f default: %d",text.c_str(),this->display_duration,this->config_->screen_time);
    this->endtime = this->config_->clock->now().timestamp + et * 60;
    if (this->alarm)
    {
      this->endtime += 2 * 60;
    }
    this->icon = icon;
  }

  void EHMTX_screen::set_text_color(int r, int g, int b)
  {
    this->text_color = Color((uint8_t)r & 248, (uint8_t)g & 252, (uint8_t)b & 248);
  }

}
