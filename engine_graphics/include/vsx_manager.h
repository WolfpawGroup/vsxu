/**
* Project: VSXu: Realtime visual programming language, music/audio visualizer, animation tool and much much more.
*
* @author Jonatan Wallmander, Vovoid Media Technologies Copyright (C) 2003-2011
* @see The GNU Public License (GPL)
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#ifndef VSX_MANAGER_H
#define VSX_MANAGER_H

#include <vsx_string.h>
#include <vsx_platform.h>
#include <string>
#include <vector>

#if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
#define VSX_MANAGER_DLLIMPORT
#else
  #if defined(VSX_ENG_DLL)
    #define VSX_MANAGER_DLLIMPORT __declspec (dllexport)
  #else 
    #define VSX_MANAGER_DLLIMPORT __declspec (dllimport)
  #endif
#endif
class vsx_manager_abs
{
public:
  // init manager with base path to where the effects (.vsx files) can be found
  // i.e. if base_path is /usr/share/vsxu/   then the engine will look in
  // /usr/share/vsxu/_visuals (linux)
  //
  // sound types can be "pulseaudio", "media_player", "fmod"
  virtual void init(const char* base_path, const char* sound_type) {};

  // before you render first time, you need to start
  virtual void start() {};
  virtual void render() {};
  // if you are going to destroy the GL Context - as is the case
  // when destroying a window - when undocking a window or going fullscreen perhaps
  // you need top stop() the engine so all visuals can unload their OpenGL handles and
  // get ready to be started again.
  virtual void stop() {};

  // flipping through different visuals
  virtual void toggle_randomizer() {};
  virtual void set_randomizer(bool status) {};
  virtual bool get_randomizer_status() { return false; };
  virtual void pick_random_visual() {};
  virtual void next_visual() {};
  virtual void prev_visual() {};
  // if not empty string vsxu is asynchronously loading a new visualization before
  // fading to it, this method can be used to display to the user "loading visual xxxxxx..."
  virtual std::string visual_loading() { return ""; };

  // dump the list of all loadable visuals
  std::vector<std::string> get_visual_filenames();

  // provide metadata for information about current running visualization
  virtual std::string get_meta_visual_filename() {return ""; };
  virtual std::string get_meta_visual_name() { return "";    };
  virtual std::string get_meta_visual_creator() { return ""; };
  virtual std::string get_meta_visual_company() { return ""; };

  // amplification/fx level (more = flashier, less = less busy)
  virtual float get_fx_level() { return 0.0f;};
  virtual void inc_fx_level() {};
  virtual void dec_fx_level() {};

  // time speed (more = faster movements, less = slow motion)
  virtual float get_speed() { return 0.0f;};
  virtual void inc_speed() {};
  virtual void dec_speed() {};

  // update engine sound data,
  // arrays MUST be 512 floats long
  virtual void set_sound_freq(float* data) {};
  virtual void set_sound_wave(float* data) {};

  // arbitrary engine information (statistics etc)
  // returns information about currently playing effect
  virtual int get_engine_num_modules() { return 0; };
};

extern "C" {
VSX_MANAGER_DLLIMPORT vsx_manager_abs* manager_factory();
VSX_MANAGER_DLLIMPORT void manager_destroy(vsx_manager_abs* manager);
}
#endif
