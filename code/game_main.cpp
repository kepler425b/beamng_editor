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

#include <Windows.h>
#include <SDL.h>
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

#include "structs.cpp"
#include "camera.cpp"
#include "utility_functions.cpp"
#include "shaders.cpp"
#include "model.cpp"
#include "collision.cpp"
#include "render_functions.cpp"
#include "file_io.cpp"
object o;
vector <object> objects;
static Input input_state = {};
Time time_state = {};
display display_info;

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
vec4 clear_color = vec4(0.45f, 0.55f, 0.60f, 1.00f);
vec4 clear_color_last = clear_color;


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
    
    
	glDisable(GL_CULL_FACE);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_ALWAYS);
    
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
	JModel jmodel = init_jbeam(objects);
    
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
	double second_per_tick = 1.0 / (double)Frequency.LowPart;
    
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
	float gravity = -9.8f;
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
		camera.orbit_mode = !camera.free_cam_mode;
        
        tomato.vc = tomato.pos;
        tomato.acceleration = (tomato.velocity * time_state.dt) * time_state.dt;
        
        
		c_cur = camera.pos;
        
		c_velocity = c_dt * time_state.dt;
		c_force = c_mass * c_velocity;
        
        if(input_state.k_r)
        {
            camera.pos = vec3_up;
        }
        
		if (input_state.k_space)
		{
			f_g += vec3(0, 0.5f, 0);
			input_state.k_space = false;
		}
        
        vec3 a = (c_dt / time_state.dt) / time_state.dt;
        
        f_g += vec3(0, gravity, 0) * c_mass * time_state.dt;
		camera.pos += f_g;
        
		cp_f = (pow(length(c_velocity * c_mass), 2) + pow(length(vec3_up), 2)) / 2;
		if (camera.pos.y <= -2.0f)
		{
			//camera.pos += cp_f;
		}
		if (camera.pos.y <= -2.0f)
		{
			camera.pos.y += -2.0 - camera.pos.y;
			f_g = vec3_zero;
		}
		vec2 hwin = { display_info.w / 2, display_info.h / 2 };
        
		float dt = time_state.dt;
		vec2 mdt = vec2(input_state.mouse_dt2.x, input_state.mouse_dt2.y);
        
		if (camera.free_cam_mode)
		{
			if (camera.mouse_look)
			{
				camera_update_rotation(&camera, &mdt, false);
			}
			camera_update_translation(&camera, &input_state, dt);
			camera_look_at(&camera, camera.pos, camera.pos + camera.forward);
		}
		else if (camera.orbit_mode)
		{
			camera_look_at(&camera, camera.forward * 3.f, vec3_zero);
			if (input_state.m_middle)
			{
				camera_update_rotation(&camera, &mdt, true);
			}
		}
		camera.mat_projection = glm::perspective(glm::radians(camera.zoom), 16.0f / 9.0f, 0.1f, 1000.0f);
        
        
        
        
		//if (isWire) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        
		vec3 p_dir = p_near - p_far;
		vec3 p_dir_n = normalize(p_dir);
		vec3 closest_p = p_near + p_dir_n;
        
        
		jmodel.line_color = vec3(0.2f, 1.0f, 0.2f);
        
		jmodel.translate(jmodel.local_position);
		jmodel.uniformScale(1.0f);
        
		vec3 p;
		p.x = sinf(time_state.seconds_passed);
		p.y = cosf(time_state.seconds_passed);
		p.z = cosf(time_state.seconds_passed);
        
		p *= 1.0f;
		debug_point(p, RED + GREEN, 10, &default_shader);
		vec3 d = input_state.mouse_w - sphere.get_pos();
		//vl += normalize(d) * ((int)input_state.m_left * time_state.dt);
		//sphere.add_pos(vl);
		//sphere.add_pos(normalize(input_state.mouse_w) * speed * time_state.dt);
		sphere.look_at(d);
		//camera.pos = sphere.pos + vec3_up * 0.5f + (-vec3_forward * 0.2f);
		//sphere.r(90+time_state.seconds_passed, 45+time_state.dt);
		//sphere.show_basis();
        
		//if (sphere.get_r().x < 0) { clear_color_last = clear_color; clear_color = RED + GREEN * 0.5f; }
		//else clear_color = clear_color_last;
        
        
		//nlohmann_loop(jbeam_d, input_state);
		rapidjson_loop(document, input_state);
        
        
        
		draw_quad(&circle_shader, p, vec3(1.0, 1.0, 1.0), 0.0f, p.x, vec4(0, 0, 0, p.z), &camera, false);
		//draw_quad(&default_shader, vec3(0, -2, 0), vec3(1, 0, 0), 80, 10.0f, vec4(0, 0, 0, p.z), &camera, false);
		debug_point(vec3(0, -2, 0), RED + GREEN, 10, &default_shader);
        
		for (int y = -circle_width; y <= circle_width; y++)
		{
			for (int x = -circle_width; x <= circle_width; x++)
			{
				if (x*x + y * y <= circle_width * circle_width)
				{
					debug_point(vec3(x, y, 0), RED + GREEN, 10, &default_shader);
				}
			}
		}
		
        
        
		
		float dist = distance(input_state.mouse_w, tomato.pos);
		//debug_line(input_state.mouse_w, n, BLUE, &default_shader, &camera);
		if (dist < 0.5f && input_state.m_left)
		{
			ss = true;
		}
		if (ss)
		{
            tomato.velocity += vec3(input_state.mouse_dt2.x, -input_state.mouse_dt2.y, 0) * 0.5f;
			if (!input_state.m_left) ss = false;
		}
        
		float dist_ab = distance(n, n2);
		if (dist_ab < 0.5f)
		{
			tpos += n2_delta * length(n2_delta);
			tpos2 += n_delta * length(n_delta);
        }
        
        if(tomato.pos.y <= -2.0f)
        {
            vec3 dir = normalize(tomato.velocity);
            vec3 nr = vec3(0, 1.0f, 0);
            float l = dot(dir, nr);
            vec3 r = -2*l*nr + dir;
            //+= -2.0 - tpos.y;
            tomato.velocity += r;
            debug_line(n, n + r, RED, &default_shader, &camera);
            
        }
        tomato.velocity += vec3(0, 0, 0) * c_mass * time_state.dt;
        tomato.pos += tomato.velocity * time_state.dt;
        
        
        
		kpl_draw_texture(texture_info, tomato.pos, 1.0f, show_outline, is_billboard);
        
		//kpl_draw_texture(texture_info, n2, 1.0f, show_outline, is_billboard);
        
		//kpl_draw_texture(texture_info, input_state.mouse_w, 0.1f, true, is_billboard);
        
		debug_line(n, n + n_delta * length(n_delta) * 1000.0f, BLUE + GREEN, &default_shader, &camera);
        
		debug_point(n, BLUE, 8, &default_shader);
		debug_point(n2, RED + BLUE, 8, &default_shader);
        
		draw_model(sphere, vec3_zero, GREEN * 0.5f);
        
        for(int i = 0; i < circle_num; i++)
        {
            draw_circle(circle_num, &default_shader, &camera, BLUE, p*(i*2.0f), fill_circle, 0.5f);
        }
        
        
        
        
		p_near = get_mouse_3d(0.0, camera);
		p_far = get_mouse_3d(1.0, camera);
		input_state.p_near = p_near;
		input_state.p_far = p_far;
        
		int gl_error = glGetError();
		gl_error = glGetError();
        
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(mainWindow);
		ImGui::NewFrame();
        
		lines_to_render.p.insert(lines_to_render.p.end(), selected_lines_to_render.p.begin(), selected_lines_to_render.p.end());
		lines_to_render.color.insert(lines_to_render.color.end(), selected_lines_to_render.color.begin(), selected_lines_to_render.color.end());
        
		points_to_render.p.insert(points_to_render.p.end(), selected_points_to_render.p.begin(), selected_points_to_render.p.end());
		points_to_render.color.insert(points_to_render.color.end(), selected_points_to_render.color.begin(), selected_points_to_render.color.end());
		render_line_group(lines_to_render, &default_shader, &camera);
		render_point_group(points_to_render, &default_shader, &camera);
		selected_lines_to_render = {};
		selected_points_to_render = {};
        
		for (int i = 0; i < circle_precision; i++)
		{
			vec3 ps = input_state.mouse_w;
			ps.x += cosf(i) * circle_width;
			ps.y += sinf(i) * circle_width;
			//ps.z += p.x + p.y * 0.01f;
			debug_point(ps, vec4(ps.x, ps.y, 0.0f, 1.0f), 5, &default_shader);
		}
        
		if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);
		{
			static float f = 0.0f;
			static int counter = 0;
            
			ImGui::SetNextWindowSize(ImVec2(320, 480));
			ImGui::SetNextWindowSizeConstraints(ImVec2(320, 480), ImVec2(display_info.w, display_info.h));
            
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
			ImGui::Checkbox("is billboard", &is_billboard);
			ImGui::SliderInt("circle_precision", &circle_precision, 4, 720, "%d");
			ImGui::SliderFloat("circle_width", &circle_width, 0.1f, 32.0f, "%.3f");
			ImGui::SliderFloat("camera mass", &c_mass, 0.1f, 3200.0f, "%.3f");
			ImGui::SliderFloat("gravity", &gravity, -10.0f, 32.0f, "%.3f");
            ImGui::Checkbox("fill circle", &fill_circle);
            ImGui::SliderInt("circle_num", &circle_num, 4, 720, "%d");
            
			if (ImGui::TreeNode("Debug information"))
			{
				ImGui::Text("camera.f() = %.3f, %.3f, %.3f", camera.f().x, camera.f().y, camera.f().z);
				ImGui::Text("sphere.pos = %.3f, %.3f, %.3f", sphere.get_pos().x, sphere.get_pos().y, sphere.get_pos().z);
				ImGui::Text("sphere.f = %.3f, %.3f, %.3f", sphere.get_f().x, sphere.get_f().y, sphere.get_f().z);
				ImGui::Text("sphere.r = %.3f, %.3f, %.3f", sphere.get_r().x, sphere.get_r().y, sphere.get_r().z);
				ImGui::Text("sphere.u = %.3f, %.3f, %.3f", sphere.get_u().x, sphere.get_u().y, sphere.get_u().z);
				ImGui::Text("camera dt = %.3f, %.3f, %.3f", c_dt.x, c_dt.y, c_dt.z);
				ImGui::Text("camera velocity = %.3f, %.3f, %.3f", c_velocity.x, c_velocity.y, c_velocity.z);
				ImGui::Text("camera force = %.3f, %.3f, %.3f", c_force.x, c_force.y, c_force.z);
				ImGui::Text("f_g = %.3f, %.3f, %.3f", f_g.x, f_g.y, f_g.z);
				ImGui::Text("contact point f = %.3f, %.3f, %.3f", cp_f);
                
                ImGui::Text("tomato.acceleration = %.3f, %.3f, %.3f", tomato.acceleration.x, tomato.acceleration.y, tomato.acceleration.z);
                ImGui::Text("tomato.pos = %.3f, %.3f, %.3f", tomato.pos.x, tomato.pos.y, tomato.pos.z);
                
				ImGui::Text("dist = %.3f", dist);
				ImGui::Text("dist_ab = %.3f", dist_ab);
				ImGui::Text("tomato.velocity = %.3f, %.3f, %.3f", tomato.velocity.x, tomato.velocity.y, tomato.velocity.z);
                
				ImGui::Text("input_state.move_dt = %.3f, %.3f, %.3f", input_state.move_dt.x, input_state.move_dt.y, input_state.move_dt.z);
				ImGui::Text("mouse_world = %.3f, %.3f, %.3f", input_state.mouse_w.x, input_state.mouse_w.y, input_state.mouse_w.z);
				ImGui::Text("time_state.seconds_passed: %.3f", time_state.seconds_passed);
				ImGui::Text("time_state.fps: %.3f", time_state.fps);
				ImGui::Text("mouse_mode_switch: %d", mouse_mode_switch);
				ImGui::Text("input_state.m_left: %d", input_state.m_left);
				ImGui::Text("was_pressed: %d", was_pressed);
				ImGui::Text("input_state.lshift: %d", input_state.lshift);
				ImGui::Text("beam_window.ishowered: %d", beam_debug_window_howered);
				ImGui::Text("node_debug_window_howered: %d", node_debug_window_howered);
                
				//ImGui::Text("o + camera.f() = %.3f, %.3f, %.3f", o.x, o.y, o.z);
                
                
				ImGui::Text("p_near = %.3f, %.3f, %.3f", p_near.x, p_near.y, p_near.z);
				ImGui::Text("mouse delta = %.3f, %.3f, %.3f", mouse_delta2d.x, mouse_delta2d.y);
				ImGui::Text("mousepos = %d, %d", input_state.mouse.x, input_state.mouse.y);
				ImGui::Text("camera pitch yaw = %d, %d", camera.pitch, camera.yaw);
				ImGui::Text("x, y, z = %.3f, %.3f, %.3f", camera.pos.x, camera.pos.y, camera.pos.z);
                
				ImGui::Text("counter = %d", counter);
                
				ImGui::Text("state = %d", input_state.a_right);
				ImGui::Text("mouse_mode_switch = %d", mouse_mode_switch);
                
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
        
		was_pressed = input_state.m_left;
		input_state.k_delete = false;
		input_state.k_enter = false;
        
        tomato.velocity = tomato.vl - tomato.vc;
        tomato.vl = tomato.vc;
        
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
