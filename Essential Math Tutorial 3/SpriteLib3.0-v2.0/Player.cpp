#include "Player.h"

Player::Player()
{
}

Player::Player(std::string& fileName, std::string& animationJSON, int width, int height, Sprite* sprite, AnimationController* controller, Transform* transform, bool hasPhys, PhysicsBody* body)
{
	InitPlayer(fileName, animationJSON, width, height, sprite, controller, transform, hasPhys, body);
}

void Player::InitPlayer(std::string& fileName, std::string& animationJSON, int width, int height, Sprite* sprite, AnimationController* controller, Transform* transform, bool hasPhys, PhysicsBody* body)
{
	//Store references to the components
	m_sprite = sprite;
	m_animController = controller;
	m_transform = transform;
	m_hasPhysics = hasPhys;
	if (hasPhys)
	{
		m_physBody = body;
	}

	//Initialize UVs
	m_animController->InitUVs(fileName);

	//Loads the texture and sets width and height
	m_sprite->LoadSprite(fileName, width, height, true, m_animController);
	m_animController->SetVAO(m_sprite->GetVAO());
	m_animController->SetTextureSize(m_sprite->GetTextureSize());

	//Loads in the animations json file
	nlohmann::json animations = File::LoadJSON(animationJSON);

	//Idle Animations\\

	//Idle Left
	m_animController->AddAnimation(animations["IdleLeft"].get<Animation>());
	//Idle Right
	m_animController->AddAnimation(animations["IdleRight"].get<Animation>());
	//Idle Up
	m_animController->AddAnimation(animations["IdleUp"].get<Animation>());
	//Idle Down 
	m_animController->AddAnimation(animations["IdleDown"].get<Animation>());

	//Walk Animation\\

	//Walk Left
	m_animController->AddAnimation(animations["WalkLeft"].get<Animation>());
	//WalkRight
	m_animController->AddAnimation(animations["WalkRight"].get<Animation>());
	//WalkUp
	m_animController->AddAnimation(animations["WalkUp"].get<Animation>());
	//WalkDown
	m_animController->AddAnimation(animations["WalkDown"].get<Animation>());

	//Attack Animation\\

	//AttackLeft
	m_animController->AddAnimation(animations["AttackLeft"].get<Animation>());
	//AttackRight
	m_animController->AddAnimation(animations["AttackRight"].get<Animation>());
	//AttackUp
	m_animController->AddAnimation(animations["AttackUp"].get<Animation>());
	//AttackDown
	m_animController->AddAnimation(animations["AttackDown"].get<Animation>());

	//Set Default Animation
	m_animController->SetActiveAnim(IDLELEFT);

}

void Player::Update()
{
	if (!m_locked)
	{
		MovementUpdate();
	}

	AnimationUpdate();
}

void Player::MovementUpdate()
{
	//Automatically sets Link as not moving
	m_moving = false;

	//If link has physics
	if (m_hasPhysics)
	{	
		//Variable Declaration/Implementation
		float acceleration = 300.f;
		vec3 vel = vec3(0.f, 0.f, 0.f);
		vel.y = m_physBody->GetBody()->GetLinearVelocity().y;
		vel.x = m_physBody->GetBody()->GetLinearVelocity().x;

		
		//If A or D is pressed, start timer for input
		if (Input::GetKeyDown(Key::A) || Input::GetKeyDown(Key::D))
		{
			startTime = Timer::currentClock;
			
		}

		//If Else If statement to accelerate with the A key and deccelerate when letting go and facing the keys direction
		if (Input::GetKey(Key::A))
		{
			
			vel.x -= acceleration * Timer::deltaTime;
			m_facing = LEFT;
			m_moving = true;
			
		}
		else if (m_facing == LEFT && vel.x < 0)
		{

			vel.x += acceleration * Timer::deltaTime;

			if (vel.x >= 0)
			{
				vel.x = 0;
			}
		}

		//If Else If statement to accelerate with the D key and deccelerate when letting go and facing the keys direction
		if (Input::GetKey(Key::D))
		{
			
			vel.x += acceleration * Timer::deltaTime;
			m_facing = RIGHT;
			m_moving = true;

		}
		else if (m_facing == RIGHT && vel.x > 0)
		{

			vel.x -= acceleration * Timer::deltaTime;

			if (vel.x <= 0)
			{
				vel.x = 0;
			}
		}

		//Nested If statements to check if the Link is already jumping, and if not to allow Link to jump
		if(vel.y == 0)
		{

		if (Input::GetKeyDown(Key::W))
		{
			vel.y = 5000;
			m_moving = true;
			startTime = Timer::currentClock;

		}

		}


		//If else if to limit the maximum speed Link can reach when moving facing LEFT or RIGHT
		if (fabsf(vel.x) >= 120)
		{
			if (m_facing == LEFT)
			{

				vel.x = -120;

			}
			else if (m_facing == RIGHT)
			{

				vel.x = 120;

			}
		}


		//Timer for calculating movement per time frame
		float DeltaTime = Timer::currentClock - startTime;
	
		std::cout << DeltaTime << "speed x: " << fabsf(vel.x) << "\n";
		
		m_physBody->SetVelocity(vel);

	}
	else
	{
		//Regular Movement
		float speed = 15.f;
	
		if (Input::GetKey(Key::A))
		{
			m_transform->SetPositionX(m_transform->GetPositionX() - (speed * Timer::deltaTime));
			m_facing = LEFT;
			m_moving = true;
		}
		if (Input::GetKey(Key::D))
		{
			m_transform->SetPositionX(m_transform->GetPositionX() + (speed * Timer::deltaTime));
			m_facing = RIGHT;
			m_moving = true;
		}
	}

	if (Input::GetKeyDown(Key::Space))
	{
		vec3 vel = vec3(0.f, 0.f, 0.f);
		vel.y = m_physBody->GetBody()->GetLinearVelocity().y;
		m_moving = false;
		if (m_hasPhysics && vel.y == 0)
		{
			m_physBody->SetVelocity(vec3());
		}

		m_attacking = true;
		m_locked = true;
	}
}

void Player::AnimationUpdate()
{
	int activeAnimation = 0;

	if (m_moving)
	{
		//Puts it into the WALK category
		activeAnimation = WALK;
	}
	else if (m_attacking)
	{
		activeAnimation = ATTACK;

		//Check if the attack animation is done
		if (m_animController->GetAnimation(m_animController->GetActiveAnim()).GetAnimationDone())
		{
			//Will auto set to idle
			m_locked = false;
			m_attacking = false;
			//Resets the attack animation
			m_animController->GetAnimation(m_animController->GetActiveAnim()).Reset();

			activeAnimation = IDLE;
		}
	}
	else
	{
		activeAnimation = IDLE;
	}

	SetActiveAnimation(activeAnimation + (int)m_facing);
}

void Player::SetActiveAnimation(int anim)
{
	m_animController->SetActiveAnim(anim);
}
