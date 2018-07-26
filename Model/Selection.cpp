#include "Selection.h"
#include "Model.h"
#include "Notification.h"
#include "VectorObject.h"

using namespace GMT;
using namespace GMT::Model;

bool Selection::Contains(const Selection& other) const
{
	return m_vert && m_vert == other.m_vert;
}

bool Selection::Remove(const Selection& other)
{
	if (Contains(other))
	{
		m_vert = nullptr;
		return true;
	}
	
	return false;
}
