#pragma once
#include<functional>
#include<typeinfo>
#include "Box.h"
#include <unordered_map>

using PairType = std::pair<const std::type_info*, const std::type_info*>;

namespace std
{
	template<>
	struct hash<PairType>
	{
		size_t operator()(const PairType& p) const
		{
			const auto hash0 = p.first->hash_code();
			return hash0 ^ (p.second->hash_code() + 0x9e3779b9 + (hash0 << 6) + (hash0 >> 2));
		}
	};

	template<>
	struct equal_to<PairType>
	{
		bool operator()(const PairType& lhs, const PairType& rhs) const
		{
			return *lhs.first == *rhs.first && *lhs.second == *rhs.second;
		}
	};
}

class PatternMatchingListener : public b2ContactListener
{
public:
	void BeginContact(b2Contact* contact) override
	{
		const b2Body* bodyPtrs[] = { contact->GetFixtureA()->GetBody(),contact->GetFixtureB()->GetBody() };
		if (bodyPtrs[0]->GetType() == b2BodyType::b2_dynamicBody &&
			bodyPtrs[1]->GetType() == b2BodyType::b2_dynamicBody)
		{
			Execute(
				*static_cast<Box*>(bodyPtrs[0]->GetUserData()),
				*static_cast<Box*>(bodyPtrs[1]->GetUserData())
			);
		}
	}

	// T represents colortrait 1, U represents colortrait 2, F is the function based on the condition of T and U
	// Among them, T and U are template types while F is the function input argument type
	// This function is called to define handlers
	template<class T, class U, class F>
	void AddCases(F f)
	{
		// give compilation error if the criteria doesn't meet
		static_assert(std::is_base_of<Box::ColorTrait, T>::value, "AddCases requires type Box");
		static_assert(std::is_base_of<Box::ColorTrait, U>::value, "AddCases requires type Box");
		handlers[{&typeid(T), &typeid(U)}] = f;
		handlers[{&typeid(U), &typeid(T)}] = std::bind(f, std::placeholders::_2, std::placeholders::_1);

	}

	template<class T, class U>
	void HasCases()
	{
		handlers.count({ &typeid(T), &typeid(U) }) > 0;
	}

	template<class T, class U>
	void ClearCase()
	{
		handlers.erase({ &typeid(T), &typeid(U) });
	}

private:
	// It runs the target "pair"'s corresponding function
	void Execute(Box& b1, Box& b2)
	{
		//return an iterator
		auto i = handlers.find(
			PairType{ &typeid(b1.GetColorTrait()), &typeid(b2.GetColorTrait()) }
		);
		if (i != handlers.end())
		{
			i->second(b1, b2);
		}
		else
		{
			//default execution: doing nothing
			def(b1, b2);
		}
	}
private:
	std::unordered_map<PairType, std::function<void(Box&, Box&)>> handlers;
	std::function<void(Box&, Box&)> def = [](Box&, Box&){};
};