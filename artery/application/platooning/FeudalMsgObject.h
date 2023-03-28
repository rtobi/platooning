/*
 * FeudalMsgObject.h
 *
 *      Author: rtobi
 */

#ifndef ARTERY_FEUDALMSGOBJECT_H_
#define ARTERY_FEUDALMSGOBJECT_H_

#include <boost/optional.hpp>
#include <omnetpp/cobject.h>
#include "platoon_msgs/FeudalMsg_m.h"
#include <memory>

namespace artery
{
namespace platooning{

class FeudalMsgObject : public omnetpp::cObject
{
    public:
		FeudalMsgObject(FeudalMsg&&);
		FeudalMsgObject(const FeudalMsg&);
		FeudalMsgObject(const std::shared_ptr<const FeudalMsg>&);
        const FeudalMsg& sharedObject() const;

        std::shared_ptr<const FeudalMsg> shared_ptr() const;

    private:
        std::shared_ptr<const FeudalMsg> m_spm_wrapper;
};

} // namespace platooning
} // namespace artery

#endif /* ARTERY_FEUDALMSGOBJECT_H_ */
