#define _CRT_SECURE_NO_WARNINGS
#define TINYOBJLOADER_IMPLEMENTATION
#define GLEW_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#define NK_IMPLEMENTATION

typedef unsigned short ui16;
typedef unsigned int   ui32;
typedef unsigned char  uc16;
typedef short          i16;
typedef int            i32;

double second_per_tick;

#include <Windows.h>
#include <SDL.h>
#include <SDL_syswm.h>
#include <SDL_image.h>
#include <vec3.hpp> // glm::vec3
#include <vec4.hpp> // glm::vec4
#include <mat4x4.hpp> // glm::mat4
#include <gtc/matrix_transform.hpp>
#include <gtc/matrix_inverse.hpp>
#include <gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <gl/GL.h>

#include <map>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <cassert>
#include <math.h>
#include <malloc.h>
#include <algorithm>
#include <tiny_obj_loader.h>
#include <regex>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <nlohmann/json.hpp>


//#include <SDL_ttf.h>
#include <ft2build.h>
#include FT_FREETYPE_H

//@kepler: namespaces before cpp files because they rely on them.
using namespace rapidjson;
using namespace glm;
using namespace std;

std::vector<vec3> s_data;
std::vector<vec3> tmp_data;
std::vector<vec2> texture_data;
std::vector<vec3> normal_data;

std::vector<unsigned int> xover;
std::vector<unsigned int> v_index;
std::vector<unsigned int> indices;
map <string, unsigned int> temp_map;

float VAR_G = -9.8f;
#include "defines.cpp"
#include "structs.cpp"
#include "camera.cpp"
#include "utility_functions.cpp"
#include "shaders.cpp"
#include "model.cpp"
#include "render_functions.cpp"
#include "collision.cpp"
#include "file_io.cpp"
#include "entities.cpp"

//#pragma comment(lib, "Ws2_32.lib") winsock
#pragma comment(lib, "winmm.lib")

#include "dsound.h"
#define DIRECT_SOUND_CREATE(name) HRESULT WINAPI name(LPCGUID pcGuidDevice, LPDIRECTSOUND* ppDS, LPUNKNOWN pUnkOuter)

typedef DIRECT_SOUND_CREATE(direct_sound_create);

#include "sound.cpp"

object o;
vector <object> objects;

vec2 window_center = { display_info.w / 2, display_info.h / 2 };
vec2 mouse_delta2d;
vec2 mouse_delta2d_last;
vec2 mouse_delta2d_current;

vec2 mouse_dt;
vec2 mouse_dt_last;
vec2 mouse_dt_current;

vec3 mouse_dt3;
vec3 mouse_delta3d_last;
vec3 mouse_delta3d_current;

fvec3 translation_delta;
fvec3 translation_delta_last;
fvec3 translation_delta_current;

//#include "nlohmann_loop.cpp"
#include "rapidjson_loop.cpp"
#include "game_loop.cpp"

//size of memory block measured in sizeof(beams) * num
#define beam_mem_size = 1024
#define sine_degrees sinf(time_state.seconds_passed) * (180/pi)
//#include <stb_truetype.h>



SDL_Event Event;
SDL_Window *mainWindow;
SDL_GLContext glContext;

GLuint model_list;
Camera camera_object;

static int isWire = 1;
static int isFog = 1;
int		   isOrtho = 1;
bool	   quit = false;
__int64	   FPS = 0;
float	   speed = 0.1f;
bool	   mouse_look = false;
bool	   mouse_mode_switch = false;
bool	   show_text = false;

#include "input_poll.cpp"

void close()
{
	SDL_DestroyWindow(mainWindow);
	mainWindow = NULL;
	SDL_Quit();
}

struct buffer_data {
	ui32 id;
};

struct urstupid {
	int a, b;
	vec3 midpoint;
};

void GLAPIENTRY
MessageCallback(GLenum source,
                GLenum type,
                GLuint id,
                GLenum severity,
                GLsizei length,
                const GLchar* message,
                const void* userParam)
{
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
            type, severity, message);
}

bool show_demo_window = true;
bool show_another_window = false;
vec4 clear_color = fvec4(0.15f, 0.15f, 0.1f, 1.0f);

int main(int argc, char* argv[])
{
	const char* glsl_version = "#version 130";
    
	if (SDL_Init(SDL_INIT_VIDEO) == 0)
	{
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
        
		display_info.num_displays = SDL_GetNumVideoDisplays();
        
		SDL_DisplayMode mode;
		SDL_GetCurrentDisplayMode(0, &mode);
        
		SDL_Rect a;
		SDL_GetDisplayUsableBounds(0, &a);
		display_info.cl_area.x = a.x;
		display_info.cl_area.y = a.y;
		display_info.cl_area.z = a.w;
		display_info.cl_area.w = a.h;
        
		display_info.w = a.w;
		display_info.h = a.h;
        display_info.show_entity_bases = 1;
		mainWindow = SDL_CreateWindow("Beaminster", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      display_info.cl_area.x, display_info.cl_area.y, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
		if (!mainWindow)
		{
			printf("ERROR: can't create window");
		}
        
		ivec2 drawable_area;
		SDL_GL_GetDrawableSize(mainWindow, &drawable_area.x, &drawable_area.y);
        
		ivec4 border_area;
		SDL_GetWindowBordersSize(mainWindow, &border_area.x, &border_area.y,
                                 &border_area.z, &border_area.w);
        
		glContext = SDL_GL_CreateContext(mainWindow);
		SDL_GL_MakeCurrent(mainWindow, glContext);
		//SDL_SetWindowResizable(mainWindow, SDL_TRUE);
		if (SDL_GL_SetSwapInterval(0) < 0)
		{
			printf("SDL_GL_SetSwapInterval(0) failed: %s\n", SDL_GetError());
		}
        
		glViewport(0, 0, drawable_area.x, drawable_area.y);
        
		glewExperimental = GL_TRUE;
		if (!glewInit() == GLEW_OK)
		{
			printf("ERROR: glew init failed.");
		}
		glEnable(GL_DEBUG_OUTPUT);
		//glDebugMessageCallback(MessageCallback, 0);
	}
	else
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return 0;
	}
    
    
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
    
	SDL_GL_SetSwapInterval(1);
	int w, h;
	SDL_GetWindowSize(mainWindow, &w, &h);
	glViewport(0, 0, w, h);
    
	//Model icosphere = import_model("E:/icosphere.obj");
    
    Model camera_model = import_model("../data/camera.obj");
    
	string jbeam_file = file_to_buf("../data/Cube.jbeam");
    
	string fixed_jbeam = JBEAM_FixCommas(jbeam_file);
    
	//vector <object> objects;
    
	Document document;
	document.Parse(fixed_jbeam.c_str());
    
	o.nodes.reserve(1024);
	vector<map<string, unsigned int>> node_beam_maps;
    
	ui32 beam_group_index = 0;
	ui32 node_start_offset = 0;
    
	nlohmann::json jbeam_d;
	{
		using namespace nlohmann;
        
		jbeam_d = json::parse(fixed_jbeam);
	}
    
	vector <unsigned int> indices;
	vector <jvertex> sorted_nodes;
	vector <urstupid> verteces_edges;
	int ai = 0;
	int bi = 0;
	unsigned int b, a;
	map <string, unsigned int>::iterator ait;
	map <string, unsigned int>::iterator bit;
	map <string, unsigned int>::iterator e;
    
	for (ui32 osz = 0; osz < objects.size(); osz++)
	{
		for (int sz = 0; sz < objects[osz].beams.size(); sz++)
		{
			ui32 num_of_beams = 0;
			for (unsigned int i = 0; i < objects[osz].beams[sz].beams.size(); i++)
			{
				string sa = objects[osz].beams[sz].beams.at(i).a;
				string sb = objects[osz].beams[sz].beams.at(i).b;
                
				e = node_beam_maps[osz].end();
                
                
				ait = node_beam_maps[osz].find(sa);
				if (ait != e) ai = ait->second;
                
				bit = node_beam_maps[osz].find(sb);
				if (bit != e) bi = bit->second;
                
				if ((ai || bi) != node_beam_maps[osz].size())
				{
					jvertex tai, tbi;
					tai = objects[osz].nodes.at(ai);
					tbi = objects[osz].nodes.at(bi);
					sorted_nodes.push_back(tai);
					sorted_nodes.push_back(tbi);
					num_of_beams++;
					objects[osz].beams[sz].beam_num++;
				}
			}
			objects[osz].beam_num_total += num_of_beams;
			for (int i = 0; i < sorted_nodes.size(); i++)
			{
				//a = 84;
				b = sorted_nodes[i].index;
				indices.push_back(b);
				//m = (a<=b) ? b : a;
			}
			objects[osz].beams[sz].indices = indices;
			indices = {};
			sorted_nodes = {};
		}
	}
	//@kepler: important to move object o with all nodes to a already declared struct (jmodel), before initilizing pointers!
	JModel jmodel;// = init_jbeam(objects);
    
	for (int i = 0; i < jmodel.data.size(); i++)
	{
		jmodel.data[i].nodes.reserve(1024);
	}
	jmodel.data = objects;
    
	void *beam_arena = malloc(16384 * sizeof(beam_pointer));
    
	bool start = false;
	beam_pointer *walker = (beam_pointer*)beam_arena;
	beam_pointer* ptr_offset = (beam_pointer*)beam_arena;
	beam_pointer *prev = 0;
	beam_pointer *next = 0;
	beam_pointer *head = 0;
	bool is_ok = 0;
	int bgsz = 0;
	int offset = 0;
	int index = 0;
    
	for (int osz = 0; osz < jmodel.data.size(); osz++)
	{
		for (int sz = 0; sz < jmodel.data[osz].beams.size(); sz++)
		{
			for (bgsz = 0; bgsz < jmodel.data[osz].beams[sz].beams.size(); bgsz++)
			{
				string sa = jmodel.data[osz].beams[sz].beams.at(bgsz).a;
				string sb = jmodel.data[osz].beams[sz].beams.at(bgsz).b;
                
				e = node_beam_maps[osz].end();
                
				ait = node_beam_maps[osz].find(sa);
				bit = node_beam_maps[osz].find(sb);
				if (ait != e && bit != e)
				{
					bi = bit->second;
					ai = ait->second;
                    
					jmodel.data[osz].nodes[ai].weight = rand();
					jmodel.data[osz].nodes[bi].weight = rand();
                    
					walker->a = &jmodel.data[osz].nodes[ai];
					walker->b = &jmodel.data[osz].nodes[bi];
					walker->selected = false;
					walker->id = index;
					walker->group = &jmodel.data[osz].beams[sz];
					walker->parent_object = &jmodel.data[osz];
					index++;
					if (start) {
						prev->next = walker;
						walker->previous = prev;
					}
					else
					{
						head = walker;
					}
					prev = walker;
                    
					walker++;
					start = true;
					is_ok = true;
					offset++;
#if 0
					std::cout << "beam at object - " << to_string(osz) << " and b. group - " << to_string(sz) << " [" << sa << ", " << sb << "] is ok." << endl;
#endif
				}
				else {
#if 0
					std::cout << "beam at object - " << to_string(osz) << " and b. group - " << to_string(sz) << " [" << sa << ", " << sb << "] couldn't match nodes in jbeam file." << endl;
#endif
				}
			}
			if (is_ok)
			{
				head->previous = prev;
				prev->next = head;
				jmodel.data[osz].beams[sz].beam_data = ptr_offset;
				ptr_offset = ptr_offset + offset;
				offset = 0;
				is_ok = false;
			}
			//objects.push_back(o);
		}
        jmodel.did_import_fail = false;
	}
    
	default_shader = create_shader("shaders/vertex_shader.txt", "shaders/fragment_shader.txt");
	
	cube_shader = create_shader("shaders/cube_shader.txt", "shaders/fragment_shader.txt");
	
	cursor_shader = create_shader("shaders/plain_shader.txt", "shaders/fragment_shader.txt");
	text_shader = create_shader("shaders/text_shader.txt", "shaders/text_fragment.txt");
	billboard_shader = create_shader("shaders/billboard_shader.txt", "shaders/fragment_shader.txt");
	texture_shader = create_shader("shaders/texture_shader.txt", "shaders/texture_fragment.txt");
	circle_shader = create_shader("shaders/circle_shader.txt", "shaders/circle_fragment.txt");
    
    
	float depth = 0.0f;
    
    
	Transform t = {};
	t.m = mat4(1.0f);
    
	camera.init_camera(vec3(0.0f, 0.0f, 0.0f), vec3_zero, vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, 1.0f, 0.0f),
                       90.0f, 0.0f, 0.5f, 0.5f, ZOOM, ZOOM_SENSITIVITY, 0.1f, 1000.0f);
    
	glm::mat4 m_projection = glm::perspective(glm::radians(45.0f), 16.0f / 9.0f, 0.1f, 10.0f);
    
	//glBindVertexArray(0);
    
	//glEnableClientState(GL_VERTEX_ARRAY);
    
    
	LARGE_INTEGER Frequency;
	QueryPerformanceFrequency(&Frequency);
	second_per_tick = 1.0 / (double)Frequency.LowPart;
    
	LARGE_INTEGER tick_before_loop;
	QueryPerformanceCounter(&tick_before_loop);
    
	vec3 p_near;
	vec3 p_far;
    
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
    
	ImGui_ImplSDL2_InitForOpenGL(mainWindow, glContext);
	ImGui_ImplOpenGL3_Init(glsl_version);
    
	//ImGui::StyleColorsDark();
	ImGui::StyleColorsClassic();
    
	int frame_rate = 0;
	float current_time = 0;
	float last_time = 0;
    
	vector <ui32> active_nodes;
	vector <beam_pointer*> free_beam_space;
	bool was_pressed = false;
	bool selected = false;
	bool is_initted = false;
	bool beam_debug_window_toggle = false;
	bool beam_debug_window_howered = false;
	bool node_debug_window_howered = false;
	ui32 node_mean_weight = 0;
	ui32 node_mean_weight_cached = 0;
	jvertex *active_node = 0;
	bool is_active = 0;
	bool node_displayed = 0;
	vec3 vl = {};
	Model sphere = import_model("../data/model.obj");
	
	
	Model star = import_model("../data/icosphere.obj");
	
	texture_info texture_info;
    
	init_texture(texture_info, "../data/tomato.png");
	init_text_renderer(text_info);
	float offset_ratio = 0.5f;
	bool show_outline = false;
	bool is_billboard = false;
	bool ss = false;
	vec3 tpos = vec3_zero;
	vec3 n, n2, n_last, n2_last, n_delta, n2_delta;
	vec3 tpos2 = vec3_zero;
    
	float circle_width = 0.2f;
	int circle_precision = 64;
	float g = 0;
	vec3 c_velocity;
	vec3 c_last, c_cur, c_dt;
	float c_mass = 1.0f;
	vec3 c_force;
	
	vec3 f_g;
    vec3 asd;
	float cp_f;
	vec3 bl;
    int circle_num = 32;
    bool fill_circle = true;
	static int fogMode = GL_LINEAR; // Fog mode
	static float fogStart = 1.0; // Fog start z value.
	static float fogEnd = 5.0; // Fog end z value.
	float fogColor[4] = { 1.0, 1.0, 1.0, 0.0 };
    
    RigidBody tomato = {};
    tomato.mass = 1.0f;
    tomato.dim = vec2(texture_info.dim.x, texture_info.dim.y);
    
    vector<RigidBody> bullets;
    Sphere A;
    A.r = 1.0f;
    
    Sphere B;
    B.r = 1.0f;
    
	Collider_Rect rect_A, rect_B;
	rect_A.r[0] = 0.4f;
	rect_A.r[1] = 0.6f;
	
	rect_B.r[0] = 0.2f;
	rect_B.r[1] = 0.8f;
	
	sphere.material = default_shader;
	star.material = default_shader;
	star.material.color = TRAN;
	
	/*
 int size = 8;
 for(int x = 0; x < size; x++)
 {
  Entity test;
  test.do_logic = logic;
  vec3 p;
  for(int y = 0; y < size; y++)
  {
   p = vec3(x*2.0f, 4.0f, x*2.0f);
   test.transform.set_position(p);
   test.collider.t.set_position(p);
   test.collider.add_force(normalize(rand_vec3(-1.0f, 1.0f)));
   test.collider.mass = rand_frange(1.0, 5000.0f);
   attach_component(test, COMPONENT_MESH, star);
   //attach_component(test, COMPONENT_MOVER, sphere);
   model_mesh_memory[test.mesh_components[0].data_id].material.color = TRAN;
   push_entity(test);
  }
 }
 */
	Entity lm, hm;
	
	hm.do_logic = logic;
	hm.transform.set_position(vec3(0.0f, 7.0f, 0.0f));
	hm.collider.t.set_position(vec3(0.0f, 7.0f, 0.0f));
	
	hm.collider.mass = 50000;
	push_entity(hm);
	
	lm.do_logic = logic;
	lm.transform.set_position(vec3(6.0f, 7.0f, 0.0f));
	lm.collider.t.set_position(vec3(6.0f, 7.0f, 0.0f));
	
	lm.collider.mass = 1;
	push_entity(lm);
	
	win32_sound_output SoundOutput = {};
	SoundOutput.SamplesPerSecond = 44100;
	SoundOutput.ToneHz = 256;
	SoundOutput.ToneVolume = 3000;
	SoundOutput.WavePeriod = SoundOutput.SamplesPerSecond / SoundOutput.ToneHz;
	SoundOutput.BytesPerSample = sizeof(INT16) * 2;
	SoundOutput.SecondaryBufferSize =SoundOutput.SamplesPerSecond * SoundOutput.BytesPerSample;
	SoundOutput.LatencySampleCount = SoundOutput.SamplesPerSecond / 1;
	
	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);
	SDL_GetWindowWMInfo(mainWindow, &wmInfo);
	HWND WindowHandle = wmInfo.info.win.window;
	
	SoundBuffer.AudioBufferSize = 147172;
	init_direct_sound(WindowHandle, 44100, &SoundBuffer);
	
	wav_file_data test;
	open_wav_file("../data/test.wav", &test);
	direct_sound_load_wav(&test, &SoundBuffer);
	
	stringstream console_log_buffer;
	streambuf *old = cout.rdbuf(console_log_buffer.rdbuf());
	while (!quit) {
		if (isFog) glEnable(GL_FOG);
		else glDisable(GL_FOG);
		glHint(GL_FOG_HINT, GL_NICEST);
		glFogfv(GL_FOG_COLOR, fogColor);
		glFogi(GL_FOG_MODE, fogMode);
		glFogf(GL_FOG_START, fogStart);
		glFogf(GL_FOG_END, fogEnd);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        
		current_time = time_state.seconds_passed;
		frame_rate++;
		if (current_time - last_time >= 1.0)
		{
			time_state.fps = frame_rate;
			frame_rate = 0;
			last_time = current_time;
		}
        
		LARGE_INTEGER last_tick;
		QueryPerformanceCounter(&last_tick);
        
		process_input();
        
		update_mover(1.0f, input_state, time_state);
		
		mouse_delta3d_current = p_near;
		int x, y;
		SDL_GetMouseState(&x, &y);
		mouse_delta2d_current.x = x;
		mouse_delta2d_current.y = y;
		mouse_dt_current.x = x;
		mouse_dt_current.y = y;
        
		input_state.mouse.x = x;
		input_state.mouse.y = y;
        
		camera.mouse_look = mouse_look;
		
        
		vec3 p_dir = p_near - p_far;
		vec3 p_dir_n = normalize(p_dir);
		vec3 closest_p = p_near + p_dir_n;
        
        
        A.p = vec3(1.0, 2.0, 0.0f);
        B.p = tomato.t.position();
        A.r = 1;
        //stars
		
		rect_B.origin = mover.v;
		rect_A.origin = vec3_zero;
		
		//draw_rect(&default_shader, rect_A.origin, rect_A.r[0], rect_A.r[1], vec3(1), 0, 1.0f, vec4(1.0, 1.0, 0.0, 0.5f), &camera, true);
		
		//draw_rect(&default_shader, rect_B.origin, rect_B.r[0], rect_B.r[1], vec3(1), 0, 1.0f, vec4(1.0, 0.0, 1.0, 0.5f), &camera, true);
        
		if(resolve_rect_collisions(rect_A, rect_B))
		{
			//draw_rect(&default_shader, rect_A.origin + vec3_forward * 0.01f, rect_A.r[0], rect_A.r[1], vec3(1), 0, 1.0f, GREEN, &camera, true);
		}
        
        //draw_sphere(circle_num, &default_shader, &camera, vec4(0.2f, 0.5f, 0.8f, 0.5f), A.p, fill_circle, true, A.r);
        
        //draw_sphere(circle_num, &default_shader, &camera, vec4(0.2f, 0.8f, 0.1f, 0.5f), B.p, fill_circle, true, B.r);
        
		//draw_icosahedron(circle_num, &default_shader, &camera, vec4(0.2f, 0.5f, 0.8f, 0.5f), vec3_right + -vec3_up, fill_circle, true, A.r);
        
        if(resolve_sphere_collisions(A, B)) debug_point(B.p, RED + GREEN, 100, &default_shader);
        
		Ray_Info ray_info;
		cast_ray(A, ray_info, vec3_zero, input_state.mouse_w, 5.0f);
		
		//NOTE: since acceleration formula is a = F * m/s * m/s (m/s squared).
		//force is
        
		vec3 force = vec3(0, VAR_G, 0) * 1.0f/tomato.mass;
        tomato.acceleration =  1.0f/tomato.mass * tomato.force + force * time_state.dt * time_state.dt * 0.5f;
        tomato.velocity += tomato.acceleration;
        //tomato.velocity * 0.2f;
        tomato.t.translate(tomato.velocity);
        tomato.force = vec3_zero;
        c_cur = camera.pos;
        
        c_velocity = c_dt * time_state.dt;
        
        c_force = 1.0f * c_velocity;
        
        for(int i = 0; i < bullets.size(); i++)
        {
            bullets[i].update_physics(time_state);
        }
        
        
        if(input_state.k_r)
        {
            tomato.t.set_position(vec3(0, 4, 0));
            tomato.velocity = {};
            //camera.pos = vec3_up;
        }
        
        if (input_state.k_space)
        {
            f_g += vec3(0, 0.5f, 0);
            input_state.k_space = false;
        }
        
        vec2 hwin = { display_info.w / 2, display_info.h / 2 };
        
        float dt = time_state.dt;
        vec2 mdt = vec2(input_state.mouse_dt2.x, input_state.mouse_dt2.y);
        
        if (camera.view_mode == CMODE_FREE)
        {
            if (camera.mouse_look)
            {
                camera_update_rotation(&camera, &mdt, false);
            }
            camera_update_translation(&camera, &input_state, dt);
            camera_look_at(&camera, camera.pos, camera.pos + camera.forward);
        }
        else if (camera.view_mode == CMODE_ORBIT)
        {
            camera_look_at(&camera, camera.forward * 3.f, vec3_zero);
            if (input_state.m_middle)
            {
                camera_update_rotation(&camera, &mdt, true);
            }
        }
        else if(camera.view_mode == CMODE_ORTHOGRAPHIC)
        {
            if (camera.mouse_look)
            {
                camera_update_rotation(&camera, &mdt, false);
            }
            camera_update_translation(&camera, &input_state, dt);
            
            camera_look_at(&camera, camera.pos, camera.pos + camera.forward);
        }
        
        if(camera.view_mode == CMODE_ORTHOGRAPHIC){
            camera.mat_projection = glm::ortho(0.0f, 16.0f, 0.0f, 9.0f, 0.1f, 1000.f);
        }
        else
        {
            camera.mat_projection = glm::perspective(glm::radians(camera.zoom), 16.0f / 9.0f, 0.1f, 1000.0f);
        }
        if(input_state.k_r)
        {
            camera.pos = vec3_zero;
        }
        
        jmodel.line_color = vec3(0.2f, 1.0f, 0.2f);
        jmodel.transform.translate(jmodel.local_position);
        
        
        vec3 p;
        p.x = sinf(time_state.seconds_passed);
        p.y = cosf(time_state.seconds_passed);
        p.z = cosf(time_state.seconds_passed);
        
        p *= 1.0f;
        debug_point(p, RED + GREEN, 10, &default_shader);
        vec3 d = input_state.mouse_w - sphere.transform.position();
        //sphere.add_pos(normalize(input_state.mouse_w) * speed * time_state.dt);
        
		//nlohmann_loop(jbeam_d, input_state);
		//rapidjson_loop(document, input_state);
        
        
		vec3 v;
        if(tomato.t.position().y <= -2.0f)
        {
            vec3 dir = normalize(tomato.velocity);
            vec3 nr = vec3(0, 1.0f, 0);
            float l = dot(dir, nr);
            vec3 r = -2*l*nr + dir;
            vec3 n = normalize(r);
            tomato.t.translate(vec3(0, -2.0 - tomato.t.position().y, 0));
            v = n * (0.4f * length(tomato.velocity));
            tomato.velocity = v;
            //tomato.add_force(vec3(16.0f, 0, 0.9f));
            
            debug_line(n, n + r, RED+GREEN, &default_shader, &camera);
        }
        
        float dist = distance(input_state.mouse_w, vec3(tomato.t.position().x, tomato.t.position().y, 0));
        
        if (dist < 1.0f  && input_state.m_left)
        {
            
            ss = true;
        }
        if (ss)
        {
            tomato.add_force(vec3(input_state.mouse_dt2.x, -input_state.mouse_dt2.y, 0) * 0.001f);
            if (!input_state.m_left) ss = false;
        }
        //if(dist < 1.0f) draw_quad(&default_shader, tomato.t.position(), vec3_up, 0, 1.0f, GREEN, &camera, true);
        
        float strength = 2.0f;
        if(input_state.a_up) tomato.add_force(vec3(0, 1.0f, 0)* strength);
        
        if(input_state.a_down) tomato.add_force(vec3(0, -1.0f, 0)* strength);
        
        if(input_state.a_left) tomato.add_force(vec3(-1.0f, 0, 0)* strength);
        
        if(input_state.a_right) tomato.add_force(vec3(1.0f, 0, 0)* strength);
        
        static float delay;
        if(time_state.seconds_passed > delay + 0.5f && input_state.m_left){
            RigidBody t;
            t.t.position() = tomato.t.position();
            t.acceleration = {};
            t.mass = 8.0f;
            t.add_force(camera.forward * 100.5f);
            bullets.push_back(t);
            delay = time_state.seconds_passed;
        }
        
        
        for(int i = 0; i < bullets.size(); i++)
        {
            bullets[i].update_physics(time_state);
        }
        
        
        resolve_collisions(bullets);
        /*
        kpl_draw_texture(texture_info, tomato.t.position(), vec3(1, 1, 1), show_outline, is_billboard);
        */
        
        for(int i = 0; i < bullets.size(); i++)
        {
            /*
            kpl_draw_texture(texture_info, bullets[i].t.position(), vec3(1.0f, 1.0f, 1.0f), show_outline, is_billboard); */
            //draw_sphere(circle_num, &default_shader, &camera, vec4(0.7f, 0.2f, 0.8f, 0.5f), bullets[i].t.position(), fill_circle, true, 1.0f);
        }
        
        
        debug_line(tomato.t.position(), tomato.t.position() + tomato.velocity * 10000.0f, BLUE + GREEN, &default_shader, &camera);
        
        //debug_line(tomato.t.position(), input_state.mouse_w - tomato.t.position(), BLUE + RED, &default_shader, &camera);
        
        debug_point(tomato.t.position(), BLUE, 8, &default_shader);
        
        for (int y = -circle_width; y <= circle_width; y++)
        {
            for (int x = -circle_width; x <= circle_width; x++)
            {
                if (x*x + y * y <= circle_width * circle_width)
                {
                    debug_point(vec3(x, y, 0), GREEN, 10, &default_shader);
                }
            }
        }
        
		
		//sphere.line_color = vec3(0.2f, 1.0f, 0.2f);
		//sphere.pos = vec3_zero;
		sphere.transform.set_position(p);
		sphere.transform.look_at(p);
		render_model(sphere, sphere.transform, &camera);
		sphere.material.color = vec4(cosf(time_state.seconds_passed * time_state.dt),sinf(time_state.seconds_passed * time_state.dt), sinf(time_state.seconds_passed), 1);
		show_basis(sphere.transform);
		
		
		static float last_time;
		static float sound_delay = 0.1f;
		static float sound_decay = 0.99f;
		static float  volume = -0.0f;
		/*
  if(time_state.seconds_passed > last_time + sound_delay && volume > -10000)
  {
   volume = (volume * sound_decay);
   SoundBuffer.GlobalSecondaryBuffer->Play(0, 0, 0);
   SoundBuffer.GlobalSecondaryBuffer->SetVolume((long)volume);
   last_time = time_state.seconds_passed;
  }
  */
		static float delay_shoot;
		if(time_state.seconds_passed > delay_shoot + 0.5f && input_state.m_left)
		{
			SoundBuffer.GlobalSecondaryBuffer->SetCurrentPosition(0);
			SoundBuffer.GlobalSecondaryBuffer->SetVolume(volume);
			SoundBuffer.GlobalSecondaryBuffer->Play(0, 0, 0);
			/*
   Entity test;
   test.transform.set_position(input_state.mouse_w);
   attach_component(test, COMPONENT_MESH, sphere);
   attach_component(test, COMPONENT_MOVER, sphere);
   model_mesh_memory[test.mesh_components[0].data_id].material.color;// = vec4(1.0/time_state.seconds_passed, 1.0/time_state.seconds_passed, 1.0/time_state.seconds_passed, 1);
   push_entity(test);*/
			delay_shoot = time_state.seconds_passed;
		}
		
		//HRESULT result = SoundBuffer.GlobalSecondaryBuffer->Play(0, 0, 0);
		
		//cout << result << endl;
		
		process_entities();
		
		//cout << (input_state.mouse_left.state << 1) << (input_state.mouse_left.state << 2) << endl;
		
		if(time_state.seconds_passed > delay + 0.05f)
		{
			//delete_entity(entities.size());
			//delay = time_state.seconds_passed;
		}
		
		/*
for(int i = 0; i < circle_num; i++)
{
draw_circle(circle_num, &default_shader, &camera, TRAN, p*(i*2.0f), fill_circle, 0.5f);
}*/
		
		p_near = get_mouse_3d(0.0, camera);
		p_far = get_mouse_3d(1.0, camera);
		input_state.p_near = p_near;
		input_state.p_far = p_far;
		
		int gl_error = glGetError();
		gl_error = glGetError();
		
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(mainWindow);
		ImGui::NewFrame();
		
		if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);
		{
			static float f = 0.0f;
			static int counter = 0;
			
			ImGui::SetNextWindowSize(ImVec2(480, 640));
			ImGui::SetNextWindowSizeConstraints(ImVec2(480, 640), ImVec2(display_info.w, display_info.h));
			
			ImGui::Begin("Properties");                          // Create a window called "Hello, world!" and append into it.
			
			if (ImGui::BeginMainMenuBar())
			{
				if (ImGui::BeginMenu("Options"))
				{
					static int id = 0;
					static int last_id;
					ImGui::Combo("Display mode", &id, "Fullscreen\0Windowed fullscreen\0Windowed\0\0");
					if (id == 0) display_info.mode_id = SDL_WINDOW_FULLSCREEN; //windowed
					else if (id == 1) display_info.mode_id = SDL_WINDOW_FULLSCREEN_DESKTOP; //fullscreen
					else if (id == 2) display_info.mode_id = 0;
					if (last_id != id)
					{
						SDL_SetWindowFullscreen(mainWindow, display_info.mode_id);
						/*SDL_DisplayMode mode;
SDL_GetCurrentDisplayMode(0, &mode);

display_info.w = mode.w;
display_info.h = mode.h;*/
						//SDL_SetWindowSize(mainWindow, display_info.w, display_info.h);
					}
					last_id = id;
					if (ImGui::MenuItem("Show debug text")) { show_text = !show_text; }
					if (ImGui::MenuItem("Quit", "Alt+F4")) { quit = true; }
					ImGui::EndMenu();
				}
				ImGui::EndMainMenuBar();
			}
			
			// Display some text (you can use a format strings too)
			ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
			ImGui::Checkbox("Another Window", &show_another_window);
			
			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
			ImGui::SliderFloat("offset ratio", &offset_ratio, 0.1f, 1.0f, "%.3f");
			ImGui::Checkbox("show outline", &show_outline);
			ImGui::Checkbox("display entity bases", &display_info.show_entity_bases);
			ImGui::Checkbox("is billboard", &is_billboard);
			ImGui::SliderInt("circle_precision", &circle_precision, 4, 720, "%d");
			ImGui::SliderFloat("circle_width", &circle_width, 0.1f, 32.0f, "%.3f");
			ImGui::SliderFloat("sound_decay", &sound_decay, 0.01f, 1.0f, "%.3f");
			ImGui::SliderFloat("sound_volume", &volume, 0.0f, -10000.0f, "%.3f");
			ImGui::SliderFloat("sound_delay", &sound_delay, 0.01f, 5.0f, "%.3f");
			
			
			
			ImGui::SliderFloat("mass", &tomato.mass, 0.1f, 128.0f, "%.4f");
			ImGui::SliderFloat("VAR_G", &VAR_G, -10.0f, 10.0f, "%.4f");
			ImGui::Checkbox("fill circle", &fill_circle);
			ImGui::SliderInt("circle_num", &circle_num, 4, 720, "%d");
			
			if (ImGui::TreeNode("Debug information"))
			{
				imgui_display_list();
				
				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
				
				ImGui::TreePop();
			}
			
			if (ImGui::TreeNode("Beam information"))
			{
				ImGui::Text("Without border:");
				ImGui::Columns(3, "mycolumns3", false);  // 3-ways, no border
				ImGui::Separator();
				for (int n = 0; n < 14; n++)
				{
					char label[32];
					sprintf(label, "Item %d", n);
					if (ImGui::Selectable(label)) {}
					//if (ImGui::Button(label, ImVec2(-1,0))) {}
					ImGui::NextColumn();
				}
				ImGui::Columns(1);
				ImGui::Separator();
				
				ImGui::Text("With border:");
				ImGui::Columns(4, "mycolumns"); // 4-ways, with border
				ImGui::Separator();
				//ImGui::Text(to_string(beam->id).c_str()); ImGui::NextColumn();
				//ImGui::Text("Node a ID: " + (char)beam->a->id.c_str()); ImGui::NextColumn();
				//ImGui::Text("Node b ID: " + (char)beam->b->id.c_str()); ImGui::NextColumn();
				ImGui::Text(""); ImGui::NextColumn();
				ImGui::Text("Hovered"); ImGui::NextColumn();
				ImGui::Text("Hovered"); ImGui::NextColumn();
				
				ImGui::Separator();
				const char* names[3] = { "One", "Two", "Three" };
				const char* paths[3] = { "/path/one", "/path/two", "/path/three" };
				static int selected = -1;
				for (int i = 0; i < 3; i++)
				{
					char label[32];
					sprintf(label, "%04d", i);
					if (ImGui::Selectable(label, selected == i, ImGuiSelectableFlags_SpanAllColumns))
						selected = i;
					bool hovered = ImGui::IsItemHovered();
					ImGui::NextColumn();
					ImGui::Text(names[i]); ImGui::NextColumn();
					ImGui::Text(paths[i]); ImGui::NextColumn();
					ImGui::Text("%d", hovered); ImGui::NextColumn();
				}
				ImGui::Columns(1);
				ImGui::Separator();
				ImGui::TreePop();
			}
			
			static bool read_only = false;
			string console_log_text = console_log_buffer.str();
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0,0));
			ImGui::Checkbox("Read-only", &read_only);
			ImGui::PopStyleVar();
			ImGui::InputTextMultiline("##source", (char*)console_log_text.c_str(), IM_ARRAYSIZE((char*)console_log_text.c_str()), ImVec2(-1.0f, ImGui::GetTextLineHeight() * 16), ImGuiInputTextFlags_AllowTabInput | (read_only ? ImGuiInputTextFlags_ReadOnly : 0));
			console_log_buffer.str(" ");
			
			ImGui::End();
		}
		
		// 3. Show another simple window.
		if (show_another_window)
		{
			ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Close Me"))
				show_another_window = false;
			
			ImGui::End();
		}
		
		// Rendering
		ImGui::Render();
		
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		
		
		if (mouse_look)
		{
			mouse_dt = mouse_dt_current - mouse_dt_last;
			mouse_dt_last = mouse_dt_current;
			input_state.mouse_dt2 = mouse_dt;
			//@kepler setting mouse coordinates directly might cause problems later on?
			
			//SDL_WarpMouseInWindow(mainWindow, hwin.x, hwin.y);
			//if (mouse_mode_switch) input_state.mouse_dt2 = vec2_zero;
		}
		else
		{
			mouse_delta2d = mouse_delta2d_current - mouse_delta2d_last;
			mouse_dt3 = mouse_delta3d_current - mouse_delta3d_last;
			
			input_state.mouse_dt2 = mouse_delta2d;
			mouse_delta2d_last = mouse_delta2d_current;
			mouse_delta3d_last = mouse_delta3d_current;
		}
		//wait till mouse warps to the center, to avoid camera snapping. (Window system events can take longer than one frame)
		if (mouse_mode_switch && input_state.mouse.x == hwin.x && input_state.mouse.y == hwin.y) mouse_mode_switch = false;
		
		pressed = input_state.a_left;
		was_pressed = input_state.m_left;
		input_state.k_delete = false;
		input_state.k_enter = false;
		set_inactive(input_state.mouse_left.state, kON);
		//tomato.velocity = tomato.vl - tomato.vc;
		//tomato.vl = tomato.vc;
		
		input_state.mouse_dt3 = mouse_dt3;
		
		input_state.mouse_w = mouse_position(&camera);
		
		translation_delta = translation_delta_current - translation_delta_last;
		
		c_dt = c_cur - c_last;
		c_last = c_cur;
		
		translation_delta_last = translation_delta_current;
		
		LARGE_INTEGER tick_after_loop;
		QueryPerformanceCounter(&tick_after_loop);
		Sleep(1);
		__int64 interval = tick_after_loop.QuadPart - tick_before_loop.QuadPart;
		float seconds_passed = (float)interval * second_per_tick;
		
		time_state.seconds_passed += seconds_passed;
		time_state.dt = interval * second_per_tick;
		tick_before_loop = tick_after_loop;
		SDL_GL_SwapWindow(mainWindow);
	}
	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(mainWindow);
	SDL_Quit();
	return 0;
}
