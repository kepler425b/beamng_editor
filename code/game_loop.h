void asd()
{
	c_cur = camera.pos;

	c_velocity = c_dt * time_state.dt;
	c_force = c_velocity * c_mass;


	if (input_state.k_space)
	{
		camera.pos += vec3(0, 200.5f, 0) * c_mass * time_state.dt;
		//input_state.k_space = false;
	}

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

	jmodel.line_color = vec3(0.2f, 1.0f, 0.2f);

	jmodel.translate(jmodel.local_position);
	jmodel.uniformScale(1.0f);

	//point.mat_model = glm::translate(mat4(1.0f), point.pos) * glm::rotate(mat4(1.0f), 90.0f, vec3(1.0f, 0.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.15f));



	//render_jmodel(&jmodel, &camera, &default_shader);

	//rapidjson_loop(document, input_state);

	//sphere.rotate(normalize(input_state.mouse_w - vec3_up), sinf(input_state.mouse_dt3.x) * (180 / pi) *0.25);
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


	draw_quad(&default_shader, p, vec3(1.0, 1.0, 1.0), 0.0f, p.x, vec4(0, 0, 0, p.z), &camera, false);
	//draw_quad(&default_shader, vec3(0, -2, 0), vec3(1, 0, 0), 80, 10.0f, vec4(0, 0, 0, p.z), &camera, false);
	debug_point(vec3(0, -2, 0), RED + GREEN, 10, &default_shader);

	//tinfo.t.t = scale(mat4(1), vec3(0.25f, 0.25f, 0.25f));
	n_last = n;
	n2_last = n2;

	n += (tpos * 0.02f * time_state.dt);
	n2 += (tpos2 * 0.02f * time_state.dt);
	float dist = distance(input_state.mouse_w, n);
	//debug_line(input_state.mouse_w, n, BLUE, &default_shader, &camera);
	if (dist < 0.5f && input_state.m_left)
	{
		ss = true;
	}
	if (ss)
	{
		tpos += vec3(input_state.mouse_dt2.x, -input_state.mouse_dt2.y, 0) * 0.1f;
		if (!input_state.m_left) ss = false;
	}

	float dist_ab = distance(n, n2);
	if (dist_ab < 0.5f)
	{
		tpos += n2_delta * length(n2_delta);
		tpos2 += n_delta * length(n_delta);
	}

	kpl_draw_texture(texture_info, n, 1.0f, show_outline, is_billboard);

	kpl_draw_texture(texture_info, n2, 1.0f, show_outline, is_billboard);

	//kpl_draw_texture(texture_info, input_state.mouse_w, 0.1f, true, is_billboard);

	debug_line(n, n + n_delta * length(n_delta) * 1000.0f, BLUE + GREEN, &default_shader, &camera);

	debug_point(n, BLUE, 8, &default_shader);
	debug_point(n2, RED + BLUE, 8, &default_shader);

	draw_model(sphere, vec3_zero, GREEN * 0.5f);
};
