#pragma once

#include <SDL2/SDL.h>
#include "vsx_application_sdl_tools.h"
#include <vsx_gl_state.h>
#include <string/vsx_string_helper.h>

class vsx_application_sdl
{
  SDL_Window *window; /* Our window handle */
  SDL_GLContext context;

  void update_viewport_size()
  {
    int height = 0, width = 0;
    SDL_GL_GetDrawableSize(window, &width, &height);

    // Get window size (may be different than the requested size)
    height = height > 0 ? height : 1;

    // Set viewport
    vsx_gl_state::get_instance()->viewport_set( 0, 0, width, height );
  }

  void setup()
  {
    if (vsx_argvector::get_instance()->has_param("-help"))
    {
      vsx_application_manager::get_instance()->get()->print_help();
      exit(0);
    }

    // make sure we don't get spinning wheel
    #if (PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS)
    DisableProcessWindowsGhosting();
    #endif

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) < 0)
        sdl_tools::sdldie("Unable to initialize SDL"); /* Or die on error */

    SDL_DisableScreenSaver();

    /* Turn on double buffering with a 24bit Z buffer.
     * You may need to change this to 16 or 32 for your system */
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    if (vsx_argvector::get_instance()->has_param("f") && !vsx_argvector::get_instance()->has_param_with_value("s"))
      window = SDL_CreateWindow(
        vsx_application_manager::get_instance()->get()->window_title_get().c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1,
        1,
        SDL_WINDOW_OPENGL
        | SDL_WINDOW_ALLOW_HIGHDPI
        | SDL_WINDOW_FULLSCREEN_DESKTOP
        | SDL_WINDOW_SHOWN
      );

    if (vsx_argvector::get_instance()->has_param("f") && vsx_argvector::get_instance()->has_param_with_value("s"))
      window = SDL_CreateWindow(
        vsx_application_manager::get_instance()->get()->window_title_get().c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        vsx_string_helper::s2i( vsx_argvector::get_instance()->get_param_subvalue("s", 0, "x,", "1280") ),
        vsx_string_helper::s2i( vsx_argvector::get_instance()->get_param_subvalue("s", 1, "x,", "720") ),
        SDL_WINDOW_OPENGL
        | SDL_WINDOW_ALLOW_HIGHDPI
        | SDL_WINDOW_FULLSCREEN
        | SDL_WINDOW_SHOWN
      );

    if (!vsx_argvector::get_instance()->has_param("f"))
      window = SDL_CreateWindow(
        vsx_application_manager::get_instance()->get()->window_title_get().c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        vsx_string_helper::s2i( vsx_argvector::get_instance()->get_param_subvalue("s", 0, "x,", "1280") ),
        vsx_string_helper::s2i( vsx_argvector::get_instance()->get_param_subvalue("s", 1, "x,", "720") ),
        SDL_WINDOW_OPENGL
        | SDL_WINDOW_ALLOW_HIGHDPI
        | SDL_WINDOW_SHOWN
        | SDL_WINDOW_BORDERLESS * (vsx_argvector::get_instance()->has_param("bl") ? 1 : 0)
      );

    SDL_StartTextInput();
    SDL_ShowCursor(0);

    sdl_tools::checkSDLError(__LINE__);

    if (vsx_argvector::get_instance()->has_param("gl_debug"))
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

    context = SDL_GL_CreateContext(window);
    sdl_tools::checkSDLError(__LINE__);

    SDL_GL_MakeCurrent( window, context );

    /* This makes our buffer swap syncronized with the monitor's vertical refresh */
    SDL_GL_SetSwapInterval(1);

    glewInit();

    printf("INFO: app_init\n");
    vsx_application_manager::get_instance()->get()->init(0);
    printf("INFO: app_init done\n");

    if (vsx_argvector::get_instance()->has_param_with_value("p"))
      SDL_SetWindowPosition(
        window,
            vsx_string_helper::s2i( vsx_argvector::get_instance()->get_param_subvalue("p", 0, "x,", "720") ),
            vsx_string_helper::s2i( vsx_argvector::get_instance()->get_param_subvalue("p", 1, "x,", "300") )
      );
  }

public:

  void run()
  {
    setup();

    bool running = true;
    size_t frames = 0;

    while( running )
    {
      update_viewport_size();

      SDL_Event event;
      while (SDL_PollEvent(&event))
      {
        switch(event.type)
        {
          case SDL_JOYAXISMOTION:
            if (event.caxis.value == -32768)
              event.caxis.value = -32767;
          break;

          case SDL_WINDOWEVENT:
            switch (event.window.event)
            {
              case SDL_WINDOWEVENT_CLOSE:
                running = false;
              break;
              case SDL_WINDOWEVENT_MAXIMIZED:
              case SDL_WINDOWEVENT_RESIZED:
                update_viewport_size();
              break;

              default:
              break;

            }
          break;
        }
        vsx_application_input_manager::get_instance()->consume( event );
      }


      vsx_application_manager::get_instance()->get()->pre_draw();

      frames ++;

      // Clear color buffer
      glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
      glClear( GL_COLOR_BUFFER_BIT );
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();

      vsx_application_manager::get_instance()->get()->draw(0);

      SDL_GL_SwapWindow(window);
    }

    // Close OpenGL window and terminate
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
  }
};
