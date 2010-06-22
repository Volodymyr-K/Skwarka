#include "Renderer.h"

Renderer::Renderer(): mp_display_update_callback(NULL), m_update_period(1.0)
  {
  m_last_display_update = tbb::tick_count::now();
  }

void Renderer::SetDisplayUpdateCallback(intrusive_ptr<DisplayUpdateCallback> ip_display_update_callback, double i_update_period)
  {
  ASSERT(i_update_period>0.0);
  if (i_update_period<=0.0) i_update_period=1.0;

  mp_display_update_callback = ip_display_update_callback;
  m_update_period = i_update_period;
  }

void Renderer::_UpdateDisplay(intrusive_ptr<const Film> ip_film, bool i_force_update) const
  {
  ASSERT(ip_film);

  if (mp_display_update_callback)
    {
    tbb::tick_count now = tbb::tick_count::now();
    if (i_force_update || (now-m_last_display_update).seconds() >= m_update_period)
      {
      m_last_display_update = now;
      mp_display_update_callback->Update(ip_film);
      }
    }
  }

DisplayUpdateCallback::DisplayUpdateCallback()
  {
  }