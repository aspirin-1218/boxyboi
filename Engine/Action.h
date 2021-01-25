#pragma once
#include <functional>
#include <vector>
#include "box.h"

class Action
{
public:
	virtual void Do(std::vector<std::unique_ptr<Box>>& boxes, b2World& world) = 0;
};

class Split : public Action
{
public:
	Split(Box& target)
		: 
		pTarget(&target)
	{}

	//boxes is the vector contains that accommodates all the boxes
	void Do(std::vector<std::unique_ptr<Box>>& boxes, b2World& world) override
	{
		auto children = pTarget->Split(world);
		boxes.insert(boxes.end(),
		std::make_move_iterator(children.begin()),
		std::make_move_iterator(children.end()));
	}
private:
	Box* pTarget;
};

class Tag : public Action
{
public:
	//we don't want to transfer the ownership of target, therefore smart pointer is not being used here
	Tag(Box& target, std::unique_ptr<Box::ColorTrait> colorTarget)
		:
		pTarget(&target),
		pColorTrait(std::move(colorTarget))
	{}

	void Do(std::vector<std::unique_ptr<Box>>& boxes, b2World& world) override
	{
		pTarget->AssumeColorTrait(std::move(pColorTrait));
	}

private:
	Box* pTarget;
	std::unique_ptr<Box::ColorTrait> pColorTrait;
};