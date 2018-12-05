
//TODO: need to figure a way to set, add and retrieve/access values of matrices
//without getters and setters because it's annoying.

struct Transform {
    
	mat4 r = mat4(1.0f);
	mat4 t = mat4(1.0f);
	mat4 m = mat4(1.0f);
    
    void translate(vec3 d)
    {
        t[3][0] += d.x;
        t[3][1] += d.y;
        t[3][2] += d.z;
    }
    
    void set_position(vec3 d)
    {
        t[3][0] = d.x;
        t[3][1] = d.y;
        t[3][2] = d.z;
    }
    
    vec3 position()
	{
		return vec3(t[3][0], t[3][1], t[3][2]);
	}
    
	vec3 forward()
	{
		return vec3(t[2][0], t[2][1], t[2][2]);
	}
    
	vec3 up()
	{
		return vec3(t[1][0], t[1][1], t[1][2]);
	}
    
	vec3 right()
	{
		return vec3(t[0][0], t[0][1], t[0][2]);
	}
    
	void look_at(vec3 p)
	{
		vec3 forward = normalize(p);
		vec3 right = normalize(cross(vec3_up, forward));
		vec3 up = normalize(cross(forward, right));
		r[0][0] = right.x;
		r[1][0] = right.y;
		r[2][0] = right.z;
		r[0][1] = up.x;
		r[1][1] = up.y;
		r[2][1] = up.z;
		r[0][2] = forward.x; 
		r[1][2] = forward.y; 
		r[2][2] = forward.z; 
	}
};


struct Collider_Rect {
	float r[3];
	vec3 origin;
};

struct Time {
	float dt;
	float seconds_passed;
	float second_per_tick;
	float fps;
};

struct RigidBody
{
    Transform t;
    vec3 velocity;
    vec3 acceleration;
    vec3 force;
    vec2 dim;
    float mass;
    void add_force(vec3 f)
    {
        force += f;
    }
    void update_physics(Time &time_state)
    {
        vec3 g = vec3(0, -9.8f, 0) * mass;
        acceleration = 1.0f/mass * force + g * time_state.dt * time_state.dt * 0.5f;
        velocity += acceleration;
        
        t.translate(velocity);
        force = {};
    }
};

struct Sphere
{
    float r;
    vec3 p;
    
    float area()
    {
        float result = r - p.x * p.x + r - p.y * p.y + r - p.z * p.z;
        return result;
    }
};

struct key {
	bool pressed;
	bool last;
};

struct Input
{
	key mouse_left;
	union
	{
		bool keys[18];
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
            bool k_down;
			bool k_left;
			bool k_right;
			bool k_forward;
            bool k_backward;
			bool k_delete;
			bool k_enter;
			bool k_space;
            bool k_r;
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

struct Mover {
	vec3 v;
    vec3 velocity;
    vec3 acceleration;
	float x, y, z;
};

Mover mover;

void update_mover(float speed, Input &i, Time &t)
{
	if(i.a_up) mover.y += speed * t.dt;
	if(i.a_down) mover.y -= speed * t.dt;
	if(i.a_right) mover.x += speed * t.dt;
	if(i.a_left) mover.x -= speed * t.dt;
	mover.v = vec3(mover.x, mover.y, mover.z);
	mover.acceleration = mover.v * t.dt * t.dt * 0.5f;
	mover.v += mover.acceleration;
}

void update_movers(Transform &transform, float speed, Input &i, Time &t)
{
	Mover m;
	if(i.a_up) m.y += speed * t.dt;
	if(i.a_down) m.y -= speed * t.dt;
	if(i.a_right) m.x += speed * t.dt;
	if(i.a_left) m.x -= speed * t.dt;
	m.v = vec3(m.x, m.y, m.z);
	m.acceleration = m.v;
	transform.translate(m.acceleration);
}

struct imgui_window_state {
	bool node_window_howered;
	bool beam_window_howered;
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
	bool show_entity_bases;
};
display display_info;

struct Character {
	GLuint     TextureID;
	glm::ivec2 Size;
	glm::ivec2 Bearing;
	GLuint     Advance;
};


static Input input_state = {};
Time time_state = {};
