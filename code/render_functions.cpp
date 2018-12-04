void debug_line(vec3 a, vec3 b, vec4 color, shader *s, Camera *camera)
{
	glUseProgram(s->id);
    
	vec3 data[2];
	data[0] = a;
	data[1] = b;
	assert(asize(data) == 2);
    
	GLuint buff;
	glGenBuffers(1, &buff);
	glBindBuffer(GL_ARRAY_BUFFER, buff);
	glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(vec3), &data[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
	glUniformMatrix4fv(s->u_view_location, 1, false, glm::value_ptr(camera->get_mat()));
	//clear the model matrix
	glUniformMatrix4fv(s->u_model_location, 1, false, &mat4(1.0f)[0][0]);
	glUniformMatrix4fv(s->u_projection_location, 1, false, glm::value_ptr(camera->mat_projection));
	//default shader should  be enabled right now 
	//(after using different shaders, always switch to default)
	glUniform1i(s->u_state, 1);
	glUniform4fv(s->u_color_location, 1, &color[0]);
	glDrawArrays(GL_LINES, 0, 2);
    
	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &buff);
	glUseProgram(0);
}


void show_basis(Transform &transform)
{
	float sc = 1.0f;
	debug_line(transform.position(), transform.position() + transform.forward() * sc, RED, &default_shader, &camera);
	debug_line(transform.position(), transform.position() + transform.up() * sc, BLUE, &default_shader, &camera);
	debug_line(transform.position(), transform.position() + transform.right() * sc, GREEN, &default_shader, &camera);
}

void draw_circle(int precision, shader *s, Camera *camera, vec4 color, vec3 pos, bool fill, float radius)
{
    glUseProgram(s->id);
    if(fill) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else glPolygonMode(GL_FRONT_AND_BACK, GL_LINES);
    
    
    //vec3 *data = (vec3*)malloc(sizeof(vec3) * precision);
    vec3 data[32];
    
    for(int i = 0; i < precision; i++)
    {
        data[i].x = cos(2.0f * pi * i / precision) * radius;
        data[i].y = sin(2.0f * pi * i / precision) * radius;
    }
    mat4 m = translate(mat4(1.0f), pos);
    GLuint buff;
	glGenBuffers(1, &buff);
	glBindBuffer(GL_ARRAY_BUFFER, buff);
	glBufferData(GL_ARRAY_BUFFER, precision * sizeof(vec3), &data[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
	glUniformMatrix4fv(s->u_view_location, 1, false, glm::value_ptr(camera->get_mat()));
	//clear the model matrix
	glUniformMatrix4fv(s->u_model_location, 1, false, &m[0][0]);
	glUniformMatrix4fv(s->u_projection_location, 1, false, glm::value_ptr(camera->mat_projection));
	//default shader should  be enabled right now 
	//(after using different shaders, always switch to default)
	glUniform1i(s->u_state, 1);
	glUniform4fv(s->u_color_location, 1, &color[0]);
	glDrawArrays(GL_POLYGON, 0, precision);
    glDrawArrays(GL_POINTS, 0, precision);
    
	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &buff);
    //free(data);
}

int index = 0;
float time_since;
bool pressed = false;
void draw_icosahedron(int gradation, shader *s, Camera *camera, vec4 color, vec3 pos, bool lines, bool fill, float radius)
{
    glUseProgram(s->id);
    if(fill) glPolygonMode(GL_FRONT, GL_FILL);
	else glPolygonMode(GL_FRONT, GL_LINES);
	
	float t = (1.0 + sqrt(5.0)) / 2.0;
	
	vector<vec3> data;
	
	data.push_back(vec3(-1,  t,  0));
	data.push_back(vec3( 1,  t,  0));
	data.push_back(vec3(-1, -t,  0));
	data.push_back(vec3( 1, -t,  0));
	
	data.push_back(vec3( 0, -1,  t));
	data.push_back(vec3( 0,  1,  t));
	data.push_back(vec3( 0, -1, -t));
	data.push_back(vec3( 0,  1, -t));
	
	data.push_back(vec3( t,  0, -1));
	data.push_back(vec3( t,  0,  1));
	data.push_back(vec3(-t,  0, -1));
	data.push_back(vec3(-t,  0,  1)); 
	
	int indices[60] = 
	{
		0, 11, 5,
		0, 5, 1,
		0, 1, 7,
		0, 7, 10,
		0, 10, 11,
		
		1, 5, 9,
		5, 11, 4,
		11, 10, 2,
		10, 7, 6,
		7, 1, 8,
		
		3, 9, 4,
		3, 4, 2,
		3, 2, 6,
		3, 6, 8,
		3, 8, 9,
		
		4, 9, 5,
		2, 4, 11,
		6, 2, 10,
		8, 6, 7,
		9, 8, 1
	};
	
	mat4 m = translate(mat4(1.0f), pos);
	GLuint buff, buff_indices;
	glGenBuffers(1, &buff);
	glGenBuffers(1, &buff_indices);
	glBindBuffer(GL_ARRAY_BUFFER, buff);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(vec3), &data[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buff_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 60 * sizeof(int), &indices[0], GL_STATIC_DRAW);
	
	glUniformMatrix4fv(s->u_view_location, 1, false, glm::value_ptr(camera->get_mat()));
	//clear the model matrix
	glUniformMatrix4fv(s->u_model_location, 1, false, &m[0][0]);
	glUniformMatrix4fv(s->u_projection_location, 1, false, glm::value_ptr(camera->mat_projection));
	//default shader should  be enabled right now 
	//(after using different shaders, always switch to default)
	glUniform1i(s->u_state, 1);
	glUniform4fv(s->u_color_location, 1, &color[0]);
	glFrontFace(GL_CCW);
	
	if(input_state.a_left && !pressed) {index++; pressed = true;}
	if(input_state.a_right) index--;
	if(index >= 60) index = 0;
	cout << index << endl;
	//glDrawElements(GL_TRIANGLES, index, GL_UNSIGNED_INT, 0);
	glUniform4fv(s->u_color_location, 1, &vec4(1)[0]);
	
	if(lines ) glDrawElements(GL_LINES, index, GL_UNSIGNED_INT, 0);
	else glDrawElements(GL_POINTS, index, GL_UNSIGNED_INT, 0);
	glFrontFace(GL_CW);
	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &buff);
	glDeleteBuffers(1, &buff_indices);
	data.clear();
}


void draw_sphere(int gradation, shader *s, Camera *camera, vec4 color, vec3 pos, bool lines, bool fill, float radius)
{
    glUseProgram(s->id);
    if(fill) glPolygonMode(GL_FRONT, GL_FILL);
	else glPolygonMode(GL_FRONT, GL_LINES);
	
    vector<vec3> data;
    
    float xn, yn, zn, alpha, beta;        
    
    for (alpha = 0.0; alpha < pi; alpha += pi/gradation)
    {        
        for (beta = 0.0; beta < 2.01*pi; beta += pi/gradation)            
        {            
            xn = radius*cos(beta)*sin(alpha);
            yn = radius*sin(beta)*sin(alpha);
            zn = radius*cos(alpha);
            
            data.push_back(vec3(xn, yn, zn));
            xn = radius*cos(beta)*sin(alpha + pi/gradation);
            yn = radius*sin(beta)*sin(alpha + pi/gradation);
            zn = radius*cos(alpha + pi/gradation);            
            
            data.push_back(vec3(xn, yn, zn));
			
			xn = radius*cos(alpha); 
            yn = radius*sin(beta)*sin(alpha);
            zn = radius*cos(beta)*sin(alpha);
            
            data.push_back(vec3(xn, yn, zn));
			
            xn = radius*cos(alpha + pi/gradation);            
            yn = radius*sin(beta)*sin(alpha + pi/gradation);
            zn = radius*cos(beta)*sin(alpha + pi/gradation);
            
            data.push_back(vec3(xn, yn, zn));
        }   
	}        
	
	
	mat4 m = translate(mat4(1.0f), pos);
	GLuint buff;
	glGenBuffers(1, &buff);
	glBindBuffer(GL_ARRAY_BUFFER, buff);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(vec3), &data[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	
	glUniformMatrix4fv(s->u_view_location, 1, false, glm::value_ptr(camera->get_mat()));
	//clear the model matrix
	glUniformMatrix4fv(s->u_model_location, 1, false, &m[0][0]);
	glUniformMatrix4fv(s->u_projection_location, 1, false, glm::value_ptr(camera->mat_projection));
	//default shader should  be enabled right now 
	//(after using different shaders, always switch to default)
	glUniform1i(s->u_state, 1);
	glUniform4fv(s->u_color_location, 1, &color[0]);
	
	glDrawArrays(GL_POLYGON, 0, data.size());
	
	glUniform4fv(s->u_color_location, 1, &vec4(1)[0]);
	if(lines ) glDrawArrays(GL_LINES, 0, data.size());
	else glDrawArrays(GL_POINTS, 0, data.size());
	
	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &buff);
	data.clear();
}





struct rg_line {
	vector<vec3> p;
	vector<vec4> color;
};

struct rg_point {
	vector<vec3> p;
	vector<vec4> color;
	vector<int> size;
};

void add_force(beam_pointer &b, float dt, float vel, vec3 dir)
{
	float f = 0;
	f = (f + vel * dt) - 0.1f;
	vec3 d = normalize(dir);
	b.a->pos += d * f;
	b.b->pos += d * f;
}


void add_force_node(jvertex &b, float dt, float vel, vec3 dir)
{
	float f = 0;
	f = (f + vel * dt) - 0.1f;
	vec3 d = normalize(dir);
	b.pos += d * f;
}

void apply_gravity(beam_pointer &b, float vel, vec3 dir)
{
	
}


void render_line_group(rg_line &data, shader *s, Camera *camera)
{
	assert(data.p.size() > 0);
	glUseProgram(s->id);
	
	
	GLuint buff, color;
	glGenBuffers(1, &buff);
	glBindBuffer(GL_ARRAY_BUFFER, buff);
	glBufferData(GL_ARRAY_BUFFER, data.p.size() * sizeof(vec3), &data.p[0], GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	
	glGenBuffers(1, &color);
	glBindBuffer(GL_ARRAY_BUFFER, color);
	glEnableVertexAttribArray(1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * data.color.size(), &data.color[0], GL_DYNAMIC_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
	
	glUniformMatrix4fv(s->u_view_location, 1, false, glm::value_ptr(camera->get_mat()));
	//clear the model matrix
	glUniformMatrix4fv(s->u_model_location, 1, false, &mat4(1.0f)[0][0]);
	glUniformMatrix4fv(s->u_projection_location, 1, false, glm::value_ptr(camera->mat_projection));
	glUniform1i(s->u_state, 0);
	
	//default shader should  be enabled right now 
	//(after using different shaders, always switch to default)
	
	glDrawArrays(GL_LINES, 0, data.p.size());
	
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &buff);
	glDeleteBuffers(1, &color);
	glUseProgram(0);
	//@kepler: clear the draw list vector after it's done, so it doesn't eat memory.
	data = {};
}

void render_point_group(rg_point &data, shader *s, Camera *camera)
{
	assert(data.p.size() > 0);
	glUseProgram(s->id);
	
	GLuint buff, color;
	glGenBuffers(1, &buff);
	glBindBuffer(GL_ARRAY_BUFFER, buff);
	glBufferData(GL_ARRAY_BUFFER, data.p.size() * sizeof(vec3), &data.p[0], GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	
	glGenBuffers(1, &color);
	glBindBuffer(GL_ARRAY_BUFFER, color);
	glEnableVertexAttribArray(1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * data.color.size(), &data.color[0], GL_DYNAMIC_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
	
	glUniformMatrix4fv(s->u_view_location, 1, false, glm::value_ptr(camera->get_mat()));
	//clear the model matrix
	glUniformMatrix4fv(s->u_model_location, 1, false, &mat4(1.0f)[0][0]);
	glUniformMatrix4fv(s->u_projection_location, 1, false, glm::value_ptr(camera->mat_projection));
	glUniform1i(s->u_state, 0);
	
	//default shader should  be enabled right now 
	//(after using different shaders, always switch to default)
	glPointSize(8);
	glDrawArrays(GL_POINTS, 0, data.p.size());
	
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &buff);
	glDeleteBuffers(1, &color);
	glUseProgram(0);
	//@kepler: clear the draw list vector after it's done, so it doesn't eat memory.
	data = {};
}

void push_lines(rg_line &data, vec3 &a, vec3 &b, vec4 &color)
{
	data.p.push_back(a);
	data.p.push_back(b);
	data.color.push_back(color);
}

void push_point(rg_point &data, vec3 &p, vec4 &color, int size)
{
	data.p.push_back(p);
	data.color.push_back(color);
	data.size.push_back(size);
}

void debug_point(vec3 a, vec4 color, float size, shader *s)
{
	glUseProgram(s->id);
	vec3 data = a;
	
	GLuint buff;
	glGenBuffers(1, &buff);
	glBindBuffer(GL_ARRAY_BUFFER, buff);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3), &data[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	
	//clear the model matrix
	glUniformMatrix4fv(s->u_model_location, 1, false, &mat4(1.0f)[0][0]);
	
	//default shader should  be enabled right now 
	//(after using different shaders, always switch to default)
	glUniform1i(s->u_state, 1);
	glUniform4fv(s->u_color_location, 1, &color[0]);
	
	glPointSize(size);
	glDrawArrays(GL_POINTS, 0, 1);
	
	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &buff);
	glUseProgram(0);
}
struct text_renderer_info {
	std::map<GLchar, Character> text_characters;
	GLuint text_vao, text_vertices, text_vertices_uv;
	Transform t;
};
text_renderer_info text_info;


bool init_text_renderer(text_renderer_info &info)
{
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
	{
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		return false;
	}
	
	FT_Face face;
	if (FT_New_Face(ft, "C:/Windows/Fonts/arial.ttf", 0, &face))
	{
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		return false;
	}
	
	FT_Set_Pixel_Sizes(face, 0, 48);
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
	for (GLubyte c = 0; c < 128; c++)
	{
		// Load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// Generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Now store character for later use
		Character t = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			(GLuint)face->glyph->advance.x
		};
		info.text_characters.insert(std::pair<GLchar, Character>(c, t));
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
	
	GLfloat uv[6][2] = {
		{ 0.0, 0.0 },
		{ 0.0, 1.0 },
		{ 1.0, 1.0 },
		
		{ 0.0, 0.0 },
		{ 1.0, 1.0 },
		{ 1.0, 0.0 } };
	
	glGenBuffers(1, &info.text_vertices);
	glGenBuffers(1, &info.text_vertices_uv);
	glGenVertexArrays(1, &info.text_vao);
	
	glBindVertexArray(info.text_vao);
	glBindBuffer(GL_ARRAY_BUFFER, info.text_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 3, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, info.text_vertices_uv);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 2, uv, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	return true;
}

struct texture_info
{
	GLuint texture_vao, texture_quad_vertices, texture_quad_vertices_uv;
	GLuint id;
	Transform t;
	bool fail;
	vec2 dim;
};

void init_texture(texture_info &info, string path)
{
	
	int x, y, bbp;
	unsigned char* data = stbi_load(path.c_str(), &x, &y, &bbp, 4);
	if(!data){ 
		cout << "STBI_LOAD error, couldn't load image: " + path << endl;
		info.fail = true;
		return;
	}
	
	glGenTextures(1, &info.id);
	glBindTexture(GL_TEXTURE_2D, info.id);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGBA,
		x,
		y,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		data);
	
	info.dim = vec2(x, y);
	
	cout << "STBI_LOAD: bbp = " + to_string(bbp) << endl;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glBindTexture(GL_TEXTURE_2D, 0);
	
	//kepler: coordinates are flipped.
	GLfloat uv[] =
	{  0,  0, 
		1,  0, 
		1,  1,
		0,  1 }; 
	
	GLfloat vertices[] =
	{ -1,  1, 0,
		1,  1, 0, 
		1, -1, 0, 
		-1, -1, 0 }; 
	
	//@kepler: get buffers before setting up the VAO
	glGenBuffers(1, &info.texture_quad_vertices);
	glGenBuffers(1, &info.texture_quad_vertices_uv);
	glGenVertexArrays(1, &info.texture_vao);
	
	//@kepler: feed the data
	glBindVertexArray(info.texture_vao);
	glBindBuffer(GL_ARRAY_BUFFER, info.texture_quad_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * 3, &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, info.texture_quad_vertices_uv);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * 2, &uv[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	info.fail = false;
}

void draw_texture_quad(texture_info &info, shader *s, vec3 pos, Camera *camera, bool debug, bool billboard)
{
	if(info.fail) {cout << "draw_texture_quad: not rendering false texture info" << endl; return;
	}
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	// Activate corresponding render state	
	glUseProgram(s->id);
	
	glUniformMatrix4fv(s->u_view_location, 1, false, glm::value_ptr(camera->mat_view));
	glUniformMatrix4fv(s->u_model_location, 1, false, glm::value_ptr(info.t.t));
	glUniformMatrix4fv(s->u_projection_location, 1, false, glm::value_ptr(camera->mat_projection));
	
	glUniform1i(s->u_is_billboard, billboard);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, info.id);
	glBindVertexArray(info.texture_vao);
	
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	if (debug)
	{
		//@todo: what if you want to init it at any frame after initing the uniforms up at the top of the function?
		glUniform1i(s->u_draw_outline, 1);
		glUniform4f(s->u_color_location, 1, 0, 0, 1);
		glPointSize(8.0f);
		glDrawArrays(GL_LINE_LOOP, 0, 4);
		glDrawArrays(GL_POINTS, 0, 4);
		glUniform1i(s->u_draw_outline, 0);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glDisable(GL_BLEND);
	glUseProgram(0);
}

void kpl_draw_texture(texture_info &info, vec3 pos, vec3 sc, bool debug, bool billboard)
{
	info.t.t = glm::translate(mat4(1.0f), pos) * scale(mat4(1.0f), sc);
	draw_texture_quad(info, &texture_shader, pos, &camera, debug, billboard);
}

void render_text(text_renderer_info &info, shader *s, std::string text, vec3 pos, GLfloat scale, vec4 color,
                 Camera *camera, bool debug)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	// Activate corresponding render state	
	glUseProgram(s->id);
	
	glUniformMatrix4fv(s->u_view_location, 1, false, glm::value_ptr(camera->mat_view));
	glUniformMatrix4fv(s->u_model_location, 1, false, glm::value_ptr(info.t.t));
	glUniformMatrix4fv(s->u_projection_location, 1, false, glm::value_ptr(camera->mat_projection));
	
	//glBindVertexArray(text_VAO_UV);
	
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(info.text_vao);
	
	GLfloat origin_x = 0;
	GLfloat origin_y = 0;
	
	scale *= 0.01f;
	
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = info.text_characters[*c];
		
		GLfloat xpos = origin_x + ch.Bearing.x * scale;
		GLfloat ypos = origin_y - (ch.Size.y - ch.Bearing.y) * scale;
		
		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;
		// Update VBO for each character
		GLfloat vertices[6][3] = {
			{ xpos,     ypos + h, 0.0 },
			{ xpos,     ypos, 0.0 },
			{ xpos + w, ypos, 0.0 },
			
			{ xpos,     ypos + h, 0.0 },
			{ xpos + w, ypos, 0.0 },
			{ xpos + w, ypos + h, 0.0 }
		};
		
		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, info.text_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), &vertices[0]); // Be sure to use glBufferSubData and not glBufferData
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Render quad
		glUniform4fv(s->u_text_color, 1, &color[0]);
		
		glDrawArrays(GL_TRIANGLES, 0, 6);
		if (debug)
		{
			//@todo: what if you want to init it at any frame after initing the uniforms up at the top of the function?
			glUniform1i(s->u_draw_outline, 1);
			glUniform4f(s->u_color_location, 1, 0, 0, 1);
			glPointSize(4.0f);
			glDrawArrays(GL_LINE_LOOP, 0, 6);
			glDrawArrays(GL_POINTS, 0, 6);
			glUniform1i(s->u_draw_outline, 0);
		}
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		origin_x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_BLEND);
	glUseProgram(0);
}

void kpl_draw_text(text_renderer_info &info, vec3 pos, string text, float sc, vec4 color, bool debug)
{
	info.t.t = translate(mat4(1.0f), pos) * scale(mat4(1.0f), vec3(sc, sc, sc));
	render_text(info, &text_shader, text, pos, sc, color, &camera, debug);
}


void draw_quad(shader *s, vec3 pos, vec3 axis, float angle, GLfloat scale, glm::vec4 color,
               Camera *camera, bool debug)
{
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	
	// Activate corresponding render state	
	glUseProgram(s->id);
	
	mat4 m = glm::translate(mat4(1.0f), pos) * rotate(mat4(1.0f), angle, axis);
	
	glUniformMatrix4fv(s->u_view_location, 1, false, glm::value_ptr(camera->mat_view));
	glUniformMatrix4fv(s->u_model_location, 1, false, glm::value_ptr(m));
	glUniformMatrix4fv(s->u_projection_location, 1, false, glm::value_ptr(camera->mat_projection));
	glUniform4fv(s->u_color_location, 1, &color[0]);
	
	GLfloat vertices[] = 
	{ -1, -1, 0, // bottom left corner
		-1,  1, 0, // top left corner
		1,  1, 0, // top right corner
		1, -1, 0 }; // bottom right corner
	
	GLuint buff;
	glGenBuffers(1, &buff);
	glBindBuffer(GL_ARRAY_BUFFER, buff);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12, &vertices[0], GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	if (debug)
	{
		//@todo: what if you want to init it at any frame after initing the uniforms up at the top of the function?
		glUniform1i(s->u_draw_outline, 1);
		glUniform4f(s->u_color_location, 1, 0, 0, 1);
		glPointSize(1.0f);
		glDrawArrays(GL_LINE_LOOP, 0, 4);
		glDrawArrays(GL_POINTS, 0, 4);
		glUniform1i(s->u_draw_outline, 0);
	}
	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glDisable(GL_BLEND);
	glDeleteBuffers(1, &buff);
	glUseProgram(0);
}

void draw_rect(shader *s, vec3 pos, float w, float h, vec3 axis, float angle, float  scale, vec4 color,
               Camera *camera, bool debug)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_CULL_FACE);
	
	// Activate corresponding render state	
	glUseProgram(s->id);
	
	mat4 m = glm::translate(mat4(1.0f), pos) * rotate(mat4(1.0f), angle, axis);
	
	glUniformMatrix4fv(s->u_view_location, 1, false, glm::value_ptr(camera->mat_view));
	glUniformMatrix4fv(s->u_model_location, 1, false, glm::value_ptr(m));
	glUniformMatrix4fv(s->u_projection_location, 1, false, glm::value_ptr(camera->mat_projection));
	glUniform4fv(s->u_color_location, 1, &color[0]);
	
	GLfloat vertices[] = 
	{ 0, 0, 0, // bottom left corner
		0,  h, 0, // top left corner
		w,  h, 0, // top right corner
		w, 0, 0 }; // bottom right corner
	
	GLuint buff;
	glGenBuffers(1, &buff);
	glBindBuffer(GL_ARRAY_BUFFER, buff);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12, &vertices[0], GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	if (debug)
	{
		//@todo: what if you want to init it at any frame after initing the uniforms up at the top of the function?
		glUniform1i(s->u_draw_outline, 1);
		glUniform4f(s->u_color_location, 1, 0, 0, 1);
		glPointSize(1.0f);
		glDrawArrays(GL_LINE_LOOP, 0, 4);
		glDrawArrays(GL_POINTS, 0, 4);
		glUniform1i(s->u_draw_outline, 0);
	}
	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glDisable(GL_BLEND);
	glDeleteBuffers(1, &buff);
	glUseProgram(0);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	debug_point(pos, vec4(1), 8, &default_shader);
}


void draw_circle(shader *s, vec3 pos, vec3 axis, float angle, GLfloat scale, glm::vec4 color,
                 Camera *camera, bool debug)
{
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	
	// Activate corresponding render state	
	glUseProgram(s->id);
	
	mat4 m = glm::translate(mat4(1.0f), pos) * rotate(mat4(1.0f), angle, axis);
	
	glUniformMatrix4fv(s->u_view_location, 1, false, glm::value_ptr(camera->mat_view));
	glUniformMatrix4fv(s->u_model_location, 1, false, glm::value_ptr(m));
	glUniformMatrix4fv(s->u_projection_location, 1, false, glm::value_ptr(camera->mat_projection));
	glUniform4fv(s->u_color_location, 1, &color[0]);
	
	GLfloat vertices[] =
	{ -1, -1, 0, // bottom left corner
		-1,  1, 0, // top left corner
		1,  1, 0, // top right corner
		1, -1, 0 }; // bottom right corner
	
	GLuint buff;
	glGenBuffers(1, &buff);
	glBindBuffer(GL_ARRAY_BUFFER, buff);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12, &vertices[0], GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	if (debug)
	{
		//@todo: what if you want to init it at any frame after initing the uniforms up at the top of the function?
		glUniform1i(s->u_draw_outline, 1);
		glUniform4f(s->u_color_location, 1, 0, 0, 1);
		glPointSize(1.0f);
		glDrawArrays(GL_LINE_LOOP, 0, 4);
		glDrawArrays(GL_POINTS, 0, 4);
		glUniform1i(s->u_draw_outline, 0);
	}
	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glDisable(GL_BLEND);
	glDeleteBuffers(1, &buff);
	glUseProgram(0);
}

void render_model(Model &model, Transform &transform, Camera *camera)
{
	if(model.did_import_fail) return;
	glUseProgram(model.material.id);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	//@kepler: IMPORTANT! since glsl does something withmatrices, before sending, matrices needs to be sent as transpose.
	mat4 t = transform.r * transform.t;
	
	glUniformMatrix4fv(model.material.u_view_location, 1, false, glm::value_ptr(camera->get_mat()));
	glUniformMatrix4fv(model.material.u_model_location, 1, false, glm::value_ptr(t));
	glUniformMatrix4fv(model.material.u_projection_location, 1, false, glm::value_ptr(camera->mat_projection));
	
	glUniform1i(model.material.u_state, 1);
	glUniform4fv(model.material.u_color_location, 1, &model.material.color[0]);
	
	glBindBuffer(GL_ARRAY_BUFFER, model.vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * model.vertices_data.size(), &model.vertices_data[0], GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.index_buffer);
	
	glDrawElements(GL_TRIANGLES, model.indices.size(), GL_UNSIGNED_INT, 0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glUniform4fv(model.material.u_color_location, 1, &vec4(model.material.color * 0.75f)[0]);
	glDrawElements(GL_TRIANGLES, model.indices.size(), GL_UNSIGNED_INT, 0);
	//glDrawElements(GL_POINTS, model.indices.size(), GL_UNSIGNED_INT, 0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glUseProgram(0);
}

/*
void draw_model(Model &model, vec3 pos, vec4 color)
{
 model.line_color = color;
 render_model(model, &camera, &default_shader);
}
*/
void render_jmodel(JModel *model, Camera *camera, shader *s)
{
	if(model->did_import_fail) return;
	glUseProgram(s->id);
	
	Transform *t = &model->transform;
	
	if (s->rand_colors.size() == 0 && s->is_inited)
	{
		float r, g, b;
		for (int i = 0; i < model->data.size();)
		{
			for (int j = 0; j < model->data[i].beams.size();)
			{
				r = rand_range();
				g = rand_range();
				b = rand_range();
				vec4 color(r, g, b, 1.0f);
				s->rand_colors.push_back(color);
				j++;
			}
			i++;
		}
	}
	
	glUniformMatrix4fv(s->u_view_location, 1, false, glm::value_ptr(camera->get_mat()));
	glUniformMatrix4fv(s->u_model_location, 1, false, glm::value_ptr(t->m));
	glUniformMatrix4fv(s->u_projection_location, 1, false, glm::value_ptr(camera->mat_projection));
	glBindBuffer(GL_ARRAY_BUFFER, model->vertex_buffer);
	glEnableVertexAttribArray(0);
	glPointSize(4);
	
	for (int i = 0; i < model->data.size(); i++)
	{
		glBufferData(GL_ARRAY_BUFFER, sizeof(jvertex) * model->data[i].nodes.size(), &model->data[i].nodes[0].pos[0], GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(jvertex), 0);
		
		for (int i = 0; i < model->data[i].beams.size(); i++)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->data[i].beams[i].index_buffer);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINES);
			
			glUniform4fv(s->u_color_location, 1, &s->rand_colors[i][0]);
			glDrawElements(GL_LINES, model->data[i].beams[i].indices.size(), GL_UNSIGNED_INT, 0);
			
			//glDrawArrays(GL_LINES, 0, model->data.nodes.size());
			glPolygonMode(GL_POINT, GL_LINE);
			
			glUniform4fv(s->u_color_location, 1, &vec4(1, 0.5, 0.1, 1)[0]);
			glDrawElements(GL_POINTS, model->data[i].beams[i].indices.size(), GL_UNSIGNED_INT, 0);
		}
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisableClientState(GL_VERTEX_ARRAY);
	glUseProgram(0);
}
