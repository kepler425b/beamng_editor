
struct Transform {
	mat4 r;
	mat4 t;
	mat4 m;
};


struct Time {
	float dt;
	float seconds_passed;
	float second_per_tick;
	float fps;
};

struct Input
{
	union
	{
		bool keys[12];
		struct
		{
			bool m_left_click;
			bool m_left;
			bool m_right;
			bool m_middle;
			bool a_up;
			bool a_down;
			bool a_left;
			bool a_right;
			bool a_forward;
			bool a_backward;
			bool lshift;
			bool k_delete;
			bool k_enter;
			bool k_space;
		};
	};
	bool is_pressed_m;
	fvec3 move_dt;
	vec3  mouse_w;
	ivec2 mouse;
	vec3  mouse_dt3;
	vec2  mouse_dt2;
	vec3  translation_delta;
	vec3 p_near, p_far;
	size_t kmod;
};

struct imgui_window_state {
	bool node_window_howered;
	bool beam_window_howered;
};

struct shader {
	ui32 buffer_id;
	bool is_inited = false;
	GLuint id;
	int u_view_location, u_model_location, u_projection_location, u_color_location, u_state, u_draw_outline, u_text_color, u_is_billboard;
	vector<vec4> rand_colors;
};

struct jvertex {
	unsigned int index;
	string id;
	vec3 pos;
	bool active = false;
	int weight;
	bool pad;
};

struct beam_pointer;
struct beam_group;
struct object;

struct beam_pointer {
	ui32 id;
	bool selected;
	jvertex *a, *b;
	beam_pointer *next;
	beam_pointer *previous;
	beam_group *group;
	object* parent_object;
};

struct beam_pair {
	string a, b;
};

struct bounded_beam {
	float precompression;
	string type;
	float long_bound, short_bound;
};

struct beam_group {
	ui32 id;
	string mesh_group;
	float beam_spring;
	float beam_damp;
	float beam_deform;
	float beam_strength;
	bounded_beam bounded;
	vector <beam_pair> beams;
	beam_pointer *beam_data;
	ui32 beam_num;
	vector <unsigned int> indices;
	GLuint index_buffer;
	vec4 color;
};

struct object {
	ui32 beam_num_total = 0;
	string name;
	vector <jvertex> nodes;
	vector <beam_group> beams;
};

struct jbeam_data {
	vector <object> objects;
};

struct node {
	string id;
	fvec3 pos;
	float weight;
};

struct display {
	ui32 h, w;
	ui32 num_displays;
	ui32 mode_id; // SDL_WINDOW_FULLSCREEN, SDL_WINDOW_FULLSCREEN_DESKTOP or 0
	ivec4 cl_area;
};

struct Character {
	GLuint     TextureID;
	glm::ivec2 Size;
	glm::ivec2 Bearing;
	GLuint     Advance;
};
