/*
#include "animation_system.h"
#include "..\ragebot\aim.h"
void resolver::initialize(player_t* e, adjust_data* record, const float& goal_feet_yaw, const float& pitch)
{
	player = e;
	player_record = record;

	original_goal_feet_yaw = math::normalize_yaw(goal_feet_yaw);
	original_pitch = math::normalize_pitch(pitch);
}

Vector player_t::get_eye_pos() {
	return m_vecOrigin() + m_vecViewOffset();
}

void resolver::reset()
{
	player = nullptr;
	player_record = nullptr;

	side = false;

	was_first_bruteforce = false;
	was_second_bruteforce = false;

	original_goal_feet_yaw = 0.0f;
	original_pitch = 0.0f;
}

	bool resolver::low_delta()
	{
		auto record = player_record;
		if (!csgo.local()->is_alive())
			return false;
		float angle_diff = math::angle_diff(player->m_angEyeAngles().y, player->get_animation_state()->m_flGoalFeetYaw);
		Vector first = ZERO, second = ZERO, third = ZERO;
		first = Vector(player->hitbox_position(HITBOX_HEAD).x, player->hitbox_position(HITBOX_HEAD).y + min(angle_diff, 35), player->hitbox_position(HITBOX_HEAD).z);
		second = Vector(player->hitbox_position(HITBOX_HEAD).x, player->hitbox_position(HITBOX_HEAD).y, player->hitbox_position(HITBOX_HEAD).z);
		third = Vector(player->hitbox_position(HITBOX_HEAD).x, player->hitbox_position(HITBOX_HEAD).y - min(angle_diff, 35), player->hitbox_position(HITBOX_HEAD).z);
		Ray_t one, two, three;
		trace_t tone, ttwo, ttree;
		CTraceFilter fl;
		fl.pSkip = player;
		one.Init(csgo.local()->get_shoot_position(), first);
		two.Init(csgo.local()->get_shoot_position(), second);
		three.Init(csgo.local()->get_shoot_position(), third);
		m_trace()->TraceRay(one, MASK_PLAYERSOLID, &fl, &tone);
		m_trace()->TraceRay(two, MASK_PLAYERSOLID, &fl, &ttwo);
		m_trace()->TraceRay(three, MASK_PLAYERSOLID, &fl, &ttree);
		if (!tone.allsolid && !ttwo.allsolid && !ttree.allsolid && tone.fraction < 0.97 && ttwo.fraction < 0.97 && ttree.fraction < 0.97)
			return true;

		float lby = fabs(math::normalize_yaw(player->m_flLowerBodyYawTarget()));
		if (lby < 35 && lby > -35)
			return true;
		return false;
	}

bool resolver::low_delta2()
{
	auto record = player_record;
	if (!csgo.local()->is_alive())
		return false;
	float angle_diff = math::angle_diff(player->m_angEyeAngles().y, player->get_animation_state()->m_flGoalFeetYaw);
	Vector first = ZERO, second = ZERO, third = ZERO;
	first = Vector(player->hitbox_position(HITBOX_HEAD).x, player->hitbox_position(HITBOX_HEAD).y + min(angle_diff, 20), player->hitbox_position(HITBOX_HEAD).z);
	second = Vector(player->hitbox_position(HITBOX_HEAD).x, player->hitbox_position(HITBOX_HEAD).y, player->hitbox_position(HITBOX_HEAD).z);
	third = Vector(player->hitbox_position(HITBOX_HEAD).x, player->hitbox_position(HITBOX_HEAD).y - min(angle_diff, 20), player->hitbox_position(HITBOX_HEAD).z);
	Ray_t one, two, three;
	trace_t tone, ttwo, ttree;
	CTraceFilter fl;
	fl.pSkip = player;
	one.Init(csgo.local()->get_shoot_position(), first);
	two.Init(csgo.local()->get_shoot_position(), second);
	three.Init(csgo.local()->get_shoot_position(), third);
	m_trace()->TraceRay(one, MASK_PLAYERSOLID, &fl, &tone);
	m_trace()->TraceRay(two, MASK_PLAYERSOLID, &fl, &ttwo);
	m_trace()->TraceRay(three, MASK_PLAYERSOLID, &fl, &ttree);
	if (!tone.allsolid && !ttwo.allsolid && !ttree.allsolid && tone.fraction < 0.97 && ttwo.fraction < 0.97 && ttree.fraction < 0.97)
		return true;

	float lby = fabs(math::normalize_yaw(player->m_flLowerBodyYawTarget()));
	if (lby < 20 && lby > -20)
		return true;
	return false;
}

float resolver::GetBackwardYaw(player_t* ent) {
	return math::calculate_angle(csgo.local()->GetAbsOrigin(), ent->GetAbsOrigin()).y;
}

float resolver::GetForwardYaw(player_t* ent) {
	return math::normalize_yaw(GetBackwardYaw(ent) - 180.f);
}

void resolver::resolve_yaw()
{
	player_info_t player_info;

	if (!m_engine()->GetPlayerInfo(player->EntIndex(), &player_info))
		return;

	static int side[63];
	auto animstate  = player->get_animation_state();
	auto speed = csgo.local()->m_vecVelocity().Length2D();
	if (speed <= 0.1f)
	{
		if (player_record->layers[3].m_flWeight == 0.0 && player_record->layers[3].m_flCycle == 0.0)
		{
			side[player->EntIndex()] = 2 * (math::normalize_diff(player->m_angEyeAngles().y, player_record->abs_angles.y) <= 0.0) - 1;
		}
	}
	else
	{
		const float f_delta = abs(player_record->layers[6].m_flPlaybackRate -   player_record->left_layers[6].m_flPlaybackRate);
		const float s_delta = abs(player_record->layers[6].m_flPlaybackRate - player_record->center_layers[6].m_flPlaybackRate);
		const float t_delta = abs(player_record->layers[6].m_flPlaybackRate -  player_record->right_layers[6].m_flPlaybackRate);

		if (f_delta < s_delta || t_delta <= s_delta || (s_delta * 1000.0))
		{
			if (f_delta >= t_delta && s_delta > t_delta && !(t_delta * 1000.0))
			{
				side[player->EntIndex()] = 1;
			}
		}
		else
		{
			side[player->EntIndex()] = -1;
		}
	}

	if (player->GetAbsOrigin().y == GetForwardYaw(player))
		side[player->EntIndex()] *= -1;
	
	if (csgo.globals.missed_shots[player->EntIndex()] == 0) {
		if (side[player->EntIndex()] >= 1) {
			//right
			player_record->side = low_delta() ? RESOLVER_LOW_FIRST : RESOLVER_FIRST;
		}
		else if (side[player->EntIndex()] <= -1) {
			//left
			player_record->side = low_delta() ? RESOLVER_LOW_SECOND : RESOLVER_SECOND;
		}
	}
	else if (csgo.globals.missed_shots[player->EntIndex()] == 1) {
		if (side[player->EntIndex()] >= 1) {
			//right
			player_record->side = low_delta2() ? RESOLVER_LOW_FIRST1 : RESOLVER_FIRST;
		}
		else if (side[player->EntIndex()] <= -1) {
			//left
			player_record->side = low_delta2() ? RESOLVER_LOW_SECOND1 : RESOLVER_SECOND;
		}
	}
	else if (csgo.globals.missed_shots[player->EntIndex()] == 2) {
		player_record->side = RESOLVER_SECOND;
	}
	else if (csgo.globals.missed_shots[player->EntIndex()] == 3) {
		player_record->side = RESOLVER_FIRST;
	}
	else if (csgo.globals.missed_shots[player->EntIndex()] == 4) {
		player_record->side = RESOLVER_LOW_FIRST;
	}
	else if (csgo.globals.missed_shots[player->EntIndex()] == 5) {
		player_record->side = RESOLVER_LOW_SECOND;
	}
	else if (csgo.globals.missed_shots[player->EntIndex()] >= 6) {
		m_cvar()->ConsoleColorPrintf(Color(255, 0, 0), u8"èäè íàõóé, ýòî ñïðèä, ó òåáÿ êôã èññóå\n");
		csgo.globals.missed_shots[player->EntIndex()] = 0;
	}

}

float resolver::resolve_pitch()
{
}
*/

// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "animation_system.h"
#include "..\ragebot\aim.h"

void resolver::initialize(player_t* e, adjust_data* record, const float& goal_feet_yaw, const float& pitch)
{
	player = e;
	player_record = record;

	original_goal_feet_yaw = math::normalize_yaw(goal_feet_yaw);
	original_pitch = math::normalize_pitch(pitch);
}

void resolver::reset()
{
	player = nullptr;
	player_record = nullptr;

	side = false;
	bool fake = false;
	fake = false;

	was_first_bruteforce = false;
	was_second_bruteforce = false;

	original_goal_feet_yaw = 0.0f;
	original_pitch = 0.0f;
}

Vector player_t::get_eye_pos() {
	return m_vecOrigin() + m_vecViewOffset();
}

float flAngleMod(float flAngle)
{
	return((360.0f / 65536.0f) * ((int32_t)(flAngle * (65536.0f / 360.0f)) & 65530));
}

float ApproachAngle(float target, float value, float speed)
{
	target = flAngleMod(target);
	value = flAngleMod(value);

	float delta = target - value;

	if (speed < 0)
		speed = -speed;

	if (delta < -180)
		delta += 360;
	else if (delta > 180)
		delta -= 360;

	if (delta > speed)
		value += speed;
	else if (delta < -speed)
		value -= speed;
	else
		value = target;

	return value;
}

float AngleDiff(float destAngle, float srcAngle)
{
	float delta;

	delta = fmodf(destAngle - srcAngle, 360.0f);
	if (destAngle > srcAngle)
	{
		if (delta >= 180)
			delta -= 360;
	}
	else
	{
		if (delta <= -180)
			delta += 360;
	}
	return delta;
}

float GetChokedPackets(player_t* e)
{
	float simtime = e->m_flSimulationTime();
	float oldsimtime = e->m_flOldSimulationTime();
	float simdiff = simtime - oldsimtime;

	auto chokedpackets = TIME_TO_TICKS(max(0, simdiff));

	if (chokedpackets >= 1)
		return chokedpackets;

	return 0;
}


bool resolver::Side()
{
	AnimationLayer layers[13];
	AnimationLayer moveLayers[3][13];
	AnimationLayer preserver_anim_layers[13];

	auto speed = player->m_vecVelocity().Length2D();

	if (speed > 1.1f)
	{
		if (!layers[12].m_flWeight * 1000.f)
		{
			if ((layers[6].m_flWeight * 1000.f) == (layers[6].m_flWeight * 1000.f))
			{
				float EyeYaw = fabs(layers[6].m_flPlaybackRate - moveLayers[0][6].m_flPlaybackRate);
				float Negative = fabs(layers[6].m_flPlaybackRate - moveLayers[1][6].m_flPlaybackRate);
				float Positive = fabs(layers[6].m_flPlaybackRate - moveLayers[2][6].m_flPlaybackRate);

				if (Positive >= EyeYaw || Positive >= Negative || (Positive * 1000.f))
				{
					last_anims_update_time = m_globals()->m_realtime;
					return true;
				}
			}
			else
			{
				last_anims_update_time = m_globals()->m_realtime;
				return false;
			}
		}
	}
	else if (layers[3].m_flWeight == 0.0f && layers[3].m_flCycle == 0.0f)
	{
		auto delta = std::remainder(math::normalize_yaw(player->m_angEyeAngles().y - player->m_flLowerBodyYawTarget()), 360.f) <= 0.f;
		if (2 * delta)
		{
			if (2 * delta == 2)
			{
				return false;
			}
		}
		else
		{
			return true;
		}
	}
}

bool resolver::DesyncDetect()
{
	if (!player->is_alive())
		return false;
	if (!player->m_fFlags() & FL_ONGROUND)
		return false;
	if (player->get_max_desync_delta() < 10)
		return false;
	if (!player->m_iTeamNum() == csgo.local()->m_iTeamNum())
		return false;
	if (GetChokedPackets(player) == 0)
		return false;
	if (!player->m_hActiveWeapon().Get()->can_fire(true))
		return false;

	return true;
}

void resolver::shitresolver()
{
	auto animstate = player->get_animation_state();

	static auto GetSmoothedVelocity = [](float min_delta, Vector a, Vector b) {
		Vector delta = a - b;
		float delta_length = delta.Length();

		if (delta_length <= min_delta) {
			Vector result;
			if (-min_delta <= delta_length) {
				return a;
			}
			else {
				float iradius = 1.0f / (delta_length + FLT_EPSILON);
				return b - ((delta * iradius) * min_delta);
			}
		}
		else {
			float iradius = 1.0f / (delta_length + FLT_EPSILON);
			return b + ((delta * iradius) * min_delta);
		}
	};

	float maxMovementSpeed = 260.0f;

	auto weapon = player->m_hActiveWeapon().Get();

	Vector velocity = player->m_vecVelocity();
	Vector animVelocity = GetSmoothedVelocity(m_clientstate()->iChokedCommands * 2000.0f, velocity, player->m_vecVelocity());

	float speed = std::fminf(animVelocity.Length(), 260.0f);
	float runningSpeed = speed / (maxMovementSpeed * 0.520f);
	float duckSpeed = speed / (maxMovementSpeed * 0.340);
	float yawModifer = (((animstate->m_bOnGround * -0.3f) - 0.2f) * runningSpeed) + 1.0f;
	float maxBodyYaw = *(float*)(uintptr_t(animstate) + 0x334) * yawModifer;
	float minBodyYaw = *(float*)(uintptr_t(animstate) + 0x330) * yawModifer;
	float eyeYaw = player->m_angEyeAngles().y;
	float left = eyeYaw - minBodyYaw;
	float right = eyeYaw + maxBodyYaw;
	float eyeDiff = std::remainderf(eyeYaw - original_goal_feet_yaw, 360.f);

	if (eyeDiff <= minBodyYaw)
	{
		if (minBodyYaw > eyeDiff)
			original_goal_feet_yaw = fabs(minBodyYaw) + eyeYaw;
	}
	else
	{
		original_goal_feet_yaw = eyeYaw - fabs(maxBodyYaw);
	}

	original_goal_feet_yaw = std::remainderf(original_goal_feet_yaw, 360.f);

	if (speed > 0.1f || fabs(velocity.z) > 100.0f)
	{
		original_goal_feet_yaw = ApproachAngle(
			eyeYaw,
			original_goal_feet_yaw,
			((animstate->m_bOnGround * 15.0f) + 30.0f) * m_clientstate()->iChokedCommands
		);
	}
	else
	{
		original_goal_feet_yaw = ApproachAngle(
			player->m_flLowerBodyYawTarget(), original_goal_feet_yaw, m_clientstate()->iChokedCommands * 90.0f);
	}

	if (resolver::Side())
	{
		switch (csgo.globals.missed_shots[player->EntIndex()] % 3)
		{
		case 0:
			animstate->m_flGoalFeetYaw = math::normalize_yaw(right);
			break;
		case 1:
			animstate->m_flGoalFeetYaw = math::normalize_yaw(original_goal_feet_yaw);
			break;
		case 2:
			animstate->m_flGoalFeetYaw = math::normalize_yaw(left);
			break;
		}
	}
	else
	{
		switch (csgo.globals.missed_shots[player->EntIndex()] % 3)
		{
		case 0:
			animstate->m_flGoalFeetYaw = math::normalize_yaw(left);
			break;
		case 1:
			animstate->m_flGoalFeetYaw = math::normalize_yaw(original_goal_feet_yaw);
			break;
		case 2:
			animstate->m_flGoalFeetYaw = math::normalize_yaw(right);
			break;
		}
	}
}


void resolver::resolve_yaw()
{
	player_info_t player_info;

	if (!player || !player->get_animation_state() || !m_engine()->GetPlayerInfo(player->EntIndex(), &player_info) || player_info.fakeplayer || !resolver::DesyncDetect())
		return;

	if (csgo.globals.missed_shots[player->EntIndex()] <= 2)
	{
		resolver::shitresolver();
		return;
	}

	auto animstate = player->get_animation_state();
	float resolveValue;
	int resolveSide = resolver::Side() ? 1 : -1;

	if (player->m_vecVelocity().Length2D() > 1.1f && player->m_vecVelocity().Length2D() < 110.0f)
	{
		switch (csgo.globals.missed_shots[player->EntIndex()] % 3)
		{
		case 0:
			resolveValue = 30.0f;
			break;
		case 1:
			resolveValue = 58.0f;
			break;
		case 2:
			resolveValue = 19.0f;
			break;
		}
	}
	else
	{

			switch (csgo.globals.missed_shots[player->EntIndex()] % 3)
			{
			case 0:
				resolveValue = 69.0f;
				break;
			case 1:
				resolveValue = 30.0f;
				break;
			case 2:
				resolveValue = 19.0f;
				break;
			}
	}

	if (resolveValue > player->get_max_desync_delta())
		resolveValue = player->get_max_desync_delta();

	player->get_animation_state()->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + resolveValue * resolveSide);
	}
void resolver::skeetresik(player_t* e)
{
	if (!vars.ragebot.enable)
		return;

	auto anim_state = player->get_animation_state(); // get state

	static float v42, v56, v49,
		v51, v46;

	float desync_delta; // xmm0_4
	float v4; // xmm1_4
	float v5; // xmm1_4
	float v6; // xmm1_4
	float v7; // xmm1_4
	float v8; // xmm1_4
	float v9; // xmm1_4

	if (anim_state->m_fDuckAmount > 0.0)
	{
		auto v29 = 0.0;
		if (anim_state->m_flFeetSpeedUnknownForwardOrSideways < 0.0)
			v29 = 0.0;

		float v52 = anim_state->m_fDuckAmount * v51;
		v49 = v49 + (float)(v52 * (float)(0.5 - v49));
	}

	float v47 = (float)((anim_state->m_flStopToFullRunningFraction * -0.30000001f) - 0.19999999f) * v46;
	v49 = v47 + 1.0;

	float speed;
	if (*(float*)(anim_state + 0xF8) < 0.f)
	{
		speed = 0.0;
	}
	else
	{
		speed = fminf(*(DWORD*)(anim_state + 0xF8), 1.0f);
	}
	float flYawModifier = (*(float*)(anim_state + 0x11C) * -0.30000001 - 0.19999999) * speed;
	desync_delta = flYawModifier * 58.0;
	switch (csgo.globals.missed_shots[player->EntIndex()] % 4)
	{
	case 1:
		v4 = anim_state->m_flGoalFeetYaw + (desync_delta + desync_delta);
		if (v4 > 180.0 || v4 < -180.0)
			break;
	case 2:
		v5 = anim_state->m_flGoalFeetYaw + (desync_delta * 0.5);
		if (v5 > 180.0 || v5 < -180.0)
			break;
	case 4:
		v6 = anim_state->m_flGoalFeetYaw + (desync_delta * -0.5);
		if (v6 > 180.0 || v6 < -180.0)
			break;
	case 5:
		v8 = anim_state->m_flGoalFeetYaw - (desync_delta + desync_delta);
		if (v8 > 180.0 || v8 < -180.0)
			break;
	case 7:
		v9 = anim_state->m_flGoalFeetYaw + 120.0;
		if ((anim_state->m_flGoalFeetYaw + 120.0) > 180.0 || v9 < -180.0)
			break;
	case 8:
		v7 = anim_state->m_flGoalFeetYaw + -120.0;
		if ((anim_state->m_flGoalFeetYaw + -120.0) > 180.0 || v7 < -180.0)
			break;
	default:
		return;
	}
	float lby_delta = e->m_flLowerBodyYawTarget();
	lby_delta = std::remainderf(lby_delta, 360.f);
	lby_delta = math::clamp(lby_delta, -60.f, 60.f);

	float v73 = std::remainderf(lby_delta, 360.f);

	if (v73 < 0.f) {
		v73 += 360.f;
	}

	static int v36;
	if (anim_state)
	{
		if (csgo.globals.missed_shots[e->EntIndex()] <= 2) {

			if (v36 > 180.0)
				v36 = v36 - 360.0;
			if (v36 < 180.0)
				v36 = v36 + 360.0;
			anim_state->m_flGoalFeetYaw = v36;
		}
	}

	float v25 = math::clamp(anim_state->m_fDuckAmount + anim_state->m_fLandingDuckAdditiveSomething, 0.0f, 1.0f);
	float v26 = anim_state->m_fDuckAmount;
	float v27 = 6.0f;
	float v28;

	if ((v25 - v26) <= v27) { // clamp
		if (-v27 <= (v25 - v26))
			v28 = v25;
		else
			v28 = v26 - v27;
	}
	else {
		v28 = v26 + v27;
	}
}

void resolver::ResolveAngles(player_t* player)
{
	// lets be real this is the most p thing we ever seen
	float m_flResolveValue;
	int m_flResolveSide;
	AnimationLayer layers[15];
	AnimationLayer moveLayers[3][15];
	int m_flSide;
	bool m_bAnimatePlayer;
	bool m_bAnimsUpdated;
	bool m_bResolve;
	bool m_flPreviousDelta;
	// yee jarvis nanotechnology please.
   // ok sir...
  // fly mode activated

	if (player->is_alive() && !player->is_player())
	{
		if (!(player->m_fFlags() & FL_ONGROUND))
		{
			m_flResolveSide = 0;
		}
		auto standing = layers[3].m_flWeight == 0.0f && layers[3].m_flCycle == 0.0f;
		auto animating = layers[12].m_flWeight * 1000.f;
		auto moving = !animating && (layers[6].m_flWeight * 1000.f) == (layers[6].m_flWeight * 1000.f);

		float m_flSpeed = player->m_vecVelocity().Length2D();
		if (m_flSpeed > 1.1f)
		{
			if (moving)
			{
				float EyeYaw = fabs(layers[6].m_flPlaybackRate - moveLayers[0][6].m_flPlaybackRate);
				float Negative = fabs(layers[6].m_flPlaybackRate - moveLayers[2][6].m_flPlaybackRate);
				float Positive = fabs(layers[6].m_flPlaybackRate - moveLayers[1][6].m_flPlaybackRate);
				if (Positive > EyeYaw || Positive >= Negative || (Positive * 1000.0))
				{
					if (EyeYaw >= Negative && Positive > Negative && !(Negative * 1000.0))
					{
						m_bAnimsUpdated = true;
						m_bResolve = true;
						m_flSide = 1;
					}
				}
				else
				{
					m_bAnimsUpdated = true;
					m_bResolve = true;
					m_flSide = -1;
				}
			}
		}
		else if (standing)
		{
			auto m_flEyeDelta = std::remainderf(math::NormalizedAngle(player->m_angEyeAngles().y - player->m_flLowerBodyYawTarget()), 360.f) <= 0.f;
			if (2 * m_flEyeDelta)
			{
				if (2 * m_flEyeDelta == 2)
				{
					m_flSide = -1;
				}
			}
			else
			{
				m_flSide = 1;
			}
			m_bResolve = true;
			m_flPreviousDelta = m_flEyeDelta;
		}
		m_flResolveValue = 58.f;
		player->get_animation_state()->m_flGoalFeetYaw = math::NormalizedAngle(player->m_angEyeAngles().y + m_flResolveValue * m_flSide);

	}

}


float resolver::resolve_pitch()
{
	return original_pitch;
}
