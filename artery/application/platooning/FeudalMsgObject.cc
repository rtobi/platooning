/*
 * FeudalMsgObject.cc
 *
 *      Author: rtobi
 */

#include "artery/application/platooning/FeudalMsgObject.h"
#include <omnetpp.h>
#include <cassert>

Register_Abstract_Class(artery::platooning::FeudalMsgObject)

namespace artery
{
namespace platooning
{

FeudalMsgObject::FeudalMsgObject(FeudalMsg&& msg) :
	m_spm_wrapper(std::make_shared<FeudalMsg>(std::move(msg)))
{
}

FeudalMsgObject::FeudalMsgObject(const FeudalMsg& msg) :
	m_spm_wrapper(std::make_shared<FeudalMsg>(msg))
{
}

FeudalMsgObject::FeudalMsgObject(const std::shared_ptr<const FeudalMsg>& msg) :
	m_spm_wrapper(msg)
{
    assert(m_spm_wrapper);
}

const FeudalMsg& FeudalMsgObject::sharedObject() const
{
    return *m_spm_wrapper;
}

std::shared_ptr<const FeudalMsg> FeudalMsgObject::shared_ptr() const
{
    return m_spm_wrapper;
}

} // namespace platooning
} // namespace artery



