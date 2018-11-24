bool col(vec3 &pA, vec3 &pB, jvertex &Target, bool &lshift)
{
	vec3 target_vec3 = { Target.pos.x, Target.pos.y, Target.pos.z };
	vec3 closest_p;
	vec3 p_dir = pB - pA;
	vec3 p_dir_n = normalize(p_dir);
	vec3 t_dir = target_vec3 - pA;
	float t_dis = distance(pB, pA);
	float t = dot(t_dir, p_dir_n);
	float result_dis;


	if (t <= 0.0)
	{
		closest_p = pA;
	}
	else if (t >= t_dis)
	{
		closest_p = pB;
	}
	else
	{
		closest_p = pA + p_dir_n * t;
	}

	result_dis = distance(target_vec3, closest_p);


	if (result_dis <= 0.02f) {
		Target.active = true;
		return true;
	}
	else
	{
		if (!lshift) Target.active = false;

		return false;
	}
}

vec3 col_area(vec3 &pA, vec3 &pB)
{
	vec3 closest_p;
	vec3 p_dir = pB - pA;
	vec3 p_dir_n = normalize(p_dir);
	float t_dis = distance(pB, pA);
	float result_dis;

	return vec3(pA.x * t_dis * pA.y);
}


bool col_edge(vec3 &pA, vec3 &pB, jvertex &Target)
{
	vec3 target_vec3 = { Target.pos.x, Target.pos.y, Target.pos.z };
	vec3 closest_p;
	vec3 p_dir = pB - pA;
	vec3 p_dir_n = normalize(p_dir);
	vec3 t_dir = target_vec3 - pA;
	float t_dis = distance(pB, pA);
	float t = dot(t_dir, p_dir_n);
	float result_dis;

	if (t <= 0.0)
	{
		closest_p = pA;
	}
	else if (t >= t_dis)
	{
		closest_p = pB;
	}
	else
	{
		closest_p = pA + p_dir_n * t;
	}

	result_dis = distance(target_vec3, closest_p);


	if (result_dis <= 0.8f) {
		Target.active = true;
		return true;
	}
	return false;
}

bool col_beam(vec3 &pA, vec3 &pB, beam_pointer *beam, bool &lshift)
{
	vec3 target_vec3 = vec3_midpoint(beam->a->pos, beam->b->pos);
	vec3 closest_p;
	vec3 p_dir = pB - pA;
	vec3 p_dir_n = normalize(p_dir);
	vec3 t_dir = target_vec3 - pA;
	float t_dis = distance(pB, pA);
	float t = dot(t_dir, p_dir_n);
	float result_dis;

	if (t <= 0.0)
	{
		closest_p = pA;
	}
	else if (t >= t_dis)
	{
		closest_p = pB;
	}
	else
	{
		closest_p = pA + p_dir_n * t;
	}

	result_dis = distance(target_vec3, closest_p);

	if (result_dis <= 0.05f) {
		beam->a->active = true;
		beam->b->active = true;
		return true;
	}
	else
	{
		if (!lshift)
		{
			beam->a->active = false;
			beam->b->active = false;
		}
		return false;
	}
}

bool col_beam_ishowered(vec3 &pA, vec3 &pB, beam_pointer *beam)
{
	vec3 target_vec3 = vec3_midpoint(beam->a->pos, beam->b->pos);
	vec3 closest_p;
	vec3 p_dir = pB - pA;
	vec3 p_dir_n = normalize(p_dir);
	vec3 t_dir = target_vec3 - pA;
	float t_dis = distance(pB, pA);
	float t = dot(t_dir, p_dir_n);
	float result_dis;

	if (t <= 0.0)
	{
		closest_p = pA;
	}
	else if (t >= t_dis)
	{
		closest_p = pB;
	}
	else
	{
		closest_p = pA + p_dir_n * t;
	}

	result_dis = distance(target_vec3, closest_p);

	if (result_dis <= 0.05f) {
		return true;
	}
	return false;
}

bool col2(vec3 &pA, vec3 &pB, vec3 &Target)
{
	vec3 target_vec3 = { Target.x, Target.y, Target.z };
	vec3 closest_p;
	vec3 p_dir = pB - pA;
	vec3 p_dir_n = normalize(p_dir);
	vec3 t_dir = target_vec3 - pA;
	float t_dis = distance(pB, pA);
	float t = dot(t_dir, p_dir_n);
	float result_dis;

	if (t <= 0.0)
	{
		closest_p = pA;
	}
	else if (t >= t_dis)
	{
		closest_p = pB;
	}
	else
	{
		closest_p = pA + p_dir_n * t;
	}

	result_dis = distance(target_vec3, closest_p);

	if (result_dis <= 0.05f) {
		return true;
	}
	return false;
}
