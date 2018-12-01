

//NOTE: I should remove entities that passed out of the world by big distances.

void resolve_collisions(vector<RigidBody> &t)
{
    for(int i = 0; i < t.size(); i++)
    {
        if(t[i].t.position().y <= -2.0f)
        {
            vec3 dir = normalize(t[i].velocity);
            vec3 nr = vec3(0, 1.0f, 0);
            float l = dot(dir, nr);
            vec3 r = -2.0f*l*nr + dir;
            vec3 n = normalize(r);
            t[i].t.translate(vec3(0, -2.0f - t[i].t.position().y, 0));
            vec3 v = n * (0.4f * length(t[i].velocity)); 
            t[i].velocity = v;
            //tomato.add_force(vec3(16.0f, 0, 0.9f));
            
            debug_line(n, n + r, RED+GREEN, &default_shader, &camera);
        }
        else if(t[i].t.position().y >= 8.0f)
        {
            vec3 dir = normalize(t[i].velocity);
            vec3 nr = vec3(0, 1.0f, 0);
            float l = dot(dir, nr);
            vec3 r = -2.0f*l*nr + dir;
            vec3 n = normalize(r);
            t[i].t.translate(vec3(0, 8.0f - t[i].t.position().y, 0));
            vec3 v = n * (0.4f * length(t[i].velocity)); 
            t[i].velocity = v;
            //tomato.add_force(vec3(16.0f, 0, 0.9f));
            
            debug_line(n, n + r, RED+GREEN, &default_shader, &camera);
        }
        else {
            continue;
        }
    }
}

struct Ray_Info {
	vec3 contact_point;
	vec3 direction;
};

bool cast_ray(Sphere &target, Ray_Info &info, vec3 origin, vec3 direction, float length)
{
	vec3 d = normalize(direction);
	vec3 m = origin - target.p;
	float b = dot(m, d);
	float c = dot(m, m) - target.r * target.r;
	debug_line(origin, origin + d * length, GREEN, &default_shader, &camera);
	cout << "c: " << c << endl;
	cout << "b: " << b << endl;
	if(c > 0.0f && b > 0.0f) return false;
	
	float discr = b*b - c;
	
	cout << "discr: " << discr << endl;
	if (discr < 0.0f) return false;
	debug_line(origin, origin + d * discr, GREEN, &default_shader, &camera);
	
	float t = -b - sqrt(discr);
	
	info.contact_point = origin + t * d;
	
	cout << "RAY COLLISION" << endl;
	//info.contact_point = origin + (dn * ray_distance - t);
	//info.direction = dn;
	
	debug_point(info.contact_point, GREEN, 16, &default_shader);
	debug_line(origin, d * t, RED + GREEN, &default_shader, &camera);
	
	if (t < 0.0f) 
	{
		t = 0.0f;
		info.contact_point = origin + t * d;
		
		cout << "RAY STARTED INSIDE SPHERE" << endl;
		//info.contact_point = origin + (dn * ray_distance - t);
		//info.direction = dn;
		
		debug_point(info.contact_point, GREEN, 16, &default_shader);
		debug_line(origin, d * t, RED + GREEN, &default_shader, &camera);
		return true;
	}
}

bool resolve_rect_collisions(Collider_Rect a, Collider_Rect b)
{
	int r;
	
	r = a.r[0] + b.r[0]; if(((unsigned int)a.origin.x - b.origin.x + r) > r + r) return 0;r = a.r[1] + b.r[1]; if(((unsigned int)a.origin.y - b.origin.y + r) > r + r) return 0;
	cout << "asd" << endl;
	return 1; 
}

bool resolve_sphere_collisions(Sphere &a, Sphere &b)
{
    vec3 tmp = a.p - b.p;
    float d = dot(tmp, tmp);
    float r = a.r + b.r;
    if (d <= r * r) return true;
    else return false;
}

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
