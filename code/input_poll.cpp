
void process_input()
{
	while (SDL_PollEvent(&Event)) {
		if (Event.type == SDL_QUIT) {
			quit = true;
		}
		ImGui_ImplSDL2_ProcessEvent(&Event);
		if (Event.type == SDL_QUIT)
			quit = true;
		if (Event.type == SDL_WINDOWEVENT && Event.window.event == SDL_WINDOWEVENT_CLOSE && Event.window.windowID == SDL_GetWindowID(mainWindow))
			quit = true;
		switch (Event.type)
		{
            case SDL_WINDOWEVENT:
			switch (Event.window.event)
			{
                case SDL_WINDOWEVENT_RESIZED:
                {
                    SDL_SetWindowSize(mainWindow, Event.window.data1, Event.window.data2);
                    display_info.w = Event.window.data1;
                    display_info.h = Event.window.data2;
                    glViewport(0, 0, display_info.w, display_info.h);
                    break;
                    default:
                    break;
                }
			}
            case SDL_MOUSEMOTION:
            {
                SDL_Keymod kmod = SDL_GetModState();
                if (Event.motion.state & SDL_BUTTON_LMASK)
                {
                }
                break;
            }
            case SDL_MOUSEBUTTONDOWN:
            {
                SDL_Keymod kmod = SDL_GetModState();
                if (Event.button.button & SDL_BUTTON_LMASK)
                {
                    input_state.m_left = true;
                }
                if (Event.button.button & SDL_BUTTON_RMASK)
                {
                    input_state.m_right = true;
                }
                if ((Event.button.button & SDL_BUTTON_MIDDLE))
                {
                    input_state.m_middle = true;
                }
                break;
            }
            case SDL_MOUSEBUTTONUP:
            {
                if (Event.button.button & SDL_BUTTON_LMASK)
                {
                    input_state.m_left = false;
                }
                if (Event.button.button & SDL_BUTTON_RMASK)
                {
                    input_state.m_right = false;
                }
                if ((Event.button.button & SDL_BUTTON_MIDDLE))
                {
                    input_state.m_middle = false;
                }
                break;
            }
            
            case SDL_MOUSEWHEEL:
            {
                if (Event.wheel.y > 0)
                {
                    camera.zoom -= camera.zoom_sensitivity;
                }
                else if (Event.wheel.y < 0)
                {
                    camera.zoom += camera.zoom_sensitivity;
                }
                if (Event.wheel.x > 0)
                {
                }
                else if (Event.wheel.x < 0)
                {
                }
                break;
            }
            case SDL_KEYDOWN:
            {
                switch (Event.key.keysym.sym)
                {
                    case SDLK_m:
                    isWire = !isWire;
                    break;
                    
                    case SDLK_z:
                    mouse_look = !mouse_look;
                    mouse_mode_switch = true;
                    break;
                    
                    case SDLK_r:
                    input_state.k_r = true;
                    break;
                    
                    case SDLK_o:
                    camera.view_mode = CMODE_ORBIT;
                    mouse_mode_switch = true;
                    break;
                    
                    case SDLK_u:
                    camera.view_mode = CMODE_ORTHOGRAPHIC;
                    mouse_mode_switch = true;
                    break;
                    
                    case SDLK_i:
                    camera.view_mode = CMODE_FREE;
                    mouse_mode_switch = true;
                    break;
                    
                    
                    case SDLK_UP:
                    input_state.a_up = true;
                    translation_delta_current.z += 1.000 * time_state.dt;
                    break;
                    
                    case SDLK_DOWN:
                    input_state.a_down = true;
                    translation_delta_current.z -= 1.000 * time_state.dt;
                    break;
                    
                    case SDLK_RIGHT:
                    input_state.a_right = true;
                    translation_delta_current.x += 1.000 * time_state.dt;
                    break;
                    
                    case SDLK_LEFT:
                    input_state.a_left = true;
                    translation_delta_current.x -= 1.000 * time_state.dt;
                    break;
                    
                    case SDLK_a:
                    input_state.a_left = true;
                    input_state.move_dt.x = -1.0f;
                    break;
                    
                    case SDLK_d:
                    input_state.a_right = true;
                    input_state.move_dt.x = 1.0f;
                    break;
                    
                    case SDLK_w:
                    input_state.a_forward = true;
                    input_state.move_dt.z = 1.0f;
                    break;
                    
                    case SDLK_s:
                    input_state.a_backward = true;
                    input_state.move_dt.z = -1.0f;
                    break;
                    
                    case SDLK_LSHIFT:
                    input_state.lshift = true;
                    input_state.move_dt.y = 1.0f;
                    break;
                    
                    case SDLK_DELETE:
                    //input_state.k_delete = true;
                    break;
                    
                    case SDLK_SPACE:
                    input_state.k_space = true;
                    break;
                }
                break;
            }
            case SDL_KEYUP:
            {
                switch (Event.key.keysym.sym)
                {
                    case SDLK_UP:
                    input_state.a_up = false;
                    break;
                    
                    case SDLK_DOWN:
                    input_state.a_down = false;
                    break;
                    
                    case SDLK_RIGHT:
                    input_state.a_right = false;
                    break;
                    
                    case SDLK_LEFT:
                    input_state.a_left = false;
                    break;
                    
                    case SDLK_r:
                    input_state.k_r = false;
                    break;
                    
                    case SDLK_a:
                    input_state.a_left = false;
                    break;
                    
                    case SDLK_d:
                    input_state.a_right = false;
                    break;
                    
                    case SDLK_w:
                    input_state.a_forward = false;
                    break;
                    
                    case SDLK_s:
                    input_state.a_backward = false;
                    break;
                    
                    case SDLK_LSHIFT:
                    input_state.lshift = false;
                    break;
                    
                    case SDLK_DELETE:
                    input_state.k_delete = true;
                    break;
                    
                    case SDLK_RETURN:
                    input_state.k_enter = true;
                    break;
                    
                    case SDLK_SPACE:
                    input_state.k_space = false;
                    break;
                }
                break;
            }
		}
	}
}