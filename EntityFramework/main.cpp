#include <iostream>

#include "EntityFramework.h"

#include "VisualLeakDetector/vld.h"

using namespace ef;     


struct TurnedZombieEvent : public Event<TurnedZombieEvent>
{
	 
};

struct ZombieThrewUpEvent : public Event<ZombieThrewUpEvent>
{
};

struct ZombieComponent : public Component<ZombieComponent>
{
	int health;
	ZombieComponent(int h):health(h){}
};

struct HumanComponent : public Component<HumanComponent>
{
	int health;
	HumanComponent(int h):health(h){}
};

class ZombieSystem : public System<ZombieSystem>
{
	void init(EventManager & em)
	{
		System::init(em);
		em.listenTo<TurnedZombieEvent>(this, &ZombieSystem::receive);
		em.listenTo<ZombieThrewUpEvent>(this, &ZombieSystem::receive);
	}

	void receive(TurnedZombieEvent & ev)
	{
		std::cout<<"A human turned into a zombie!\n";
	}

	void receive(ZombieThrewUpEvent & ev)
	{
		std::cout<<"A zombie threw up!\n";
	}


	bool isInterested(Entity * e)
	{
		return e->hasComponent<ZombieComponent>();
	}

	void processEntity(Entity * e, EventManager & em, double dt)
	{
		int & health=e->getComponent<ZombieComponent>()->health;
		
		if(health < 0)
		{
			e->removeFromWorld();
		}
		else
		{
			if(health==1)
				em.emit(ZombieThrewUpEvent());
			std::cout<<"Zombie hp: "<<health<<"\n";
			health-=1;
		}
	}
};

class HumanSystem : public System<HumanSystem>
{

	bool isInterested(Entity * e)
	{
		return e->hasComponent<HumanComponent>();
	}

	void processEntity(Entity * e, EventManager & em, double dt)
	{
		int & health=e->getComponent<HumanComponent>()->health;
		if(health < 0)
		{
			e->removeComponent<HumanComponent>();
			e->addComponent(new ZombieComponent(5));
			em.emit(TurnedZombieEvent());
		}
		else
		{
			std::cout<<"Human hp: "<<health<<"\n";
			health-=1;
		}
	}
};


int main()
{
	std::cout<<std::boolalpha;
	World world;
	world.addSystem(new ZombieSystem);
	world.addSystem(new HumanSystem);
	world.init();
	

	for (int i=0; i<1; ++i)
	{
		Entity * e=world.createEntity();
		e->addComponent(new HumanComponent(i));
		e->addToWorld();
	}
	

	for(int i=0; i<10; ++i)
	{
		world.step(1);
	}
	


	std::cin.get();
}
