vector<Model> model_mesh_memory;
vector<Mover> mover_memory;

enum Entity_Component_Type {
	COMPONENT_MESH,
	COMPONENT_PRIMITIVE,
	COMPONENT_MOVER
};

struct component_mesh_renderer {
	ui32 data_id;
};

struct component_mover {
	ui32 data_id;
};

struct component_logic {
	ui32 data_id;
};

struct Entity;

void logic(Entity *e);

struct Entity {
	ui32 id;
	Transform transform;
	vector<component_mesh_renderer> mesh_components;
	vector<component_mover> mover_components;
	RigidBody collider;
	void (*do_logic)(Entity *);
};

void logic(Entity *e)
{
	vec3 p = rand_vec3(2.0f, 16.0f);
	p.y = 6.0f;
	e->collider.t.set_position(p);
	e->collider.add_force(normalize(input_state.mouse_w - e->collider.t.position())*100.0f);
}

void mouse_follower(Entity *e)
{
	float dist = distance(e->transform.position(), camera.pos);
	if(dist > 0.5f)
	{
		vec3 d;
		d = normalize(camera.pos - e->transform.position());
		e->transform.translate(d / (dist));
	}
}

void assemble_entity(Entity &e)
{
	//put needed entities
}

vector<Entity> entities;
void attach_component(Entity &e, Entity_Component_Type type, Model &mesh_data)
{
	switch(type)
	{
		case COMPONENT_MESH: 
		{
			component_mesh_renderer t;
			t.data_id = e.mesh_components.size();
			e.mesh_components.push_back(t);
			mesh_data.transform.r = mat4(1.0f);
			model_mesh_memory.push_back(mesh_data);
			
			//assert(t.data_id == model_mesh_memory.size()-1);
		} break;
		
		case COMPONENT_MOVER:
		{
			component_mover t;
			t.data_id = e.mover_components.size();
			e.mover_components.push_back(t);
			Mover m = {};
			mover_memory.push_back(m);
			
			//assert(t.data_id == mover_memory.size()-1);
		} break;
		
		case COMPONENT_PRIMITIVE:
		{
			
		} break;
		
		default: cout << "no type was recognized" << endl; e.id = -1;
	}
}

void push_entity(Entity &e)
{
	e.id = entities.size();
	entities.push_back(e);
}

void delete_entity(ui32 id)
{
	if(id > entities.size()) 
	{
		cout << "trying to delete non existant entity at: " << id << endl;
		return;
	}
	else if(entities.size() != 0)
	{
		entities.erase(entities.begin() + index);
	}
}

void delete_component(ui32 entity_index, Entity_Component_Type type)
{
	switch(type)
	{
		case COMPONENT_MESH: 
		{
			if(entity_index < entities.size())
			{
				if(entities[entity_index].mesh_components.size() > 0)
				{
					int index = entities[entity_index].mesh_components.size();
					entities[entity_index].mesh_components.erase(entities[entity_index].mesh_components.begin() + index);
					model_mesh_memory.erase(model_mesh_memory.begin() + index);
				}
			}
		} break;
		
		case COMPONENT_MOVER:
		{
			if(entity_index < entities.size())
			{
				if (entities[entity_index].mover_components.size() > 0)
				{
					ui32 index = entities[entity_index].mover_components.size();
					entities[entity_index].mover_components.erase(entities[entity_index].mover_components.begin() + index);
					mover_memory.erase(mover_memory.begin() + index);
				}
			}
		} break;
		
		case COMPONENT_PRIMITIVE:
		{
			
		} break;
		
		default: cout << "no type was recognized." << endl;
	}
}

Entity* get_entity(int id)
{
	if(id <= entities.size() && id >= 0)
	{
		return &entities[id];
	}
	else 
	{
		Entity e;
		return &e;
	}
}

Model* get_mesh_component(int entity_id)
{
	if(entity_id <= entities.size())
	{
		int index = entities[entity_id].mesh_components[entity_id].data_id;
		return &model_mesh_memory[index];
	}
	else 
	{
		Model m;
		return &m; 
	}
}

Entity *selected_entity = 0;

void select_entity(Entity *e)
{
	if(distance(e->transform.position(), input_state.mouse_w) < 2.0f && (input_state.mouse_left.state & kON))
	{
		selected_entity = e;
	}
	
}

static float bf, af;

RigidBody *rg = 0;
vec4 color;
void process_entities()
{
	for(ui32 eid = 0; eid < entities.size(); eid++)
	{
		Entity *e = &entities[eid];
		
		if(e->id == -1)
		{
			cout << "skipped entity id: " << e->id << endl;
			continue;
		}
		if(e->mesh_components.size() > 0)
		{
			//e->transform.translate(vec3(sinf(time_state.seconds_passed)*eid, cosf(time_state.seconds_passed)*eid, 0.0f) * (eid * time_state.dt));
			ui32 index = e->mesh_components[e->mesh_components.size()-1].data_id;
			//render_model(model_mesh_memory[index], e->transform, &camera);
		}
		if(e->mover_components.size() == 1)
		{
			ui32 index = e->mover_components[e->mover_components.size()-1].data_id;
			update_movers(e->transform, 1.0f, input_state, time_state);
		}
		if(display_info.show_entity_bases)
		{
			show_basis(e->transform);
		}
		select_entity(e);
		if(selected_entity && input_state.k_delete)
		{
			delete_entity(selected_entity->id);
			selected_entity = 0;
		}
		push_text(&render_group_text, to_string(e->id), e->transform.position(), 0.5f, BLUE);
		
		
		/*
  float l0, l1;
  l0 = e->collider.t.position().y;
  l1 = 2.0f;
  
  float fdelta = (e->collider.mass * VAR_G) / 0.5f; 
  float f = -0.5f * e->collider.mass * (l0 - l1);
  cout << "force" << f << endl;
  
  e->collider.add_force(vec3(0.0f, f, 0.0f));
  */
		
		if(e->do_logic && input_state.k_enter) e->do_logic(e);
		
		e->collider.update_physics(time_state);
		e->transform.set_position(e->collider.t.position());
		resolve_collision(e->collider);
		e->transform.set_position(e->collider.t.position());
		
		
		
		//e->do_logic(e);
		
		color =vec4(abs(e->collider.velocity.y), abs(e->collider.velocity.x), abs(e->collider.velocity.z), 1.0f);
		
		push_cube(&render_group_cubes, e->transform.position(), color, 1.0f);
	}
	if(selected_entity)
	{
		imgpushf("m", selected_entity->collider.mass);
		imgpushv3f("a", selected_entity->collider.acceleration);
		imgpushf("f", VAR_G);
		imgpushv3f("v", selected_entity->collider.velocity);
		imgpushv3f("v/t", selected_entity->collider.velocity * time_state.dt);
		imgpushv4f("color", color);
		push_cube(&render_group_cubes, selected_entity->transform.position(), GREEN, 1.25f);
		push_text(&render_group_text, to_string(selected_entity->id), selected_entity->transform.position(), 0.5f, GREEN);
		kpl_draw_text(text_info, to_string(selected_entity->id), selected_entity->transform.position()+vec3_up*0.25f, 0.75f, GREEN, 1);
	}
	
	vec3 mouse_noz = input_state.mouse_w;
	mouse_noz.z = 0.0f;
	push_text(&render_group_text, "num of entities:" + to_string(entities.size()),  mouse_noz + vec3_up * 0.25f, 0.5f, GREEN);
	
	push_text(&render_group_text, to_string(input_state.mouse_w.x) + ", " + to_string(input_state.mouse_w.y) + ", " + to_string(input_state.mouse_w.z), vec3(512, 512, 0), 0.5f, GREEN);
	
	
	LARGE_INTEGER tick_after_loop, tick_before_loop;
	QueryPerformanceCounter(&tick_before_loop);
	__int64 interval;
	
	render_text_group(render_group_text, text_info, &text_shader, &camera, 0);
	render_line_group(render_list_lines, &default_shader, &camera);
	render_cube_group(render_group_cubes, &default_shader, &camera);
	QueryPerformanceCounter(&tick_after_loop);
	interval = tick_after_loop.QuadPart - tick_before_loop.QuadPart;
	
	bf = interval * second_per_tick;
	render_group_text.clear();
}








