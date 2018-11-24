
struct Model {
    bool did_import_fail;
	jvertex *vertices;
	vector<unsigned int> indices;
	vec3 mass_center;
	vec3 pos;
	quat rotation;
	Transform transform;
	size_t vertices_size;
	vec3 line_color;
	vec3 forward;
	vec3 up;
	vec3 right;
	unsigned int index_buffer;
	unsigned int vertex_buffer;
    
	void init(size_t size)
	{
		this->vertices = (jvertex*)my_malloc(size, sizeof(jvertex));
		//this->offscreen_vertices = (jvertex*)my_malloc(size, sizeof(jvertex));
		this->vertices_size = size;
		forward = vec3_forward;
		up = vec3_up;
		right = vec3_right;
		transform.t = mat4(1.f);
		transform.r = mat4(1.f);
	}
	void init_gl_buffers()
	{
		glGenBuffers(1, &index_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*indices.size(), &indices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glGenBuffers(1, &vertex_buffer);
	}
	void add_pos(vec3 p)
	{
		transform.t[0][3] += p.x;
		transform.t[1][3] += p.y;
		transform.t[2][3] += p.z;
	}
	void rotate(vec3 axis, float angle)
	{
		transform.r *= glm::rotate(mat4(), angle, axis);
	}
	void r(float rx, float ry)
	{
		transform.r = mat4(1);
		vec3 front;
		front.x = cos(radians(rx)) * cos(radians(ry));
		front.y = sin(radians(ry));
		front.z = sin(radians(rx)) * cos(radians(ry));
		forward = normalize(front);
		right = cross(forward, vec3_up);
		up = cross(right, forward);
		mat4 t(1);
		t[0][0] = right.x;
		t[1][0] = right.y;
		t[2][0] = right.z;
		t[0][1] = up.x;
		t[1][1] = up.y;
		t[2][1] = up.z;
		t[0][2] = forward.x;
		t[1][2] = forward.y;
		t[2][2] = forward.z;
        /*	t[0][3] = 0;
      t[1][3] = 0;
      t[2][3] = 0;
      t[3][0] = 0;
      t[3][1] = 0;
      t[3][2] = 0;*/
		transform.r = t;
	}
	vec3 get_f()
	{
		return vec3(transform.m[0][2], transform.m[1][2], transform.m[2][2]);
	}
	vec3 get_r()
	{
		return vec3(transform.m[0][0], transform.m[1][0], transform.m[2][0]);
	}
	vec3 get_u()
	{
		return vec3(transform.m[0][1], transform.m[1][1], transform.m[2][1]);
	}
	void look_at(vec3 p)
	{
		forward = normalize(p);
		right = normalize(cross(vec3_up, forward));
		up = normalize(cross(forward, right));
		transform.r[0][0] = right.x;
		transform.r[1][0] = right.y;
		transform.r[2][0] = right.z;
		transform.r[0][1] = up.x;
		transform.r[1][1] = up.y;
		transform.r[2][1] = up.z;
		transform.r[0][2] = forward.x; 
		transform.r[1][2] = forward.y; 
		transform.r[2][2] = forward.z; 
        
		//debug_line(get_pos() + forward, get_pos() + forward * forward.z, RED, &default_shader, &camera);
		//debug_line(get_pos() + up, get_pos() + up * up.y, BLUE, &default_shader, &camera);
		//debug_line(get_pos() + right, get_pos() + right * right.x, GREEN, &default_shader, &camera);
        /*	cout << "------------------------------------------------------------------" << endl;
      cout << "forward(" << forward.x << ", " << forward.y << ", " << forward.z << ")"<< endl;
      cout << "------------------------------------------------------------------" << endl;
      cout << "right(" << right.x << ", " << right.y << ", " << right.z << ")" << endl;
      cout << "------------------------------------------------------------------" << endl;
      cout << "up(" << up.x << ", " << up.y << ", " << up.z << ")" << endl;
      cout << "------------------------------------------------------------------" << endl;*/
	}
	vec3 get_pos()
	{
		return vec3(transform.m[3][0], transform.m[3][1], transform.m[3][2]);
	}
	void show_basis()
	{
		float sc = 1.0f;
		debug_line(get_pos(), get_pos() + forward * sc, RED, &default_shader, &camera);
		debug_line(get_pos(), get_pos() + up * sc, BLUE, &default_shader, &camera);
		debug_line(get_pos(), get_pos() + right * sc, GREEN, &default_shader, &camera);
	}
};

struct JModel {
    bool did_import_fail;
	vector<object> data;
	vec3 mass_center;
	vec3 local_position;
	vec3 world_position;
	quat rotation;
	mat4 mat_model = {};
	size_t vertices_size;
	vec3 line_color;
	vec3 front;
	vec3 f;
	vec3 r;
	vec3 u;
	vec3 world_up = vec3_up;
	Transform transform;
	unsigned int vertex_buffer;
    
	void init_gl_buffers()
	{
		for (int osz = 0; osz < data.size(); osz++)
		{
			for (int bsz = 0; bsz < data[osz].beams.size(); bsz++)
			{
				if (data[osz].beams[bsz].beams.size() > 0)
				{
					glGenBuffers(1, &data[osz].beams[bsz].index_buffer);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data[osz].beams[bsz].index_buffer);
					glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*data[osz].beams[bsz].indices.size(), &data[osz].beams[bsz].indices[0], GL_STATIC_DRAW);
				}
			}
		}
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glGenBuffers(1, &vertex_buffer);
	}
    
    
	void translate(vec3 translation)
	{
		glm::translate(mat_model, translation);
	}
    
	void uniformScale(float scale)
	{
		glm::scale(mat_model, vec3(scale, scale, scale));
	}
    
	void rotate(vec3 axis, float angle)
	{
		glm::rotate(mat_model, angle, axis);
	}
    
	vec3 worldPosition()
	{
		return vec3(mat_model[3][0], mat_model[3][1], mat_model[3][2]);
	}
    
	vec3 forward()
	{
		return this->f = vec3(mat_model[2][0], mat_model[2][1], mat_model[2][2]);
	}
	vec3 up()
	{
		return this->u = vec3(mat_model[1][0], mat_model[1][1], mat_model[1][2]);
	}
	vec3 right()
	{
		return this->r = vec3(mat_model[0][0], mat_model[0][1], mat_model[0][2]);
	}
    
};

JModel init_jbeam(vector<object> &file)
{
	JModel model;
    
	std::string err;
	model.data = file;
    
	vec3 temp;
	vec3 temp_mass_center;
	/*for (int i = 0; i < allocated_size(model.vertices); i++)
 {
 temp.x = attrib.vertices[0 + i * 3];
 temp.y = attrib.vertices[1 + i * 3];
 temp.z = attrib.vertices[2 + i * 3];
 
 model.vertices[i].active = false;
 model.vertices[i].pos = temp;
 
 temp_mass_center += temp;
 }*/
	/*model.mass_center.x = temp_mass_center.x / model.vertices_size;
 model.mass_center.y = temp_mass_center.y / model.vertices_size;
 model.mass_center.z = temp_mass_center.z / model.vertices_size;
 */
    
	/*for (int i = 0; i < shapes[0].mesh.indices.size(); i++)
 {
 model.indices.push_back(shapes[0].mesh.indices[i].vertex_index);
 }
 */
	model.init_gl_buffers();
    
    
	assert(model.vertices_size != 0);
    model.did_import_fail = false;
    return model;
}
